// VisualScriptDialog.cpp : implementation file
//

//
// A stab at a visual script editor.  Currently abandoned due to the myriad ways the code
// can specify properties on an item, and how we were only using one form.
//

#include "stdafx.h"

#ifdef VISUAL_SCRIPT_EDITOR
#include "AppState.h"
#include "VisualScriptDialog.h"
#include "ScriptOM.h"
#include "PicResource.h"
#include "ViewResource.h"
#include "ClassBrowser.h"
#include "SCIProps.h"
#include "PicDrawManager.h"

using namespace sci;

// CVisualScriptDialog dialog

IMPLEMENT_DYNAMIC(CVisualScriptDialog, CDialog)
CVisualScriptDialog::CVisualScriptDialog(Script *pScript, SCIClassBrowser *pBrowser, CWnd* pParent /*=NULL*/)
	: CDialog(CVisualScriptDialog::IDD, pParent)
{
    _pScript = pScript;
    _pBrowser = pBrowser;
    _iSelected = -1;
    m_wndVisualScript.SetScript(_pScript, this);
    _size.SetSize(0, 0);
    _fAnimating = FALSE;
    _fMoving = FALSE;
    _fInitialized = false;
}

CVisualScriptDialog::~CVisualScriptDialog()
{
    for (INT_PTR i = 0; i < _views.GetSize(); i++)
    {
        delete _views[i];
    }
}

void InitObjectLogFont(LOGFONT *plogFont)
{
    ZeroMemory(plogFont, sizeof(*plogFont));
    StringCchCopy(plogFont->lfFaceName, ARRAYSIZE(plogFont->lfFaceName), TEXT("Arial"));
    plogFont->lfHeight = -12;
    plogFont->lfWeight = FW_BOLD;
    plogFont->lfCharSet = DEFAULT_CHARSET;
    plogFont->lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE;
}

void CVisualScriptDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

    if (!_fInitialized)
    {
        _fInitialized = true;
        CRect rc;
        GetClientRect(&rc);
        _size.cx = rc.Width();
        _size.cy = rc.Height();

        CRect rcScreen;
        GetWindowRect(&rcScreen);

        // TODO: put this in an if statement for doing only once
        // TODO: also see if we leak gdi objects with this font thing

        CRect rcView;
        GetDlgItem(IDC_STATICVIEW)->GetWindowRect(&rcView);
        ::MapWindowPoints(HWND_DESKTOP, this->GetSafeHwnd(), (LPPOINT)&rcView, 2);
        m_wndVisualScript.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP, rcView, this, 0);

        DDX_Control(pDX, IDC_BUTTONANIMATE, m_wndAnimate);
        DDX_Control(pDX, IDC_BUTTONMOVE, m_wndMove);

        // Hook up the property control
        DDX_Control(pDX, IDC_LISTPROPS, m_wndProperties);
        m_wndProperties.Initialize(_pScript, _pBrowser, GetDlgItem(IDC_PROPDESCRIPTION));
        

        // The control that lists all the objects
        DDX_Control(pDX, IDC_TREEOBJECTS, m_wndObjectTree);

        // The tab control
        DDX_Control(pDX, IDC_TABS, m_wndTabs);
        m_wndTabs.InsertItem(0, TEXT("Initial values"));
        m_wndTabs.InsertItem(1, TEXT("Get coordinates"));

        LOGFONT logFont;
        InitObjectLogFont(&logFont);
        m_fontObjectName.CreateFontIndirect(&logFont);
        GetDlgItem(IDC_OBJECTNAME)->SetFont(&m_fontObjectName);

        // Prepare the views
        if (_pScript)
        {
            // Make a nice title
            WORD w = _pScript->GetScriptNumber();
            TCHAR szMessage[100];
            StringCchPrintf(szMessage, ARRAYSIZE(szMessage), TEXT("Room: %s"), appState->GetResourceMap().FigureOutName(ResourceType::Script, (int)w).c_str());
            SetWindowText(szMessage);

            m_imagelist.Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 5);
            CBitmap bitmapStock;
            int iObjectsIndex = -1;
            int iStaticsIndex = -1;
            if (bitmapStock.LoadBitmap(IDB_BITMAPOBJECTTREE))
            {
                iObjectsIndex = m_imagelist.Add(&bitmapStock, RGB(255, 0, 255));
                if (iObjectsIndex != -1)
                {
                    iStaticsIndex = iObjectsIndex + 1;
                }
            }

            HTREEITEM hitemFirst = NULL;
            INT_PTR iViewIndexFirst = -1;

            // Insert a root guy in the tree
            HTREEITEM htreeRootObjects = m_wndObjectTree.InsertItem(TEXT("Room objects"), iObjectsIndex, iObjectsIndex, TVI_ROOT);
            m_wndObjectTree.SetImageList(&m_imagelist, TVSIL_NORMAL);

            const ClassVector &classes = _pScript->GetClasses();
            for (size_t i = 0; i < classes.size(); i++)
            {
                ClassDefinition *pClass = classes[i].get();
                if (pClass->IsInstance())
                {
                    if (_pBrowser->IsSubClassOf(pClass->GetSuperClass().c_str(), "View"))
                    {
                        // We got a live one.
                        WORD wView;
                        _pBrowser->GetPropertyValue(TEXT("view"), pClass, &wView);
                        WORD wLoop;
                        _pBrowser->GetPropertyValue(TEXT("loop"), pClass, &wLoop);
                        WORD wCel;
                        _pBrowser->GetPropertyValue(TEXT("cel"), pClass, &wCel);
                        CVisualScriptView *pView = new CVisualScriptView(_pBrowser,
                                                                        pClass,
                                                                        wView);
                        if (pView)
                        {
                            pView->AddListener(this);
                            INT_PTR iViewIndex = _views.Add(pView);
                            if (iViewIndexFirst == -1)
                            {
                                iViewIndexFirst = iViewIndex;
                            }

                            // Insert an item in the tree for this guy.
                            ViewResource *pRes = static_cast<ViewResource*>(appState->GetResourceMap().CreateResourceFromNumber(ResourceType::View, wView));
                            if (pRes)
                            {
                                HBITMAP hbmp = pRes->GetBitmap(CelIndex(wLoop, wCel), 16, 16);
                                if (hbmp)
                                {
                                    CBitmap bitmap;
                                    bitmap.Attach(hbmp);
                                    int iImageIndex = m_imagelist.Add(&bitmap, EGA_TO_COLORREF(pRes->GetTransparentColor(CelIndex(wLoop, wCel))));
                                    HTREEITEM hitem = m_wndObjectTree.InsertItem(TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM,
                                        pClass->GetName().c_str(), iImageIndex, iImageIndex, 0, 0, (LPARAM)static_cast<ISCIPropertyBag*>(pView), htreeRootObjects, TVI_LAST);
                                    if (hitemFirst == NULL)
                                    {
                                        hitemFirst = hitem;
                                    }
                                }
                                delete pRes;
                            }
                        }
                    }
                    else if (_pBrowser->IsSubClassOf(pClass->GetSuperClass().c_str(), "Rm"))
                    {
                        // Figure out the pic.
                        WORD wPic;
                        _pBrowser->GetPropertyValue("picture", pClass, &wPic);
                        m_wndVisualScript.SetPic(wPic);
                    }
                }
            }
            m_wndObjectTree.Expand(htreeRootObjects, TVE_EXPAND);

            HTREEITEM htreeRootViews = m_wndObjectTree.InsertItem(TEXT("Static views"), iStaticsIndex, iStaticsIndex, TVI_ROOT);

            if (hitemFirst)
            {
                // Select the first object item:
                m_wndObjectTree.Select(hitemFirst, TVGN_CARET);
                // It won't produce a selection change, so do it ourselves:
                SetSelectedViewIndex((int)iViewIndexFirst);
            }

        }
        // Need to call this any time the number of views changed.
        m_wndVisualScript.UpdateViews();
    }
}

void CVisualScriptDialog::SetSelectedViewIndex(int iSelected)
{
    if (_iSelected != iSelected)
    {
        _iSelected = max(-1, iSelected);
        _iSelected = min(_iSelected, (int)_views.GetUpperBound());

        // Selection changed - reflect that in the property control.
        if (_iSelected != -1)
        {
            m_wndProperties.SetControl(_views[_iSelected]);
            _UpdateNameDisplay();
        }
        // Repaint our picture
        m_wndVisualScript.Invalidate(FALSE);
    }
}

void CVisualScriptDialog::OnPropertyChange(ISCIPropertyBag *pSource, PCTSTR pszName, PropertyValue value)
{
    // Repaint?
    m_wndVisualScript.Invalidate(FALSE);
}

