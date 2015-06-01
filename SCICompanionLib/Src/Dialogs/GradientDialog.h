#pragma once

#include "BaseColorDialog.h"
#include "ChooseColorStatic.h"
#include "ColorDialogCallback.h"
#include "resource.h"

struct PaletteComponent;

class GradientDialog : public CExtResizableDialog
{
public:
    GradientDialog(PaletteComponent &palette, uint8_t start, uint8_t end, CWnd* pParent = NULL);  // standard constructor

    // Dialog Data
    enum { IDD = IDD_DIALOGGRADIENTS };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    void _SyncPalette();

    DECLARE_MESSAGE_MAP()

    CChooseColorStatic m_wndStatic;

    // Visuals
    CExtButton m_wndOk;
    CExtButton m_wndCancel;
    CExtButton m_wndCenter;
    CExtButton m_wndEdges;

    PaletteComponent &_palette;
    uint8_t _start;
    uint8_t _endInclusive;
    COLORREF _edge;
    COLORREF _center;
public:
    afx_msg void OnBnClickedButtoncenter();
    afx_msg void OnBnClickedButtonedges();
};
