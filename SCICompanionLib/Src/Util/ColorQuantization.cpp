#include "stdafx.h"

// From http://rosettacode.org/wiki/Color_quantization/C

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
//#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

typedef struct {
    int w, h;
    unsigned char *pix;
} image_t, *image;


image img_new(int w, int h)
{
    image im = (image)malloc(sizeof(image_t) + h * w * 3);
    im->w = w; im->h = h;
    im->pix = (unsigned char *)(im + 1);
    return im;
}

#define ON_INHEAP	1

typedef struct oct_node_t oct_node_t, *oct_node;
struct oct_node_t{
    int64_t r, g, b; /* sum of all child node colors */
    int count, heap_idx;
    unsigned char n_kids, kid_idx, flags, depth;
    oct_node kids[8], parent;
};

typedef struct {
    int alloc, n;
    oct_node* buf;
} node_heap;

inline int cmp_node(oct_node a, oct_node b)
{
    if (a->n_kids < b->n_kids) return -1;
    if (a->n_kids > b->n_kids) return 1;

    int ac = a->count >> a->depth;
    int bc = b->count >> b->depth;
    return ac < bc ? -1 : ac > bc;
}

void down_heap(node_heap *h, oct_node p)
{
    int n = p->heap_idx, m;
    while (1) {
        m = n * 2;
        if (m >= h->n) break;
        if (m + 1 < h->n && cmp_node(h->buf[m], h->buf[m + 1]) > 0) m++;

        if (cmp_node(p, h->buf[m]) <= 0) break;

        h->buf[n] = h->buf[m];
        h->buf[n]->heap_idx = n;
        n = m;
    }
    h->buf[n] = p;
    p->heap_idx = n;
}

void up_heap(node_heap *h, oct_node p)
{
    int n = p->heap_idx;
    oct_node prev;

    while (n > 1) {
        prev = h->buf[n / 2];
        if (cmp_node(p, prev) >= 0) break;

        h->buf[n] = prev;
        prev->heap_idx = n;
        n /= 2;
    }
    h->buf[n] = p;
    p->heap_idx = n;
}

void heap_add(node_heap *h, oct_node p)
{
    if ((p->flags & ON_INHEAP)) {
        down_heap(h, p);
        up_heap(h, p);
        return;
    }

    p->flags |= ON_INHEAP;
    if (!h->n) h->n = 1;
    if (h->n >= h->alloc) {
        while (h->n >= h->alloc) h->alloc += 1024;
        h->buf = (oct_node*)realloc(h->buf, sizeof(oct_node) * h->alloc);
    }

    p->heap_idx = h->n;
    h->buf[h->n++] = p;
    up_heap(h, p);
}

oct_node pop_heap(node_heap *h)
{
    if (h->n <= 1) return 0;

    oct_node ret = h->buf[1];
    h->buf[1] = h->buf[--h->n];

    h->buf[h->n] = 0;

    h->buf[1]->heap_idx = 1;
    down_heap(h, h->buf[1]);

    return ret;
}

static oct_node pool = 0;
static int len = 0;
oct_node node_new(unsigned char idx, unsigned char depth, oct_node p)
{
    if (len <= 1) {
        oct_node p = (oct_node)calloc(sizeof(oct_node_t), 2048);
        p->parent = pool;
        pool = p;
        len = 2047;
    }

    oct_node x = pool + len--;
    x->kid_idx = idx;
    x->depth = depth;
    x->parent = p;
    if (p) p->n_kids++;
    return x;
}

void node_free()
{
    oct_node p;
    while (pool) {
        p = pool->parent;
        free(pool);
        pool = p;
    }
}

oct_node node_insert(oct_node root, unsigned char *pix)
{
    unsigned char i, bit, depth = 0;

    for (bit = 1 << 7; ++depth < 8; bit >>= 1) {
        i = !!(pix[1] & bit) * 4 + !!(pix[0] & bit) * 2 + !!(pix[2] & bit);
        if (!root->kids[i])
            root->kids[i] = node_new(i, depth, root);

        root = root->kids[i];
    }

    root->r += pix[0];
    root->g += pix[1];
    root->b += pix[2];
    root->count++;
    return root;
}

