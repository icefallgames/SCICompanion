#include "stdafx.h"
#include "PicHelperPane.h"
#include "PicDoc.h"
#include "Pic.h"
#include "PicOperations.h"

PicHelperPane::PicHelperPane(CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(PicHelperPane::IDD, pParent),
    m_wndZoomCelStatic(11, true, false, PicPosition::PostPlugin),
    m_wndFinal(1, false, true, PicPosition::Final)
{
    _hAccel = nullptr;
    _initialized = false;
    _be = nullptr;
}

PicHelperPane::~PicHelperPane()
{
}

void PicHelperPane::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_STATIC1, m_wndText1);
    DDX_Control(pDX, IDC_STATIC2, m_wndText2);

    DDX_Control(pDX, IDC_CHECKV, m_wndCheckV);
    DDX_Control(pDX, IDC_CHECKP, m_wndCheckP);
    DDX_Control(pDX, IDC_CHECKC, m_wndCheckC);
    DDX_Control(pDX, IDC_CHECKV2, m_wndCheckV2);
    DDX_Control(pDX, IDC_CHECKP2, m_wndCheckP2);
    DDX_Control(pDX, IDC_CHECKC2, m_wndCheckC2);

    DDX_Control(pDX, IDC_STATICZOOM, m_wndZoomCelStatic);

    DDX_Control(pDX, IDC_CHECK_FOLLOWVIEW, m_wndFollowMainView);
    m_wndFollowMainView.SetCheck(TRUE);

    DDX_Control(pDX, IDC_CHECK_FOLLOWVIEW2, m_wndFollowMainView2);
    m_wndFollowMainView2.SetCheck(TRUE);
    
    DDX_Control(pDX, IDC_STATICFINAL, m_wndFinal);
    
    AddAnchor(IDC_STATICZOOM, CPoint(0, 0), CPoint(100, 0));
    AddAnchor(IDC_STATICFINAL, CPoint(0, 0), CPoint(100, 0));

    if (!_initialized)
    {
        m_wndCheckV.SetIcon(IDI_SCREENVISUAL);
        m_wndCheckP.SetIcon(IDI_SCREENPRIORITY);
        m_wndCheckC.SetIcon(IDI_SCREENCONTROL);
        m_wndCheckV2.SetIcon(IDI_SCREENVISUAL);
        m_wndCheckP2.SetIcon(IDI_SCREENPRIORITY);
        m_wndCheckC2.SetIcon(IDI_SCREENCONTROL);
    }

    _Sync();
}

void PicHelperPane::_Sync()
{
    PicScreen screen = m_wndZoomCelStatic.GetCurrentScreen();
    m_wndCheckV.SetCheck(screen == PicScreen::Visual);
    m_wndCheckP.SetCheck(screen == PicScreen::Priority);
    m_wndCheckC.SetCheck(screen == PicScreen::Control);

    screen = m_wndFinal.GetCurrentScreen();
    m_wndCheckV2.SetCheck(screen == PicScreen::Visual);
    m_wndCheckP2.SetCheck(screen == PicScreen::Priority);
    m_wndCheckC2.SetCheck(screen == PicScreen::Control);
}


BEGIN_MESSAGE_MAP(PicHelperPane, CExtDialogFwdCmd)
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_BN_CLICKED(IDC_CHECKV, &PicHelperPane::OnBnClickedCheckv)
    ON_BN_CLICKED(IDC_CHECKP, &PicHelperPane::OnBnClickedCheckp)
    ON_BN_CLICKED(IDC_CHECKC, &PicHelperPane::OnBnClickedCheckc)
    ON_BN_CLICKED(IDC_CHECK_FOLLOWVIEW2, &PicHelperPane::OnBnClickedCheckFollowview2)
    ON_BN_CLICKED(IDC_CHECKV2, &PicHelperPane::OnBnClickedCheckv2)
    ON_BN_CLICKED(IDC_CHECKP2, &PicHelperPane::OnBnClickedCheckp2)
    ON_BN_CLICKED(IDC_CHECKC2, &PicHelperPane::OnBnClickedCheckc2)
    ON_BN_CLICKED(IDC_CHECK_FOLLOWVIEW, &PicHelperPane::OnBnClickedCheckFollowview)
END_MESSAGE_MAP()

BOOL PicHelperPane::OnEraseBkgnd(CDC *pDC)
{
    return TRUE;
}


// PicHelperPane message handlers


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
BOOL PicHelperPane::PreTranslateMessage(MSG* pMsg)
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

BOOL PicHelperPane::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();

    // Set up anchoring for resize
    //AddAnchor(IDC_LISTCOMMANDS, CPoint(0, 0), CPoint(100, 100));
    // Hide the sizing grip
    ShowSizeGrip(FALSE);

    return fRet;
}

