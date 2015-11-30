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

// CInsertObject dialog
namespace sci
{
    class ClassDefinition;
    class MethodDefinition;
}

extern const char c_szUnnamedObject[];


class AvailableMethods
{
public:
    AvailableMethods(LangSyntax language);

    std::vector<const sci::MethodDefinition*> &GetMethods() {
        return _methods;
    }

    void PrepareBuffer(const sci::MethodDefinition *methodDef, CString &buffer);

private:
    std::vector<const sci::MethodDefinition*> _methods;
    std::unique_ptr<sci::Script> _script;
    LangSyntax _targetLanguage;
};
    

class AvailableObjects
{
public:
    AvailableObjects(LangSyntax language);
    
    std::vector<sci::ClassDefinition*> &GetObjects() {
        return _objects;
    }

    void PrepareBuffer(sci::ClassDefinition *classDef, CString &buffer, CListBox *pListProps = nullptr, CListBox *pListMethods = nullptr);

private:
    std::vector<sci::ClassDefinition*> _objects;
    std::vector<std::unique_ptr<sci::Script>> _scripts;
    std::unordered_map<sci::ClassDefinition*, sci::Script*> _objectToScript;
    LangSyntax _targetLanguage;
};

class CInsertObject : public CExtResizableDialog
{
public:
	CInsertObject(LangSyntax lang, CWnd* pParent = NULL);   // standard constructor
    CString &GetBuffer() { return _strBuffer; }

// Dialog Data
	enum { IDD = IDD_INSERTSCIOBJECT };

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    void _SyncSelection();
    void _PrepareControls();
    BOOL _PrepareBuffer();
    void _SetupObjects();
    sci::ClassDefinition *_GetCurrentObject();
	DECLARE_MESSAGE_MAP()

    CString _strBuffer;
    CExtEdit m_wndEditName;
    CExtComboBox m_wndComboType;

    CExtButton m_wndOK;
    CExtButton m_wndCancel;
    CExtLabel m_wndStatic1;
    CExtLabel m_wndStatic2;

    CExtLabel m_wndStatic3;
    CExtLabel m_wndStatic4;
    CListBox m_wndListProps;
    CListBox m_wndListMethods;

    AvailableObjects _availableObjects;
public:
    afx_msg void OnCbnSelchangeObjecttype();
};
