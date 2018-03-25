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

// InsertObject.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "InsertObject.h"
#include "ScriptOMAll.h"
#include "ScriptMakerHelper.h"
#include "SyntaxParser.h"
#include "CrystalScriptStream.h"
#include "OutputCodeHelper.h"
#include "ClipboardUtil.h"
#include "FakeEgo.h"

using namespace sci;
using namespace std;


bool _IsMarkedOptional(const string &name, string &resolved)
{
    bool optional = false;
    if (!name.empty() && (name[name.size() - 1] == '_'))
    {
        optional = true;
        resolved = name.substr(0, name.size() - 1);
    }
    else
    {
        resolved = name;
    }
    return optional;
}

bool _IsMarkedOptional(const string &name)
{
    string dummy;
    return _IsMarkedOptional(name, dummy);
}

bool _CleanName(NamedNode &named)
{
    string name = named.GetName();
    bool optional = _IsMarkedOptional(name, name);
    named.SetName(name);
    return optional;
}

class DummyLog : public ICompileLog
{
    void ReportResult(const CompileResult &result) override {}
};

AvailableMethods::AvailableMethods(LangSyntax language) : _targetLanguage(language)
{
    string fullPath = appState->GetResourceMap().GetObjectsFolder() + "\\Methods.sc";
    DummyLog log;
    // Make a new buffer.
    CCrystalTextBuffer buffer;
    if (buffer.LoadFromFile(fullPath.c_str()))
    {
        CScriptStreamLimiter limiter(&buffer);
        CCrystalScriptStream stream(&limiter);
        _script = std::make_unique<sci::Script>(ScriptId(fullPath));
        if (SyntaxParser_Parse(*_script, stream, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), &log, false, nullptr, true))
        {
            for (const auto &theClass : _script->GetClassesNC())
            {
                transform(theClass->GetMethods().begin(), theClass->GetMethods().end(), back_inserter(_methods),
                    [](const unique_ptr<MethodDefinition> &theMethod) { return theMethod.get(); }
                );
            }
        }
        buffer.FreeAll();
    }
}

void AvailableMethods::PrepareBuffer(const sci::MethodDefinition *methodDef, CString &buffer)
{
    PrepForLanguage(_targetLanguage, *_script);
    std::stringstream ss;
    //sci::SourceCodeWriter out(ss, _targetLanguage, _objectToScript[theClass]);
    // Providing the script lets us sync comments, but it is not working properly. They merge with newlines, and comments in
    // unused functions are included. Really, we need an option to parse comments inline.
    sci::SourceCodeWriter out(ss, _targetLanguage, nullptr);
    DebugIndent indent(out);
    out.pszNewLine = "\r\n";
    out.fAlwaysExpandCodeBlocks = true;
    ss << out.pszNewLine;
    methodDef->OutputSourceCode(out);
    buffer = ss.str().c_str();
}

AvailableObjects::AvailableObjects(LangSyntax language) : _targetLanguage(language)
{
    vector<string> filenames;

    /*
    std::string objFolder = appState->GetResourceMap().GetObjectsFolder();
    objFolder += "\\*.sc";
    WIN32_FIND_DATA findData = { 0 };
    HANDLE hFFF = FindFirstFile(objFolder.c_str(), &findData);
    if (hFFF != INVALID_HANDLE_VALUE)
    {
        BOOL fOk = TRUE;
        while (fOk)
        {
            filenames.push_back(findData.cFileName);
            fOk = FindNextFile(hFFF, &findData);
        }
        FindClose(hFFF);
    }*/

    // Hard-code it for now, instead of enumerating all, since we'll have other files for methods, etc..
    filenames.push_back("Objects.sc");

    // Now compile them.
    for (string filename : filenames)
    {
        string fullPath = appState->GetResourceMap().GetObjectsFolder() + "\\" + filename;
        DummyLog log;
        // Make a new buffer.
        CCrystalTextBuffer buffer;
        if (buffer.LoadFromFile(fullPath.c_str()))
        {
            CScriptStreamLimiter limiter(&buffer);
            CCrystalScriptStream stream(&limiter);
            std::unique_ptr<sci::Script> pScript = std::make_unique<sci::Script>(ScriptId(fullPath));
            if (SyntaxParser_Parse(*pScript, stream, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), &log, false, nullptr, true))
            {
                transform(pScript->GetClassesNC().begin(), pScript->GetClassesNC().end(), back_inserter(_objects),
                    [](unique_ptr<ClassDefinition> &theClass) { return theClass.get(); }
                );

                for (auto &classDef : pScript->GetClassesNC())
                {
                    _objectToScript[classDef.get()] = pScript.get();
                }

                _scripts.push_back(move(pScript));
            }
            buffer.FreeAll();
        }
    }
}

