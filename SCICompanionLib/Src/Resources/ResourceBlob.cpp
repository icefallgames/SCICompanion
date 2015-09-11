
#include "stdafx.h"
#include "Codec.h"
#include "CodecAlt.h"
#include "AppState.h"
#include <errno.h>
#include "crc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


bool IsValidResourceName(PCTSTR pszName)
{
    size_t cChars = lstrlen(pszName);
    for (size_t i = 0; i < cChars; i++)
    {
        char ch = pszName[i];
        if (!isalpha(ch) && !isdigit(ch) && (ch != '_') && (ch != ' ') && (ch != '-'))
        {
            return false;
        }
    }
    return true;
}

void DisplayInvalidResourceNameMessage(PCTSTR pszName)
{
    TCHAR szBuffer[MAX_PATH];
    StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), TEXT("Invalid resource name: %s.\nNames must consist of letters, numbers, spaces, '_', or '-'."), pszName);
    AfxMessageBox(szBuffer, MB_ERRORFLAGS);
}

bool ValidateResourceSize(DWORD cb, ResourceType type)
{
    bool fRet = true;
    if (cb > ((type == ResourceType::Audio) ? MaxResourceSizeAud : MaxResourceSize))
    {
        TCHAR szBuffer[MAX_PATH];
        StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), TEXT("Resources can't be bigger than %d bytes.  This resource is %d bytes."), MaxResourceSize, cb);
        AfxMessageBox(szBuffer, MB_ERRORFLAGS);
        fRet = false;
    }
    return fRet;
}

ResourceBlob::ResourceBlob()
{
    header.Version = sciVersion0; // Just something by default;
    _id = (int)appState->CreateUniqueRuntimeID();
    header.CompressionMethod = 0;
    header.Number = -1;
    header.Type = ResourceType::None;
    header.PackageHint = 1;
    _fComputedChecksum = false;
    _resourceLoadStatus = ResourceLoadStatusFlags::None;
    _hasNumber = false;
}

ResourceBlob::ResourceBlob(PCTSTR pszName, ResourceType iType, const std::vector<BYTE> &data, int iPackageHint, int iNumber, SCIVersion version, ResourceSourceFlags sourceFlags)
{
    _resourceLoadStatus = ResourceLoadStatusFlags::None;
    _pData = data;
    // REVIEW: do some validation
    header.Version = version;
    header.SourceFlags = sourceFlags;
    header.Type = iType;
    header.Number = (WORD)iNumber;
    _hasNumber = (iNumber != -1);
    header.CompressionMethod = 0;
    header.cbDecompressed = (DWORD)data.size();
    header.cbCompressed = header.cbDecompressed;
    _iChecksum = 0;
    _fComputedChecksum = false;

    header.PackageHint = iPackageHint;
    if (!pszName || (*pszName == 0))
    {
        _strName = appState->GetResourceMap().Helper().FigureOutName(iType, iNumber);
    }
    else
    {
        _strName = pszName;
    }
}

int ResourceBlob::GetLengthOnDisk() const
{
    // The size of the header plus the compressed size.
    uint32_t headerSize =
        (header.Version.MapFormat > ResourceMapFormat::SCI0_LayoutSCI1) ? sizeof(RESOURCEHEADER_SCI1) : sizeof(RESOURCEHEADER_SCI0);

    return headerSize + header.cbCompressed;
}

HRESULT ResourceBlob::CreateFromBits(PCTSTR pszName, ResourceType iType, sci::istream *pStream, int iPackageHint, int iNumber, SCIVersion version, ResourceSourceFlags sourceFlags)
{
    HRESULT hr = E_FAIL;
    if (pStream)
    {
        hr = S_OK;
        _pData.resize(pStream->GetDataSize());
        pStream->read_data(&_pData[0], pStream->GetDataSize());
        assert(pStream->good()); // It told us the size, so it should always succeed.
        // REVIEW: do some validation
        header.Type = iType;
        header.Number = (WORD)iNumber;
        _hasNumber = (iNumber != -1);
        header.CompressionMethod = 0;
        header.cbDecompressed = pStream->GetDataSize();
        header.cbCompressed = header.cbDecompressed;
        header.PackageHint = iPackageHint;
        header.Version = version;
        header.SourceFlags = sourceFlags;

        if (!pszName || (*pszName == 0))
        {
            _strName = appState->GetResourceMap().Helper().FigureOutName(iType, iNumber);
        }
        else
        {
            _strName = pszName;
        }
    }
    return hr; // TODO Do data validation
}

