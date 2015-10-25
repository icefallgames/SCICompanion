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
// NewRoomDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "NewRoomDialog.h"
#include "resource.h"
#include "ScriptOMAll.h"
#include "ScriptMakerHelper.h"
#include "MessageSource.h"
#include "Message.h"
#include "MessageHeaderFile.h"
#include "ResourceEntity.h"
#include "InsertObject.h"
#include <format.h>
#include "Text.h"
#include "Message.h"
#include "Polygon.h"

using namespace sci;
using namespace std;
using namespace fmt;


enum class NewRoomProfile
{
    SCI0,
    SCI11
};

// Simplistic way to figure out which template game this is
NewRoomProfile GetProfile(SCIVersion version)
{
    return version.SeparateHeapResources ? NewRoomProfile::SCI11 : NewRoomProfile::SCI0;
}

//
// Scripts that are used by default
//
const TCHAR* g_UsedByDefaultSCI0[] =
{
    TEXT("Controls"),
    TEXT("Cycle"),
    TEXT("Door"),
    TEXT("Feature"),
    TEXT("Game"),
    TEXT("Inv"),
    TEXT("Main"),
    TEXT("Obj"),
};

const TCHAR* g_UsedByDefaultSCI11[] =
{
    TEXT("Cycle"),
    TEXT("Door"),
    TEXT("Feature"),
    TEXT("View"),
    TEXT("Game"),
    TEXT("Inv"),
    TEXT("Main"),
    TEXT("Obj"),
    TEXT("Polygon"),
};

bool IsDefaultUse(PCTSTR pszName)
{
	bool fRet = false;
    if (GetProfile(appState->GetVersion()) == NewRoomProfile::SCI11)
    {
        for (int i = 0; !fRet && (i < ARRAYSIZE(g_UsedByDefaultSCI11)); i++)
        {
            fRet = (0 == _strcmpi(pszName, g_UsedByDefaultSCI11[i]));
        }
    }
    else
    {
        for (int i = 0; !fRet && (i < ARRAYSIZE(g_UsedByDefaultSCI0)); i++)
        {
            fRet = (0 == _strcmpi(pszName, g_UsedByDefaultSCI0[i]));
        }
    }
    return fRet;
}

// CNewRoomDialog dialog

IMPLEMENT_DYNAMIC(CNewRoomDialog, CNewScriptDialog)
CNewRoomDialog::CNewRoomDialog(CWnd* pParent /*=NULL*/)
	: CNewScriptDialog(CNewRoomDialog::IDD, pParent)
{
}

CNewRoomDialog::~CNewRoomDialog()
{
}

void CNewRoomDialog::_DiscoveredScriptName(PCTSTR pszName)
{
    // Add the script name to the listbox
    int iItem = m_wndListBox.AddString(pszName);
    if (IsDefaultUse(pszName))
    {
        // Select the item.
        m_wndListBox.SetSel(iItem, TRUE);
    }
}

void CNewRoomDialog::_PrepareDialog()
{
    __super::_PrepareDialog();

    // Ok, so now we've populated our combobox, and suggested a new script number.
    // Check the default radio button:
    m_wndRadioUseRoom.SetCheck(TRUE);
    // And we're good to go.
}