void PicHelperPane::UpdateNonView(CObject *pObject)
{
    RECT rc;
    m_wndCheckC.GetClientRect(&rc);
    CString strText;
    m_wndCheckV.GetWindowTextA(strText);

    bool update = false;

    PicChangeHint hint = GetHint<PicChangeHint>(pObject);
    if (IsFlagSet(hint, PicChangeHint::NewPic))
    {
        hint |= PicChangeHint::EditPicPos; // New pic, so we'll update pos changed too.
    }

    if (IsFlagSet(hint, PicChangeHint::EditPicInvalid | PicChangeHint::NewPic))
    {
        update = true;
    }

    if (IsFlagSet(hint, PicChangeHint::EditPicPos))
    {
        update = true;
    }

    if (IsFlagSet(hint, PicChangeHint::ViewScreenChange | PicChangeHint::CursorPosition))
    {
        _be = UnwrapObject<IBitmapEditor*>(pObject);

        if (IsFlagSet(hint, PicChangeHint::ViewScreenChange))
        {
            if (m_wndFollowMainView.GetCheck() != BST_UNCHECKED)
            {
                m_wndZoomCelStatic.SetCurrentScreen(_be->GetPicScreen());
            }
            if (m_wndFollowMainView2.GetCheck() != BST_UNCHECKED)
            {
                m_wndFinal.SetCurrentScreen(_be->GetPicScreen());
            }
            update = true;
        }

        if (IsFlagSet(hint, PicChangeHint::CursorPosition))
        {
            if (_cursorPos != _be->GetCursorPos())
            {
                _cursorPos = _be->GetCursorPos();
                update = true;
            }
        }
    }

    if (update && _be)
    {
        m_wndZoomCelStatic.OnUpdateBitmapEditor(_be);
        m_wndFinal.OnUpdateBitmapEditor(_be);
        _Sync();
    }
}

const PicComponent *PicHelperPane::_GetEditPic()
{
    const PicComponent *pepic = nullptr;
    // Selection changed.  Show a new pic.
    CPicDoc* pDoc = GetDocument();
    if (pDoc)
    {
        pepic = pDoc->GetPic();
    }
    return pepic;
}

PicScreenFlags PicHelperPane::GetRequiredScreens()
{
    return PicScreenToFlags(m_wndFinal.GetCurrentScreen()) | PicScreenToFlags(m_wndZoomCelStatic.GetCurrentScreen());
}

void PicHelperPane::SetDocument(CDocument *pDoc)
{
    _pDoc = static_cast<CPicDoc*>(pDoc);
    UpdateNonView(&WrapHint(PicChangeHint::NewPic));
    if (_pDoc)
    {
        m_wndZoomCelStatic.SetDrawManager(&_pDoc->GetDrawManager());
        m_wndFinal.SetDrawManager(&_pDoc->GetDrawManager());
        _pDoc->AddNonViewClient(this);
        _pDoc->GetDrawManager().AddPicPlugin(this);
    }
}

void PicHelperPane::OnBnClickedCheckFollowview()
{
    if (m_wndFollowMainView.GetCheck() != BST_UNCHECKED)
    {
        if (_be)
        {
            m_wndZoomCelStatic.SetCurrentScreen(_be->GetPicScreen());
        }
        _Sync();
    }
}

void PicHelperPane::OnBnClickedCheckv()
{
    m_wndZoomCelStatic.SetCurrentScreen(PicScreen::Visual);
    m_wndFollowMainView.SetCheck(BST_UNCHECKED);
    _Sync();
}


void PicHelperPane::OnBnClickedCheckp()
{
    m_wndZoomCelStatic.SetCurrentScreen(PicScreen::Priority);
    m_wndFollowMainView.SetCheck(BST_UNCHECKED);
    _Sync();
}


void PicHelperPane::OnBnClickedCheckc()
{
    m_wndZoomCelStatic.SetCurrentScreen(PicScreen::Control);
    m_wndFollowMainView.SetCheck(BST_UNCHECKED);
    _Sync();
}


void PicHelperPane::OnBnClickedCheckFollowview2()
{
    if (m_wndFollowMainView2.GetCheck() != BST_UNCHECKED)
    {
        if (_be)
        {
            m_wndFinal.SetCurrentScreen(_be->GetPicScreen());
        }
        _Sync();
    }
}

void PicHelperPane::OnBnClickedCheckv2()
{
    m_wndFinal.SetCurrentScreen(PicScreen::Visual);
    m_wndFollowMainView2.SetCheck(BST_UNCHECKED);
    _Sync();
}

void PicHelperPane::OnBnClickedCheckp2()
{
    m_wndFinal.SetCurrentScreen(PicScreen::Priority);
    m_wndFollowMainView2.SetCheck(BST_UNCHECKED);
    _Sync();
}


void PicHelperPane::OnBnClickedCheckc2()
{
    m_wndFinal.SetCurrentScreen(PicScreen::Control);
    m_wndFollowMainView2.SetCheck(BST_UNCHECKED);
    _Sync();
}
