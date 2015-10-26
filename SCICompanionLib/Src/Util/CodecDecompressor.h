#pragma once

class ReadStream
{
public:
    ReadStream(byte *data) : _data(data), _index(0) {}

    byte readByte()
    {
        return _data[_index++];
    }

private:
    int _index;
    byte *_data;
};

class Decompressor
{
public:
    virtual ~Decompressor() {};

protected:
    /**
    * Initialize decompressor.
    * @param src		source stream to read from
    * @param dest		destination stream to write to
    * @param nPacked	size of packed data
    * @param nUnpacked	size of unpacked data
    */
    void init(ReadStream *src, byte *dest, uint32_t nPacked, uint32_t nUnpacked);

    /**
    * Get a number of bits from _src stream, starting with the least
    * significant unread bit of the current four byte block.
    * @param n		number of bits to get
    * @return n-bits number
    */
    uint32_t getBitsLSB(int n);

    /**
    * Get a number of bits from _src stream, starting with the most
    * significant unread bit of the current four byte block.
    * @param n		number of bits to get
    * @return n-bits number
    */
    uint32_t getBitsMSB(int n);

    /**
    * Get one byte from _src stream.
    * @return byte
    */
    byte getByteLSB();

    byte getByteMSB();

    void fetchBitsLSB();
    void fetchBitsMSB();

    /**
    * Write one byte into _dest stream
    * @param b byte to put
    */
    void putByte(byte b);

    bool isFinished();

    uint32_t _dwBits;		///< bits buffer
    byte _nBits;		///< number of unread bits in _dwBits
    uint32_t _szPacked;	///< size of the compressed data
    uint32_t _szUnpacked;	///< size of the decompressed data
    uint32_t _dwRead;		///< number of bytes read from _src
    uint32_t _dwWrote;	///< number of bytes written to _dest
    ReadStream *_src;
    byte *_dest;
};