void CNewRoomDialog::_AttachControls(CDataExchange* pDX)
{
    DDX_Control(pDX, IDC_RADIOOTHER, m_wndRadioUseOther);
    DDX_Control(pDX, IDC_RADIOSAMEASROOM, m_wndRadioUseRoom);
    DDX_Control(pDX, IDC_EDITPICNUMBER, m_wndEditPicNumber);
    DDX_Control(pDX, IDC_EDITROOMNUMBER, m_wndEditScriptNumber);
    DDX_Control(pDX, IDC_LISTUSES, m_wndListBox);
    DDX_Control(pDX, IDC_CHECKMESSAGE, m_wndCheckMessage);
    DDX_Control(pDX, IDC_CHECKPOLYS, m_wndCheckPolys);
    if (!appState->GetVersion().SupportsMessages)
    {
        m_wndCheckMessage.ShowWindow(SW_HIDE);
    }
    else
    {
        m_wndCheckMessage.SetCheck(BST_CHECKED);
    }

    // Enable polygons for VGA
    if (appState->GetVersion().PicFormat == PicFormat::EGA)
    {
        m_wndCheckPolys.ShowWindow(SW_HIDE);
    }
    else
    {
        m_wndCheckPolys.SetCheck(BST_CHECKED);
    }

    // Values from 0 to 999 or 16384
    m_wndEditScriptNumber.SetLimitText(5);
    m_wndEditPicNumber.SetLimitText(5);

    // Visuals
    DDX_Control(pDX, IDC_STATICROOMNUM, m_wndRoomNumberLabel);
    DDX_Control(pDX, IDC_STATICUSES, m_wndUses);
    DDX_Control(pDX, IDC_PICGROUP, m_wndPicNumberGroup);
}

void _AddPrevRoomNumSwitch(MethodDefinition &method, NewRoomProfile profile)
{
	unique_ptr<SwitchStatement> pSwitch = std::make_unique<SwitchStatement>();

    // Make the switch value and add it to the swtich
	pSwitch->SetStatement1(move(_MakeTokenStatement("gPreviousRoomNumber")));
    
    // Make the case statement and add it to the switch
	unique_ptr<sci::CaseStatement> pCase = std::make_unique<sci::CaseStatement>();
    SingleStatement statementZero;
    pCase->SetDefault(true);

    if (profile == NewRoomProfile::SCI11)
    {
        unique_ptr<ProcedureCall> pSetUpEgo = std::make_unique<ProcedureCall>();
        pSetUpEgo->SetName("SetUpEgo");
        _AddStatement(*pCase, std::make_unique<Comment>("// Set up ego view and loop (direction)"));
        _AddStatement(*pSetUpEgo, std::make_unique<PropertyValue>(-1, true)); // view = -1
        _AddStatement(*pSetUpEgo, std::make_unique<PropertyValue>(0));  // loop = 0
        _AddStatement(*pCase, std::move(pSetUpEgo));
    }

    // Now set the ego's position
	unique_ptr<SendCall> pSend = std::make_unique<SendCall>();
    _SetSendVariableTarget(*pSend, "gEgo");

    {
		unique_ptr<SendParam> pParam = std::make_unique<SendParam>();
        pParam->SetName("posn");
        {
			unique_ptr<SyntaxNode> pValue = std::make_unique<ComplexPropertyValue>();
            static_cast<ComplexPropertyValue*>(pValue.get())->SetValue(150);
            _AddStatement(*pParam, std::move(pValue));
        }
        {
			unique_ptr<SyntaxNode> pValue = std::make_unique<ComplexPropertyValue>();
			static_cast<ComplexPropertyValue*>(pValue.get())->SetValue(100);
            _AddStatement(*pParam, std::move(pValue));
        }
        pSend->AddSendParam(std::move(pParam));
    }

    if (profile == NewRoomProfile::SCI0)
    {
		unique_ptr<SendParam> pParam = std::make_unique<SendParam>();
        pParam->SetName("loop");
		unique_ptr<SyntaxNode> pValue = std::make_unique<ComplexPropertyValue>();
		static_cast<ComplexPropertyValue*>(pValue.get())->SetValue(1);
        _AddStatement(*pParam, std::move(pValue));
        pSend->AddSendParam(std::move(pParam));
    }

    _AddStatement(*pCase, std::move(pSend));


    //_AddComment(*pCase, case0Comments);
    pSwitch->AddCase(std::move(pCase));

    // Add the switch to the method
    _AddStatement(method, std::move(pSwitch));
}