//
// Loads a resource that was saved into its own file by this application, or SCIStudio.
// These resources are not prepended with a header.  Instead, they have a WORD that is 0x80 | type,
// where type is the resource type.
//
HRESULT ResourceBlob::CreateFromFile(PCTSTR pszName, std::string strFileName, SCIVersion version, int iPackage, int iNumber)
{
    HRESULT hr;
    HANDLE hFile = CreateFile(strFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        hr = CreateFromHandle(pszName, hFile, -1, version);
        if (SUCCEEDED(hr))
        {
            if (iNumber == -1)
            {
                iNumber = ResourceNumberFromFileName(strFileName.c_str());
            }

            _Init(iPackage, iNumber);
        }
        CloseHandle(hFile);
    }
    else
    {
        hr = ResultFromLastError();
    }
    return hr;
}

sci::istream ResourceBlob::GetReadStream() const
{
    return (header.cbDecompressed > 0) ? sci::istream(&_pData[0], header.cbDecompressed) : sci::istream();
}

DecompressionAlgorithm VersionAndCompressionNumberToAlgorithm(SCIVersion version, int compressionNumber)
{
    switch (compressionNumber)
    {
    case 0:
        return DecompressionAlgorithm::None;
    case 1:
        return (version.CompressionFormat == CompressionFormat::SCI0) ? DecompressionAlgorithm::LZW : DecompressionAlgorithm::Huffman;
    case 2:
        return (version.CompressionFormat == CompressionFormat::SCI0) ? DecompressionAlgorithm::Huffman : DecompressionAlgorithm::LZW1;
    case 3:
        return DecompressionAlgorithm::LZW_View;
    case 4:
        return DecompressionAlgorithm::LZW_Pic;
    case 18:
    case 19:
    case 20:
        return DecompressionAlgorithm::DCL;
    }
    return DecompressionAlgorithm::Unknown;
}

std::string ResourceBlob::GetEncodingString() const
{
    switch (VersionAndCompressionNumberToAlgorithm(appState->GetVersion(), header.CompressionMethod))
    {
    case DecompressionAlgorithm::None:
        return "None";
    case DecompressionAlgorithm::Huffman:
        return "Huffman";
    case DecompressionAlgorithm::LZW:
        return "LZW";
    case DecompressionAlgorithm::LZW1:
        return "LZW1";
    case DecompressionAlgorithm::LZW_View:
        return "LZW_View";
    case DecompressionAlgorithm::LZW_Pic:
        return "LZW_Pic";
    case DecompressionAlgorithm::DCL:
        return "DCL";
    }
    return "Unknown";
}

bool _SanityCheckHeader(const ResourceHeaderAgnostic &header)
{
    return ((header.cbCompressed < 0x01000000) && (header.cbDecompressed < 0x01000000));
}

