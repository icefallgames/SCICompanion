#include "stdafx.h"
#include "Stream.h"
#include "PerfTimer.h"

namespace sci
{
    ostream::ostream()
    {
        _cbReserved = 500;
        _pData = std::make_unique<uint8_t[]>(_cbReserved);
        _cbSizeValid = 0;
        _iIndex = 0;
        _state = std::ios_base::goodbit;
    }

    void ostream::reset()
    {
        _cbSizeValid = 0;
        _iIndex = 0;
        _state = std::ios_base::goodbit;
    }

    void  ostream::EnsureCapacity(uint32_t size)
    {
        if (size > _cbReserved)
        {
            _Grow(size - _cbReserved);
        }
    }

    void ostream::_Grow(uint32_t cbGrowMin)
    {
        // Grow by 50% or cbGrowMin
        uint32_t cbSizeNew = max(_cbReserved + (_cbReserved / 3 * 2), _cbReserved + cbGrowMin);
        std::unique_ptr<uint8_t[]> pDataNew = std::make_unique<uint8_t[]>(cbSizeNew);
        memcpy(pDataNew.get(), _pData.get(), _cbReserved);
        std::swap(_pData, pDataNew);
        _cbReserved = cbSizeNew;
    }

    void ostream::WriteByte(uint8_t b)
    {
        if (_iIndex >= _cbReserved)
        {
            _Grow(1);
        }
        _pData[_iIndex] = b;
        _iIndex++;
        _cbSizeValid = max(_cbSizeValid, _iIndex);
    }

    void ostream::WriteWord(uint16_t w)
    {
        WriteByte((uint8_t)(w & 0xff));
        WriteByte((uint8_t)(w >> 8));
    }


    void ostream::WriteBytes(const uint8_t *pDataIn, int cCount)
    {
        if ((_iIndex + cCount) >= _cbReserved)
        {
            _Grow((uint32_t)cCount);
        }

        memcpy(&_pData[_iIndex], pDataIn, (size_t)cCount);
        _iIndex += (uint32_t)cCount;
        _cbSizeValid = max(_cbSizeValid, _iIndex);
    }

    void ostream::FillByte(uint8_t value, int cCount)
    {
        if ((_iIndex + cCount) >= _cbReserved)
        {
            _Grow((uint32_t)cCount);
        }
        memset(&_pData[_iIndex], value, (size_t)cCount);
        _iIndex += (uint32_t)cCount;
        _cbSizeValid = max(_cbSizeValid, _iIndex);
    }

    void ostream::seekp(uint32_t newPosition)
    {
        if (newPosition >= _cbSizeValid)
        {
            throw std::exception("Attempt to seek past end of stream.");
        }
        else
        {
            _iIndex = newPosition;
        }
    }

    void ostream::seekp(int32_t offset, std::ios_base::seekdir way)
    {
        switch (way)
        {
        case std::ios_base::beg:
            seekp(offset);
            break;
        case std::ios_base::cur:
            assert(false); // I think this is broken
            if (offset > (int32_t)_iIndex)
            {
                throw std::exception("Attempt to seek past the beginning of the stream.");
            }
            seekp(_iIndex - offset);
            break;
        case std::ios_base::end:
            if ((offset > (int32_t)_cbSizeValid) || (offset > 0))
            {
                throw std::exception("Attempt to seek outside stream.");
            }
            seekp(_cbSizeValid - offset);
            break;
        }
    }







    istream::istream(const sci::istream &original, uint32_t absoluteOffset) : istream(original)
    {
        _iIndex = absoluteOffset;
    }

    istream::istream()
    {
        _throwExceptions = false;
        _iIndex = 0;
        _cbSizeValid = 0;
        _pDataReadOnly = nullptr;
        _state = std::ios_base::goodbit;
    }

    istream::istream(const uint8_t *pData, uint32_t cbSize)
    {
        _throwExceptions = false;
        _iIndex = 0;
        _cbSizeValid = cbSize;
        _pDataReadOnly = pData;
        _state = std::ios_base::goodbit;
    }

