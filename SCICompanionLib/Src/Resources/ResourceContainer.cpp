#include "stdafx.h"
#include "ResourceContainer.h"
#include "AppState.h"
#include "ResourceMap.h"

using namespace std;

std::string GetGameIniFileName(const std::string &gameFolder)
{
    string filename = gameFolder;
    filename += "\\game.ini";
    return filename;
}

ResourceContainer::ResourceContainer(
    const std::string &gameFolder,
    std::unique_ptr<ResourceSourceArray> mapAndVolumes,
    ResourceTypeFlags resourceTypes,
    ResourceEnumFlags resourceEnumFlags,
    ResourceRecency *pResourceRecency) :
_resourceTypes(resourceTypes),
_resourceEnumFlags(resourceEnumFlags),
_pResourceRecency(pResourceRecency),
_gameFolder(gameFolder),
_mapAndVolumes(move(mapAndVolumes))
{
}

bool ResourceContainer::_PassesFilter(ResourceType type, int resourceNumber)
{
    bool pass = (ResourceTypeFlags::None != (_resourceTypes & ResourceTypeToFlag(type)));
    if (pass && (ResourceEnumFlags::None != (_resourceEnumFlags & ResourceEnumFlags::MostRecentOnly)))
    {
        // Generate an index consisting of type and number
        uint32_t index = ((uint32_t)type) + (resourceNumber << 16);
        pass = (_trackResources.find(index) == _trackResources.end());
        if (pass)
        {
            _trackResources.emplace(index);
        }
    }
    return pass;
}

ResourceContainer::iterator ResourceContainer::begin() { return ResourceIterator(this, false); }
ResourceContainer::iterator ResourceContainer::end() { return ResourceIterator(this, true); }

// Iterator
ResourceContainer::ResourceIterator::ResourceIterator(ResourceContainer *container, bool atEnd) : _container(container), _atEnd(atEnd)
{
    if (!_atEnd)
    {
        _GetNextEntry();
    }
}

ResourceContainer::ResourceIterator::ResourceIterator(ResourceContainer *container, bool atEnd, IteratorState state) : _container(container), _atEnd(atEnd), _state(state)
{
    if (!_atEnd)
    {
        _GetNextEntry();
    }
}

// Regardless of the index, if _atEnd is true, then both iterators are treated as equal.
bool operator==(const ResourceContainer::ResourceIterator &one, const ResourceContainer::ResourceIterator &two)
{
    return (one._container == two._container) &&
        (one._atEnd == two._atEnd) &&
        (
        one._atEnd || (one._state == two._state)
        );
}
bool operator!=(const ResourceContainer::ResourceIterator &one, const ResourceContainer::ResourceIterator &two)
{
    return (one._container != two._container) ||
        (one._atEnd != two._atEnd) ||
        (
        !one._atEnd && (one._state != two._state)
        );
}

bool operator==(const IteratorState &one, const IteratorState &two)
{
    return one.lookupTableIndex == two.lookupTableIndex &&
        one.mapIndex == two.mapIndex &&
        one.mapStreamOffset == two.mapStreamOffset;
}
bool operator!=(const IteratorState &one, const IteratorState &two)
{
    return !(one == two);
}

sci::istream ResourceContainer::ResourceIterator::_GetResourceHeaderAndPackage(ResourceHeaderAgnostic &rh) const
{
    if (_atEnd)
    {
        throw std::exception("invalid iterator!");
    }

    sci::istream temp = (*_container->_mapAndVolumes)[_state.mapIndex]->GetHeaderAndPositionedStream(_currentEntry, rh);

    assert(rh.Number == _currentEntry.Number && "Corrupt resource map");
    assert(rh.PackageHint == _currentEntry.PackageNumber && "Corrupt resource map");
    // By setting these to those in the resource map (instead of the header), we can ensure that the ResourceBlob matches
    // the resource map information. This ensures that we can delete resources in the case of a corrupt resource map/package.
    rh.Number = _currentEntry.Number;
    rh.PackageHint = _currentEntry.PackageNumber;

    return temp;
}

ResourceHeaderAgnostic ResourceContainer::ResourceIterator::GetResourceHeader() const
{
    ResourceHeaderAgnostic rh;
    sci::istream packageByteStream = _GetResourceHeaderAndPackage(rh);
    return rh;
}

