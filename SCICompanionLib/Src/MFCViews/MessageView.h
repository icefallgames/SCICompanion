#pragma once

struct TextComponent;
struct TextEntry;
class CMessageDoc;

class CMessageView : public CListView
{
	DECLARE_DYNCREATE(CMessageView)

protected:
	CMessageView();           // protected constructor used by dynamic creation
	virtual ~CMessageView();

public:
    CMessageDoc* GetDocument() const;
    const TextComponent* GetTextComponent() const;

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
    void OnAddNew();
    void OnCloneMessage();
    void OnAddSeq();
    void OnUpdateIfSelection(CCmdUI *pCmdUI);
    void OnNewString();
    void OnExportMessage();
    void OnImportMessage();
    void OnCommandAudioSize(CCmdUI *pCmdUI);

    DECLARE_MESSAGE_MAP()

    void _InitColumns();
    void _SetItem(int itemIndex, int visualIndex, PCTSTR pszString, const TextComponent *text, bool insert);
    void _ChangeView();
    int _GetSelectedItem();
    void _UpdateSelection(int *topIndex = nullptr);
    void _AddEntryAtCurrentPosition(const TextEntry &entry);
    const TextEntry *_GetEntry();

private:
    void OnColumnClicked(NMHDR* pNMHDR, LRESULT* pResult);
    void _SortItemsHelper(int sortColumn, bool toggle);
    int _GetViewIndex(int index);

    int _iView;
    bool _fInited;
    std::vector<LVCOLUMN> _columns;

    std::unordered_map<int, bool> _sortOrder;
    int _iSortColumn;
};

#ifndef _DEBUG  // debug version in TextView.cpp
inline CMessageDoc* CMessageView::GetDocument() const
{ return reinterpret_cast<CMessageDoc*>(m_pDocument); }
#endif


