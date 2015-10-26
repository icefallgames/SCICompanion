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

#include "ResourceUtil.h"
#include "Version.h"

enum class BlobKey;

bool IsValidResourceName(PCTSTR pszName);
void DisplayInvalidResourceNameMessage(PCTSTR pszName);
bool ValidateResourceSize(DWORD cb, ResourceType type);
bool IsValidPackageNumber(int iPackageNumber);

static const DWORD MaxResourceSize = 0xffef; // (0xfff0 - 1)
static const DWORD MaxResourceSizeAud = 0xffffef; // I just made this up to test things
static const DWORD SCIResourceBitmapMarker = (('S' << 24) + ('C' << 16) + ('I' << 8) + 'R');

// Common way to talk about resource map entries that is SCI version agnostic.
struct ResourceMapEntryAgnostic
{
    ResourceMapEntryAgnostic() : Number(0), Type(ResourceType::None), PackageNumber(0), Offset(0), ExtraData(0), Base36Number(0xffffffff) {}
    uint16_t Number;            // e.g. 0-999
    ResourceType Type;          // (0 to x) e.g. PIC == 1
    uint8_t PackageNumber;      // Specifies which resource.xxx to use.
    DWORD Offset;               // Offset within the file resource.iPackageNumber
    uint32_t ExtraData;
    uint32_t Base36Number;
    // Adding more members? Update operator==
};

bool operator==(const ResourceMapEntryAgnostic &one, const ResourceMapEntryAgnostic &two);

enum class ResourceSourceFlags
{
    ResourceMap = 0,
    MessageMap = 0x0001,
    PatchFile = 0x0002,
    Aud = 0x0004,
    Sfx = 0x0008,
    AudioCache = 0x0010,    // Our special audio folder
    AudioMapCache = 0x0020  // 
};

DEFINE_ENUM_FLAGS(ResourceSourceFlags, uint16_t)

// Common way to talk about resource headers that is SCI version agnostic.
struct ResourceHeaderAgnostic
{
    ResourceHeaderAgnostic() : Base36Number(NoBase36) {}
    ResourceHeaderAgnostic(const ResourceHeaderAgnostic &src) = default;
    ResourceHeaderAgnostic &operator=(const ResourceHeaderAgnostic &src) = default;

    int16_t Number;                 // The number of the resource (0 to 999, or higher for SCI1+)
    int16_t PackageHint;
    uint16_t CompressionMethod;     // Compression method.
    // These two values are actually persisted as WORDs.  However, we need to allow for creation of these
    // objects while editing, and we allow the user to go over the limit at that time.  We validate at
    // persistence time that they are ok.
    DWORD cbCompressed;
    DWORD cbDecompressed;
    ResourceType Type;
    SCIVersion Version;
    ResourceSourceFlags SourceFlags;
    uint32_t Base36Number;
    // This only needs to be an unordered_map, but an empty map (the common case) takes up less space (12 bytes vs 40):
    std::map<BlobKey, uint32_t> PropertyBag;
};

// The following two structure need to have 1-byte packing, since
// they are used to read from disk.
#include <pshpack1.h>

//
// SCI0
//

template<typename _TLayout>
struct RESOURCEMAPENTRY_SCI0_BASE : public _TLayout
{
    ResourceType GetType() const { return (ResourceType)iType; }

    ResourceMapEntryAgnostic ToAgnostic()
    {
        ResourceMapEntryAgnostic agnostic;
        agnostic.Number = iNumber;
        agnostic.Type = (ResourceType)iType;
        agnostic.Offset = iOffset;
        agnostic.PackageNumber = iPackageNumber;
        return agnostic;
    }

    void FromAgnostic(const ResourceMapEntryAgnostic &agnostic)
    {
        iNumber = agnostic.Number;
        iType = (uint16_t)agnostic.Type;
        iOffset = agnostic.Offset;
        iPackageNumber = agnostic.PackageNumber;
    }
};

struct LAYOUT_SCI0
{
    uint16_t iNumber : 11;        // (0 to 999)
    uint16_t iType : 5;           // (0 to 9) e.g. PIC == 1
    DWORD iOffset : 26;       // Offset within the file resource.iPackageNumber
    DWORD iPackageNumber : 6; // Specifies which resource.xxx to use.
};


struct LAYOUT_SCI1
{
    uint16_t iNumber : 11;        // (0 to 999)
    uint16_t iType : 5;           // (0 to 9) e.g. PIC == 1
    DWORD iOffset : 28;       // Offset within the file resource.iPackageNumber
    DWORD iPackageNumber : 4; // Specifies which resource.xxx to use.
};

