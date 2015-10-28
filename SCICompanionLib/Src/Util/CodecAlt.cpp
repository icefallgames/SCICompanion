#include "stdafx.h"
#include "CodecAlt.h"
#include "AppState.h"
#include "PicCommands.h"

#define VIEW_HEADER_COLORS_8BIT 0x80

WORD READ_LE_UINT16(const byte *src)
{
    return *((const __unaligned WORD *)(src));
}

void WRITE_LE_UINT16(byte *dest, int value)
{
    WORD wValue = value;
    *((__unaligned WORD *)(dest)) = wValue;
}

void WRITE_LE_UINT16(BoundsCheckedArray<byte> dest, int value)
{
    WORD wValue = value;
    dest[0] = value & 0xff;
    dest[1] = value >> 8;
}

void WRITE_LE_UINT32(byte *dest, int value)
{
    DWORD dwValue = value;
    *((__unaligned DWORD *)(dest)) = dwValue;
}

void WRITE_LE_UINT32(BoundsCheckedArray<byte> destSafe, int value)
{
    byte *dest = destSafe.GetRawDataEnsureSpace(sizeof(DWORD));
    DWORD dwValue = value;
    *((__unaligned DWORD *)(dest)) = dwValue;
}


void decodeRLE(byte **rledata, byte **pixeldata, byte *outbuffer, int size) {
    int pos = 0;
    byte nextbyte;
    byte *rd = *rledata;
    byte *ob = outbuffer;
    byte *pd = *pixeldata;

    while (pos < size) {
        nextbyte = *rd++;
        *ob++ = nextbyte;
        pos++;
        switch (nextbyte & 0xC0) {
        case 0x40:
        case 0x00:
            memcpy(ob, pd, nextbyte);
            pd += nextbyte;
            ob += nextbyte;
            pos += nextbyte;
            break;
        case 0xC0:
            break;
        case 0x80:
            nextbyte = *pd++;
            *ob++ = nextbyte;
            pos++;
            break;
        }
    }

    *rledata = rd;
    *pixeldata = pd;
}


void decodeRLE(byte **rledata, byte **pixeldata, BoundsCheckedArray<byte> outbuffer, int size) {
    int pos = 0;
    byte nextbyte;
    byte *rd = *rledata;
    BoundsCheckedArray<byte> ob = outbuffer;
    byte *pd = *pixeldata;

    while (pos < size) {
        nextbyte = *rd++;
        *ob++ = nextbyte;
        pos++;
        switch (nextbyte & 0xC0) {
        case 0x40:
        case 0x00:
            memcpy(ob.GetRawDataEnsureSpace(nextbyte), pd, nextbyte);
            pd += nextbyte;
            ob += nextbyte;
            pos += nextbyte;
            break;
        case 0xC0:
            break;
        case 0x80:
            nextbyte = *pd++;
            *ob++ = nextbyte;
            pos++;
            break;
        }
    }

    *rledata = rd;
    *pixeldata = pd;
}

/**
* Does the same this as decodeRLE, only to determine the length of the
* compressed source data.
*/
int getRLEsize(byte *rledata, int dsize) {
    int pos = 0;
    byte nextbyte;
    int size = 0;

    while (pos < dsize) {
        nextbyte = *(rledata++);
        pos++;
        size++;

        switch (nextbyte & 0xC0) {
        case 0x40:
        case 0x00:
            pos += nextbyte;
            break;
        case 0xC0:
            break;
        case 0x80:
            pos++;
            break;
        }
    }

    return size;
}

void buildCelHeaders(byte **seeker, BoundsCheckedArray<BYTE> *writer, int celindex, int *cc_lengths, int max) {
    for (int c = 0; c < max; c++) {
        memcpy((*writer).GetRawDataEnsureSpace(6), *seeker, 6);
        *seeker += 6;
        *writer += 6;
        int w = *((*seeker)++);
        WRITE_LE_UINT16(*writer, w); /* Zero extension */
        *writer += 2;

        *writer += cc_lengths[celindex];
        celindex++;
    }
}

