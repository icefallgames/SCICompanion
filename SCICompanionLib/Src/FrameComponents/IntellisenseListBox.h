#pragma once

class IAutoCompleteClient;

// CIntellisenseListBox

class CIntellisenseListBox : public CListBox
{
	DECLARE_DYNAMIC(CIntellisenseListBox)

public:
	CIntellisenseListBox();
	virtual ~CIntellisenseListBox();
    BOOL GetSelectedText(CString &strText);
    void Show(CPoint pt);
    void Hide();
    void SetClient(IAutoCompleteClient *pClient) { _pClient = pClient; }
    void Highlight(PCTSTR pszText);

private:
    virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
    virtual void OnDestroy();
    void OnDrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
    void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    void OnDblClick();

	DECLARE_MESSAGE_MAP()

    CFont m_font;
    CImageList _imageList;
    IAutoCompleteClient *_pClient;
};


class IAutoCompleteClient
{
public:
    // Returns TRUE if there was an entry selected in the AC
    virtual BOOL OnACDoubleClick() = 0;
};

