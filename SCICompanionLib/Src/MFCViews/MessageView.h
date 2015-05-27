#pragma once

struct TextComponent;
class CMessageDoc;

class CMessageView : public CListView
{
	DECLARE_DYNCREATE(CMessageView)

protected:
	CMessageView();           // protected constructor used by dynamic creation
	virtual ~CMessageView();

public:
    CMessageDoc* GetDocument() const;
    TextComponent* GetTextComponent() const;

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
    virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    BOOL PreTranslateMessage(MSG *pMsg);

protected:
    void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
    void OnDelete();
    void OnUpdateIfSelection(CCmdUI *pCmdUI);
    void OnNewString();

    DECLARE_MESSAGE_MAP()

    void _InitColumns();
    void _InsertItem(int iItem, PCTSTR pszString, const TextComponent *text);
    void _ChangeView();
    int _GetSelectedItem();
    void _SwapStrings(int iIndex1, int iIndex2);
    void _UpdateSelection();

private:
    int _iView;
    bool _fInited;
    std::vector<LVCOLUMN> _columns;
};

#ifndef _DEBUG  // debug version in TextView.cpp
inline CMessageDoc* CMessageView::GetDocument() const
{ return reinterpret_cast<CMessageDoc*>(m_pDocument); }
#endif


