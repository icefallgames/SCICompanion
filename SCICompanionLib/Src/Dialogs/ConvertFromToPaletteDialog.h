#pragma once
#include "afxwin.h"

class ConvertFromToPaletteDialog : public CExtResizableDialog
{
public:
    ConvertFromToPaletteDialog(CWnd* pParent = NULL);   // standard constructor

    // Dialog Data
    enum { IDD = IDD_CONVERTTOPALETTE };

    int From;
    int To;

protected:
    void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
    BOOL OnInitDialog() override;
    void OnOK() override;

    DECLARE_MESSAGE_MAP()
private:
    std::vector<int> _paletteChoices;

    CExtComboBox m_wndComboFrom;
    CExtComboBox m_wndComboTo;
    CExtLabel m_wndStatic1;
    CExtLabel m_wndStatic2;
    CExtButton m_wndOk;
    CExtButton m_wndCancel;
};
