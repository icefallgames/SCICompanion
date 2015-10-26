#include "stdafx.h"
#include "CodecDecompressor.h"

void Decompressor::init(ReadStream *src, byte *dest, uint32_t nPacked, uint32_t nUnpacked) {
    _src = src;
    _dest = dest;
    _szPacked = nPacked;
    _szUnpacked = nUnpacked;
    _nBits = 0;
    _dwRead = _dwWrote = 0;
    _dwBits = 0;
}

bool Decompressor::isFinished()
{
    return (_dwWrote == _szUnpacked) && (_dwRead >= _szPacked);
}

void Decompressor::fetchBitsLSB() {
    while (_nBits <= 24) {
        _dwBits |= ((uint32_t)_src->readByte()) << _nBits;
        _nBits += 8;
        _dwRead++;
    }
}

uint32_t Decompressor::getBitsLSB(int n) {
    // fetching more data to buffer if needed
    if (_nBits < n)
        fetchBitsLSB();
    uint32_t ret = (_dwBits & ~((~0) << n));
    _dwBits >>= n;
    _nBits -= n;
    return ret;
}

void Decompressor::fetchBitsMSB() {
    while (_nBits <= 24) {
        _dwBits |= ((uint32_t)_src->readByte()) << (24 - _nBits);
        _nBits += 8;
        _dwRead++;
    }
}

uint32_t Decompressor::getBitsMSB(int n) {
    // fetching more data to buffer if needed
    if (_nBits < n)
        fetchBitsMSB();
    uint32_t ret = _dwBits >> (32 - n);
    _dwBits <<= n;
    _nBits -= n;
    return ret;
}

byte Decompressor::getByteLSB() {
    return getBitsLSB(8);
}

void Decompressor::putByte(byte b) {
    _dest[_dwWrote++] = b;
}
byte Decompressor::getByteMSB() {
    return getBitsMSB(8);
}