void AvailableObjects::PrepareBuffer(sci::ClassDefinition *theClass, CString &buffer, CListBox *pListProps, CListBox *pListMethods)
{
    for (auto &script : _scripts)
    {
        PrepForLanguage(_targetLanguage, *script);
    }

    // Grab any properties from the "fake ego"
    std::unique_ptr<FakeEgo> fakeEgo;
    ProcessClipboardDataIfAvailable(appState->ViewAttributesClipboardFormat, AfxGetMainWnd(),
        [&fakeEgo](sci::istream &data)
    {
        fakeEgo = std::make_unique<FakeEgo>();
        size_t count;
        data >> count;
        assert(count >= 1);
        // We only care about the first one:
        data >> *fakeEgo;
    }
        );

    // Remove props that aren't selected.
    auto &props = theClass->GetPropertiesNC();
    int propCount = props.size();
    for (int i = propCount - 1; i >= 0; i--)
    {
        bool erase = false;
        if (pListProps)
        {
            erase = (pListProps->GetSel(i) <= 0);
        }
        bool wasOptional = _CleanName(*props[i]);
        if (!pListProps && wasOptional)
        {
            erase = true;
        }
        
        if (erase)
        {
            props.erase(props.begin() + i);
        }
        else if (fakeEgo)
        {
            auto &prop = props[i];
            if (prop->GetName() == "view")
            {
                prop->SetValue(sci::PropertyValue((uint16_t)fakeEgo->View));
            }
            else if (prop->GetName() == "x")
            {
                prop->SetValue(sci::PropertyValue((uint16_t)fakeEgo->Location.x, true));
            }
            else if (prop->GetName() == "y")
            {
                prop->SetValue(sci::PropertyValue((uint16_t)fakeEgo->Location.y, true));
            }
            else if (prop->GetName() == "priority")
            {
                prop->SetValue(sci::PropertyValue((uint16_t)fakeEgo->Pri, true));
            }
            else if (prop->GetName() == "loop")
            {
                prop->SetValue(sci::PropertyValue((uint16_t)fakeEgo->Loop));
            }
            else if (prop->GetName() == "cel")
            {
                prop->SetValue(sci::PropertyValue((uint16_t)fakeEgo->Cel));
            }
        }
    }

    // Remove methods that aren't selected.
    auto &methods = theClass->GetMethodsNC();
    int methodCount = methods.size();
    for (int i = methodCount - 1; i >= 0; i--)
    {
        bool erase = false;
        if (pListMethods)
        {
            erase = (pListMethods->GetSel(i) <= 0);
        }
        bool wasOptional = _CleanName(*methods[i]);
        if (!pListMethods && wasOptional)
        {
            erase = true;
        }

        if (erase)
        {
            methods.erase(methods.begin() + i);
        }
    }

    std::stringstream ss;
    //sci::SourceCodeWriter out(ss, _targetLanguage, _objectToScript[theClass]);
    // Providing the script lets us sync comments, but it is not working properly. They merge with newlines, and comments in
    // unused functions are included. Really, we need an option to parse comments inline.
    sci::SourceCodeWriter out(ss, _targetLanguage, nullptr);
    out.pszNewLine = "\r\n";
    out.fAlwaysExpandCodeBlocks = true;
    ss << out.pszNewLine;
    theClass->OutputSourceCode(out);
    buffer = ss.str().c_str();
}