void _CreateMessageFile(int scriptNumber)
{
    // Create the message file. If it already exists, that's fine.
    std::unique_ptr<MessageHeaderFile> messageHeaderFile = GetMessageFile(appState->GetResourceMap().Helper().GetMsgFolder(), scriptNumber);
    // By default, add a "room" noun (assuming there isn't one already)
    MessageSource *nounSource = messageHeaderFile->GetMessageSource("NOUNS");
    if (nounSource->GetDefines().empty())
    {
        nounSource->AddDefine("N_ROOM", 1);
    }
    messageHeaderFile->Commit();

    // And a message resource.
    std::unique_ptr<ResourceBlob> messageResource = appState->GetResourceMap().MostRecentResource(ResourceType::Message, scriptNumber, false);
    if (!messageResource)
    {
        // Look at 0.msg to see the version we should use
        std::unique_ptr<ResourceBlob> mainMessageResource = appState->GetResourceMap().MostRecentResource(ResourceType::Message, 0, false);
        if (mainMessageResource)
        {
            sci::istream byteStream = mainMessageResource->GetReadStream();
            uint16_t msgVersion = CheckMessageVersion(byteStream);
            std::unique_ptr<ResourceEntity> newMessageResource(CreateNewMessageResource(appState->GetVersion(), msgVersion));
            // Add a description for the room background
            TextComponent &text = newMessageResource->GetComponent<TextComponent>();
            TextEntry entry = { 0 };
            entry.Noun = 1; // N_NOUN, we just added this
            entry.Verb = 1; // V_LOOK in our template game
            entry.Talker = 99; // NARRATOR in our template game
            entry.Sequence = 1; 
            entry.Text = "This is the room description";
            text.Texts.push_back(entry);
            appState->GetResourceMap().AppendResource(*newMessageResource, appState->GetVersion().DefaultVolumeFile, scriptNumber, NoBase36);
        }
    }
}

int CNewRoomDialog::_GetMinSuggestedScriptNumber()
{
    // Rooms start from 100 in SCI1.1 (convention)
    return appState->GetVersion().SeparateHeapResources ? 100 : 0;
}

