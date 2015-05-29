// InsertObject.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "InsertObject.h"
#include "ScriptOMAll.h"
#include "ScriptMakerHelper.h"

using namespace sci;
using namespace std;

const TCHAR *g_szObjects[] =
{
    "Prop",   
    "Act",
    "Door",
    "Rgn",
    "Locale",
    "Script",
};

enum Species
{
    SpeciesProp = 0,
    SpeciesAct = 1,
    SpeciesDoor = 2,
    SpeciesRgn = 3,
    SpeciesLocale = 4,
    SpeciesScript = 5,
};

void _SetSendVariableTarget(SendCall &send, const std::string &target)
{
    unique_ptr<LValue> lValue = make_unique<LValue>();
    lValue->SetName(target);
    send.SetLValue(move(lValue));
}

void _AddProperties(ClassDefinition &instance, Species species, WORD wx, WORD wy, WORD wView)
{
    switch(species)
    {
    case SpeciesProp:
    case SpeciesAct:
    case SpeciesDoor:
        instance.AddProperty(ClassProperty("x", wx));
        instance.AddProperty(ClassProperty("y", wy));
        instance.AddProperty(ClassProperty("view", wView));
        if (species == SpeciesDoor)
        {
            // Add a few more
            instance.AddProperty(ClassProperty("entranceTo", "ROOMNUM"));
            instance.AddProperty(ClassProperty("locked", "TRUE"));
        }
        break;
    default:
        // For now, other species don't get any...
        break;
    }
}

// CInsertObject dialog
CInsertObject::CInsertObject(LangSyntax lang, CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CInsertObject::IDD, pParent), _lang(lang)
{
}

CInsertObject::~CInsertObject()
{
}

int g_nSelObjectType = 0;

void CInsertObject::_PrepareControls()
{
    // Populate the combo
    for (int i = 0; i < ARRAYSIZE(g_szObjects); i++)
    {
        m_wndComboType.AddString(g_szObjects[i]);
    }

    // Auto-select last used object type.
    m_wndComboType.SetCurSel(g_nSelObjectType);
}

unique_ptr<SingleStatement> _MakeTokenStatement(const string &token)
{
	unique_ptr<ComplexPropertyValue> pValue = std::make_unique<ComplexPropertyValue>();
    pValue->SetValue(token, ValueType::Token);
	unique_ptr<SingleStatement> temp = make_unique<SingleStatement>();
	temp->SetSyntaxNode(std::move(pValue));
	return temp;
}

unique_ptr<SingleStatement> _MakeNumberStatement(WORD w)
{
	unique_ptr<ComplexPropertyValue> pValue = std::make_unique<ComplexPropertyValue>();
    pValue->SetValue(w);
	unique_ptr<SingleStatement> temp = make_unique<SingleStatement>();
	temp->SetSyntaxNode(move(pValue));
	return temp;
}

void _AddAssignment(MethodDefinition &method, const string &lvalueName, const string &assigned)
{
	unique_ptr<Assignment> pEquals = std::make_unique<Assignment>();
    pEquals->SetName("=");
    unique_ptr<LValue> lvalue = make_unique<LValue>();
    lvalue->SetName(lvalueName);
    pEquals->SetVariable(move(lvalue));
	pEquals->SetStatement1(std::move(_MakeTokenStatement(assigned)));
    _AddStatement(method, std::move(pEquals));
}

void _AddBasicSwitch(MethodDefinition &method, const string &switchValue, const string &case0Comments)
{
	unique_ptr<SwitchStatement> pSwitch = std::make_unique<SwitchStatement>();

    // Make the switch value and add it to the swtich
	pSwitch->SetStatement1(std::move(_MakeTokenStatement(switchValue)));
    
    // Make the case statement and add it to the switch
	unique_ptr<CaseStatement> pCase = std::make_unique<CaseStatement>();
	pCase->SetStatement1(move(_MakeNumberStatement(0)));
    _AddComment(*pCase, case0Comments);
    pSwitch->AddCase(move(pCase));
    pCase.release();

    // Add the switch to the method
    _AddStatement(method, std::move(pSwitch));
}



// parameter may be empty.
void _AddSendCall(MethodDefinition &method, const string &objectName, const string &methodName, const string &parameter, bool isVariable)
{
	unique_ptr<SendCall> pSend = std::make_unique<SendCall>();
    if (isVariable)
    {
        _SetSendVariableTarget(*pSend, objectName);
    }
    else
    {
        pSend->SetName(objectName);
    }

    // Create the send param to add to the send call
	unique_ptr<SendParam> pParam = std::make_unique<SendParam>();
    pParam->SetName(methodName);
    pParam->SetIsMethod(true);

    if (!parameter.empty())
    {
        // Add the parameter to the sendparam.
		unique_ptr<ComplexPropertyValue> pValue = std::make_unique<ComplexPropertyValue>();
        pValue->SetValue(parameter, ValueType::Token);
        _AddStatement(*pParam, std::move(pValue));
    }

    pSend->AddSendParam(move(pParam));
    pParam.release();
    _AddStatement(method, std::move(pSend));
}

