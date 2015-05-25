#include "stdafx.h"
#include "AppState.h"
#include "MessageSidePane.h"
#include "TextDoc.h"
#include "Text.h"
#include "Message.h"
#include "CObjectWrap.h"


MessageSidePane::MessageSidePane(CWnd* pParent /*=NULL*/)
    : CExtDialogFwdCmd(IDD, pParent), _hAccel(nullptr)
{
    // Load our accelerators
    // HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(IDR_ACCELERATORPICCOMMANDS), RT_ACCELERATOR);
    // _hAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATORPICCOMMANDS));
}

MessageSidePane::~MessageSidePane()
{
}

void MessageSidePane::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_GOTOVERBS, m_wndGotoVerbs);
    DDX_Control(pDX, IDC_GOTOTALKERS, m_wndGotoTalkers);
    DDX_Control(pDX, IDC_LISTCONDITIONS, m_wndConditions);
    DDX_Control(pDX, IDC_LISTNOUNS, m_wndNouns);
    DDX_Control(pDX, IDC_STATIC1, m_wndLabel1);
    DDX_Control(pDX, IDC_STATIC2, m_wndLabel2);
}


BEGIN_MESSAGE_MAP(MessageSidePane, CExtDialogFwdCmd)
    ON_WM_DRAWITEM()
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_COMMAND(IDC_GOTOVERBS, OnGotoVerbs)
    ON_COMMAND(IDC_GOTOTALKERS, OnGotoTalkers)
END_MESSAGE_MAP()

BOOL MessageSidePane::OnEraseBkgnd(CDC *pDC)
{
    return __super::OnEraseBkgnd(pDC);
}


// MessageSidePane message handlers

//
// To properly handle commands we do two things here:
// 1) Process our own accelerators
//      - The main frame's accelerator table doesn't get used in dialogs in control bars,
//        so we need to do our own.  So, e.g. Ctrl-Z becomes ID_EDIT_UNDO
// 2) Fwd any commands we receive to our frame
//      if we don't handle them ourselves.
//      - they don't bubble up naturally in dialogs in control bars.
//      - we do this by inheriting from CExtDialogFwdCmd
//
BOOL MessageSidePane::PreTranslateMessage(MSG* pMsg)
{
    BOOL fRet = FALSE;
    if (_hAccel && (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST))
    {
        fRet = ::TranslateAccelerator(GetSafeHwnd(), _hAccel, pMsg);
    }
    if (!fRet)
    {
        fRet = __super::PreTranslateMessage(pMsg);
    }
    return fRet;
}

BOOL MessageSidePane::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();

    // Set up anchoring for resize
    AddAnchor(IDC_LISTNOUNS, CPoint(0, 0), CPoint(100, 50));
    AddAnchor(IDC_LISTCONDITIONS, CPoint(0, 50), CPoint(100, 100));
    AddAnchor(IDC_STATIC2, CPoint(0, 50), CPoint(100, 50));
    // Hide the sizing grip
    ShowSizeGrip(FALSE);

    return fRet;
}

void MessageSidePane::UpdateNonView(CObject *pObject)
{

}

void MessageSidePane::SetDocument(CDocument *pDoc)
{
    _pDoc = static_cast<CTextDoc*>(pDoc);
    if (_pDoc)
    {
        _pDoc->AddNonViewClient(this);
    }
}

void MessageSidePane::OnGotoVerbs()
{
    //appState->OpenScript(_pDoc->GetNumber());
}
void MessageSidePane::OnGotoTalkers()
{
    //appState->OpenScript(_pDoc->GetNumber());
}