// CInsertObject dialog
CInsertObject::CInsertObject(LangSyntax lang, CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(CInsertObject::IDD, pParent), _availableObjects(lang)
{
}

int g_nSelObjectType = 0;

void CInsertObject::_PrepareControls()
{
    // Populate the combo
    for (const auto &theClass : _availableObjects.GetObjects())
    {
        m_wndComboType.AddString(theClass->GetSuperClass().c_str());
    }

    // Auto-select last used object type.
    m_wndComboType.SetCurSel(g_nSelObjectType);

    _SyncSelection();
}

ClassDefinition *CInsertObject::_GetCurrentObject()
{
    ClassDefinition *theClass = nullptr;
    int selection = m_wndComboType.GetCurSel();
    if (selection != CB_ERR)
    {
        theClass = _availableObjects.GetObjects()[selection];
    }
    return theClass;
}

void CInsertObject::_SyncSelection()
{
    m_wndListProps.SetRedraw(FALSE);
    m_wndListMethods.SetRedraw(FALSE);
    m_wndListProps.ResetContent();
    m_wndListMethods.ResetContent();
    ClassDefinition *theClass = _GetCurrentObject();
    int count = 0;
    for (const auto &method : theClass->GetMethods())
    {
        string name = method->GetName();
        bool optional = _IsMarkedOptional(name, name);
        m_wndListMethods.AddString(name.c_str());
        if (!optional)
        {
            m_wndListMethods.SetSel(count, TRUE);
        }
        count++;
    }

    count = 0;
    for (const auto &prop : theClass->GetProperties())
    {
        string name = prop->GetName();
        bool optional = _IsMarkedOptional(name, name);
        m_wndListProps.AddString(name.c_str());
        if (!optional)
        {
            m_wndListProps.SetSel(count, TRUE);
        }
        count++;
    }

    m_wndListProps.SetRedraw(TRUE);
    m_wndListMethods.SetRedraw(TRUE);
}

BOOL CInsertObject::_PrepareBuffer()
{
    ClassDefinition *theClass = _GetCurrentObject();
    if (theClass)
    {
        CString strName;
        m_wndEditName.GetWindowText(strName);
        theClass->SetName((PCSTR)strName);

        _availableObjects.PrepareBuffer(theClass, _strBuffer, &m_wndListProps, &m_wndListMethods);
    }
    else
    {
        AfxMessageBox(TEXT("Please choose an object type."), MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
        return FALSE;
    }

    return TRUE;
}

void CInsertObject::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_EDITNAME, m_wndEditName);
    DDX_Control(pDX, IDC_OBJECTTYPE, m_wndComboType);
    DDX_Control(pDX, IDC_LISTPROPS, m_wndListProps);
    DDX_Control(pDX, IDC_LISTMETHODS, m_wndListMethods);

    // Visuals
    DDX_Control(pDX, IDOK, m_wndOK);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_STATIC1, m_wndStatic1);
    DDX_Control(pDX, IDC_STATIC2, m_wndStatic2);
    DDX_Control(pDX, IDC_STATIC3, m_wndStatic3);
    DDX_Control(pDX, IDC_STATIC4, m_wndStatic4);

    _PrepareControls();
}

const char c_szUnnamedObject[] = "type_name_here";

BOOL CInsertObject::OnInitDialog()
{
    __super::OnInitDialog();

    m_wndEditName.SetWindowText(TEXT(c_szUnnamedObject));
    m_wndEditName.SetSel(0, -1);
    m_wndEditName.SetFocus();

    ShowSizeGrip(FALSE);

    // the edit control is the first one, so focus will be there.
    return FALSE;
}

void CInsertObject::OnOK()
{
    // Store last used object type index
    g_nSelObjectType = m_wndComboType.GetCurSel();

    if (_PrepareBuffer())
    {
        __super::OnOK(); // Close the dialog
    }
}

BEGIN_MESSAGE_MAP(CInsertObject, CExtResizableDialog)
    ON_CBN_SELCHANGE(IDC_OBJECTTYPE, &CInsertObject::OnCbnSelchangeObjecttype)
END_MESSAGE_MAP()



void CInsertObject::OnCbnSelchangeObjecttype()
{
    _SyncSelection();
}
