#pragma once

// CEditViewCelChooser view
class CNewRasterResourceDocument;

class CEditViewCelChooser : public CListView
{
	DECLARE_DYNCREATE(CEditViewCelChooser)

protected:
	CEditViewCelChooser();           // protected constructor used by dynamic creation
	virtual ~CEditViewCelChooser();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);
    void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
    void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
    void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);

protected:
	DECLARE_MESSAGE_MAP()
    void OnMouseMove(UINT nFlags, CPoint point);
    void OnLButtonUp(UINT nFlags, CPoint point);

    CSize _CalcMaxSize();
    BOOL _RegenImageList(int cCountHint);
    void _RefreshView(bool forceRegen);

private:
    CNewRasterResourceDocument *_GetDoc();
    HIMAGELIST _himlCels;
    CSize _sizeHiml;
    BOOL _fSetViewMode;

    int _iDragIndex;
    std::unique_ptr<CImageList> _pDragImage;

    BOOL _fDragEnabled;
};


