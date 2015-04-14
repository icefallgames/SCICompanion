#include "stdafx.h"
#include "Codec.h"
#include "AppState.h"

//
// The following decryption algorithms are from the freesci source code.
//

/*****************  Decryption Method 3  *******************************
* The following code was originally created by Carl Muckenhoupt for his
* SCI decoder. It has been ported to the FreeSCI environment by Sergey Lapin.
***************************************************************************/

/* TODO: Clean up, re-organize, improve speed-wise */
struct tokenlist {
    BYTE data;
    __int16 next;
} tokens[0x1004];

static __int8 stak[0x1014] = {0};
static __int8 lastchar = 0;
static __int16 stakptr = 0;
static WORD numbits, bitstring, lastbits, decryptstart;
static __int16 curtoken, endtoken;


DWORD gbits(int numbits,  BYTE * data, int dlen)
{
    int place; /* indicates location within byte */
    DWORD bitstring;
    static DWORD whichbit=0;
    int i;

    if(numbits==0) {whichbit=0; return 0;}

    place = whichbit >> 3;
    bitstring=0;
    for(i=(numbits>>3)+1;i>=0;i--)
        {
            if (i+place < dlen)
                bitstring |=data[place+i] << (8*(2-i));
        }
    /*  bitstring = data[place+2] | (long)(data[place+1])<<8
        | (long)(data[place])<<16;*/
    bitstring >>= 24-(whichbit & 7)-numbits;
    bitstring &= (0xffffffff >> (32-numbits));
    /* Okay, so this could be made faster with a table lookup.
       It doesn't matter. It's fast enough as it is. */
    whichbit += numbits;
    return bitstring;
}

void decryptinit3()
{
    int i;
    lastbits = bitstring = stakptr = 0;
    lastchar = 0;
    numbits = 9;
    curtoken = 0x102;
    endtoken = 0x1ff;
    decryptstart = 0;
    gbits(0,0,0);
    for(i=0;i<0x1004;i++) {
        tokens[i].next=0;
        tokens[i].data=0;
    }
}

int decryptComp3Helper(BoundsCheckedArray<BYTE> &dest, BYTE *src, int length, int complength, __int16 &token)
{
    //while(length != 0) {
    while(length >= 0) {
        switch (decryptstart) {
        case 0:
        case 1:
            //bitstring = gbits(numbits, src, complength);
            bitstring = (WORD)gbits(numbits, src, complength);
            if (bitstring == 0x101) { /* found end-of-data signal */
                decryptstart = 4;
                return 0;
            }
            if (decryptstart == 0) { /* first char */
                decryptstart = 1;
                lastbits = bitstring;
                *(dest++) = lastchar = (bitstring & 0xff);
                if (--length != 0) continue;
                return 0;
            }
            if (bitstring == 0x100) { /* start-over signal */
                numbits = 9;
                endtoken = 0x1ff;
                curtoken = 0x102;
                decryptstart = 0;
                continue;
            }
            token = bitstring;
            if (token >= curtoken) { /* index past current point */
                token = lastbits;
                if (stakptr >= ARRAYSIZE(stak))
                {
                    return -1;
                }
                stak[stakptr++] = lastchar;
            }
            while ((token > 0xff)&&(token < 0x1004)) { /* follow links back in data */
                if (stakptr >= ARRAYSIZE(stak))
                {
                    return -1;
                }
                stak[stakptr++] = tokens[token].data;
                token = tokens[token].next;
            }
            if (stakptr >= ARRAYSIZE(stak))
            {
                return -1;
            }
            lastchar = stak[stakptr++] = token & 0xff;
        case 2:
            while (stakptr > 0) { /* put stack in buffer */
                if (stakptr >= ARRAYSIZE(stak))
                {
                    return -1;
                }
                *(dest++) = stak[--stakptr];
                length--;
                if (length == 0) {
                    decryptstart = 2;
                    return 0;
                }
            }
            decryptstart = 1;
            if (curtoken <= endtoken) { /* put token into record */
                tokens[curtoken].data = lastchar;
                tokens[curtoken].next = lastbits;
                curtoken++;
                if (curtoken == endtoken && numbits != 12) {
                    numbits++;
                    endtoken <<= 1;
                    endtoken++;
                }
            }
            lastbits = bitstring;
            continue; /* When are "break" and "continue" synonymous? */
        case 4:
            return 0;
        }
    }
    return 0;     /* [DJ] shut up compiler warning */    
}


int decompressLZW_1(BYTE *dest, BYTE *src, int length, int complength)
{
    decryptinit3();

    static __int16 token;
    BoundsCheckedArray<BYTE> bcaDest(dest, length);
    return decryptComp3Helper(bcaDest, src, length, complength, token);
}





