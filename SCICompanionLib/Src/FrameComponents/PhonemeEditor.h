#pragma once

#include "ColoredToolTip.h"

class ResourceEntity;
class PhonemeMap;

class IPhonemeMapNotify
{
public:
    virtual void OnMapUpdated() = 0;
};

class PhonemeEditor : public CStatic
{
    DECLARE_DYNAMIC(PhonemeEditor)

public:
    PhonemeEditor(IPhonemeMapNotify *notify, const ResourceEntity *viewResource, int view, int loop, PhonemeMap &map);
    virtual ~PhonemeEditor();

private:
    afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

    CPoint _HitTest(CPoint pt);
    void _EnsureSCIBitmap(CSize size);
    void _OnDraw(CDC *pDC, LPRECT prc);
    void _GenerateDoubleBuffer(CDC *pDC, LPRECT prc);
    void _DrawCels(CDC *pDC, LPRECT prc);
    void _SetHighlight(CPoint index);
    void _ShowPhonemeTip(CPoint index);
    void _ShowPhonemeTipAtCursor(CPoint index);
    CRect _GetIndexRect(CPoint index);
    CRect _GetRowRect(int row);
    void _InvalidateRow(int row);
    void _InvalidateIndex(CPoint pt);
    void _SetDragOverIndex(CPoint index, bool force = false);

    DECLARE_MESSAGE_MAP()

    int _nView, _nLoop;
    std::unique_ptr<CBitmap> _pbitmapDoubleBuf;
    CSize _sizeDoubleBuf;
    const ResourceEntity *_viewResource;

    uint8_t *_dibBits;
    std::unique_ptr<CBitmap> _sciBitmap;

    PhonemeMap &_map;

    // To make things easy for the UI:
    std::vector<std::vector<std::string>> _cache;
    std::vector<int> _rowYs;
    int _phonemeWidth;
    int _phonemeHeight;
    std::unordered_map<std::string, std::string> _phonemeToExample;

    CColoredToolTip _toolTip;

    // UI Behavior
    bool _capturing;
    bool _dragging;
    CPoint _dragSourceIndex;
    CPoint _dragSourcePoint;
    CPoint _hoverPointCurrent;
    CPoint _dragOverIndex;
    CPoint _highlightIndex;

    IPhonemeMapNotify *_notify;
};
