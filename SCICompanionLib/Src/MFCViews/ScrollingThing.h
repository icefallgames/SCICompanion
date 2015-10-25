/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
#pragma once


// CScrollingThing
//
// Template that implements scrolling functionality for a CWnd-derived class.
// The template parameter is the base class.
//
// 
//

#pragma once


// CScrollingThing
template <class T>
class CScrollingThing : public T
{
protected:
	CScrollingThing();           // protected constructor used by dynamic creation
	virtual ~CScrollingThing();

    void _InvalidateScrollStuff();
    void _ScrollDelta(int iType, HWND hwnd, int dpos);
    void _ScrollTo(int iType, HWND hwnd, int pos);
    void _OnScroll(int iType, UINT nSBCode, UINT nPos);

    // Overridables:
    virtual int _GetViewWidth() { return 0; }  // MFC won't let me make it virtual
    virtual int _GetViewHeight() { return 0; }  // MFC won't let me make it virtual
    virtual void _OnMouseWheel(UINT nFlags, BOOL fForward, CPoint pt, short nNotches) {};

public:
    virtual void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    virtual void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    virtual void OnSize(UINT nType, int cx, int cy);
    virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    virtual void OnDraw(CDC *pDC) { };  // MFC won't let me make it virtual

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
    int _xOrigin;
    int _yOrigin;

private:
    short _zDeltaMouseWheel;
};


template <class T>
CScrollingThing<T>::CScrollingThing()
{
    // Scrolling:
    _yOrigin = 0;
    _xOrigin = 0;
    _zDeltaMouseWheel = 0;
}

template <class T>
CScrollingThing<T>::~CScrollingThing()
{
}

BEGIN_TEMPLATE_MESSAGE_MAP(CScrollingThing, T, T)
    ON_WM_VSCROLL()
    ON_WM_HSCROLL()
    ON_WM_MOUSEWHEEL()
    ON_WM_SIZE()
END_MESSAGE_MAP()

// Scrolling
template <class T>
void CScrollingThing<T>::_ScrollDelta(int iType, HWND hwnd, int dpos)
{
    _ScrollTo(iType, hwnd, ((iType == SB_VERT) ? _yOrigin : _xOrigin) + dpos);
}
template <class T>
void CScrollingThing<T>::_InvalidateScrollStuff()
{
    _ScrollDelta(SB_HORZ, this->m_hWnd, 0);
    _ScrollDelta(SB_VERT, this->m_hWnd, 0);
}

template <class T>
void CScrollingThing<T>::_ScrollTo(int iType, HWND hwnd, int pos)
{
    pos = max(pos, 0);
    RECT rc;
    GetClientRect(&rc);
    if (iType == SB_VERT)
    {
        pos = min(pos, max(0, (_GetViewHeight() - RECTHEIGHT(rc))));
    }
    else
    {
        pos = min(pos, max(0, (_GetViewWidth() - RECTWIDTH(rc))));
    }

    //
    //  Scroll the window contents accordingly.
    //
    if (iType == SB_HORZ)
    {
        ::ScrollWindowEx(hwnd, _xOrigin - pos, 0,
                    nullptr, nullptr, nullptr, nullptr,
                    SW_ERASE | SW_INVALIDATE);

    }
    else
    {
        ::ScrollWindowEx(hwnd, 0, _yOrigin - pos,
                    nullptr, nullptr, nullptr, nullptr,
                    SW_ERASE | SW_INVALIDATE);
    }

    // Just invalidate everything, since the ScrollWindowEx blt doesn't appear to put things at the correct
    // position (or the DC contains not-up-to-date data when blt'd)
    ::InvalidateRect(hwnd, nullptr, FALSE);

    /*
     *  Now that the window has scrolled, a new item is at the top.
     */
    if (iType == SB_VERT)
    {
        _yOrigin = max(pos, 0);
    }
    else
    {
        _xOrigin = max(pos, 0);
    }

    /*
     *  And make the scrollbar look like what we think it is.
     */
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
    si.nPage = (iType == SB_VERT) ? RECTHEIGHT(rc) : RECTWIDTH(rc);
    si.nMin = 0;
    si.nMax = (iType == SB_VERT) ? (_GetViewHeight() - 1) : (_GetViewWidth() - 1);     /* endpoint is inclusive */
    si.nPos = (iType == SB_VERT) ? _yOrigin : _xOrigin;
    ::SetScrollInfo(hwnd, iType, &si, TRUE);
}

template <class T>
void CScrollingThing<T>::_OnScroll(int iType, UINT nSBCode, UINT nPos)
{
    HWND hwnd = this->m_hWnd;
    RECT rc;
    GetClientRect(&rc);
    int iPageDelta = (iType == SB_VERT) ? RECTHEIGHT(rc) : RECTWIDTH(rc);
    switch (nSBCode)
    {
        case SB_LINEUP:         _ScrollDelta(iType, hwnd, -1); break;
        case SB_LINEDOWN:       _ScrollDelta(iType, hwnd, +1); break;
        case SB_PAGEUP:         _ScrollDelta(iType, hwnd, -iPageDelta); break;
        case SB_PAGEDOWN:       _ScrollDelta(iType, hwnd, +iPageDelta); break;
        case SB_THUMBPOSITION:  _ScrollTo(iType, hwnd, nPos); break;
        case SB_THUMBTRACK:     _ScrollTo(iType, hwnd, nPos); break;
        case SB_TOP:            _ScrollTo(iType, hwnd, 0); break;
        case SB_BOTTOM:         _ScrollTo(iType, hwnd, MAXLONG); break;
    }
}

template <class T>
void CScrollingThing<T>::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
    if (pScrollBar == nullptr) // Filter out random scroll events from other children of the frame.
    {
        _OnScroll(SB_VERT, nSBCode, nPos);
    }
}

template <class T>
void CScrollingThing<T>::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
    if (pScrollBar == nullptr)
    {
        _OnScroll(SB_HORZ, nSBCode, nPos);
    }
}

template <class T>
BOOL CScrollingThing<T>::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    _zDeltaMouseWheel += zDelta;
    if (abs(_zDeltaMouseWheel) >= WHEEL_DELTA)
    {
        BOOL fForward = (_zDeltaMouseWheel > 0);
        short nNotches = abs(_zDeltaMouseWheel) / WHEEL_DELTA;
        
        // And reset it, with the left over
        _zDeltaMouseWheel = _zDeltaMouseWheel % WHEEL_DELTA;

        _OnMouseWheel(nFlags, fForward, pt, nNotches);
    }
    return TRUE;
}

template <class T>
void CScrollingThing<T>::OnSize(UINT nType, int cx, int cy)
{
    _InvalidateScrollStuff();
}


// CScrollingThing diagnostics

#ifdef _DEBUG
template <class T>
void CScrollingThing<T>::AssertValid() const
{
	T::AssertValid();
}

template <class T>
void CScrollingThing<T>::Dump(CDumpContext& dc) const
{
	T::Dump(dc);
}
#endif //_DEBUG


// CScrollingThing message handlers