void CVisualScriptDialog::_UpdateNameDisplay()
{
    // Update the name of the thing
    TCHAR szText[200];
    StringCchPrintf(szText, ARRAYSIZE(szText), TEXT("%s of %s"), _views[_iSelected]->GetBagName().c_str(),
            _views[_iSelected]->GetSuperClass().c_str());
    GetDlgItem(IDC_OBJECTNAME)->SetWindowText(szText);
}

void CVisualScriptDialog::OnNameChange(ISCIPropertyBag *pSource, PCTSTR pszName)
{
    // Update the name thing
    if (_iSelected != -1)
    {
        _UpdateNameDisplay();
    }
}

BEGIN_MESSAGE_MAP(CVisualScriptDialog, CDialog)
    ON_WM_SIZE()
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREEOBJECTS, OnTvnSelchanged)
    ON_BN_CLICKED(IDC_BUTTONANIMATE, OnAnimateClicked)
    ON_BN_CLICKED(IDC_BUTTONMOVE, OnMoveClicked)
    ON_WM_TIMER()
END_MESSAGE_MAP()


#define ID_TIMER_ANIMATE 0

void CVisualScriptDialog::OnAnimateClicked()
{
    _fAnimating = !_fAnimating;
    if (!_fAnimating)
    {
        _fMoving = FALSE;
        KillTimer(ID_TIMER_ANIMATE);
        // Reset cycling on the views:
        for (INT_PTR i = 0; i < _views.GetSize(); i++)
        {
            _views[i]->Cycle(FALSE);
        }
        m_wndVisualScript.Invalidate(FALSE);
    }
    else
    {
        SetTimer(ID_TIMER_ANIMATE, 100, NULL); // Each SCI cycle is 100ms.
    }
    m_wndAnimate.SetCheck(_fAnimating);
    m_wndMove.SetCheck(_fMoving);
}

void CVisualScriptDialog::OnMoveClicked()
{
    _fMoving = !_fMoving;

    if (!_fMoving)
    {
        _fAnimating = FALSE;
        KillTimer(ID_TIMER_ANIMATE);
        // Reset cycling on the views:
        for (INT_PTR i = 0; i < _views.GetSize(); i++)
        {
            _views[i]->Cycle(FALSE);
        }
        m_wndVisualScript.Invalidate(FALSE);
    }
    else
    {
        _fAnimating = TRUE;
        SetTimer(ID_TIMER_ANIMATE, 100, NULL); // Each SCI cycle is 100ms.
    }
    m_wndAnimate.SetCheck(_fAnimating);
    m_wndMove.SetCheck(_fMoving);
}

void CVisualScriptDialog::OnTimer(UINT_PTR nIDEvent)
{
    if (_fAnimating && (nIDEvent == ID_TIMER_ANIMATE))
    {
        // Cycle everyone.
        for (INT_PTR i = 0; i < _views.GetSize(); i++)
        {
            _views[i]->Cycle();
            if (_fMoving)
            {
                _views[i]->Move();
            }
        }
        // Make sure we redraw
        m_wndVisualScript.Invalidate(FALSE);
    }
}

