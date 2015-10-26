
#pragma once

enum class DecompressionAlgorithm
{
    None = 0,
    Unknown,
    LZW,
    Huffman,
    LZW1,
    LZW_View,
    LZW_Pic,
    DCL,
    STACpack
};

int decompressHuffman(BYTE* dest, BYTE* src, int length, int complength);
int decompressLZW_1(BYTE *dest, BYTE *src, int length, int complength);
int decompressLZW(BYTE *dest, BYTE *src, int length, int complength);
bool decompressDCL(byte *dest, byte *src, uint32_t unpackedSize, uint32_t packedSize);
bool decompressLZS(byte *dest, byte *src, uint32_t unpackedSize, uint32_t packedSize);
int decrypt4(byte* dest, byte* src, int length, int complength);

/*** INITIALIZATION RESULT TYPES ***/
#define SCI_ERROR_IO_ERROR 1
#define SCI_ERROR_EMPTY_OBJECT 2
#define SCI_ERROR_INVALID_RESMAP_ENTRY 3
#define SCI_ERROR_NO_RESOURCE_FILES_FOUND 5
/* No resource at all was found */
#define SCI_ERROR_UNKNOWN_COMPRESSION 6
#define SCI_ERROR_DECOMPRESSION_OVERFLOW 7
/* decompression failed: Buffer overflow (wrong SCI version?)  */
#define SCI_ERROR_DECOMPRESSION_INSANE 8
/* sanity checks failed during decompression */
#define SCI_ERROR_RESOURCE_TOO_BIG 9
/* Resource size exceeds SCI_MAX_RESOURCE_SIZE */
#define SCI_ERROR_UNSUPPORTED_VERSION 10
#define SCI_ERROR_INVALID_SCRIPT_VERSION 11

// Simple bounds checker for a blob of data, which we can easily plop into
// these "old" decompression routines to avoid buffer overflows.
template<typename _T>
class BoundsCheckedArray
{
public:
    BoundsCheckedArray() : BoundsCheckedArray(nullptr, 0) {}
    BoundsCheckedArray(_T *data, int length) : _data(data), _length(length) {}

    BoundsCheckedArray(const BoundsCheckedArray<_T> &src) = default;
    BoundsCheckedArray<_T> &operator=(const BoundsCheckedArray<_T> &src) = default;

    _T &operator[](int index)
    {
        if ((index < 0) || (index >= _length))
        {
            throw std::exception("Array out of bounds.");
        }
        return _data[index];
    }

    BoundsCheckedArray<_T>& operator+=(int amount)
    {
        _data += amount;
        _length -= amount;
        return *this;
    }

    // Prefix
    BoundsCheckedArray<_T>& operator++()
    {
        (*this) += 1;
        return *this;
    }

    // Postfix
    BoundsCheckedArray<_T> operator++(int unused)
    {
        BoundsCheckedArray<_T> result(*this);
        ++(*this);
        return result;
    }

    _T &operator*()
    {
        if (_length < 1)
        {
            throw std::exception("Dereferencing past end of data.");
        }
        return *_data;
    }

    int operator-(const BoundsCheckedArray<_T> &rhs) const
    {
        return _data - rhs._data;
    }

    BoundsCheckedArray<_T> operator+ (int increment) const
    {
        return BoundsCheckedArray<_T>(_data + increment, _length - increment);
    }


    _T *GetRawDataEnsureSpace(int requiredLength)
    {
        if (_length < requiredLength)
        {
            throw std::exception("Insufficient space in array.");
        }
        return _data;
    }

private:
    _T *_data;
    int _length;
};