/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
#pragma once
#include "ResourceRecency.h"
#include "ResourceSources.h"

class ResourceBlob;

#define LSL6_FIX 1

enum class ResourceEnumFlags : uint16_t
{
    None            = 0x0000,
    NameLookups     = 0x0001,
    CalculateRecency =0x0002,
    MostRecentOnly  = 0x0004,
    ExcludePatchFiles = 0x0008,
    IncludeCacheFiles = 0x0010,
};

std::string GetGameIniFileName(const std::string &gameFolder);

DEFINE_ENUM_FLAGS(ResourceEnumFlags, uint16_t)

// This is used for iterating through various resources in the game (views, pics, etc...)
class ResourceContainer
{
    friend class ResourceIterator;
public:
    // Disable copying this object, as our sci::istream does not survive a copy intact.
    // To support this, we'd need to modify our usage of sci::istream so that it copies
    // the data.
    ResourceContainer(const ResourceContainer &src) = delete;
    ResourceContainer(ResourceContainer &&src) = delete;
    ResourceContainer& operator=(const ResourceContainer &src) = delete;

    ResourceContainer(
        const std::string &gameFolder,
        std::unique_ptr<ResourceSourceArray> mapAndVolumes,
        ResourceTypeFlags resourceTypes,
        ResourceEnumFlags resourceEnumFlags,
        ResourceRecency *pResourceRecency = nullptr);

    class ResourceIterator
    {
    public:
        ResourceIterator(const ResourceIterator &src) = default;
        ResourceIterator& operator=(const ResourceIterator &src) = default;
        ResourceIterator(ResourceContainer *container, bool atEnd);
        ResourceIterator(ResourceContainer *container, bool atEnd, IteratorStatePrivate state);

        typedef std::unique_ptr<ResourceBlob> *pointer;
        typedef std::ptrdiff_t difference_type; // ??
        typedef std::forward_iterator_tag iterator_category;
        typedef std::unique_ptr<ResourceBlob> reference;    // Is this really the same as value_type?
        typedef std::unique_ptr<ResourceBlob> value_type;

        // Regardless of the index, if _atEnd is true, then both iterators are treated as equal.
        friend bool operator==(const ResourceIterator &one, const ResourceIterator &two);
        friend bool operator!=(const ResourceIterator &one, const ResourceIterator &two);

        reference operator*() const;

        reference CreateButDelayDecompression() const;

        ResourceHeaderAgnostic GetResourceHeader() const;

        ResourceIterator& operator++();
        ResourceIterator operator++(int);

        int GetResourceNumber();

    private:
        sci::istream _GetResourceHeaderAndPackage(ResourceHeaderAgnostic &rh) const;
        void _GetNextEntry();
        reference _CreateHelper(bool delayDecompression) const;

        IteratorStatePrivate _state;

        ResourceContainer *_container;
        bool _atEnd;
        ResourceMapEntryAgnostic _currentEntry;
    };

    typedef ResourceIterator iterator;
    typedef ResourceIterator const_iterator;

    iterator begin();
    iterator end();

private:
    bool _PassesFilter(ResourceType type, int resourceNumber, uint32_t base36Number);

    std::string _gameFolder;
    std::set<uint64_t> _trackResources;

    std::unique_ptr<ResourceSourceArray> _mapAndVolumes;

    // TODO: Support "most recent only"? e.g. with a set
    ResourceTypeFlags _resourceTypes;
    ResourceEnumFlags _resourceEnumFlags;

    // Optional
    ResourceRecency *_pResourceRecency;
};

std::unique_ptr<ResourceContainer> CreateResourceContainerSCI0(SCIVersion version, const std::string &gameFolder, std::unique_ptr<ResourceSourceArray> mapAndVolumes, ResourceTypeFlags types, ResourceEnumFlags enumFlags, ResourceRecency *pRecency);

#ifdef LSL6_FIX
#define EXTRA_SPACE 4
#else
#define EXTRA_SPACE 0
#endif

template<typename _TReaderMapHeader>
class SCI1MapNavigator
{
public:
    const AppendBehavior AppendBehavior = AppendBehavior::Replace;
    const size_t ReasonableLimit = 20;

