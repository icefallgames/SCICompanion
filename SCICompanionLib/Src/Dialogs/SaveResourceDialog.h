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


// SaveResourceDialog dialog
#include "resource.h"

class SaveResourceDialog : public CExtResizableDialog
{
public:
	SaveResourceDialog(bool warnOnOverwrite, ResourceType type, CWnd* pParent = NULL);   // standard constructor
	virtual ~SaveResourceDialog();
    void Init(int iPackageNumber, int iResourceNumber) { _iResourceNumber = iResourceNumber; _iPackageNumber = iPackageNumber; }
    void Init(int iPackageNumber, int iResourceNumber, const std::string &name) { _iResourceNumber = iResourceNumber; _iPackageNumber = iPackageNumber; _name = name; }
    int GetPackageNumber() { return _iPackageNumber; }
    int GetResourceNumber() { return _iResourceNumber; }
    std::string GetName() { return _name; }
    virtual void OnOK();

// Dialog Data
	enum { IDD = IDD_RESOURCENUM };

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    BOOL _ValidateData();

	DECLARE_MESSAGE_MAP()

    int _iResourceNumber;
    int _iPackageNumber;
    std::string _name;

    bool _warnOnOverwrite;
    ResourceType _type;

    CExtEdit m_wndEditPackage;
    CExtEdit m_wndEditResource;
    CExtEdit m_wndEditName;

    // Visuals
    CExtButton m_wndOk;
    CExtButton m_wndCancel;
    CExtLabel m_wndLabel1;
    CExtLabel m_wndLabel2;
    CExtLabel m_wndLabel3;
};
