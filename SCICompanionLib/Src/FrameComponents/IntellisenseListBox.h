#pragma once

class IAutoCompleteClient;
class AutoCompleteChoice;

// CIntellisenseListBox

class CIntellisenseListBox : public CListBox
{
	DECLARE_DYNAMIC(CIntellisenseListBox)

public:
	CIntellisenseListBox();
	virtual ~CIntellisenseListBox();
    BOOL GetSelectedText(CString &strText);
    void RememberChoice(PCSTR pszChoice);
    void Show(CPoint pt);
    void Hide();
    void SetClient(IAutoCompleteClient *pClient) { _pClient = pClient; }
    void Highlight(PCTSTR pszText);
    void UpdateChoices(const std::vector<AutoCompleteChoice> &choices);

private:
    void _Highlight(int index);
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
    int _nextRememberChoiceStamp;
    bool _hasUserInteracted;

    struct MruEntry
    {
        std::string TextLower;
        int TimeStamp;
    };

    std::vector<MruEntry> _sortedRememberedChoices;
};


class IAutoCompleteClient
{
public:
    // Returns TRUE if there was an entry selected in the AC
    virtual BOOL OnACDoubleClick() = 0;
};