void ResourceBlob::_DecompressFromBits(sci::istream &byteStream)
{
    uint32_t cbCompressedRemaining = header.cbCompressed; // Because cbCompressed includes 4 bytes of the header.
    assert(_pData.empty());

    // If the sizes are ridiculous, catch the corruption here.
    if (_SanityCheckHeader(header))
    {
        _pData.resize(header.cbDecompressed);
        bool cantDecompress = false;

        int iResult = SCI_ERROR_UNKNOWN_COMPRESSION;
        DecompressionAlgorithm algorithm = VersionAndCompressionNumberToAlgorithm(appState->GetVersion(), header.CompressionMethod);
        if (algorithm == DecompressionAlgorithm::None)
        {
            iResult = 0;
            if (header.cbDecompressed > 0)
            {
                byteStream.read_data(&_pData[0], header.cbDecompressed);
            }
        }
        else
        {
            assert(_pDataCompressed.empty()); // Verify no leaks
            _pDataCompressed.resize(cbCompressedRemaining);
            byteStream.read_data(&_pDataCompressed[0], cbCompressedRemaining);
            switch (algorithm)
            {
                case DecompressionAlgorithm::Huffman:
                    iResult = decompressHuffman(&_pData[0], &_pDataCompressed[0], header.cbDecompressed, cbCompressedRemaining);
                    break;
                case DecompressionAlgorithm::LZW:
                    iResult = decompressLZW(&_pData[0], &_pDataCompressed[0], header.cbDecompressed, cbCompressedRemaining);
                    break;
                case DecompressionAlgorithm::LZW1:
                    iResult = decompressLZW_1(&_pData[0], &_pDataCompressed[0], header.cbDecompressed, cbCompressedRemaining);
                    break;
                case DecompressionAlgorithm::LZW_View:
                {
                    std::unique_ptr<uint8_t[]> pTemp(new uint8_t[header.cbDecompressed]);
                    iResult = decompressLZW_1(&pTemp[0], &_pDataCompressed[0], header.cbDecompressed, cbCompressedRemaining);
                    if (iResult == 0)
                    {
                        BoundsCheckedArray<BYTE> dest(&_pData[0], (int)_pData.size());
                        reorderView(&pTemp[0], dest);
                    }
                }
                break;
                case DecompressionAlgorithm::LZW_Pic:
                {
                    std::unique_ptr<uint8_t[]> pTemp(new uint8_t[header.cbDecompressed]);
                    iResult = decompressLZW_1(&pTemp[0], &_pDataCompressed[0], header.cbDecompressed, cbCompressedRemaining);
                    if (iResult == 0)
                    {
                        reorderPic(&pTemp[0], &_pData[0], header.cbDecompressed);
                    }
                }
                break;
                case DecompressionAlgorithm::DCL:
                    iResult =
                        decompressDCL(&_pData[0], &_pDataCompressed[0], header.cbDecompressed, cbCompressedRemaining) ?
                        0 : -1;
                    break;

            }
        }

        if (iResult != 0)
        {
            _resourceLoadStatus |= ResourceLoadStatusFlags::DecompressionFailed;
        }
    }
    else
    {
        _pData.resize(1);   // To avoid problems...
        _resourceLoadStatus |= ResourceLoadStatusFlags::Corrupted;
        header.cbCompressed = 0;
        header.cbDecompressed = 0;
    }
}

void ResourceBlob::CreateFromPackageBits(const std::string &name, const ResourceHeaderAgnostic &prh, sci::istream &byteStream)
{
    header = prh;
    _hasNumber = true;
    _strName = name;

    _DecompressFromBits(byteStream);
}

uint32_t GetResourceOffsetInFile(uint8_t secondHeaderByte)
{
    // The upper byte appears to indicate where the data starts.
    // Some isolated resource files have random data stuffed at the beginning (e.g. name and such)

    // In SQ5 though, we need to special case:
    if (secondHeaderByte & 0x80)
    {
        switch (secondHeaderByte & 0x7f)
        {
        case 0:
            return 24;
        case 1:
            return 2;
        case 4:
            return 8;
        }
    }
    return secondHeaderByte;
}

void _AssignDefaultResourceSourceFlags(ResourceBlob &blob)
{
    switch (blob.GetType())
    {
        case ResourceType::Audio:
            if (appState->GetVersion().AudioVolumeName == AudioVolumeName::Sfx)
            {
                blob.GetHeader().SourceFlags = ResourceSourceFlags::Sfx;
            }
            else
            {
                blob.GetHeader().SourceFlags = ResourceSourceFlags::Aud;
            }
            break;

        case ResourceType::Message:
            if (appState->GetVersion().SeparateMessageMap)
            {
                blob.GetHeader().SourceFlags = ResourceSourceFlags::MessageMap;
            }
            else
            {
                blob.GetHeader().SourceFlags = ResourceSourceFlags::ResourceMap;
            }
            break;

        default:
            blob.GetHeader().SourceFlags = ResourceSourceFlags::ResourceMap;
            break;
    }
}