    bool istream::_Read(uint8_t *pDataDest, uint32_t cCount)
    {
        if ((_cbSizeValid > _iIndex) && ((_cbSizeValid - _iIndex) >= cCount))
        {
            // we're good.
            CopyMemory(pDataDest, _pDataReadOnly + _iIndex, cCount);
            _iIndex += cCount;
            return true;
        }
        return false;
    }

    void istream::seekg(uint32_t dwIndex)
    {
        _iIndex = dwIndex;
        if (_iIndex > _cbSizeValid)
        {
            _OnReadPastEnd();
        }
    }

    void istream::seekg(int32_t offset, std::ios_base::seekdir way)
    {
        switch (way)
        {
        case std::ios_base::beg:
            seekg(offset);
            break;
        case std::ios_base::cur:
            seekg(_iIndex + offset);
            break;
        case std::ios_base::end:
            seekg(_cbSizeValid - offset);
            break;
        }
    }

    void istream::_OnReadPastEnd()
    {
        _state = std::ios_base::eofbit | std::ios_base::failbit;
        if (_throwExceptions)
        {
            throw std::exception("Read past end of stream.");
        }
    }

    istream &istream::operator>> (uint16_t &w)
    {
        uint32_t dwSave = tellg();
        if (!_ReadWord(&w))
        {
            seekg(dwSave);
            w = 0;
            _OnReadPastEnd();
        }
        return *this;
    }
    istream &istream::operator>> (uint8_t &b)
    {
        uint32_t dwSave = tellg();
        if (!_ReadByte(&b))
        {
            seekg(dwSave);
            b = 0;
            _OnReadPastEnd();
        }
        return *this;
    }
    istream &istream::operator>> (std::string &str)
    {
        uint32_t dwSave = tellg();
        str.clear();
        // Read a null terminated string.
        bool fDone = false;
        while (_iIndex < _cbSizeValid)
        {
            char x = (char)_pDataReadOnly[_iIndex];
            ++_iIndex;
            if (x)
            {
                str += x;
            }
            else
            {
                fDone = true;
                break;
            }
        }
        if (!fDone && (_iIndex == _cbSizeValid))
        {
            // Failure
            str.clear();
            seekg(dwSave);
            _OnReadPastEnd();
        }
        return *this;
    }

    void istream::skip(uint32_t cBytes)
    {
        if ((_iIndex + cBytes) < _cbSizeValid)
        {
            _iIndex += cBytes;
        }
        else
        {
            _OnReadPastEnd();
        }
    }

    bool istream::peek(uint8_t &b)
    {
        if (_iIndex < _cbSizeValid)
        {
            b = _pDataReadOnly[_iIndex];
            return true;
        }
        return false;
    }

    bool istream::peek(uint16_t &w)
    {
        if ((_iIndex + 1) < _cbSizeValid)
        {
            w = *reinterpret_cast<const uint16_t*>(&_pDataReadOnly[_iIndex]);
            return true;
        }
        return false;
    }

    uint8_t istream::peek()
    {
        uint8_t b;
        peek(b);
        return b;
    }

    void istream::getRLE(std::string &str)
    {
        // Vocab files strings are run-length encoded.
        uint16_t wSize;
        uint32_t dwSave = tellg();
        *this >> wSize;
        if (wSize == 0)
        {
            str.clear();
        }
        else
        {
            while (good() && wSize)
            {
                uint8_t b;
                *this >> b;
                if (b)
                {
                    str += (char)b;
                    --wSize;
                }
                else
                {
                    // Be very careful here... in addition to being RLE, these strings
                    // are null-terminated.  We don't want to go adding a null character to
                    // str, because it will not compare == when it should.
                    break;
                }
            }
        }
        if (!good())
        {
            str.clear();
            seekg(dwSave);
        }
    }