// CVisualScriptDialog message handlers
void CVisualScriptDialog::OnSize(UINT nType, int cx, int cy)
{
    if ((_size.cx != 0) && (_size.cy != 0))
    {
        CPoint ptDelta(cx - _size.cx, cy - _size.cy);
        //CPoint ptDelta(_size.cx - cx, _size.cy - cy);
        if (ptDelta != CPoint(0, 0))
        {

            // Controls that move
            const int c_ids[] = { IDOK, IDCANCEL, IDC_LISTPROPS, IDC_OBJECTNAME, IDC_PROPDESCRIPTION };
            for (int i = 0; i < ARRAYSIZE(c_ids); i++)
            {
                CWnd *pwnd = GetDlgItem(c_ids[i]);
                CRect rc;
                pwnd->GetWindowRect(&rc);
                ::MapWindowPoints(HWND_DESKTOP, this->GetSafeHwnd(), (LPPOINT)&rc, 2);
                pwnd->SetWindowPos(NULL, rc.left + ptDelta.x, rc.top + ptDelta.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            }

            // Controls that move horizontally.
            const int c_ids2[] = { IDC_BUTTONMOVE, IDC_BUTTONANIMATE };
            for (int i = 0; i < ARRAYSIZE(c_ids2); i++)
            {
                CWnd *pwnd = GetDlgItem(c_ids2[i]);
                CRect rc;
                pwnd->GetWindowRect(&rc);
                ::MapWindowPoints(HWND_DESKTOP, this->GetSafeHwnd(), (LPPOINT)&rc, 2);
                pwnd->SetWindowPos(NULL, rc.left + ptDelta.x, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            }

            // Special case for scriptview
            CRect rc;
            m_wndVisualScript.GetWindowRect(&rc);
            ::MapWindowPoints(HWND_DESKTOP, this->GetSafeHwnd(), (LPPOINT)&rc, 2);
            m_wndVisualScript.SetWindowPos(NULL, 0, 0, rc.Width() + ptDelta.x, rc.Height() + ptDelta.y, SWP_NOMOVE | SWP_NOZORDER);

            // And tabs.
            m_wndTabs.GetWindowRect(&rc);
            ::MapWindowPoints(HWND_DESKTOP, this->GetSafeHwnd(), (LPPOINT)&rc, 2);
            m_wndTabs.SetWindowPos(NULL, 0, 0, rc.Width() + ptDelta.x, rc.Height(), SWP_NOMOVE | SWP_NOZORDER);

            // Object tree
            m_wndObjectTree.GetWindowRect(&rc);
            ::MapWindowPoints(HWND_DESKTOP, this->GetSafeHwnd(), (LPPOINT)&rc, 2);
            m_wndObjectTree.SetWindowPos(NULL, rc.left + ptDelta.x, rc.top, rc.Width(), rc.Height() + ptDelta.y, SWP_NOZORDER);

            _size.cx = cx;
            _size.cy = cy;
        }
    }
}

void CVisualScriptDialog::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    ISCIPropertyBag *pBag = (ISCIPropertyBag *)pNMTreeView->itemNew.lParam;
    if (pBag)
    {
        for (INT_PTR i = 0; i < _views.GetSize(); i++)
        {
            if (static_cast<ISCIPropertyBag*>(_views.GetAt(i)) == pBag)
            {
                SetSelectedViewIndex((int)i);
                break;
            }
        }
    }
    else
    {
        SetSelectedViewIndex(-1);
    }
}

BEGIN_MESSAGE_MAP(CVisualScript, CScrollingThing<CWnd>)
    ON_WM_PAINT()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_ERASEBKGND()
    ON_WM_KILLFOCUS()
    ON_WM_SETFOCUS()
    ON_WM_KEYDOWN()
    ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

CVisualScript::~CVisualScript()
{
    _CleanDoubleBuffer();
    _CleanUp();
}

void CVisualScript::_CleanUp()
{
    delete _pPic;
    delete [] _pdataDisplay;
    delete [] _pdataAux;
}

void CVisualScriptView::OnDraw(BYTE *pDataDisplay, const BYTE *pDataPriority, int iPriorityLevel, BOOL fOutline)
{
    // Draw ourselves to the dc, but only if this is the right priority level
    int iPri = (_iPri == -1) ? PriorityFromY((WORD)_y) : (WORD)_iPri;
    if ((iPriorityLevel == iPri) && _pView)
    {
        DrawViewWithPriority(pDataDisplay,
            pDataPriority,
            iPri,
            (WORD)_GetX(),
            (WORD)(_GetY() - _z),
            _pView,
            _nLoop,
            _GetCel(),
            fOutline);
    }
}

BOOL CVisualScriptView::HitTestView(CPoint ptPic)
{
    return ::HitTestView((WORD)ptPic.x, (WORD)ptPic.y, (WORD)_GetX(), (WORD)(_GetY() - _z), _pView, _nLoop, _GetCel());
}

void CVisualScriptView::GetViewRect(CRect *prc)
{
    if (_pView)
    {
        ::GetViewRect(prc, (WORD)_GetX(), (WORD)(_GetY() - _z), _pView, _nLoop, _GetCel());
    }
    else
    {
        prc->SetRectEmpty();
    }
}

void CVisualScriptView::Offset(CPoint ptDelta)
{
    // Assign to local vars, since they will change.
    int x = _x;
    int y = _y;

    PropertyValue value;
    value.SetValue((WORD)(x + (__int16)ptDelta.x));
    SetProperty(TEXT("x"), value);
    value.SetValue((WORD)(y + (__int16)ptDelta.y));
    SetProperty(TEXT("y"), value);
}


void CVisualScriptView::_CacheProperties(BOOL fFirstTime)
{
    int nLoop = _nLoop;
    int nCel = _nCel;
    WORD wView = _wView;
    _pBrowser->GetPropertyValue(SCIPROP_LOOP, this, _pClass, (WORD*)&_nLoop);
    _pBrowser->GetPropertyValue(SCIPROP_CEL, this, _pClass, (WORD*)&_nCel);
    _pBrowser->GetPropertyValue(SCIPROP_X, this, _pClass, (WORD*)&_x);
    _pBrowser->GetPropertyValue(SCIPROP_Y, this, _pClass, (WORD*)&_y);
    _pBrowser->GetPropertyValue(SCIPROP_Z, this, _pClass, (WORD*)&_z);
    _pBrowser->GetPropertyValue(SCIPROP_CYCLESPEED, this, _pClass, (WORD*)&_cycleSpeed);
    _pBrowser->GetPropertyValue(SCIPROP_MOVESPEED, this, _pClass, (WORD*)&_moveSpeed);
    _pBrowser->GetPropertyValue(SCIPROP_XSTEP, this, _pClass, (WORD*)&_xStep);
    _pBrowser->GetPropertyValue(SCIPROP_YSTEP, this, _pClass, (WORD*)&_yStep);
    _pBrowser->GetPropertyValue(SCIPROP_PRIORITY, this, _pClass, (WORD*)&_iPri);
    _pBrowser->GetPropertyValue(SCIPROP_VIEW, this, _pClass, &_wView);

    // If loop or cel are different, reload image.
    if (!fFirstTime)
    {
        if ((nLoop != _nLoop) || (nCel != _nCel) || (_wView != wView))
        {
            _RefreshView();
        }
    }

    _fActor = _pBrowser->IsSubClassOf(GetSpecies().c_str(), "Act");
}



void CVisualScriptView::SetPriority(int iPri)
{
    _iPri = iPri;
    PropertyValue value;
    value.SetValue((WORD)_iPri);
    SetProperty(SCIPROP_PRIORITY, value);
}

void CVisualScriptView::Cycle(BOOL fCycle)
{
    if (fCycle)
    {
        if (_cycleSpeed == -1)
        {
            _cycleSpeed = 0; // Avoid div by zero.
        }

        // Only increment the cycle when our _cycleSpeedCount has reached zero again.
        _cycleSpeedCount = (_cycleSpeedCount + 1) % (_cycleSpeed + 1);
        if (_cycleSpeedCount == 0)
        {
            _iCycle++;
            int nCels = _pView->GetCelCount(_nLoop);
            _iCycle = _iCycle % nCels;
        }
    }
    else
    {
        _iCycle = -1;
        _fMoving = FALSE;
    }
}

void CVisualScriptView::Move()
{
    if (_fActor)
    {
        // Move in a 50 x 50 square
        if (!_fMoving)
        {
            _xTemp = _x;
            _yTemp = _y + 20;
            _iDir = 0; // right
        }
        _fMoving = TRUE;
        if (_moveSpeed == -1)
        {
            _moveSpeed = 0; // Avoid div by zero
        }
        _moveSpeedCount = (_moveSpeedCount + 1) % (_moveSpeed + 1);
        if (_moveSpeedCount == 0)
        {
            switch (_iDir)
            {
            case 0:
                _xTemp += _xStep;
                if (_xTemp > (_x + 20))
                {
                    _iDir = 1; // Up
                }
                break;
            case 1:
                _yTemp -= _yStep;
                if (_yTemp < (_y - 20))
                {
                    _iDir = 2; // left
                }
                break;

            case 2:
                _xTemp -= _xStep;
                if (_xTemp < (_x - 20))
                {
                    _iDir = 3; // down
                }
                break;
            case 3:
                _yTemp += _yStep;
                if (_yTemp > (_y + 20))
                {
                    _iDir = 0; // right
                }
                break;
            }
        }
    }
}

void CVisualScriptView::_RefreshView()
{
    delete _pView;
    _pView = static_cast<ViewResource*>(appState->GetResourceMap().CreateResourceFromNumber(ResourceType::View, _wView));

}


CVisualScriptView::~CVisualScriptView()
{
    delete _pView;
}



//
// CVisualScript
//
void CVisualScript::_CleanDoubleBuffer()
{
    // Reset our double buffering, so we regenerate it on the next paint cycle.
    _fDoubleBuf = FALSE;
    delete _pbitmapDoubleBuf;
    _pbitmapDoubleBuf = FALSE;
}


void CVisualScript::_GenerateDoubleBuffer(CDC *pDC)
{
    ASSERT(!_pbitmapDoubleBuf);
    _pbitmapDoubleBuf = new CBitmap();
    if (_pbitmapDoubleBuf)
    {
        _fDoubleBuf = _pbitmapDoubleBuf->CreateCompatibleBitmap(pDC, sPIC_WIDTH, sPIC_HEIGHT);
    }
}


void CVisualScript::_GeneratePicResource()
{
    if (_pPic)
    {
        _CleanUp();
    }
    if (_pScript)
    {
        _pPic = static_cast<PicResource*>(appState->GetResourceMap().CreateResourceFromNumber(ResourceType::Pic, _wPic));
        if (_pPic)
        {
            _pdm.SetPic(_pPic);
            ASSERT(_pdataAux == NULL && _pdataDisplay == NULL);
            _pdataDisplay = new BYTE[BMPSIZE];
            _pdataAux = new BYTE[BMPSIZE];
            if (!_pdataDisplay || !_pdataAux)
            {
                _CleanUp();
            }
        }
    }
}

CPoint CVisualScript::_MapClientPointToPic(CPoint ptClient)
{
    return CPoint((ptClient.x + _xOrigin) / _iZoom,
                  (ptClient.y + _yOrigin) / _iZoom);
}

void CVisualScript::_OnDraw(CDC *pDC)
{
    if (_pPic)
    {
        BITMAPINFO *pbmi = NULL;
        _pdm.SetPic(_pPic);
        _pdm.CopyBitmap(PicScreenFlags::Visual, _pdataDisplay, _pdataAux, &pbmi);

        CVisualScriptViewArray &views = _pDialog->GetViews();
        // Draw from from front to back.
        for (int iPriLevel = 0; iPriLevel < 16; iPriLevel++)
        {
            for (INT_PTR i = 0; i < views.GetSize(); i++)
            {
                views[i]->OnDraw(_pdataDisplay, _pdm.GetPriorityBits(), iPriLevel, _viewHotState[i]);
            }
        }

        StretchDIBits((HDC)*pDC, 0, 0, sPIC_WIDTH, sPIC_HEIGHT, 0, 0, sPIC_WIDTH, sPIC_HEIGHT, _pdataDisplay, pbmi, DIB_RGB_COLORS, SRCCOPY);
        HGDIOBJ hOldBrush = pDC->SelectObject(&_brushHollow);
        for (INT_PTR i = 0; i < views.GetSize(); i++)
        {
            if (_pDialog->GetSelectedViewIndex() == i)
            {
                // highlight the mofo
                HBITMAP hbmOverlay = LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_ALPHABLENDWHITE));
                if (hbmOverlay)
                {
                    CDC dcMem;
                    if (dcMem.CreateCompatibleDC(pDC))
                    {
                        HGDIOBJ hOld = dcMem.SelectObject(hbmOverlay);
                        BLENDFUNCTION blend = { 0 };
                        blend.AlphaFormat = 0;
                        blend.SourceConstantAlpha = 128;
                        blend.BlendFlags = 0;
                        blend.BlendOp = AC_SRC_OVER;
                        CRect rectFinal;
                        views[i]->GetViewRect(&rectFinal);
                        pDC->AlphaBlend(rectFinal.left, rectFinal.top, rectFinal.Width(), rectFinal.Height(),
                            &dcMem, 0, 0, 32, 4, blend);
                        dcMem.SelectObject(hOld);
                    }
                    DeleteObject(hbmOverlay);
                }
            }
            if (_viewHotState[i] || ((_pDialog->GetSelectedViewIndex() == i) && (GetFocus() == this)))
            {
                CPen pen(PS_SOLID, 1, (_pDialog->GetSelectedViewIndex() == i) ? RGB(255, 0, 0) : RGB(255, 255, 255));
                HGDIOBJ hOldPen = pDC->SelectObject(&pen);
                CRect rectView;
                views[i]->GetViewRect(&rectView);
                pDC->Rectangle(&rectView);
                pDC->SelectObject(hOldPen);
            }
        }
        pDC->SelectObject(hOldBrush);
        delete pbmi;
    }
}