//
// hFile is assumed to be offset to the beginning of a resource header.
//
HRESULT ResourceBlob::CreateFromHandle(PCTSTR pszName, HANDLE hFile, int iPackageHint, SCIVersion version)
{
    header.PackageHint = (uint16_t)iPackageHint;
    HRESULT hr = E_FAIL;
    // This is the case of a resource existing in a solitary file.
    // Just a WORD consisting of 0x80 | type
    uint16_t w;
    DWORD cbRequested = sizeof(w);
    DWORD cbRead;
    BOOL fRead = ReadFile(hFile, &w, cbRequested, &cbRead, NULL);
    fRead = (cbRead == cbRequested);
    if (fRead)
    {
        if ((w & 0x80) == 0)
        {
            // No good.
            fRead = FALSE;
            SetLastError(ERROR_INVALID_DATA);
        }
        else
        {
            // Set the type.
            header.Type = (ResourceType)((w & 0x00ff) & ~0x0080);
            header.Version = version;

            // Figure out the size from the filesize.
            DWORD dwSize = GetFileSize(hFile, NULL);
            if (dwSize != INVALID_FILE_SIZE)
            {
                DWORD gapUntilData = GetResourceOffsetInFile((uint8_t)(w >> 8));
                SetFilePointer(hFile, gapUntilData, nullptr, FILE_CURRENT);
                dwSize -= gapUntilData;

                header.cbDecompressed = (WORD)dwSize - sizeof(w);
                // Fake this value up for consistency:
                header.cbCompressed = header.cbDecompressed;
                // No compression:
                header.CompressionMethod = 0;
            }
            else
            {
                fRead = FALSE;
            }
        }
    }
    if (pszName && *pszName)
    {
        _strName = pszName;
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
        _AssignDefaultResourceSourceFlags(*this);
    }    
    return hr;
}

//
// If pszName is null, the resource is called nxxx, where xxx is the resource number.
//
void ResourceBlob::_SetName(PCTSTR pszName)
{
    if (pszName)
    {
        _strName = pszName;
    }
    else
    {
        TCHAR szDefName[MAX_PATH];
        StringCchPrintf(szDefName, ARRAYSIZE(szDefName), TEXT("n%03d"), header.Number);
        _strName = szDefName;
    }
}


//
// Export to a separate file
//
HRESULT ResourceBlob::SaveToFile(const std::string &strFileName) const
{
    // Open the file
    HRESULT hr;
    HANDLE hFile = CreateFile(strFileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        // Save it
        hr = SaveToHandle(hFile, TRUE);
        CloseHandle(hFile);
    }
    else
    {
        hr = ResultFromLastError();
    }
    return hr;
}

template<typename _THeader>
BOOL _WriteHeaderToFile(HANDLE hFile, const ResourceHeaderAgnostic &header, DWORD *pcbWrittenHeader)
{
    _THeader rh;
    rh.FromAgnostic(header);
    return WriteFile(hFile, &rh, sizeof(rh), pcbWrittenHeader, nullptr) && (*pcbWrittenHeader == sizeof(rh));
}

