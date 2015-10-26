#include "stdafx.h"
#include "Codec.h"
#include "CodecDecompressor.h"
#include "AppState.h"

// Based on ScummVM, which is originally based on Andre Beck's code from http://micky.ibh.de/~beck/stuff/lzs4i4l/

/**
* STACpack decompressor for SCI32
*/
class DecompressorLZS : public Decompressor {
public:
    bool unpack(ReadStream *src, byte *dest, uint32_t nPacked, uint32_t nUnpacked);

protected:
    bool unpackLZS();
    uint32_t getCompLen();
    void copyComp(int offs, uint32_t clen);
};

bool DecompressorLZS::unpack(ReadStream *src, byte *dest, uint32_t nPacked, uint32_t nUnpacked) {
    init(src, dest, nPacked, nUnpacked);
    return unpackLZS();
}

bool DecompressorLZS::unpackLZS() {
    uint16_t offs = 0;
    uint32_t clen;

    while (!isFinished()) {
        if (getBitsMSB(1)) { // Compressed bytes follow
            if (getBitsMSB(1)) { // Seven bit offset follows
                offs = getBitsMSB(7);
                if (!offs) // This is the end marker - a 7 bit offset of zero
                    break;
                if (!(clen = getCompLen())) {
                    appState->LogInfo("lzsDecomp: length mismatch");
                    return false;
                }
                copyComp(offs, clen);
            }
            else { // Eleven bit offset follows
                offs = getBitsMSB(11);
                if (!(clen = getCompLen())) {
                    appState->LogInfo("lzsDecomp: length mismatch");
                    return false;
                }
                copyComp(offs, clen);
            }
        }
        else // Literal byte follows
            putByte(getByteMSB());
    } // end of while ()
    return _dwWrote == _szUnpacked;
}

uint32_t DecompressorLZS::getCompLen() {
    uint32_t clen;
    int nibble;
    // The most probable cases are hardcoded
    switch (getBitsMSB(2)) {
        case 0:
            return 2;
        case 1:
            return 3;
        case 2:
            return 4;
        default:
            switch (getBitsMSB(2)) {
                case 0:
                    return 5;
                case 1:
                    return 6;
                case 2:
                    return 7;
                default:
                    // Ok, no shortcuts anymore - just get nibbles and add up
                    clen = 8;
                    do {
                        nibble = getBitsMSB(4);
                        clen += nibble;
                    } while (nibble == 0xf);
                    return clen;
            }
    }
}

void DecompressorLZS::copyComp(int offs, uint32_t clen) {
    int hpos = _dwWrote - offs;

    while (clen--)
        putByte(_dest[hpos++]);
}

bool decompressLZS(byte *dest, byte *src, uint32_t unpackedSize, uint32_t packedSize)
{
    ReadStream readStream(src);
    DecompressorLZS stac;
    return stac.unpack(&readStream, dest, packedSize, unpackedSize);
}