    bool NavAndReadNextEntry(ResourceTypeFlags typeFlags, sci::istream &mapStream, IteratorState &state, ResourceMapEntryAgnostic &entryOut, std::vector<uint8_t> *optionalRawData = nullptr)
    {
        _InitLookupPointers(mapStream);

        if ((state.mapStreamOffset == 0) && (state.lookupTableIndex == 0))// indicating a reset
        {
            // We're guaranteed to have a least one entry, per above code:
            state.mapStreamOffset = lookupPointers[state.lookupTableIndex].wOffset;
        }

        mapStream.seekg(state.mapStreamOffset);

        assert(lookupPointers.size() > 0);
        // If the current seek pointer is beyond the next lookup then stop.
        // We use a while loop, since it's possible some sections are of zero length.
        // And we can do a perf optimization to skip unneeded sections too
        while ((state.lookupTableIndex < (lookupPointers.size() - 1))  &&
            
            ((mapStream.tellg() >= (uint32_t)lookupPointers[state.lookupTableIndex + 1].wOffset) ||
            !IsFlagSet(typeFlags, ResourceTypeToFlag((ResourceType)(lookupPointers[state.lookupTableIndex].bType & ~0x80)))))
        {
            state.lookupTableIndex++;
            mapStream.seekg(lookupPointers[state.lookupTableIndex].wOffset);
        }

        if (lookupPointers[state.lookupTableIndex].bType == 0xff)
        {
            // We're done
            return false;
        }

        _TReaderMapHeader entry;
        mapStream >> entry;
        if (optionalRawData)
        {
            uint8_t *rawData = reinterpret_cast<uint8_t*>(&entry);
            optionalRawData->assign(rawData, rawData + sizeof(entry));
        }

        // Strip out the 0x80 from the resource type.
        entryOut.Type = (ResourceType)(lookupPointers[state.lookupTableIndex].bType & 0x7f);
        entry.SetOffsetNumberAndPackage(entryOut);

        state.mapStreamOffset = mapStream.tellg();
        return mapStream.good();
    }

    void WriteEntry(const ResourceMapEntryAgnostic &entryIn, sci::ostream &mapStreamWriteMain, sci::ostream &mapStreamWriteSecondary, bool isNewEntry)
    {
        // We have: RESOURCEMAPPREENTRY_SCI1
        // And either RESOURCEMAPENTRY_SCI1 / RESOURCEMAPENTRY_SCI1_1
        // The situation is a lot more complicated than SCI0, since we have a lookuptable of RESOURCEMAPPREENTRY_SCI1
        // We don't control the order in which the WriteEntry requests come in, so for now, we'll just write the ResourceMapEntryAgnostic
        // to the secondary stream, and do the bulk of the work in FinalizeMapStream

        // Hold on though. In SCI1, you can't reliably have multiple resources of a type/number in a game.
        // So when appending resources, we need to delete the old ones. To delete them, we just won't copy them over.
        // The resource data will still be appeneded to the volume file, but that can be pruned by rebuilding resources.
        bool writeEntry;
        if (isNewEntry)
        {
            appendedResources[(int)entryIn.Type].insert(entryIn.Number);
            writeEntry = true;
        }
        else
        {
            // Only write it if it's not already been written
            writeEntry = (appendedResources[(int)entryIn.Type].find(entryIn.Number) == appendedResources[(int)entryIn.Type].end());
        }

        if (writeEntry)
        {
            mapStreamWriteSecondary << entryIn;
        }
    }

    void FinalizeMapStreams(sci::ostream &mapStreamWriteMain, sci::ostream &mapStreamWriteSecondary)
    {
        for (int i = 0; i < ARRAYSIZE(appendedResources); i++)
        {
            appendedResources[i].clear();
        }

        sci::ostream subStreams[NumResourceTypes];
        std::vector<ResourceMapEntryAgnostic> sortedMapEntries[NumResourceTypes];
        sci::istream readStream = istream_from_ostream(mapStreamWriteSecondary);
        // Write the version-specific map entries to individual streams corresponding to their type.
        // First stick them in an array to sort them though. SCI1.1 requires sorted resources. Not sure if SCI1.0 does.
        while (readStream.getBytesRemaining() > 0)
        {
            ResourceMapEntryAgnostic mapEntry;
            readStream >> mapEntry;
            sortedMapEntries[(int)mapEntry.Type].push_back(mapEntry);
        }
        // Then sort
        for (int i = 0; i < ARRAYSIZE(sortedMapEntries); i++)
        {
            sort(sortedMapEntries[i].begin(), sortedMapEntries[i].end(),
                [](const ResourceMapEntryAgnostic &mapEntry1, const ResourceMapEntryAgnostic &mapEntry2)
            {
                return mapEntry1.Number < mapEntry2.Number;
            }
                );
        }
        // Then write to stream
        for (int i = 0; i < ARRAYSIZE(sortedMapEntries); i++)
        {
            for (const ResourceMapEntryAgnostic &mapEntry : sortedMapEntries[i])
            {
                _TReaderMapHeader versionedMapEntry;
                versionedMapEntry.FromAgnostic(mapEntry);
                subStreams[i] << versionedMapEntry;
            }
        }


        assert(readStream.getBytesRemaining() == 0);

        // Now write the lookup table for all non-empty types. First, see how many entries we need.
        assert(mapStreamWriteMain.GetDataSize() == 0);
        int preEntryCount = 1; // For the terminator
        for (int i = 0; i < ARRAYSIZE(subStreams); i++)
        {
            if (subStreams[i].GetDataSize() > 0)
            {
                preEntryCount++;
            }
        }

        // Then write the lookup table
        uint32_t baseOffset = preEntryCount * sizeof(RESOURCEMAPPREENTRY_SCI1) + EXTRA_SPACE;
        uint32_t curOffset = baseOffset;
        for (int i = 0; i < ARRAYSIZE(subStreams); i++)
        {
            if (subStreams[i].GetDataSize() > 0)
            {
                RESOURCEMAPPREENTRY_SCI1 preEntry;
                preEntry.bType = (uint8_t)(i + 0x80);
                preEntry.wOffset = curOffset;
                mapStreamWriteMain << preEntry;
                curOffset += subStreams[i].GetDataSize();
            }
        }
        RESOURCEMAPPREENTRY_SCI1 terminator;
        terminator.bType = 0xff;
        terminator.wOffset = curOffset;
        mapStreamWriteMain << terminator;

#ifdef LSL6_FIX
        uint32_t versionStamp = 0x139c;
        mapStreamWriteMain << versionStamp;
#endif

        assert(mapStreamWriteMain.GetDataSize() == baseOffset);

        // Now append the actual map entries
        for (int i = 0; i < ARRAYSIZE(subStreams); i++)
        {
            transfer(istream_from_ostream(subStreams[i]), mapStreamWriteMain, subStreams[i].GetDataSize());
        }
        // That's it!
    }