oct_node node_fold(oct_node p)
{
    if (p->n_kids) abort();
    oct_node q = p->parent;
    q->count += p->count;

    q->r += p->r;
    q->g += p->g;
    q->b += p->b;
    q->n_kids--;
    q->kids[p->kid_idx] = 0;
    return q;
}

void color_replace(oct_node root, unsigned char *pix, uint8_t *sciPix)
{
    unsigned char i, bit;

    for (bit = 1 << 7; bit; bit >>= 1) {
        i = !!(pix[1] & bit) * 4 + !!(pix[0] & bit) * 2 + !!(pix[2] & bit);
        if (!root->kids[i]) break;
        root = root->kids[i];
    }

    pix[0] = (unsigned char) root->r;
    pix[1] = (unsigned char)root->g;
    pix[2] = (unsigned char)root->b;

    assert(root->heap_idx <= 256);
    *sciPix = (uint8_t)(root->heap_idx - 1);
}

oct_node nearest_color(int *v, node_heap *h) {
    int i;
    int diff, max = 100000000;
    oct_node o = 0;
    for (i = 1; i < h->n; i++) {
        diff = (int)( 3 * abs(h->buf[i]->r - v[0])
            + 5 * abs(h->buf[i]->g - v[1])
            + 2 * abs(h->buf[i]->b - v[2]));
        if (diff < max) {
            max = diff;
            o = h->buf[i];
        }
    }
    return o;
}

#define POS(i, j) (3 * ((i) * im->w + (j)))
#define C10 7
#define C01 5
#define C11 2
#define C00 1 
#define CTOTAL (C00 + C11 + C10 + C01)
#define clamp(x, i) if (x[i] > 255) x[i] = 255; if (x[i] < 0) x[i] = 0


void error_diffuse(image im, node_heap *h)
{
    int i, j;
    int *npx = (int*)calloc(sizeof(int), im->h * im->w * 3), *px;
    int v[3];
    unsigned char *pix = im->pix;
    oct_node nd;

    for (px = npx, i = 0; i < im->h; i++) {
        for (j = 0; j < im->w; j++, pix += 3, px += 3) {
            px[0] = (int)pix[0] * CTOTAL;
            px[1] = (int)pix[1] * CTOTAL;
            px[2] = (int)pix[2] * CTOTAL;
        }
    }
    pix = im->pix;
    for (px = npx, i = 0; i < im->h; i++) {
        for (j = 0; j < im->w; j++, pix += 3, px += 3) {
            px[0] /= CTOTAL;
            px[1] /= CTOTAL;
            px[2] /= CTOTAL;
            clamp(px, 0); clamp(px, 1); clamp(px, 2);

            nd = nearest_color(px, h);

            v[0] = (int)(px[0] - nd->r);
            v[1] = (int)(px[1] - nd->g);
            v[2] = (int)(px[2] - nd->b);

            pix[0] = (unsigned char)nd->r;
            pix[1] = (unsigned char)nd->g;
            pix[2] = (unsigned char)nd->b;
            if (j < im->w - 1) {
                npx[POS(i, j + 1) + 0] += v[0] * C10;
                npx[POS(i, j + 1) + 1] += v[1] * C10;
                npx[POS(i, j + 1) + 2] += v[2] * C10;
            }
            if (i >= im->h - 1) continue;

            npx[POS(i + 1, j) + 0] += v[0] * C01;
            npx[POS(i + 1, j) + 1] += v[1] * C01;
            npx[POS(i + 1, j) + 2] += v[2] * C01;

            if (j < im->w - 1) {
                npx[POS(i + 1, j + 1) + 0] += v[0] * C11;
                npx[POS(i + 1, j + 1) + 1] += v[1] * C11;
                npx[POS(i + 1, j + 1) + 2] += v[2] * C11;
            }
            if (j) {
                npx[POS(i + 1, j - 1) + 0] += v[0] * C00;
                npx[POS(i + 1, j - 1) + 1] += v[1] * C00;
                npx[POS(i + 1, j - 1) + 2] += v[2] * C00;
            }
        }
    }
    free(npx);
}

