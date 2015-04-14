#pragma once

//
// CExtNoFlickerStatic
// Helper class for drawing images.  The regular CStatic's SetBitmap
// behaves differently on different OS's, and is prone to leaks as a result.
// NOTE: We may not need this anymore... it's possible prof-uis's CExtLabel would suffice?
//
class CExtNoFlickerStatic : public CExtLabel
{
public:
    CExtNoFlickerStatic() {}
    virtual ~CExtNoFlickerStatic() {}
    // This does *not* take ownership of the HBITMAP
    bool FromBitmap(HBITMAP hbmp, int cx, int cy);

protected:
	DECLARE_MESSAGE_MAP()
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    CExtBitmap _bitmap;
    int _cx;
    int _cy;
};