    // A bit sketchy because we're using tellp, not "end"
    istream istream_from_ostream(ostream &src)
    {
        return istream(src.GetInternalPointer(), src.tellp());
    }

    streamOwner::streamOwner(const uint8_t *data, uint32_t size) : _dataMemoryMapped(nullptr), _hMap(nullptr), _hFile(INVALID_HANDLE_VALUE)
    {
        _pData = std::make_unique<uint8_t[]>(size);
        _cbSizeValid = size;
        memcpy(_pData.get(), data, size);
    }

    streamOwner::streamOwner(HANDLE hFile, DWORD lengthToInclude) : _dataMemoryMapped(nullptr), _hMap(nullptr), _hFile(INVALID_HANDLE_VALUE)
    {
        PerfTimer timer("streamOwner");

        DWORD dwSizeHigh = 0;
        // Start from the current position:
        uint32_t dwSize = lengthToInclude;
        if (dwSize == 0)
        {
            // If no length specifies, then until the end of the file.
            DWORD dwCurrentPosition = SetFilePointer(hFile, 0, nullptr, FILE_CURRENT);
            dwSize = GetFileSize(hFile, &dwSizeHigh) - dwCurrentPosition;
        }
        if (dwSize != INVALID_FILE_SIZE)
        {
            // Limit ourselves a little (to 4GB).
            if (dwSizeHigh == 0) 
            {
                // = std::make_unique<uint8_t[]>(dwSize);
                // Don't use make_unique, because it will zero init the array (perf).
                _pData.reset(new uint8_t[dwSize]);
                DWORD dwSizeRead;
                if (ReadFile(hFile, _pData.get(), dwSize, &dwSizeRead, nullptr) && (dwSizeRead == dwSize))
                {
                    _cbSizeValid = dwSize;
                }
                else
                {
                    throw std::exception("Unable to read file data.");
                }
            }
        }

    }

    void transfer(istream from, ostream &to, uint32_t count)
    {
        to.EnsureCapacity(to.GetDataSize() + count);

        uint8_t buffer[1024];
        while (count > 0)
        {
            uint32_t amountToTransfer = min(count, ARRAYSIZE(buffer));
            from.read_data(buffer, amountToTransfer);
            to.WriteBytes(buffer, amountToTransfer);
            count -= amountToTransfer;
        }
    }


    streamOwner::streamOwner(const std::string &filename) : _dataMemoryMapped(nullptr), _hMap(nullptr), _cbSizeValid(0), _pData(nullptr)
    {
        _hFile = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
        if (_hFile != INVALID_HANDLE_VALUE)
        {
            // If no length specifies, then until the end of the file.
            DWORD dwSizeHigh;
            DWORD dwSize = GetFileSize(_hFile, &dwSizeHigh);
            if (dwSize != INVALID_FILE_SIZE)
            {
                assert(dwSizeHigh == 0);
                _hMap = CreateFileMapping(_hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
                if (_hMap != nullptr)
                {
                    _dataMemoryMapped = reinterpret_cast<uint8_t*>(MapViewOfFile(_hMap, FILE_MAP_READ, 0, 0, 0));
                    if (_dataMemoryMapped)
                    {
                        _cbSizeValid = dwSize;
                    }
                }
            }
        }
    }

    streamOwner::~streamOwner()
    {
        if (_dataMemoryMapped)
        {
            BOOL result = UnmapViewOfFile(_dataMemoryMapped);
            assert(result);
        }
        if (_hMap)
        {
            CloseHandle(_hMap);
        }
        if (_hFile != INVALID_HANDLE_VALUE)
        {
            CloseHandle(_hFile);
        }

    }

    uint32_t streamOwner::GetDataSize() { return _cbSizeValid; }

    istream streamOwner::getReader()
    {
        if (_dataMemoryMapped)
        {
            return istream(_dataMemoryMapped, _cbSizeValid);
        }
        else
        {
            return istream(_pData.get(), _cbSizeValid);
        }
    }

}