void CNewRoomDialog::_PrepareBuffer()
{
    NewRoomProfile profile = GetProfile(appState->GetVersion());
    sci::Script script(_scriptId);
    bool includePolys = (appState->GetVersion().PicFormat != PicFormat::EGA) && (m_wndCheckPolys.GetCheck() == BST_CHECKED);

    script.AddInclude("sci.sh");
    script.AddInclude("game.sh");
    if (appState->GetVersion().SupportsMessages && (m_wndCheckMessage.GetCheck() == BST_CHECKED))
    {
        string messagefileInclude = fmt::format("{0}.shm", _scriptId.GetResourceNumber());
        script.AddInclude(messagefileInclude);
    }

    if (includePolys)
    {
        string polyfileInclude = fmt::format("{0}.shp", _nPicScript);
        script.AddInclude(polyfileInclude);
    }

    if (profile == NewRoomProfile::SCI11)
    {
        // e.g. for SCI0, keep SCIStudio compatible. Otherwise, use version 2
        script.SyntaxVersion = 2;
    }

    // Now the uses.
    for (size_t i = 0; i < _usedNames.size(); ++i)
    {
        script.AddUse(_usedNames[i]);
    }

    // Now the room.
    {
		std::unique_ptr<ClassDefinition> pClass = std::make_unique<ClassDefinition>();
        pClass->SetInstance(true);

        std::stringstream ss;
        ss << format("rm{0:0>3}", _scriptId.GetResourceNumber());
        pClass->SetName(ss.str());
        pClass->SetPublic(true);
        pClass->SetSuperClass("Rm");

        // Export it, if we're using syntax version 2
        if (script.SyntaxVersion >= 2)
        {
            std::unique_ptr<ExportEntry> roomExport = make_unique<ExportEntry>();
            roomExport->Slot = 0;
            roomExport->Name = ss.str();
            script.GetExports().push_back(move(roomExport));
        }

        if (_scriptId.GetResourceNumber() == _nPicScript)
        {
            pClass->AddProperty(make_unique<ClassProperty>("picture", "scriptNumber"));
        }
        else
        {
            pClass->AddProperty(make_unique<ClassProperty>("picture", static_cast<WORD>(_nPicScript)));
        }
        pClass->AddProperty(make_unique<ClassProperty>("north", 0));
        pClass->AddProperty(make_unique<ClassProperty>("east", 0));
        pClass->AddProperty(make_unique<ClassProperty>("south", 0));
        pClass->AddProperty(make_unique<ClassProperty>("west", 0));

        if (appState->GetVersion().SupportsMessages)
        {
            pClass->AddProperty(make_unique<ClassProperty>("noun", "N_ROOM"));
        }
            
        // The init method
        {
            unique_ptr<MethodDefinition> pInit = make_unique<MethodDefinition>();
            pInit->SetName("init");
            {
                unique_ptr<FunctionSignature> signature = make_unique<FunctionSignature>();
				signature->SetDataType("void");
				pInit->AddSignature(move(signature));
            }
                
            if (includePolys)
            {
                _AddStatement(*pInit, GetSetUpPolyProcedureCall(_nPicScript));
            }

            _AddSendCall(*pInit, "super", "init", "");
            _AddSendCall(*pInit, "self", "setScript", "RoomScript");

            _AddPrevRoomNumSwitch(*pInit, profile);

            if (profile == NewRoomProfile::SCI0)
            {
				unique_ptr<ProcedureCall> pSetUpEgo = std::make_unique<ProcedureCall>();
                pSetUpEgo->SetName("SetUpEgo");
                _AddStatement(*pInit, std::move(pSetUpEgo));
            }

            _AddSendCall(*pInit, "gEgo", "init", "", true);

			pClass->AddMethod(std::move(pInit));
        }

		script.AddClass(std::move(pClass));
    }

    // Now the room script
    {
		std::unique_ptr<ClassDefinition> pClass = std::make_unique<ClassDefinition>();
        pClass->SetInstance(true);
        pClass->SetName("RoomScript");
        pClass->SetSuperClass("Script");

        // The doit method
        {
			std::unique_ptr<MethodDefinition> pDoit = std::make_unique<MethodDefinition>();
            pDoit->SetName("doit");
            {
                unique_ptr<FunctionSignature> signature = make_unique<FunctionSignature>();
				signature->SetDataType("void");
				pDoit->AddSignature(move(signature));
            }
                
            _AddSendCall(*pDoit, "super", "doit", "");
            _AddComment(*pDoit, "// code executed each game cycle");
			pClass->AddMethod(std::move(pDoit));
        }

        // The handleEvent method, most useful in SCI0
        if (profile == NewRoomProfile::SCI0)
        {
			std::unique_ptr<MethodDefinition> pHE = std::make_unique<MethodDefinition>();
            pHE->SetName("handleEvent");
            {
				unique_ptr<FunctionSignature> signature = make_unique<FunctionSignature>();
				signature->SetDataType("void");
				unique_ptr<FunctionParameter> pParam = make_unique<FunctionParameter>();
				pParam->SetName("pEvent");
				pParam->SetDataType("Event");
				signature->AddParam(move(pParam), false);
				pHE->AddSignature(move(signature));
            }
                
            _AddSendCall(*pHE, "super", "handleEvent", "pEvent");
            _AddComment(*pHE, "// handle Said's, etc...");

			pClass->AddMethod(std::move(pHE));
        }

        // The changeState method
        {
			std::unique_ptr<MethodDefinition> pCS = std::make_unique<MethodDefinition>();
            pCS->SetName("changeState");
            {
				unique_ptr<FunctionSignature> signature = make_unique<FunctionSignature>();
				signature->SetDataType("void");
				unique_ptr<FunctionParameter> pParam = make_unique<FunctionParameter>();
				pParam->SetName("newState");
				pParam->SetDataType("int");
				signature->AddParam(move(pParam), false);
				pCS->AddSignature(move(signature));
            }
                
            _AddAssignment(*pCS, "state", "newState");
            _AddBasicSwitch(*pCS, "state", "// Handle state changes");

			pClass->AddMethod(std::move(pCS));
        }

		script.AddClass(std::move(pClass));
    }

    std::stringstream ss;
    SourceCodeWriter out(ss, script.Language());
    out.pszNewLine = "\r\n";
    script.OutputSourceCode(out);
    _strBuffer = ss.str();
}