    static void EnsureResourceAlignment(sci::ostream &volumeStream)
    {
        // Limitations in the resource map entry may enforce WORD-alignment for resources in the volume files.
        _TReaderMapHeader::EnsureResourceAlignment(volumeStream);
    }
    static void EnsureResourceAlignment(uint32_t &offset)
    {
        // Limitations in the resource map entry may enforce WORD-alignment for resources in the volume files.
        _TReaderMapHeader::EnsureResourceAlignment(offset);
    }

    const std::vector<RESOURCEMAPPREENTRY_SCI1> &GetLookupPointers(sci::istream &mapStream) { _InitLookupPointers(mapStream); return lookupPointers; }

private:
    void _InitLookupPointers(sci::istream &mapStream)
    {
        if (lookupPointers.size() == 0)
        {
            // Initialize our lookup pointers
            RESOURCEMAPPREENTRY_SCI1 preEntry = {};
            while ((preEntry.bType != 0xff) && (lookupPointers.size() < ReasonableLimit))
            {
                mapStream >> preEntry;
                lookupPointers.push_back(preEntry);
            }
            //assert(state.lookupTableIndex == 0);
            if (lookupPointers.size() > ReasonableLimit)
            {
                throw std::exception("Corrupt map lookup tables.");
            }
        }
    }

    // Resource tracking for append
    std::unordered_set<int> appendedResources[NumResourceTypes];

    std::vector<RESOURCEMAPPREENTRY_SCI1> lookupPointers;
};

template<typename _TReaderMapHeader>
class SCI0MapNavigator
{
public:
    const AppendBehavior AppendBehavior = AppendBehavior::Append;

    bool NavAndReadNextEntry(ResourceTypeFlags typeFlags, sci::istream &mapStream, IteratorState &state, ResourceMapEntryAgnostic &entryOut, std::vector<uint8_t> *optionalRawData = nullptr)
    {
        mapStream.seekg(state.mapStreamOffset);

        // lookupTableIndex not used for SCI0
        _TReaderMapHeader entry;
        mapStream >> entry;
        if (optionalRawData)
        {
            uint8_t *rawData = reinterpret_cast<uint8_t*>(&entry);
            optionalRawData->assign(rawData, rawData + sizeof(entry));
        }

        entryOut = entry.ToAgnostic();
        state.mapStreamOffset = mapStream.tellg();
        return mapStream.good();
    }

    void WriteEntry(const ResourceMapEntryAgnostic &entryIn, sci::ostream &mapStreamWriteMain, sci::ostream &mapStreamWriteSecondary, bool isNewEntry)
    {
        _TReaderMapHeader entry;
        entry.FromAgnostic(entryIn);
        mapStreamWriteSecondary << entry;
        // Don't need to touch mapStreamWriteMain, since we don't have any lookup table.

    }

    void FinalizeMapStreams(sci::ostream &mapStreamWriteMain, sci::ostream &mapStreamWriteSecondary)
    {
        // The SCI1 navigator will need to do something more complicated here.
        sci::istream reader = istream_from_ostream(mapStreamWriteSecondary);
        reader.seekg(0);
        transfer(reader, mapStreamWriteMain, reader.getBytesRemaining());

        // We need to write the terminating bits here
        _TReaderMapHeader entryTerm;
        memset(&entryTerm, 0xff, sizeof(entryTerm));
        mapStreamWriteMain.WriteBytes((uint8_t*)&entryTerm, sizeof(entryTerm));
    }
    static void EnsureResourceAlignment(sci::ostream &volumeStream) {}
    static void EnsureResourceAlignment(uint32_t &offset) {}
};