void reorderView(byte *src, BoundsCheckedArray<BYTE> dest) {
    byte *cellengths;
    int loopheaders;
    int lh_present;
    int lh_mask;
    int pal_offset;
    int cel_total;
    int unknown;
    byte *seeker = src;
    char celcounts[100];
    BoundsCheckedArray<BYTE> writer = dest;
    byte *rle_ptr;
    int l, lb, c, celindex, lh_last = -1;
    int chptr;
    int w;
    int *cc_lengths;
    std::unique_ptr<BoundsCheckedArray<byte>[]> cc_pos;

    /* Parse the main header */
    cellengths = src + READ_LE_UINT16(seeker) + 2;
    seeker += 2;
    loopheaders = *seeker++;
    lh_present = *seeker++;
    lh_mask = READ_LE_UINT16(seeker);
    seeker += 2;
    unknown = READ_LE_UINT16(seeker);
    seeker += 2;
    pal_offset = READ_LE_UINT16(seeker);
    seeker += 2;
    cel_total = READ_LE_UINT16(seeker);
    seeker += 2;

    cc_pos = std::make_unique<BoundsCheckedArray<byte>[]>(cel_total);


    cc_lengths = (int *)malloc(sizeof(int) * cel_total);

    for (c = 0; c < cel_total; c++)
        cc_lengths[c] = READ_LE_UINT16(cellengths + 2 * c);

    *writer++ = loopheaders;
    *writer++ = VIEW_HEADER_COLORS_8BIT;
    WRITE_LE_UINT16(writer, lh_mask);
    writer += 2;
    WRITE_LE_UINT16(writer, unknown);
    writer += 2;
    WRITE_LE_UINT16(writer, pal_offset);
    writer += 2;

    BoundsCheckedArray<BYTE> lh_ptr = writer;
    writer += 2 * loopheaders; /* Make room for the loop offset table */

    byte *pix_ptr;

    memcpy(celcounts, seeker, lh_present);
    seeker += lh_present;

    lb = 1;
    celindex = 0;

    rle_ptr = pix_ptr = cellengths + (2 * cel_total);
    w = 0;

    for (l = 0; l < loopheaders; l++) {
        if (lh_mask & lb) { /* The loop is _not_ present */
            if (lh_last == -1) {
                appState->LogInfo("Error: While reordering view: Loop not present, but can't re-use last loop");
                lh_last = 0;
            }
            WRITE_LE_UINT16(lh_ptr, lh_last);
            lh_ptr += 2;
        }
        else {
            lh_last = writer - dest;
            WRITE_LE_UINT16(lh_ptr, lh_last);
            lh_ptr += 2;
            WRITE_LE_UINT16(writer, celcounts[w]);
            writer += 2;
            WRITE_LE_UINT16(writer, 0);
            writer += 2;

            /* Now, build the cel offset table */
            chptr = (writer - dest) + (2 * celcounts[w]);

            for (c = 0; c < celcounts[w]; c++) {
                WRITE_LE_UINT16(writer, chptr);
                writer += 2;
                cc_pos[celindex + c] = dest + chptr;
                chptr += 8 + READ_LE_UINT16(cellengths + 2 * (celindex + c));
            }

            buildCelHeaders(&seeker, &writer, celindex, cc_lengths, celcounts[w]);

            celindex += celcounts[w];
            w++;
        }

        lb = lb << 1;
    }

    if (celindex < cel_total) {
        appState->LogInfo("View decompression generated too few (%d / %d) headers", celindex, cel_total);
        free(cc_lengths);
        return;
    }

    /* Figure out where the pixel data begins. */
    for (c = 0; c < cel_total; c++)
        pix_ptr += getRLEsize(pix_ptr, cc_lengths[c]);

    rle_ptr = cellengths + (2 * cel_total);
    for (c = 0; c < cel_total; c++)
        decodeRLE(&rle_ptr, &pix_ptr, cc_pos[c] + 8, cc_lengths[c]);

    if (pal_offset) {
        *writer++ = 'P';
        *writer++ = 'A';
        *writer++ = 'L';

        for (c = 0; c < 256; c++)
            *writer++ = c;

        seeker -= 4; /* The missing four. Don't ask why. */
        int space = 4 * 256 + 4;
        memcpy(writer.GetRawDataEnsureSpace(space), seeker, space);
    }

    free(cc_lengths);
}

#define PAL_SIZE 1284
#define EXTRA_MAGIC_SIZE 15

void reorderPic(byte *src, byte *destRaw, int dsize) {

    BoundsCheckedArray<byte> dest(destRaw, dsize);

    uint16_t view_size, view_start, cdata_size;
    int i;
    byte *seeker = src;
    BoundsCheckedArray<byte> writer = dest;
    char viewdata[7];
    byte *cdata, *cdata_start;

    *writer++ = PIC_OP_OPX;
    *writer++ = PIC_OPXSC1_SET_PALETTE;

    for (i = 0; i < 256; i++) /* Palette translation map */
        *writer++ = i;

    WRITE_LE_UINT32(writer, 0); /* Palette stamp */
    writer += 4;

    view_size = READ_LE_UINT16(seeker);
    seeker += 2;
    view_start = READ_LE_UINT16(seeker);
    seeker += 2;
    cdata_size = READ_LE_UINT16(seeker);
    seeker += 2;

    memcpy(viewdata, seeker, sizeof(viewdata));
    seeker += sizeof(viewdata);

    int sizePalette = 4 * 256;
    memcpy(writer.GetRawDataEnsureSpace(sizePalette), seeker, sizePalette);
    seeker += 4 * 256;
    writer += 4 * 256;

    if (view_start != PAL_SIZE + 2) { /* +2 for the opcode */
        memcpy(writer.GetRawDataEnsureSpace(view_start - PAL_SIZE - 2), seeker, view_start - PAL_SIZE - 2);
        seeker += view_start - PAL_SIZE - 2;
        writer += view_start - PAL_SIZE - 2;
    }

    if (dsize != view_start + EXTRA_MAGIC_SIZE + view_size) {
        memcpy((dest + view_size + view_start + EXTRA_MAGIC_SIZE).GetRawDataEnsureSpace(dsize - view_size - view_start - EXTRA_MAGIC_SIZE), seeker,
            dsize - view_size - view_start - EXTRA_MAGIC_SIZE);
        seeker += dsize - view_size - view_start - EXTRA_MAGIC_SIZE;
    }

    cdata_start = cdata = (byte *)malloc(cdata_size);
    memcpy(cdata, seeker, cdata_size);
    seeker += cdata_size;

    writer = dest + view_start;
    *writer++ = PIC_OP_OPX;
    *writer++ = PIC_OPXSC1_DRAW_BITMAP;
    *writer++ = 0;
    *writer++ = 0;
    *writer++ = 0;
    WRITE_LE_UINT16(writer, view_size + 8);
    writer += 2;

    memcpy(writer.GetRawDataEnsureSpace(sizeof(viewdata)), viewdata, sizeof(viewdata));
    writer += sizeof(viewdata);

    *writer++ = 0;

    decodeRLE(&seeker, &cdata, writer, view_size);

    free(cdata_start);
}