BEGIN_MESSAGE_MAP(CNewRoomDialog, CNewScriptDialog)
    ON_BN_CLICKED(IDC_RADIOSAMEASROOM, OnRadioSameAsRoom)
    ON_BN_CLICKED(IDC_RADIOOTHER, OnRadioOther)
END_MESSAGE_MAP()


// CNewRoomDialog message handlers

void CNewRoomDialog::OnRadioSameAsRoom()
{
    m_wndEditPicNumber.EnableWindow(FALSE);
}

void CNewRoomDialog::OnRadioOther()
{
    m_wndEditPicNumber.EnableWindow(TRUE);
}

void CNewRoomDialog::OnOK()
{
    BOOL fClose = !_ValidateScriptNumber();

    if (m_wndRadioUseRoom.GetCheck() != 0)
    {
        _nPicScript = _scriptId.GetResourceNumber();
    }
    else
    {
        CString strNumber;
        m_wndEditPicNumber.GetWindowText(strNumber);
        _nPicScript = StrToInt(strNumber);
    }

    ASSERT(_scriptId.GetResourceNumber() <= appState->GetVersion().GetMaximumResourceNumber());
    ASSERT((_nPicScript >= 0) && (_nPicScript <= appState->GetVersion().GetMaximumResourceNumber()));

    if (fClose)
    {
        // Finally, go through the listbox, and see what scripts should be included.
        int cItems = 1000;
        int *rgItems = new int[cItems];
        if (rgItems)
        {
            cItems = m_wndListBox.GetSelItems(cItems, rgItems);
            if (cItems > 0)
            {
                for (int i = 0; i < cItems; i++)
                {
                    CString strScriptName;
                    m_wndListBox.GetText(rgItems[i], strScriptName);
                    _usedNames.push_back((PCSTR)strScriptName);
                }
            }
            delete [] rgItems;
        }
    }

    if (fClose)
    {
        // Prepare the script name.
        // (Without the .sc extension)
        StringCchPrintf(_szScriptName, ARRAYSIZE(_szScriptName), TEXT("rm%03d"), _scriptId.GetResourceNumber());
        _scriptId.SetFullPath(appState->GetResourceMap().Helper().GetScriptFileName(_szScriptName, appState->GetResourceMap().GetGameLanguage()));
    }

    if (fClose)
    {
        _PrepareBuffer();
        // Make message header?
        if (appState->GetVersion().SupportsMessages && (m_wndCheckMessage.GetCheck() == BST_CHECKED))
        {
            _CreateMessageFile(_scriptId.GetResourceNumber());
        }
        // Make poly header?
        if ((appState->GetVersion().PicFormat != PicFormat::EGA) && (m_wndCheckPolys.GetCheck() == BST_CHECKED))
        {
            unique_ptr<PolygonComponent> polyComponent = CreatePolygonComponent(appState->GetResourceMap().Helper().GetPolyFolder(), _nPicScript);
            if (polyComponent->Polygons().empty())
            {
                // This means it doesn't already exist (or is empty). Commit it so it exists.
                polyComponent->Commit(_nPicScript);
            }
        }
        CDialog::OnOK(); // Not CScriptDialogOk!!
    }
}