//
// Save into a resource file
//
HRESULT ResourceBlob::SaveToHandle(HANDLE hFile, BOOL fNoHeader, DWORD *pcbWritten) const
{
    HRESULT hr;
    DWORD cbWrittenHeader;
    BOOL fWrote = FALSE;
    bool fWriteCompressedIfPossible;
    DWORD checkSize = max(header.cbCompressed, header.cbDecompressed);
    if (checkSize > ((header.Type == ResourceType::Audio) ? MaxResourceSizeAud : MaxResourceSize))
    {
        SetLastError(ERROR_OUTOFMEMORY);  // Not the best, but it will do.
    }
    else
    {
        if (fNoHeader)
        {
            // No resource header - this is in its own file.  It is just an indicator of what type it is.
            WORD w = 0x80 | (int)header.Type;
            ASSERT(sizeof(w) == 2);
            fWrote = WriteFile(hFile, &w, sizeof(w), &cbWrittenHeader, NULL) && (cbWrittenHeader == sizeof(w));
            // When we're writing a resource to a separate file, we don't have a header, so we can't have any compression methods.
            fWriteCompressedIfPossible = FALSE;
        }
        else
        {
            if (header.Version.MapFormat > ResourceMapFormat::SCI0_LayoutSCI1)
            {
                fWrote = _WriteHeaderToFile<RESOURCEHEADER_SCI1>(hFile, header, &cbWrittenHeader);
            }
            else
            {
                fWrote = _WriteHeaderToFile<RESOURCEHEADER_SCI0>(hFile, header, &cbWrittenHeader);
            }
            fWriteCompressedIfPossible = TRUE;
        }
    }

    DWORD cbWrittenData;
    if (fWrote)
    {
        if (fWriteCompressedIfPossible && !_pDataCompressed.empty())
        {
            // Write the compressed version (e.g. when rebuilding resources, etc...)
            DWORD cbActual = header.cbCompressed - 4;
            fWrote = WriteFile(hFile, &_pDataCompressed[0], cbActual, &cbWrittenData, NULL) && (cbWrittenData == cbActual);
        }
        else if (!_pData.empty() || (header.cbDecompressed == 0)) // Check for _cb == 0, since we might just have an empty resource, which is ok (not an error)
        {
            // Write the uncompressed version.
            fWrote = WriteFile(hFile, &_pData[0], header.cbDecompressed, &cbWrittenData, NULL) && (cbWrittenData == header.cbDecompressed);
        }
        else
        {
            fWrote = FALSE;
            SetLastError(ERROR_INVALID_PARAMETER);
        }
    }

    if (fWrote)
    {
        if (pcbWritten)
        {
            *pcbWritten = cbWrittenHeader + cbWrittenData;
        }
        hr = S_OK;
    }
    else
    {
        hr = ResultFromLastError();
    }
    return hr;
}

int ResourceBlob::GetChecksum() const
{
    if (!_fComputedChecksum)
    {
        size_t size = _pData.size();
        _iChecksum = (size > 0) ? crcFast(&_pData[0], _pData.size()) : 0;
        _fComputedChecksum = true;
    }
    return _iChecksum;
}

//
// What follows is code that allows us to stash a resource in a windows .bmp file.
//

//
// data: data to be encoded
// pBits: where it is put
// pBitsEnd: check to avoid overflow -> points to just past the end of the buffer at pBits
//
BYTE *_EncodeByteInHighNibble(BYTE *pBits, BYTE data, const BYTE *pBitsEnd)
{
    BYTE *pBitsReturn = NULL;
    if (pBits < pBitsEnd)
    {
        ASSERT((*pBits & 0xf0) == 0);
        *pBits |= (data & 0xf0);

        pBits++;
        if (pBits < pBitsEnd)
        {
            ASSERT((*pBits & 0xf0) == 0);
            *pBits |= ((data & 0x0f) << 4);
            pBitsReturn = pBits + 1;
        }
    }
    return pBitsReturn;
}


struct BMP_ENCODEDRESOURCE_HEADER
{
    WORD wSize; // Excluding header...
    WORD wType; // 0x80 | type
};

//
// Encodes a resource in bitmap by using the unused high nibble in each byte.
//
bool EncodeResourceInBitmap(const ResourceBlob &blob, const BITMAPINFO &info, BYTE *pBits)
{
    const BYTE *pResourceByte = blob.GetData();
    DWORD cb = blob.GetLength();
    BMP_ENCODEDRESOURCE_HEADER header = { (WORD)cb, 0x80 | (int)blob.GetType() };
    cb += sizeof(header);

    DWORD dwSizeBmp = (info.bmiHeader.biBitCount / 8) * CX_ACTUAL(info.bmiHeader.biWidth) * info.bmiHeader.biHeight;
    if (dwSizeBmp / 2 >= cb)
    {
        BYTE *pBitmapByte = pBits;
        BYTE *pBitmapByteEnd = pBits + dwSizeBmp;

        // Yucky way to do header.
        BYTE *pHeader = (BYTE*)&header;
        pBitmapByte = _EncodeByteInHighNibble(pBitmapByte, (*pHeader), pBitmapByteEnd);
        pHeader++;
        pBitmapByte = _EncodeByteInHighNibble(pBitmapByte, (*pHeader), pBitmapByteEnd);
        pHeader++;
        pBitmapByte = _EncodeByteInHighNibble(pBitmapByte, (*pHeader), pBitmapByteEnd);
        pHeader++;
        pBitmapByte = _EncodeByteInHighNibble(pBitmapByte, (*pHeader), pBitmapByteEnd);

        cb = blob.GetLength();
        while (pBitmapByte && cb)
        {
            pBitmapByte = _EncodeByteInHighNibble(pBitmapByte, (*pResourceByte), pBitmapByteEnd);
            pResourceByte++;
            cb--;
        }
        return true;
    }
    else
    {
        // It won't fit!
        return false;
    }
}



