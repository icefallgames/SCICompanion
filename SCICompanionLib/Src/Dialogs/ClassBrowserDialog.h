/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "ScriptOM.h"
#include "ClassBrowser.h"


// CClassBrowserDialog dialog

class CClassBrowserDialog : public CDialog
{
	DECLARE_DYNAMIC(CClassBrowserDialog)

public:
	CClassBrowserDialog(const SCIClassBrowser *pBrowser, CWnd* pParent = NULL);   // standard constructor
	virtual ~CClassBrowserDialog();

// Dialog Data
	enum { IDD = IDD_CLASSBROWSERDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    void OnSelChange();
    void OnInstanceSelChange();

private:
    void _PopulateTree();
    void _PopulateTree(HTREEITEM hItemParent, const SCIClassBrowserNode *pBrowserInfo);
    void _ShowFunction(const sci::FunctionBase *pFunc);
    void _ShowClass(const sci::ClassDefinition *pClass);
    void _ShowHTML(const std::stringstream &html);
    void _OnClassOrInstanceSelected(const SCIClassBrowserNode *pBrowserInfo);
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnTvnSelchangedTreeClass(NMHDR *pNMHDR, LRESULT *pResult);
protected:
    CListBox m_wndProperties;
    CListBox m_wndMethods;
    CTreeCtrl m_wndTree;
    CStatic m_wndClassName;

    // We need to keep these around, since we're
    // sticking pointers to them in the listbox.
    sci::RawClassPropertyVector _properties;

    CImageList _imagelist;

    // Keep this around, as it owns the lifetimes of the objects we inserted into the tree.
    const SCIClassBrowser *_pBrowser;
    CFont m_font;
    int _iIndexPropSC;
    int _iIndexMethodSC;
    CListBox m_wndInstances;
    CWnd m_wndBrowser;
    bool _fInitialized;

public:
    afx_msg void OnBnClickedButtonprint();
};