// resource.map consists of an array of these structures, terminated by a sequence where all bits are set to on.
struct RESOURCEMAPENTRY_SCI0 : public RESOURCEMAPENTRY_SCI0_BASE<LAYOUT_SCI0>
{
};

// LSL1 VGA, for instance, has an "sci0" resource map, but the bits are aligned as for SCI1.
struct RESOURCEMAPENTRY_SCI0_SCI1LAYOUT : public RESOURCEMAPENTRY_SCI0_BASE<LAYOUT_SCI1>
{
};

//DWORD iOffset : 28;       // Offset within the file resource.iPackageNumber
//DWORD iPackageNumber : 4; // Specifies which resource.xxx to use.


// header for each entry in resource.xxx
struct RESOURCEHEADER_SCI0
{
	uint16_t iNumber:11;        // The number of the resource (0 to 999)
	uint16_t iType:5;           // PIC == 1
	uint16_t cbCompressed;      // Compressed byte count     (includes extra 4 bytes for cbDecompressed and iMethod below)
	uint16_t cbDecompressed;    // Uncompressed byte count
	uint16_t iMethod;           // Compression method.

    ResourceType GetType() { return (ResourceType)iType; }

    ResourceHeaderAgnostic ToAgnostic(SCIVersion version, ResourceSourceFlags sourceFlags, uint16_t packageHint)
    {
        ResourceHeaderAgnostic agnostic;
        agnostic.Type = GetType();
        agnostic.cbCompressed = cbCompressed - 4;   // See above
        agnostic.cbDecompressed = cbDecompressed;
        agnostic.Number = iNumber;
        agnostic.CompressionMethod = iMethod;
        agnostic.Version = version;
        agnostic.SourceFlags = sourceFlags;
        agnostic.PackageHint = packageHint;
        return agnostic;
    }

    void FromAgnostic(const ResourceHeaderAgnostic &agnostic)
    {
        iType = (int)agnostic.Type;
        iNumber = agnostic.Number;
        cbDecompressed = (uint16_t)agnostic.cbDecompressed;
        cbCompressed = (uint16_t)agnostic.cbCompressed + 4;
        iMethod = agnostic.CompressionMethod;
    }
};

//
// SCI1
//

// resource.map starts with an array of these structures, terminated when bType = 0xff
// There should be one entry for each resource type, and resource types are grouped together.
struct RESOURCEMAPPREENTRY_SCI1
{
    uint8_t bType;             // (0x80 ... 0x91) 0xff = terminator
    uint16_t wOffset;           // absolute offset in resource.map, to resource header (for terminator, wOffset points to after last header)
};

// resource.map contains sorted arrays of these structures, grouped by resource type.  The length of the arrays
// is determined by the RESOURCEMAPPREENTRY_SCI1's.
struct RESOURCEMAPENTRY_SCI1
{
    uint16_t wNumber;           // (0 to ???)
    DWORD iOffset : 28;       // Offset within the file resource.iPackageNumber
    DWORD iPackageNumber : 4; // Specifies with resource.xxx to use.

    void SetOffsetNumberAndPackage(ResourceMapEntryAgnostic &mapEntry)
    {
        mapEntry.Number = wNumber;
        mapEntry.Offset = iOffset;
        mapEntry.PackageNumber = iPackageNumber;
    }

    void FromAgnostic(const ResourceMapEntryAgnostic &mapEntry)
    {
        wNumber = mapEntry.Number;
        iOffset = mapEntry.Offset;
        iPackageNumber = mapEntry.PackageNumber;
    }

    static void EnsureResourceAlignment(sci::ostream &volumeStream) {}
    static void EnsureResourceAlignment(uint32_t &offset) {}
};

// resource.map contains sorted arrays of these structures, grouped by resource type.  The length of the arrays
// is determined by the RESOURCEMAPPREENTRY_SCI1's.
struct RESOURCEMAPENTRY_SCI1_1
{
    // Looks like... hmm... 00, then 3 bytes, then the number.
    uint16_t wNumber;           // (0 to ???)
    uint16_t offsetLow;
    uint8_t offsetHigh;

    void SetOffsetNumberAndPackage(ResourceMapEntryAgnostic &mapEntry)
    {
        mapEntry.Number = wNumber;
        mapEntry.PackageNumber = 0;
        mapEntry.Offset = (offsetLow | (offsetHigh << 16)) << 1;
    }

    void FromAgnostic(const ResourceMapEntryAgnostic &mapEntry)
    {
        assert(!(mapEntry.Offset & 0x1));   // Must be WORD aligned.
        uint32_t offset = mapEntry.Offset >> 1;
        wNumber = mapEntry.Number;
        offsetLow = offset & 0xffff;
        offsetHigh = (offset >> 16) & 0xff;
    }