void CVisualScript::OnPaint()
{
	// standard paint routine
    RECT rcClient;
    GetClientRect(&rcClient);
	CPaintDC dc(this);
    CDC dcMem;
    if (dcMem.CreateCompatibleDC(&dc))
    {
        int cxVisible = min(_GetViewWidth(), RECTWIDTH(rcClient));
        int cyVisible = min(_GetViewHeight(), RECTHEIGHT(rcClient));
        if (!_fDoubleBuf)
        {
            _GenerateDoubleBuffer(&dc);
        }

        if (_fDoubleBuf)
        {
            ASSERT(_pbitmapDoubleBuf);
            HGDIOBJ hgdiObj = dcMem.SelectObject(_pbitmapDoubleBuf);

            // Fill background with white.
            dcMem.FillSolidRect(0, 0, sPIC_WIDTH, sPIC_HEIGHT, RGB(0xff, 0xff, 0xff));

            // Draw the picture.
            _OnDraw(&dcMem);

            // Now blt back to the real DC.
            dc.StretchBlt(-_xOrigin, -_yOrigin, _GetViewWidth(), _GetViewHeight(), &dcMem, 0, 0, sPIC_WIDTH, sPIC_HEIGHT, SRCCOPY); 

            dcMem.SelectObject(hgdiObj);
        }
    }

    // Finish off by drawing a navy background around the font.
    CBrush brush(RGB(0, 0, 128));
    int cyBottom = RECTHEIGHT(rcClient) - _GetViewHeight();
    if (cyBottom > 0)
    {
        CRect rc(CPoint(0, _GetViewHeight()), CSize(RECTWIDTH(rcClient), cyBottom));
        dc.FillRect(&rc, &brush);
    }
    int cxLeft = RECTWIDTH(rcClient) -  _GetViewWidth();
    if (cxLeft > 0)
    {
        CRect rc(CPoint(_GetViewWidth(), 0), CSize(cxLeft, RECTHEIGHT(rcClient)));
        dc.FillRect(&rc, &brush);
    }
}

