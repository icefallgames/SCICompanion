#pragma once

namespace sci
{
    // STL doesn't have a memory stream (other than stringstream), so we'll implement our own
    // simple one.
    class ostream
    {
    public:
        ostream();
        ostream(const ostream &src) = delete;
        ostream& operator=(const ostream &src) = delete;

        void WriteWord(uint16_t w);
        void WriteByte(uint8_t b);
        void WriteBytes(const uint8_t *pData, int cCount);
        void FillByte(uint8_t value, int cCount);

        uint32_t tellp() { return _cbSizeValid; }

        void seekp(uint32_t newPosition);
        void seekp(int32_t offset, std::ios_base::seekdir way);

        // Use with caution!
        uint8_t *GetInternalPointer() { return _pData.get(); }
        const uint8_t *GetInternalPointer() const { return _pData.get(); }

        template<class _T>
        ostream &operator<< (const _T &t)
        {
            WriteBytes(reinterpret_cast<const uint8_t*>(&t), sizeof(t));
            return *this;
        }

        ostream &operator<< (const std::string aString)
        {
            WriteBytes(reinterpret_cast<const uint8_t*>(aString.c_str()), (int)aString.length() + 1);
            return *this;
        }

        uint32_t GetDataSize() const { return _cbSizeValid; }

        void EnsureCapacity(uint32_t size);

    private:
        void _Grow(uint32_t cbGrowMin);

        std::unique_ptr<uint8_t[]> _pData;        // Our data
        uint32_t _iIndex;       // Current index into it.
        uint32_t _cbReserved;   // The size in bytes of pData
        uint32_t _cbSizeValid;  // Size that contains valid data.
        std::ios_base::iostate _state;
    };

    //
    // Class used to read/write data from a resource
    // It's a bit messy now with whom owns the internal byte stream. This needs some work.
    //
    class istream
    {
    public:
        istream(const uint8_t *pData, uint32_t cbSize);
        istream(const sci::istream &original, uint32_t absoluteOffset);

        istream(const sci::istream &original) = default;
        istream &operator=(const sci::istream &original) = default;

        uint32_t GetDataSize() const { return _cbSizeValid; }

        // New api
        bool good() { return (_state & (std::ios_base::failbit | std::ios_base::eofbit)) == 0; }
        bool has_more_data() { return tellg() < GetDataSize(); }
        istream &operator>> (uint16_t &w);
        istream &operator>> (uint8_t &b);
        istream &operator>> (std::string &str);
        void getRLE(std::string &str);
        void read_data(uint8_t *pBuffer, uint32_t cbBytes)
        {
            if (!_Read(pBuffer, cbBytes))
            {
                _state = std::ios_base::eofbit | std::ios_base::failbit;
            }
        }
        template<class _T>
        istream &operator>> (typename _T &t)
        {
            uint32_t dwSave = tellg();
            if (!_Read(reinterpret_cast<uint8_t*>(&t), sizeof(t)))
            {
                seekg(dwSave);
                memset(&t, 0, sizeof(t));
                _state = std::ios_base::eofbit | std::ios_base::failbit;
            }
            return *this;
        }
        void seekg(uint32_t dwIndex);
        void seekg(int32_t offset, std::ios_base::seekdir way);
        uint32_t tellg() { return _iIndex; }
        uint32_t getBytesRemaining() { return (_cbSizeValid > _iIndex) ? (_cbSizeValid - _iIndex) : 0; }
        void skip(uint32_t cBytes);
        bool peek(uint8_t &b);
        bool peek(uint16_t &w);
        uint8_t peek();
        void setThrowExceptions(bool shouldThrow) { _throwExceptions = shouldThrow; }

        // Use with caution!
        const uint8_t *GetInternalPointer() const { return _pDataReadOnly; }

    private:
        bool _ReadWord(uint16_t *pw) { return _Read((uint8_t*)pw, 2); }
        bool _Read(uint8_t *pData, uint32_t cCount);
        bool _ReadByte(uint8_t *pb) { return _Read(pb, 1); }
        void _OnReadPastEnd();

        uint32_t _iIndex;
        uint32_t _cbSizeValid;
        const uint8_t *_pDataReadOnly;
        bool _throwExceptions;
        std::ios_base::iostate _state;
    };

    istream istream_from_ostream(ostream &src);

    class streamOwner
    {
    public:
        streamOwner(const uint8_t *data, uint32_t size);
        streamOwner(HANDLE hFile, DWORD lengthToInclude = 0);
        istream getReader();

    private:
        std::unique_ptr<uint8_t[]> _pData;        // Our data
        uint32_t _cbSizeValid;
    };

    void transfer(istream from, ostream &to, uint32_t count);
}