const BYTE *_DecodeByteInHighNibble(const BYTE *pBits, BYTE *pByte, DWORD cbToRead, const BYTE *pBitsEnd)
{
    const BYTE *pBitsReturn = NULL;
    while (cbToRead)
    {
        if (pBits < pBitsEnd)
        {
            *pByte = *pBits & 0xf0;
            pBits++;
            if (pBits < pBitsEnd)
            {
                *pByte |= ((*pBits & 0xf0) >> 4);
                pBits++;
                pBitsReturn = pBits;
            }
        }
        pByte++;
        cbToRead--;
    }
    return pBitsReturn;
}

//
// Try to load a resource from a bitmap
// 
ResourceBlob *Load8BitBmp(const std::string &filename)
{
    ResourceBlob *pBlob = NULL;
    std::ifstream bmpFile(filename.c_str(), std::ios::in | std::ios::binary);
    if (bmpFile.is_open())
    {
        BITMAPFILEHEADER bmfh;
        bmpFile.read((char *)&bmfh, sizeof(bmfh));

        DWORD dwId = bmfh.bfReserved1;
        dwId |= (bmfh.bfReserved2 << 16);

        if (dwId == SCIResourceBitmapMarker)
        {
            // It's one of our secret resources!
            bmpFile.seekg(bmfh.bfOffBits, std::ios_base::beg);

            DWORD cbBitmap = (bmfh.bfSize - (bmfh.bfOffBits - sizeof(bmfh)));

            if (cbBitmap < 0xffffff) // Check to make sure we don't allocate anything crazy
            {
				std::unique_ptr<BYTE[]> data = std::make_unique<BYTE[]>(cbBitmap);
                bmpFile.read((char*) data.get(), cbBitmap);

                BMP_ENCODEDRESOURCE_HEADER header;
                const BYTE *pData = data.get();
                const BYTE *pDataEnd = pData + cbBitmap;
                pData = _DecodeByteInHighNibble(pData, (BYTE*)&header, sizeof(header), pDataEnd);

                if (header.wSize <= (cbBitmap - sizeof(header))) // Sanity check
                {
                    ResourceType type = static_cast<ResourceType>(header.wType & ~0x0080);

                    // This is the buffer that contains our extracted data.
					std::unique_ptr<BYTE[]> extractedData = std::make_unique<BYTE[]>(header.wSize);

                    // Read it.
                    pData = _DecodeByteInHighNibble(pData, extractedData.get(), header.wSize, pDataEnd);
                    if (pData)
                    {
                        sci::istream stream(extractedData.get(), header.wSize);
                        std::unique_ptr<ResourceBlob> blob(new ResourceBlob());
                        if (SUCCEEDED(blob.get()->CreateFromBits(NULL, type, &stream, -1, -1, appState->GetVersion(), ResourceSourceFlags::ResourceMap)))
                        {
                            pBlob = blob.get();
                            blob.release();
                        }
                    }
                }
            }
        }
    }
    return pBlob;
}

bool operator==(const ResourceMapEntryAgnostic &one, const ResourceMapEntryAgnostic &two)
{
    // Don't do a memcmp, because different resource sources don't set the ExtraData values reliably.
    // We don't care about ExtraData
    return one.Number == two.Number &&
        one.Offset == two.Offset &&
        one.PackageNumber == two.PackageNumber &&
        one.Type == two.Type;
}