    static void EnsureResourceAlignment(sci::ostream &volumeStream)
    {
        // Ensure the volumeStream is currently word-aligned.
        if (volumeStream.tellp() & 0x1)
        {
            volumeStream.WriteByte(0);
        }
    }
    static void EnsureResourceAlignment(uint32_t &offset)
    {
        if (offset & 0x1)
        {
            offset++;
        }
    }
};

bool DoesPackageFormatIncludeHeaderInCompressedSize(SCIVersion version);

// header for each entry in resource.xxx
template<typename _TDataSizeSize>
struct RESOURCEHEADERBASE
{
    uint8_t bType;                     // type (0x80 ... 0x91)
    uint16_t iNumber;                  // The number of the resource (0 to 999)
    _TDataSizeSize cbCompressed;       // Compressed byte count (includes cbDecompressed and iMethod)
    _TDataSizeSize cbDecompressed;     // Uncompressed byte count (doesn't include cbDecompressed or iMethod)
	uint16_t iMethod;                  // Compression method. (0 - 3?)

    ResourceHeaderAgnostic ToAgnostic(SCIVersion version, ResourceSourceFlags sourceFlags, int packageHint)
    {
        ResourceHeaderAgnostic agnostic;
        agnostic.Type = (ResourceType)(bType - 0x80);
        agnostic.cbCompressed = cbCompressed - (DoesPackageFormatIncludeHeaderInCompressedSize(version) ? 4 : 0);
        agnostic.cbDecompressed = cbDecompressed;
        agnostic.Number = iNumber;
        agnostic.CompressionMethod = iMethod;
        agnostic.Version = version;
        agnostic.SourceFlags = sourceFlags;
        agnostic.PackageHint = packageHint;
        return agnostic;
    }

    void FromAgnostic(const ResourceHeaderAgnostic &agnostic)
    {
        bType = (uint8_t)agnostic.Type + 0x80;
        iNumber = agnostic.Number;
        cbDecompressed = (_TDataSizeSize)agnostic.cbDecompressed;
        cbCompressed = (_TDataSizeSize)agnostic.cbCompressed + ((DoesPackageFormatIncludeHeaderInCompressedSize(agnostic.Version)) ? 4 : 0);
        iMethod = agnostic.CompressionMethod;
    }
};

struct RESOURCEHEADER_SCI1 : public RESOURCEHEADERBASE<uint16_t>
{

};

struct RESOURCEHEADER_SCI2 : public RESOURCEHEADERBASE<uint32_t>
{

};


#include <poppack.h>

enum class ResourceLoadStatusFlags : uint8_t
{
    None = 0x00,
    DecompressionFailed = 0x01,
    ResourceCreationFailed = 0x02,
    Corrupted = 0x04,
};

enum class BlobKey
{
    LipSyncDataSize,
};

DEFINE_ENUM_FLAGS(ResourceLoadStatusFlags, uint8_t)

//
// This represents the generic encoded resource.  From this object, we create the
// type-specific resources.
//
class ResourceBlob final : public IResourceIdentifier
{
public:
    ResourceBlob();
    ResourceBlob(const ResourceBlob &src) = default;
    ResourceBlob &operator=(const ResourceBlob &src) = default;
    ResourceBlob(PCTSTR pszName, ResourceType iType, const std::vector<uint8_t> &data, int iPackageHint, int iNumberHint, uint32_t base36Number, SCIVersion version, ResourceSourceFlags sourceFlags);

    HRESULT CreateFromBits(PCTSTR pszName, ResourceType iType, sci::istream *pStream, int iPackageHint, int iNumberHint, uint32_t base36Number, SCIVersion version, ResourceSourceFlags sourceFlags);
    HRESULT CreateFromHandle(PCTSTR pszName, HANDLE hFile, int iPackageHint, SCIVersion version);

    //
    // hFile is assumed to be offset to the beginning of a resource header.
    //
    template<typename _TReaderHeader>
    HRESULT CreateFromHandleWithHeader(PCTSTR pszName, HANDLE hFile, int iPackageHint, ResourceSourceFlags sourceFlags)
    {
        HRESULT hr = E_FAIL;
        DWORD cbRead;
        // The normal case: just suck the resource header out of the current file position.
        _TReaderHeader rh;
        DWORD cbRequested = sizeof(rh);
        BOOL fRead = ReadFile(hFile, &rh, cbRequested, &cbRead, NULL);
        fRead = (cbRead == cbRequested);
        if (fRead)
        {
            header = rh.ToAgnostic(appState->GetVersion(), sourceFlags, iPackageHint);
        }

        if (fRead)
        {
            // Figure out a name based on the number
            if (!pszName || (*pszName == 0))
            {
                _strName = appState->GetResourceMap().FigureOutName(header.Type, header.Number, header.Base36Number);
            }
        }

        if (fRead)
        {
            hr = S_OK;
        }
        else
        {
            hr = ResultFromLastError();
        }

        if (SUCCEEDED(hr))
        {
            // Now it is time to read in the bits.
            // We're already at the end of the header, so just start reading.
            sci::streamOwner streamOwner(hFile, header.cbCompressed);
            _DecompressFromBits(streamOwner.getReader());
        }
        return hr;
    }


