#pragma once


// CRoomExplorerDoc document

class CRoomExplorerDoc : public CDocument
{
	DECLARE_DYNCREATE(CRoomExplorerDoc)

public:
	CRoomExplorerDoc();
	virtual ~CRoomExplorerDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};