void CVisualScript::OnMouseMove(UINT nFlags, CPoint point)
{
    CPoint ptPic = _MapClientPointToPic(point);
    if (_fInCapture)
    {
        CPoint ptDelta = ptPic - _ptCapture;
        _ptCapture = ptPic;

        // Adjust the position of the selected view.
        int i = _pDialog->GetSelectedViewIndex();
        ASSERT(i != -1);
        CVisualScriptViewArray &views = _pDialog->GetViews();
        views[i]->Offset(ptDelta);
        Invalidate(FALSE);
    }
    else
    {
        CVisualScriptViewArray &views = _pDialog->GetViews();
        for (INT_PTR i = 0; i < views.GetSize(); i++)
        {
            BOOL fHot = views[i]->HitTestView(ptPic);
            if (_viewHotState[i] != fHot)
            {
                // TODO -> only invalidate a small piece.
                Invalidate(FALSE);
                _viewHotState[i] = fHot;
            }
        }
    }
    __super::OnMouseMove(nFlags, point);
}

void CVisualScript::_SelectItemFromPicPoint(CPoint point)
{
    // If the item under the point is already the selected item,
    // don't change selection (e.g. to another item that is already
    // under the point).  This way when items overlap, the user
    // can choose an item in a different way (e.g. treeview), and still manipulate
    // it.
    CVisualScriptViewArray &views = _pDialog->GetViews();
    int iSelected = _pDialog->GetSelectedViewIndex();
    if ((iSelected != -1) && (views[iSelected]->HitTestView(point)))
    {
        // Keep this guy selected
    }
    else
    {
        // Select the first view we encounter under the point
        INT_PTR i = 0;
        for (; i < views.GetSize(); i++)
        {
            BOOL fSelected = views[i]->HitTestView(point);
            if (fSelected)
            {
                _pDialog->SetSelectedViewIndex((int)i);
                Invalidate(FALSE);
                break;
            }
        }
        if (i == views.GetSize())
        {
            _pDialog->SetSelectedViewIndex(-1);
            Invalidate(FALSE);
        }
    }
}