ResourceContainer::ResourceIterator::reference ResourceContainer::ResourceIterator::operator*() const
{
    ResourceHeaderAgnostic rh;
    sci::istream packageByteStream = _GetResourceHeaderAndPackage(rh);

    // We should validate against the type here.
    if (!IsFlagSet(_container->_resourceTypes, ResourceTypeToFlag(rh.Type)))
    {
        throw std::exception("Corrupt resource header - mismatched types.");
    }

    std::string name;
    if ((_container->_resourceEnumFlags & ResourceEnumFlags::NameLookups) != ResourceEnumFlags::None)
    {
        name = FigureOutResourceName(GetGameIniFileName(_container->_gameFolder), _currentEntry.Type, _currentEntry.Number);
    }

    std::unique_ptr<ResourceBlob> blob = std::make_unique<ResourceBlob>();
    blob->CreateFromPackageBits(
        name,
        rh,
//        _currentEntry.PackageNumber,
        packageByteStream);

    if (_container->_pResourceRecency)
    {
        _container->_pResourceRecency->AddResourceToRecency(blob.get(), true);
    }
    return blob;
}

ResourceContainer::ResourceIterator& ResourceContainer::ResourceIterator::operator++()
{
    if (_atEnd)
    {
        throw std::exception("Can't increment an iterator past the end");
    }
    _GetNextEntry();
    return *this;
}
ResourceContainer::ResourceIterator ResourceContainer::ResourceIterator::operator++(int)
{
    assert(false); // Is this ever called?
    // This is actually the heavy weight thingy.
    if (_atEnd)
    {
        throw std::exception("Can't increment an iterator past the end");
    }
    return ResourceIterator(_container, _atEnd, _state);
}

int ResourceContainer::ResourceIterator::GetResourceNumber()
{
    return _currentEntry.Number;
}

void ResourceContainer::ResourceIterator::_GetNextEntry()
{
    assert(!_atEnd);

    // Read the entry in the map.
    while (!_atEnd)
    {
        ResourceMapEntryAgnostic entry;
        while ((_state.mapIndex < _container->_mapAndVolumes->size()) && !(*_container->_mapAndVolumes)[_state.mapIndex]->ReadNextEntry(_container->_resourceTypes, _state, entry))
        {
            // Go to next map
            _state.mapIndex++;
            _state.mapStreamOffset = 0;
            _state.lookupTableIndex = 0;
        }

        // If this is still true, we must have got an entry
        if (_state.mapIndex < _container->_mapAndVolumes->size())
        {
            // Does it pass our filter?
            if (_container->_PassesFilter(entry.Type, entry.Number))
            {
                _currentEntry = entry;
                // We're good to go.
                break;
            }
        }
        else
        {
            _atEnd = true;
        }
    }
}

bool SCI0MapNavigator::NavAndReadNextEntry(ResourceTypeFlags typeFlags, sci::istream &mapStream, IteratorState &state, ResourceMapEntryAgnostic &entryOut, std::vector<uint8_t> *optionalRawData)
{
    mapStream.seekg(state.mapStreamOffset);

    // lookupTableIndex not used for SCI0
    RESOURCEMAPENTRY_SCI0 entry;
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

void SCI0MapNavigator::WriteEntry(const ResourceMapEntryAgnostic &entryIn, sci::ostream &mapStreamWriteMain, sci::ostream &mapStreamWriteSecondary, bool isNewEntry)
{
    RESOURCEMAPENTRY_SCI0 entry;
    entry.FromAgnostic(entryIn);
    mapStreamWriteSecondary << entry;
    // Don't need to touch mapStreamWriteMain, since we don't have any lookup table.
}

void SCI0MapNavigator::FinalizeMapStreams(sci::ostream &mapStreamWriteMain, sci::ostream &mapStreamWriteSecondary)
{
    // The SCI1 navigator will need to do something more complicated here.
    sci::istream reader = istream_from_ostream(mapStreamWriteSecondary);
    reader.seekg(0);
    transfer(reader, mapStreamWriteMain, reader.getBytesRemaining());
    
    // We need to write the terminating bits here
    RESOURCEMAPENTRY_SCI0 entryTerm;
    memset(&entryTerm, 0xff, sizeof(entryTerm));
    mapStreamWriteMain.WriteBytes((uint8_t*)&entryTerm, sizeof(entryTerm));
}
