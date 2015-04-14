
#include "stdafx.h"
#include "DrawHelper.h"
#include "PaletteOperations.h"

CSCIDrawHelper::CSCIDrawHelper(CDC *pdc, BYTE *pData, const CSize &size, const RGBQUAD *pPalette, int count)
{
    _pBits = NULL;
    _hDib = NULL;
    _pData = NULL;
    _size = size;
    _hgdiObjOld = NULL;
    if (dc.CreateCompatibleDC(pdc))
    {
        SCIBitmapInfo bmi(size.cx, size.cy, pPalette, count);
        _hDib = CreateDIBSection((HDC)dc, &bmi, DIB_RGB_COLORS, &_pBits, NULL, 0);
        if (_hDib)
        {
            // Select our new HBITMAP into the DC
            _hgdiObjOld = SelectObject((HDC)dc, _hDib);

            // Copy memory from the view data into our temporary bits
            _pData = pData;
            CopyMemory(_pBits, _pData, CX_ACTUAL(size.cx) * size.cy);
        }
    }
}

CSCIDrawHelper::~CSCIDrawHelper()
{
    // Select back the original object.
    dc.SelectObject(_hgdiObjOld);

    // Copy memory back into the view data.
    if (_pData && _pBits)
    {
        CopyMemory(_pData, _pBits, CX_ACTUAL(_size.cx) * _size.cy);
    }

    // Finally delete our DIB
    if (_hDib)
    {
        DeleteObject(_hDib);
    }
}
