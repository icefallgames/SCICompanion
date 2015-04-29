// NewRoomDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "NewRoomDialog.h"
#include "resource.h"
#include "ScriptOMAll.h"
#include "ScriptMakerHelper.h"
#include <format.h>

using namespace sci;
using namespace std;
using namespace fmt;

//
// Scripts that are used by default
//
const TCHAR* g_UsedByDefault[] =
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

bool IsDefaultUse(PCTSTR pszName)
{
	bool fRet = false;
    for (int i = 0; !fRet && (i < ARRAYSIZE(g_UsedByDefault)); i++)
    {
        fRet = (0 == _strcmpi(pszName, g_UsedByDefault[i]));
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

int CNewRoomDialog::_GetSuggestedScriptNumber()
{
    int iRet = 0;
    // Now find an "empty" slot, starting from 0
    for (int i = 0; i < ARRAYSIZE(_rgUsed); i++)
    {
        if (0x00 == _rgUsed[i])
        {
            iRet = i;
            break;
        }
    }
    return iRet;
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

    // Values from 0 to 999
    m_wndEditScriptNumber.SetLimitText(3);
    m_wndEditPicNumber.SetLimitText(3);

    // Visuals
    DDX_Control(pDX, IDC_STATICROOMNUM, m_wndRoomNumberLabel);
    DDX_Control(pDX, IDC_STATICUSES, m_wndUses);
    DDX_Control(pDX, IDC_PICGROUP, m_wndPicNumberGroup);
}

void _AddPrevRoomNumSwitch(MethodDefinition &method)
{
	unique_ptr<SwitchStatement> pSwitch = std::make_unique<SwitchStatement>();

    // Make the switch value and add it to the swtich
	pSwitch->SetStatement1(move(_MakeTokenStatement("gPreviousRoomNumber")));
    
    // Make the case statement and add it to the switch
	unique_ptr<sci::CaseStatement> pCase = std::make_unique<sci::CaseStatement>();
    SingleStatement statementZero;
    pCase->SetDefault(true);

    // Now set the ego's position
	unique_ptr<SendCall> pSend = std::make_unique<SendCall>();
    pSend->SetName("gEgo");

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

void CNewRoomDialog::_PrepareBuffer()
{
    if (_scriptId.Language() == LangSyntaxSCIStudio)
    {
        _PrepareBufferOld();
    }
    else
    {
        sci::Script script(_scriptId);

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

            pClass->SetSuperClass("Rm");

            if (_scriptId.GetResourceNumber() == _nPicScript)
            {
                pClass->AddProperty(ClassProperty("picture", "scriptNumber"));
            }
            else
            {
                pClass->AddProperty(ClassProperty("picture", static_cast<WORD>(_nPicScript)));
            }
            pClass->AddProperty(ClassProperty("north", 0));
            pClass->AddProperty(ClassProperty("east", 0));
            pClass->AddProperty(ClassProperty("south", 0));
            pClass->AddProperty(ClassProperty("west", 0));

            // The init method
            {
                unique_ptr<MethodDefinition> pInit = make_unique<MethodDefinition>();
                pInit->SetName("init");
                {
                    unique_ptr<FunctionSignature> signature = make_unique<FunctionSignature>();
					signature->SetDataType("void");
					pInit->AddSignature(move(signature));
                }
                
                _AddSendCall(*pInit, "super", "init", "");
                _AddSendCall(*pInit, "self", "setScript", "RoomScript");

                _AddPrevRoomNumSwitch(*pInit);

                {
					unique_ptr<ProcedureCall> pSetUpEgo = std::make_unique<ProcedureCall>();
                    pSetUpEgo->SetName("SetUpEgo");
                    _AddStatement(*pInit, std::move(pSetUpEgo));
                }

                _AddSendCall(*pInit, "gEgo", "init", "");

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

            // The handleEvent method
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
                _AddBasicSwitch(*pCS, "0", "// Handle state changes");

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
}

void CNewRoomDialog::_PrepareBufferOld() // delete this phil
{
    _strBuffer += 
    TEXT("/******************************************************************************/\r\n"
         "(include \"sci.sh\")\r\n"
         "(include \"game.sh\")\r\n"
         "/******************************************************************************/\r\n");

    // The script number
    TCHAR szTemp[MAX_PATH];
    StringCchPrintf(szTemp, ARRAYSIZE(szTemp), TEXT("(script %d)\r\n"), _scriptId.GetResourceNumber());
    _strBuffer += szTemp;
    _strBuffer += TEXT("/******************************************************************************/\r\n");

    // Now the "uses"
    for (size_t i = 0; i < _usedNames.size(); i++)
    {
        StringCchPrintf(szTemp, ARRAYSIZE(szTemp), TEXT("(use \"%s\")\r\n"), _usedNames[i].c_str());
        _strBuffer += szTemp;
    }

    _strBuffer += TEXT("/******************************************************************************/\r\n");

    // Now the room
    StringCchPrintf(szTemp, ARRAYSIZE(szTemp), TEXT("(instance public rm%03d of Rm\r\n"), _scriptId.GetResourceNumber());
    _strBuffer += szTemp;

    _strBuffer += TEXT("\t(properties\r\n");

    if (_scriptId.GetResourceNumber() == _nPicScript)
    {
        _strBuffer += TEXT("\t\tpicture scriptNumber\r\n");
    }
    else
    {
        StringCchPrintf(szTemp, ARRAYSIZE(szTemp), TEXT("\t\tpicture %d\r\n"), _nPicScript);
        _strBuffer += szTemp;
    }

    _strBuffer +=
        TEXT("\t\tnorth 0\r\n"
        "\t\teast 0\r\n"
        "\t\tsouth 0\r\n"
        "\t\twest 0\r\n"
        "\t)\r\n"
        "\t(method (init)\r\n"
        "\t\t(super:init())\r\n"
        "\t\t(self:setScript(RoomScript))\r\n"
        "\t\t\r\n"
        "\t\t(switch(gPreviousRoomNumber)\r\n"
        "\t\t\t(default\r\n"
        "\t\t\t\t(send gEgo:\r\n"
        "\t\t\t\t\tposn(150 130)\r\n"
        "\t\t\t\t\tloop(1)\r\n"
        "\t\t\t\t)\r\n"
        "\t\t\t)\r\n"
        "\t\t)\r\n"
        "\t\t\r\n"
        "\t\tSetUpEgo()\r\n"
        "\t\t(send gEgo:init())\r\n"
        "\t)\r\n"
        ")\r\n"
        "/******************************************************************************/\r\n"
        "(instance RoomScript of Script\r\n"
        "\t(properties)\r\n"
        "\r\n"
        "\t(method (doit)\r\n"
        "\t\t(super:doit())\r\n"
        "\t\t// code executed each game cycle\r\n"
        "\t)\r\n"
        "\r\n"
        "\t(method (handleEvent pEvent)\r\n"
        "\t\t(super:handleEvent(pEvent))\r\n"
        "\t\t// handle Said's, etc...\r\n"
        "\t)\r\n"
        "\r\n"
		"\t(method (changeState newState)\r\n"
        "\t\t(= state newState)\r\n"
        "\t\t(switch (newState)\r\n"
        "\t\t\t(case 0\r\n"
        "\t\t\t\t// Handle state changes\r\n"
        "\t\t\t)\r\n"
        "\t\t)\r\n"
        "\t)\r\n"
        ")\r\n"
        "/******************************************************************************/\r\n");

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

    ASSERT(_scriptId.GetResourceNumber() < 1000);
    ASSERT((_nPicScript >= 0) && (_nPicScript < 1000));

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
        CDialog::OnOK(); // Not CScriptDialogOk!!
    }
}