void CVisualScript::OnLButtonDown(UINT nFlags, CPoint point)
{
    // Set focus to ourselves
    SetFocus();

    CPoint ptPic = _MapClientPointToPic(point);
    _SelectItemFromPicPoint(ptPic);
    
    if (!_fInCapture && (_pDialog->GetSelectedViewIndex() != -1))
    {
        // Start dragging
        _fInCapture = TRUE;
        _ptCapture = ptPic;
        SetCapture();
    }
    __super::OnLButtonDown(nFlags, point);
}


void CVisualScript::OnRButtonDown(UINT nFlags, CPoint point)
{
    // Set focus to ourselves
    SetFocus();

    CPoint ptPic = _MapClientPointToPic(point);
    _SelectItemFromPicPoint(ptPic);

    __super::OnRButtonDown(nFlags, point);

    // Bring up a menu that let's us choose the view's priority.
    if (_pDialog->GetSelectedViewIndex() != -1)
    {
        CMenu contextMenu; 
        contextMenu.LoadMenu(IDR_MENUFAKEEGO); 
        CMenu* pTracker; 
        pTracker = contextMenu.GetSubMenu(0); 
        CPoint ptScreen = point;
        ClientToScreen(&ptScreen);
        if (pTracker)
        {
            UINT nID = pTracker->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_NONOTIFY |TPM_RETURNCMD, 
                                                    ptScreen.x, ptScreen.y, AfxGetMainWnd());
            if (nID)
            {
                int iPri = nID - ID_MAIN_PRI0;
                CVisualScriptViewArray &views = _pDialog->GetViews();
                views.GetAt(_pDialog->GetSelectedViewIndex())->SetPriority(iPri);
            }
            Invalidate(FALSE);
        }
    }
}