    HRESULT CreateFromFile(PCTSTR pszName, std::string strFileName, SCIVersion version, int iPackage, int iNumber = -1);
    void CreateFromPackageBits(const std::string &name, const ResourceHeaderAgnostic &prh, sci::istream &byteStream);

    sci::istream GetReadStream() const;

    HRESULT SaveToHandle(HANDLE hFile, BOOL fNoHeader, DWORD *pcbWritten = NULL) const;
    int GetLengthOnDisk() const;

    // IResourceIdentifier
    int GetPackageHint() const override { return header.PackageHint; }
    int GetNumber() const override
    {
        if (_hasNumber)
        {
            // 0xffff -> 65535
            return (int)(uint16_t)header.Number;
        }
        else
        {
            return header.Number;
        }
    }
    uint32_t GetBase36() const override
    {
        return header.Base36Number;
    }
    ResourceType GetType() const override { return header.Type; }
    int GetChecksum() const override;

    HRESULT SaveToFile(const std::string &strFileName) const;
    int GetEncoding() const { return header.CompressionMethod; }
    std::string GetEncodingString() const;
    const uint8_t *GetData() const;
    const uint8_t *GetDataCompressed() const; // WARNING: this can be nullptr!
    int GetLength() const { return header.cbDecompressed; }
    std::string GetName() const { return _strName; }
    void SetName(PCTSTR pszName) { _SetName(pszName); }
    void SetNumber(int iNumber) { header.Number = iNumber; }
    void SetPackage(int iPackage) { header.PackageHint = (uint16_t)iPackage; }
    BOOL HasNumber() const { return _hasNumber || (header.Number != (uint16_t)-1); }  // Does this resource have a number (if you open it from a file, it won't)
    SCIVersion GetVersion() const { return header.Version; }
    ResourceLoadStatusFlags GetStatusFlags() const { return _resourceLoadStatus; }
    void AddStatusFlags(ResourceLoadStatusFlags flags) const { _resourceLoadStatus |= flags;  }
    ResourceSourceFlags GetSourceFlags()  const { return header.SourceFlags; }
    void SetSourceFlags(ResourceSourceFlags flags)  { header.SourceFlags = flags; }

    DWORD GetCompressedLength() const { return header.cbCompressed; }
    DWORD GetDecompressedLength() const { return header.cbDecompressed; }

    void SetKeyValue(BlobKey key, uint32_t value);
    bool GetKeyValue(BlobKey key, uint32_t &value) const;
    std::map<BlobKey, uint32_t> &GetPropertyBag() { return header.PropertyBag; };
    const std::map<BlobKey, uint32_t> &GetPropertyBag() const { return header.PropertyBag; };

    const ResourceHeaderAgnostic &GetHeader() const { return header; }
    ResourceHeaderAgnostic &GetHeader() { return header; }

private:
    void _Init(int iPackageNumber, int iResourceNumber)
    {
        header.PackageHint = iPackageNumber;
        header.Number = iResourceNumber;
        _hasNumber = (iResourceNumber != -1);
    }
    void _DecompressFromBits(sci::istream &byteStream);
    HRESULT _ReadBits(HANDLE hFile);
    void _SetName(PCTSTR pszName);

    // Resource header information
    ResourceHeaderAgnostic header;

    // Uncompressed data.
    std::vector<uint8_t> _pData;
    // Compressed data (optional, can be NULL)
    std::vector<uint8_t> _pDataCompressed;

    // Unique identifier for this resource.
    int _id;
    // Only the resource recency object is allowed to touch the id.
    friend class ResourceRecency;

    // The name of the resource (that which is stored in game.ini for games with source code)
    std::string _strName;

    mutable ResourceLoadStatusFlags _resourceLoadStatus;
    // To distinguish 0x65535 from -1, we can use _hasNumber
    bool _hasNumber;
    mutable bool _fComputedChecksum;
    mutable int _iChecksum;
};

//
// Methods for encoding an SCI resource inside a windows .bmp file.
//
bool EncodeResourceInBitmap(const ResourceBlob &blob, const BITMAPINFO &info, uint8_t *pBits);
ResourceBlob *Load8BitBmp(const std::string &filename);