void color_quant(image im, int n_colors, int dither, uint8_t *sciBits, RGBQUAD *tempPalette)
{
    int i;
    unsigned char *pix = im->pix;
    node_heap heap = { 0, 0, 0 };

    oct_node root = node_new(0, 0, 0), got;
    for (i = 0; i < im->w * im->h; i++, pix += 3)
        heap_add(&heap, node_insert(root, pix));

    while (heap.n > n_colors + 1)
        heap_add(&heap, node_fold(pop_heap(&heap)));

    double c;
    for (i = 1; i < heap.n; i++) {
        got = heap.buf[i];
        c = got->count;
        got->r = (int64_t)(got->r / c + .5);
        got->g = (int64_t)(got->g / c + .5);
        got->b = (int64_t)(got->b / c + .5);

        // Red and blue are inverted.
        tempPalette[i - 1].rgbRed = (BYTE)got->b;
        tempPalette[i - 1].rgbGreen = (BYTE)got->g;
        tempPalette[i - 1].rgbBlue = (BYTE)got->r;
    }

    if (dither) error_diffuse(im, &heap);
    else
        for (i = 0, pix = im->pix; i < im->w * im->h; i++, pix += 3, sciBits++)
            color_replace(root, pix, sciBits);

    node_free();
    free(heap.buf);

    // pfortier: added to work around crappy code:
    free(pool);
    len = 0;
    pool = nullptr;
}

// Returns a 24bit RGB bitmap from an input bitmap. *pDIBBits points to the resulting bits.
HBITMAP BitmapToRGB32Bitmap(const BITMAPINFO *pbmi, int desiredWidth, int desiredHeight, void **pDIBBits32)
{
    HBITMAP bmpReturn = nullptr;
    void *pDIBBits = _GetBitsPtrFromBITMAPINFO(pbmi);
    if (pDIBBits)
    {
        CDC dc;
        if (dc.CreateCompatibleDC(nullptr))
        {
            BITMAPINFO bitmapInfo;
            bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
            bitmapInfo.bmiHeader.biWidth = desiredWidth;
            bitmapInfo.bmiHeader.biHeight = desiredHeight;
            bitmapInfo.bmiHeader.biPlanes = 1;
            bitmapInfo.bmiHeader.biBitCount = 32;
            bitmapInfo.bmiHeader.biCompression = BI_RGB;
            bitmapInfo.bmiHeader.biSizeImage = 0; // 0 is ok for BI_RGB
            bitmapInfo.bmiHeader.biXPelsPerMeter = 0; // ???
            bitmapInfo.bmiHeader.biYPelsPerMeter = 0; // ??? 
            bitmapInfo.bmiHeader.biClrUsed = 0;         // ???
            bitmapInfo.bmiHeader.biClrImportant = 0;    // ???
            bmpReturn = CreateDIBSection((HDC)dc, &bitmapInfo, DIB_RGB_COLORS, pDIBBits32, nullptr, 0);
            if (bmpReturn)
            {
                // Select our new HBITMAP into the DC
                HGDIOBJ hgdiObjOld = SelectObject((HDC)dc, bmpReturn);

                StretchDIBits((HDC)dc,
                    0, 0, desiredWidth, desiredHeight,
                    0, 0, pbmi->bmiHeader.biWidth, pbmi->bmiHeader.biHeight,
                    pDIBBits, pbmi, DIB_RGB_COLORS, SRCCOPY);

                SelectObject((HDC)dc, hgdiObjOld);
            }
        }
    }

    return bmpReturn;
}