BOOL CInsertObject::_PrepareBuffer()
{
    // First, some validation.
    int iIndex = m_wndComboType.GetCurSel();
    if (iIndex == CB_ERR)
    {
        AfxMessageBox(TEXT("Please choose an object type."), MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
        return FALSE;
    }
    ASSERT(iIndex < ARRAYSIZE(g_szObjects));

    CString strName;
    m_wndEditName.GetWindowText(strName);
    if (strName.IsEmpty())
    {
        AfxMessageBox(TEXT("Please choose a name for the object."), MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
        return FALSE;
    }

    // The class definition
    ClassDefinition instance;
    instance.SetName((PCTSTR)strName);
    instance.SetSuperClass(g_szObjects[iIndex]);
    instance.SetInstance(true);
    
    // Now the properties.
    _AddProperties(instance, (Species)iIndex, (WORD)appState->_ptFakeEgo.x, (WORD)appState->_ptFakeEgo.y, (WORD)appState->_iView);

    // Before closing off, if they asked for a script, init it on the guy now:
    if (m_wndCheckScript.GetCheck() != 0)
    {
        ClassDefinition instance;
        instance.SetName((PCTSTR)strName);
        instance.SetSuperClass(g_szObjects[iIndex]);
        instance.SetInstance(true);

        // Prepare an init method.
		unique_ptr<MethodDefinition> pMethod = std::make_unique<MethodDefinition>();
        pMethod->SetName("init");

        // super.init()
        _AddSendCall(*pMethod, "super", "init", "");

        // self.setScript(***Script)
        string parameter = strName;
        parameter += "Script";
        _AddSendCall(*pMethod, "self", "setScript", parameter);

        // Add the method
		instance.AddMethod(std::move(pMethod));
    }
    stringstream ss;
    SourceCodeWriter out(ss, _lang);
    instance.OutputSourceCode(out);

    // Did they ask for a script?  Give it now.
    if (m_wndCheckScript.GetCheck() != 0)
    {
        ClassDefinition scriptInstance;
        scriptInstance.SetName((PCTSTR)(strName + "Script"));
        scriptInstance.SetSuperClass("Script");
        scriptInstance.SetInstance(true);

		unique_ptr<MethodDefinition> pMethodDoIt = std::make_unique<MethodDefinition>();
        pMethodDoIt->SetName("doit");
        _AddSendCall(*pMethodDoIt, "super", "doit", "");
        _AddComment(*pMethodDoIt, "// code executed each game cycle");
		scriptInstance.AddMethod(std::move(pMethodDoIt));

		unique_ptr<MethodDefinition> pMethodHandleEvent = std::make_unique<MethodDefinition>();
        pMethodHandleEvent->SetName("handleEvent");
        pMethodHandleEvent->AddParam("pEvent");
        _AddSendCall(*pMethodHandleEvent, "super", "handleEvent", "pEvent");
        _AddComment(*pMethodHandleEvent, "// handle Said's, etc...");
		scriptInstance.AddMethod(std::move(pMethodHandleEvent));

		unique_ptr<MethodDefinition> pMethodChangeState = std::make_unique<MethodDefinition>();
        pMethodChangeState->SetName("changeState");
        pMethodChangeState->AddParam("newState");
        _AddAssignment(*pMethodChangeState, "state", "newState");
        _AddBasicSwitch(*pMethodChangeState, "newState", "// Handle state changes");
		scriptInstance.AddMethod(std::move(pMethodChangeState));

        scriptInstance.OutputSourceCode(out);
    }
    
    _strBuffer = ss.str().c_str();
    _strBuffer.Replace("\n", "\r\n");

    return TRUE;
}

void CInsertObject::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_EDITNAME, m_wndEditName);
    DDX_Control(pDX, IDC_OBJECTTYPE, m_wndComboType);
    DDX_Control(pDX, IDC_CHECKSCRIPT, m_wndCheckScript);

    // Visuals
    DDX_Control(pDX, IDOK, m_wndOK);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_STATIC1, m_wndStatic1);
    DDX_Control(pDX, IDC_STATIC2, m_wndStatic2);

    _PrepareControls();
}

BOOL CInsertObject::OnInitDialog()
{
    __super::OnInitDialog();

    m_wndEditName.SetWindowText(TEXT("<type name here>"));
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
END_MESSAGE_MAP()