void CVisualScript::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (_fInCapture)
    {
        _fInCapture = FALSE;
        ReleaseCapture();
    }
    __super::OnLButtonUp(nFlags, point);
}


void CVisualScript::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    int iCurrentSelection = _pDialog->GetSelectedViewIndex();
    // Select next guy under mouse cursor
    iCurrentSelection++;
    CPoint ptPic = _MapClientPointToPic(point);
    CVisualScriptViewArray &views = _pDialog->GetViews();
    for (INT_PTR i = 0; i < views.GetSize(); i++, iCurrentSelection++)
    {
        iCurrentSelection = iCurrentSelection % (int)views.GetSize();
        BOOL fSelected = views[iCurrentSelection]->HitTestView(ptPic);
        if (fSelected)
        {
            _pDialog->SetSelectedViewIndex((int)iCurrentSelection);
            Invalidate(FALSE);
            break;
        }
    }
    __super::OnLButtonDown(nFlags, point);
}

//
// prevent flicker
//
BOOL CVisualScript::OnEraseBkgnd(CDC *pDC)
{
    return TRUE;
}

void CVisualScript::OnKillFocus(CWnd *pNewWnd)
{
    __super::OnKillFocus(pNewWnd);
    if (_fInCapture)
    {
        _fInCapture = FALSE;
        ReleaseCapture();
    }
    Invalidate(FALSE);
}

void CVisualScript::OnSetFocus(CWnd *pNewWnd)
{
    __super::OnSetFocus(pNewWnd);
    Invalidate(FALSE);
}


void CVisualScript::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CPoint pt(0, 0);
    if (GetKeyState(VK_CONTROL) & 0x8000)
    {
        int i = _pDialog->GetSelectedViewIndex();
        // Ctrl key held down - move among items.
        if ((nChar == VK_UP) || (nChar == VK_LEFT))
        {
            // backward
            int cItems = (int)_pDialog->GetViews().GetSize();
            i = (i - 1 + cItems) % cItems;
            _pDialog->SetSelectedViewIndex(i);
        }
        else if ((nChar == VK_DOWN) || (nChar == VK_RIGHT))
        {
            // forward
            i = (i + 1) % (int)_pDialog->GetViews().GetUpperBound();
            _pDialog->SetSelectedViewIndex(i);
        }
    }
    else
    {
        switch (nChar)
        {
        case VK_UP:
            pt.y = -1;
            break;

        case VK_DOWN:
            pt.y = 1;
            break;

        case VK_LEFT:
            pt.x = -1;
            break;

        case VK_RIGHT:
            pt.x = 1;
            break;

        case VK_TAB:
            if (GetKeyState(VK_SHIFT) & 0x8000)
            {
                // back
                CWnd *pPrev = _pDialog->GetNextDlgTabItem(this, TRUE);
                if (pPrev)
                {
                    pPrev->SetFocus();
                }
            }
            else
            {
                // fwd
                CWnd *pNext = _pDialog->GetNextDlgTabItem(this, FALSE);
                if (pNext)
                {
                    pNext->SetFocus();
                }
            }
            break;
        }
    }

    if (pt.x != 0 || pt.y != 0)
    {
        // Adjust the position of the selected view.
        int i = _pDialog->GetSelectedViewIndex();
        if (i != -1)
        {
            CVisualScriptViewArray &views = _pDialog->GetViews();
            views[i]->Offset(pt);
        }
    }
    __super::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CVisualScript::UpdateViews()
{
    // Reset our hot state
    CVisualScriptViewArray &views = _pDialog->GetViews();
    _viewHotState.RemoveAll();
    _viewHotState.SetSize(views.GetSize());
}


#endif