// globalPalette needs to have empty slots.
// data has no specific stride
// data's alpha should have been stripped (either on or off)
std::unique_ptr<uint8_t[]> QuantizeImage(const RGBQUAD *data, int width, int height, const RGBQUAD *globalPalette, RGBQUAD *imagePaletteResult, int transparentIndex, bool excludeTransparentColorFromPalette)
{
    RGBQUAD black = {};
    int outStride = CX_ACTUAL(width);

    // REVIEW: We will have to do this for views.
    size_t bitmapLength = width * height;
    std::unique_ptr<uint8_t[]> sciBits = std::make_unique<uint8_t[]>(bitmapLength);
    std::unique_ptr<bool[]> opaqueMask = std::make_unique<bool[]>(bitmapLength);

    std::vector<uint8_t> unusedIndices;

    // 1) count the number of colors we can use by looking at empty palette slots
    int colorCount = 0;
    for (int i = 0; i < 256; i++)
    {
        if (
            (!excludeTransparentColorFromPalette || (i != transparentIndex)) &&
            (!globalPalette || (globalPalette[i].rgbReserved == 0x0)))
        {
            colorCount++;
            unusedIndices.push_back((uint8_t)i);
        }
        else
        {
            // Copy it over but mark it as unused.
            imagePaletteResult[i] = globalPalette ? globalPalette[i] : black;
            imagePaletteResult[i].rgbReserved = 0x1;
        }
    }

    if (colorCount > 0)
    {
        // Now our pDIBBits32 should point to the raw bitmap data.
        image img = img_new(width, height);

        // Copy our data into img used by the algorithm, which is 3 bytes per pixel.
        for (int y = 0; y < height; y++)
        {
            unsigned char* dest = img->pix + (y * width * 3);
            for (int x = 0; x < width; x++)
            {
                const RGBQUAD *src = data + (y * width) + x;

                // The 4th source byte is alpha.
                bool isOpaque = (src->rgbReserved == 0xff);
                opaqueMask[y * width + x] = isOpaque;
                // Note: for transparent pixels, we'll still evaluate colors. Oh well. Hopefully they'll be all black.
                // Let's make them black
                // TODO/REVIEW: We should modify our quantization algorithm so it doesn't pay attention to the transparent colors. i.e. make image be one dimensional
                RGBQUAD toUse = *src;
                if (!isOpaque)
                {
                    toUse.rgbBlue = 0;
                    toUse.rgbGreen = 0;
                    toUse.rgbRed = 0;
                }

                *dest++ = toUse.rgbBlue;
                *dest++ = toUse.rgbGreen;
                *dest++ = toUse.rgbRed;
            }
        }

        RGBQUAD usedColors[256];
        color_quant(img, colorCount, 0, sciBits.get(), usedColors);

        // sciBits will now contain values from 0 to colorCount (exclusive), and usedColors will contain the RGB values for those indices.
        // unusedIndices contains the real palette indices where we want to put these things.
        for (int i = 0; i < (int)bitmapLength; i++)
        {
            if (opaqueMask[i])
            {
                sciBits[i] = unusedIndices[sciBits[i]];
            }
            else
            {
                sciBits[i] = (uint8_t)transparentIndex;
            }
        }
        for (int i = 0; i < colorCount; i++)
        {
            imagePaletteResult[unusedIndices[i]] = usedColors[i];
            imagePaletteResult[unusedIndices[i]].rgbReserved = 0x3;
        }

        free(img);
    }

    if (outStride != width)
    {
        // An extra copy when stride doesn't match width
        std::unique_ptr<uint8_t[]> sciBitsReturn = std::make_unique<uint8_t[]>(outStride * height);
        for (int y = 0; y < height; y++)
        {
            memcpy(sciBitsReturn.get() + y * outStride, sciBits.get() + y * width, width);
        }
        return sciBitsReturn;
    }
    else
    {
        return sciBits;
    }
}