/* 9-12 bit LZW encoding */
int decompressLZW(BYTE *dest, BYTE *src, int length, int complength)
     /* Doesn't do length checking yet */
{
    /* Theory: Considering the input as a bit stream, we get a series of
    ** 9 bit elements in the beginning. Every one of them is a 'token'
    ** and either represents a literal (if < 0x100), or a link to a previous
    ** token (tokens start at 0x102, because 0x101 is the end-of-stream
    ** indicator and 0x100 is used to reset the bit stream decoder).
    ** If it's a link, the indicated token and the character following it are
    ** placed into the output stream. Note that the 'indicated token' may
    ** very well consist of a link-token-plus-literal construct again, so
    ** it's possible to represent strings longer than 2 recursively.
    ** If the maximum number of tokens has been reached, the bit length is
    ** increased by one, up to a maximum of 12 bits.
    ** This implementation remembers the position each token was print to in
    ** the output array, and the length of this token. This method should
    ** be faster than the recursive approach.
    */

    WORD bitlen = 9; /* no. of bits to read (max. 12) */
    WORD bitmask = 0x01ff;
    WORD bitctr = 0; /* current bit position */
    WORD bytectr = 0; /* current byte position */
    WORD token; /* The last received value */
    WORD maxtoken = 0x200; /* The biggest token */

    WORD tokenlist[4096]; /* pointers to dest[] */
    WORD tokenlengthlist[4096]; /* char length of each token */
    WORD tokenctr = 0x102; /* no. of registered tokens (starts here)*/

    WORD tokenlastlength = 0;

    WORD destctr = 0;

    while (bytectr < complength) {

        DWORD tokenmaker = src[bytectr++] >> bitctr;
        if (bytectr < complength)
            tokenmaker |= (src[bytectr] << (8-bitctr));
        if (bytectr+1 < complength)
            tokenmaker |= (src[bytectr+1] << (16-bitctr));

        token = (WORD)(tokenmaker & bitmask);

        bitctr += bitlen - 8;

        while (bitctr >= 8) {
            bitctr -= 8;
            bytectr++;
        }

        if (token == 0x101) return 0; /* terminator */
        if (token == 0x100) { /* reset command */
            maxtoken = 0x200;
            bitlen = 9;
            bitmask = 0x01ff;
            tokenctr = 0x0102;
        } else {

            {
                int i;

                if (token > 0xff) {
                  if (token >= tokenctr)
                    {
#ifdef _SCI_DECOMPRESS_DEBUG
                      fprintf(stderr, "decompressLZW: Bad token %x!\n", token);
#endif
                      /* Well this is really bad  */
                      /* May be it should throw something like SCI_ERROR_DECOMPRESSION_INSANE */
                    } else
                      {
                    tokenlastlength = tokenlengthlist[token]+1;
                    if (destctr+tokenlastlength>length)
                      {
#ifdef _SCI_DECOMPRESS_DEBUG

                        /* For me this seems a normal situation, It's necessary to handle it*/
                        printf ("decompressLZW: Trying to write beyond the end of array(len=%d, destctr=%d, tok_len=%d)!\n",
                            length, destctr, tokenlastlength);
#endif

                        i = 0;
                        for (; destctr<length; destctr++) {
                          dest[destctr++] = dest [tokenlist[token]+i];
                          i++;
                        }
                      } else
                    for (i=0; i< tokenlastlength; i++) {
                        dest[destctr++] = dest[tokenlist[token]+i];
                    }
                      }
                } else {
                    tokenlastlength = 1;
                  if (destctr >= length)
                    {
#ifdef _SCI_DECOMPRESS_DEBUG
                      printf ("decompressLZW: Try to write single byte beyond end of array!\n");
#endif
                    } else
                    dest[destctr++] = (byte)token;
                }

            }

            if (tokenctr == maxtoken) {
                if (bitlen < 12) {
                    bitlen++;
                    bitmask <<= 1;
                    bitmask |= 1;
                    maxtoken <<= 1;
                } else continue; /* no further tokens allowed */
            }

            tokenlist[tokenctr] = destctr-tokenlastlength;
            tokenlengthlist[tokenctr++] = tokenlastlength;

        }

    }

    return 0;

}









/* Huffman-style token encoding */
/***************************************************************************/
/* This code was taken from Carl Muckenhoupt's sde.c, with some minor      */
/* modifications.                                                          */
/***************************************************************************/

static inline __int16
getInt16(BoundsCheckedArray<BYTE> &d)
{
    return (__int16)(d[0] | (d[1] << 8));
}


/* decompressHuffman helper function */
__int16 getc2(BoundsCheckedArray<BYTE> node, BYTE *src,
         WORD *bytectr, WORD *bitctr, int complength)
{
    WORD next;

    while (node[1] != 0) {
        __int16 value = (src[*bytectr] << (*bitctr));
        (*bitctr)++;
        if (*bitctr == 8) {
            (*bitctr) = 0;
            (*bytectr)++;
        }

        if (value & 0x80) {
            next = node[1] & 0x0f; /* low 4 bits */
            if (next == 0) {
                WORD result = (src[*bytectr] << (*bitctr));

                if (++(*bytectr) > complength)
                    return -1;
                else if (*bytectr < complength)
                    result |= src[*bytectr] >> (8-(*bitctr));

                result &= 0x0ff;
                return (result | 0x100);
            }
        }
        else {
            next = node[1] >> 4;  /* high 4 bits */
        }
        node += next<<1;
    }
    return getInt16(node);
}

/* Huffman token decryptor */
int decompressHuffman(BYTE* dest, BYTE* src, int length, int complength)
     /* no complength checking atm */
{
    BYTE numnodes, terminator;
    __int16 c;
    WORD bitctr = 0, bytectr;

    numnodes = src[0];
    terminator = src[1];
    bytectr = 2+ (numnodes << 1);

    try
    {
        //BYTE *nodes = src + 2;
        BoundsCheckedArray<BYTE> nodesBoundsChecked(src + 2, complength - 2);

        while (((c = getc2(nodesBoundsChecked, src, &bytectr, &bitctr, complength))
            != (0x0100 | terminator)) && (c >= 0)) {
            if (length-- == 0)
            {
                return SCI_ERROR_DECOMPRESSION_OVERFLOW;
            }

            *dest = (BYTE)c;
            dest++;
        }
    }
    catch (std::exception &e)
    {
        appState->LogInfo("Overflow while decompressing: %s", e.what());
        c = -1; // To indicate error.
    }

    return (c == -1) ? SCI_ERROR_DECOMPRESSION_OVERFLOW : 0;
}
