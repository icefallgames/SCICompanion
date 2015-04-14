#pragma once


// CResourceTypeList

class CResourceTypeList : public CListBox
{
	DECLARE_DYNAMIC(CResourceTypeList)

public:
	CResourceTypeList();
	virtual ~CResourceTypeList();

    void DrawItem(LPDRAWITEMSTRUCT);
    void MeasureItem(LPMEASUREITEMSTRUCT);
    virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
	DECLARE_MESSAGE_MAP()

private:
    void _LoadIcons();
    void OnDblClick();
    CExtCmdIcon _icons[NumResourceTypesSCI0];
};


