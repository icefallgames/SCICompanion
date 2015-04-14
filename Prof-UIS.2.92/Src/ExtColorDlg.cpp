// This is part of the Professional User Interface Suite library.
// Copyright (C) 2001-2011 FOSS Software, Inc.
// All rights reserved.
//
// http://www.prof-uis.com
// mailto:support@prof-uis.com
//
// This source code can be used, modified and redistributed
// under the terms of the license agreement that is included
// in the Professional User Interface Suite package.
//
// Warranties and Disclaimers:
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND
// INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
// IN NO EVENT WILL FOSS SOFTWARE INC. BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES,
// INCLUDING DAMAGES FOR LOSS OF PROFITS, LOSS OR INACCURACY OF DATA,
// INCURRED BY ANY PERSON FROM SUCH PERSON'S USAGE OF THIS SOFTWARE
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

#include "stdafx.h"

#if (!defined __EXTCOLORDLG_H)
	#include <ExtColorDlg.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_CONTROLS_COMMON_H)
	#include <ExtControlsCommon.h>
#endif

#if (!defined __EXT_LOCALIZATION_H)
	#include <../Src/ExtLocalization.h>
#endif

#include <Resources/Resource.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtColorDlg dialog

IMPLEMENT_DYNAMIC( CExtColorDlg, CExtResizableDialog );

CExtColorDlg::CExtColorDlg(
	COLORREF clrInit /*= RGB(255,255,255)*/ ,
	COLORREF clrNew /*= RGB(255,255,255)*/ ,
	CWnd* pParent /*=NULL*/
	)
	: __BASEOF_CExtColorDlg__ ( IDD_EXT_COLOR_DIALOG, pParent )
	, m_clrInit( clrInit )
	, m_clrNew( clrNew )
	, m_strCaption( _T("") )
	, m_bDialogInitColplete( false )
	, m_bSyncingValues( false )
	, m_wndColorCtrl( CExtColorCtrl::MODE_HEXAGON )
	, m_wndColorDiff( CExtColorCtrl::MODE_DIFF )
{
	//{{AFX_DATA_INIT(CExtColorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDD_EXT_COLOR_DIALOG);
	ASSERT( m_hIcon != NULL );
}

CExtColorDlg::~CExtColorDlg()
{
	ASSERT( m_hIcon != NULL );
	if( m_hIcon != NULL )
		::DestroyIcon( m_hIcon );
}

void CExtColorDlg::DoDataExchange(CDataExchange* pDX)
{
	__BASEOF_CExtColorDlg__ :: DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtColorDlg)
	DDX_Control(pDX, IDC_SPIN_C, m_SpinC);
	DDX_Control(pDX, IDC_SPIN_M, m_SpinM);
	DDX_Control(pDX, IDC_SPIN_Y, m_SpinY);
	DDX_Control(pDX, IDC_SPIN_S, m_SpinS);
	DDX_Control(pDX, IDC_SPIN_R, m_SpinR);
	DDX_Control(pDX, IDC_SPIN_L, m_SpinL);
	DDX_Control(pDX, IDC_SPIN_H, m_SpinH);
	DDX_Control(pDX, IDC_SPIN_G, m_SpinG);
	DDX_Control(pDX, IDC_SPIN_B, m_SpinB);
	DDX_Control(pDX, IDC_EDIT_S, m_EditS);
	DDX_Control(pDX, IDC_EDIT_R, m_EditR);
	DDX_Control(pDX, IDC_EDIT_L, m_EditL);
	DDX_Control(pDX, IDC_EDIT_H, m_EditH);
	DDX_Control(pDX, IDC_EDIT_G, m_EditG);
	DDX_Control(pDX, IDC_EDIT_B, m_EditB);
	DDX_Control(pDX, IDC_EDIT_C, m_EditC);
	DDX_Control(pDX, IDC_EDIT_M, m_EditM);
	DDX_Control(pDX, IDC_EDIT_Y, m_EditY);
	DDX_Control(pDX, IDC_COMBO_SEL_TYPE, m_ComboType);
	DDX_Control(pDX, IDCANCEL, m_BtnCancel);
	DDX_Control(pDX, IDOK, m_BtnOK);
	DDX_Control(pDX, IDC_STATIC_COLORS_TEXT, m_labelColors);
	DDX_Control(pDX, IDC_STATIC_R, m_labelChR);
	DDX_Control(pDX, IDC_STATIC_G, m_labelChG);
	DDX_Control(pDX, IDC_STATIC_B, m_labelChB);
	DDX_Control(pDX, IDC_STATIC_C, m_labelChC);
	DDX_Control(pDX, IDC_STATIC_M, m_labelChM);
	DDX_Control(pDX, IDC_STATIC_Y, m_labelChY);
	DDX_Control(pDX, IDC_STATIC_H, m_labelChH);
	DDX_Control(pDX, IDC_STATIC_S, m_labelChS);
	DDX_Control(pDX, IDC_STATIC_L, m_labelChL);
	DDX_Control(pDX, IDC_STATIC_TXT_NEW, m_labelTxtNew);
	DDX_Control(pDX, IDC_STATIC_TXT_CURRENT, m_labelTxtCurrent);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP( CExtColorDlg, CExtResizableDialog )
	//{{AFX_MSG_MAP(CExtColorDlg)
	ON_WM_CONTEXTMENU()
	ON_CBN_SELENDOK(IDC_COMBO_SEL_TYPE, OnSelendokComboSelType)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(CExtColorCtrl::g_nMsgColorChanged,OnColorChanged)
	ON_EN_CHANGE(IDC_EDIT_C, SyncFromEditCMY)
	ON_EN_CHANGE(IDC_EDIT_M, SyncFromEditCMY)
	ON_EN_CHANGE(IDC_EDIT_Y, SyncFromEditCMY)
	ON_EN_CHANGE(IDC_EDIT_R, SyncFromEditRGB)
	ON_EN_CHANGE(IDC_EDIT_B, SyncFromEditRGB)
	ON_EN_CHANGE(IDC_EDIT_G, SyncFromEditRGB)
	ON_EN_CHANGE(IDC_EDIT_H, SyncFromEditHSL)
	ON_EN_CHANGE(IDC_EDIT_S, SyncFromEditHSL)
	ON_EN_CHANGE(IDC_EDIT_L, SyncFromEditHSL)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtColorDlg message handlers

BOOL CExtColorDlg::OnInitDialog()
{
	m_bDialogInitColplete = false;

	if( ! __BASEOF_CExtColorDlg__ :: OnInitDialog() )
	{
		ASSERT( FALSE );
		return FALSE;
	}

	m_ComboType.ResetContent();
	ASSERT( m_ComboType.GetCount() == 0 );
	for(	int i = int(CExtColorCtrl::MODE_NAMED_MIN);
			i <= int(CExtColorCtrl::MODE_NAMED_MAX);
			i++
			)
			m_ComboType.AddString(
				CExtColorCtrl::GetColorModeName(
					CExtColorCtrl::e_mode_t(i)
					)
				);


	ASSERT( m_hIcon != NULL );
	SetIcon( m_hIcon, TRUE );
	SetIcon( m_hIcon, FALSE );

	if( !m_strCaption.IsEmpty() )
	{
		int nCP = m_strCaption.ReverseFind( _T('\n') );
		if( nCP > 0 )
		{
			int nLen = m_strCaption.GetLength();
			ASSERT( nLen > 0 );
			int nCPR = nLen - nCP - 1;
			m_strCaption = m_strCaption.Right( nCPR );
		}
		if( !m_strCaption.IsEmpty() )
			SetWindowText( m_strCaption );
	}

	m_wndColorCtrl.m_selClrs.m_clrCurrent = m_clrInit;
	m_wndColorCtrl.m_selClrs.m_clrPrev = m_clrInit;
	m_wndColorCtrl.m_selClrs.m_clrNew = m_clrNew;

	m_wndColorDiff.m_selClrs.m_clrCurrent = m_clrInit;
	m_wndColorDiff.m_selClrs.m_clrPrev = m_clrInit;
	m_wndColorDiff.m_selClrs.m_clrNew = m_clrInit; //m_clrNew;

	//m_ComboType.SetCurSel( 0 );
	m_ComboType.SetCurSel( (int)(CExtColorCtrl::e_mode_t)m_wndColorCtrl );

	AddAnchor( IDC_COMBO_SEL_TYPE, __RDA_LT, __RDA_RT );
	AddAnchor( IDC_STATIC_HEX_FULL, __RDA_LT, __RDA_RB );
	AddAnchor( IDC_SPIN_R, __RDA_RT );
	AddAnchor( IDC_STATIC_R, __RDA_RT );
	AddAnchor( IDC_EDIT_R, __RDA_RT );
	AddAnchor( IDC_SPIN_G, __RDA_RT );
	AddAnchor( IDC_STATIC_G, __RDA_RT );
	AddAnchor( IDC_EDIT_G, __RDA_RT );
	AddAnchor( IDC_SPIN_B, __RDA_RT );
	AddAnchor( IDC_STATIC_B, __RDA_RT );
	AddAnchor( IDC_EDIT_B, __RDA_RT );
	AddAnchor( IDC_SPIN_H, __RDA_RT );
	AddAnchor( IDC_STATIC_H, __RDA_RT );
	AddAnchor( IDC_EDIT_H, __RDA_RT );
	AddAnchor( IDC_SPIN_S, __RDA_RT );
	AddAnchor( IDC_STATIC_S, __RDA_RT );
	AddAnchor( IDC_EDIT_S, __RDA_RT );
	AddAnchor( IDC_SPIN_L, __RDA_RT );
	AddAnchor( IDC_STATIC_L, __RDA_RT );
	AddAnchor( IDC_EDIT_L, __RDA_RT );
	AddAnchor( IDC_SPIN_C, __RDA_RT );
	AddAnchor( IDC_STATIC_C, __RDA_RT );
	AddAnchor( IDC_EDIT_C, __RDA_RT );
	AddAnchor( IDC_SPIN_M, __RDA_RT );
	AddAnchor( IDC_STATIC_M, __RDA_RT );
	AddAnchor( IDC_EDIT_M, __RDA_RT );
	AddAnchor( IDC_SPIN_Y, __RDA_RT );
	AddAnchor( IDC_STATIC_Y, __RDA_RT );
	AddAnchor( IDC_EDIT_Y, __RDA_RT );
	AddAnchor( IDC_STATIC_TXT_NEW, __RDA_RT );
	AddAnchor( IDC_STATIC_TXT_CURRENT, __RDA_RT );
	AddAnchor( IDC_STATIC_CMP, __RDA_RT );
	AddAnchor( IDOK, __RDA_LB );
	AddAnchor( IDCANCEL, __RDA_LB );
	ShowSizeGrip( TRUE );

	VERIFY(
		m_wndColorCtrl.SubclassDlgItem(
			IDC_STATIC_HEX_FULL,
			this
			)
		);
	VERIFY(
		m_wndColorDiff.SubclassDlgItem(
			IDC_STATIC_CMP,
			this
			)
		);

	m_SpinR.SetRange( 0, 255 );
	m_SpinG.SetRange( 0, 255 );
	m_SpinB.SetRange( 0, 255 );
	m_SpinH.SetRange( 0, 255 );
	m_SpinL.SetRange( 0, 255 );
	m_SpinS.SetRange( 0, 255 );
	m_SpinC.SetRange( 0, 255 );
	m_SpinM.SetRange( 0, 255 );
	m_SpinY.SetRange( 0, 255 );

	m_bDialogInitColplete = true;

	OnColorChanged(
		(WPARAM)m_wndColorCtrl.GetSafeHwnd(),
		(LPARAM)&m_wndColorCtrl.m_selClrs
		);
	m_wndColorDiff.SetupColor( m_clrInit, false );

	_ColorModelLoad();
	if(	( ! m_strSection.IsEmpty() ) && ( ! m_strEntryDialogPosition.IsEmpty() ) )
		EnableSaveRestore( LPCTSTR(m_strSection), LPCTSTR(m_strEntryDialogPosition) );

	return TRUE;
}

void CExtColorDlg::OnOK()
{
	ASSERT_VALID( this );
	_ColorModelSave();
	__BASEOF_CExtColorDlg__::OnOK();
}

void CExtColorDlg::OnCancel()
{
	ASSERT_VALID( this );
	_ColorModelSave();
	__BASEOF_CExtColorDlg__::OnCancel();
}

bool CExtColorDlg::_ColorModelLoad()
{
	ASSERT_VALID( this );
	if(		m_strSection.IsEmpty()
		||	m_strEntryColorModelSelection.IsEmpty()
		||	m_strEntryColorModelSelection == m_strEntryDialogPosition
		||	GetSafeHwnd() == NULL
		||	m_ComboType.GetSafeHwnd() == NULL
		)
		return false;
INT nVal = (INT)
		::AfxGetApp() ->
			GetProfileInt( LPCTSTR(m_strSection), LPCTSTR(m_strEntryColorModelSelection), -1 );
	if( ! ( INT(CExtColorCtrl::MODE_NAMED_MIN) <= nVal && nVal <= INT(CExtColorCtrl::MODE_NAMED_MAX) ) )
		return false;
CExtColorCtrl::e_mode_t eMode = (CExtColorCtrl::e_mode_t) nVal;
	m_ComboType.SetCurSel( nVal );
	m_wndColorCtrl = eMode;
	return true;
}

bool CExtColorDlg::_ColorModelSave()
{
	ASSERT_VALID( this );
	if(		m_strSection.IsEmpty()
		||	m_strEntryColorModelSelection.IsEmpty()
		||	m_strEntryColorModelSelection == m_strEntryDialogPosition
		||	GetSafeHwnd() == NULL
		||	m_ComboType.GetSafeHwnd() == NULL
		)
		return false;
CExtColorCtrl::e_mode_t eMode = (CExtColorCtrl::e_mode_t) m_ComboType.GetCurSel();
INT nVal = (INT) eMode;
	if( ! ( INT(CExtColorCtrl::MODE_NAMED_MIN) <= nVal && nVal <= INT(CExtColorCtrl::MODE_NAMED_MAX) ) )
		return false;
bool bRetVal =
		::AfxGetApp() ->
			WriteProfileInt( LPCTSTR(m_strSection), LPCTSTR(m_strEntryColorModelSelection), nVal )
				? true : false;
	return bRetVal;
}

__EXT_MFC_INT_PTR CExtColorDlg::DoModal()
{
	m_bDialogInitColplete = false;
__EXT_MFC_INT_PTR nRetVal = __BASEOF_CExtColorDlg__ :: DoModal();
	m_bDialogInitColplete = false;
	return nRetVal;
}

void CExtColorDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	pWnd;
	point;
}

void CExtColorDlg::OnSelendokComboSelType() 
{
	if( ! m_bDialogInitColplete )
		return;
CExtColorCtrl::e_mode_t eMode =
		(CExtColorCtrl::e_mode_t)m_ComboType.GetCurSel();
	m_wndColorCtrl = eMode;
}

void CExtColorDlg::OnSize(UINT nType, int cx, int cy) 
{
	__BASEOF_CExtColorDlg__ :: OnSize(nType, cx, cy);
	Invalidate();
	UpdateWindow();	
}

LRESULT CExtColorDlg::OnColorChanged(WPARAM wParam,LPARAM lParam)
{
	wParam;
	if( m_bSyncingValues )
		return 0;
	m_bSyncingValues = true;
CExtColorCtrl::selection_colors_t * pClrs =
		(CExtColorCtrl::selection_colors_t *)lParam;
	ASSERT( pClrs != NULL );
	SyncToEditRGB( pClrs->m_clrNew, true, true, true );
	m_bSyncingValues = false;
	return 0;
}

void CExtColorDlg::SyncToEditRGB(
	COLORREF clr,
	bool bSyncRGB,
	bool bSyncCMY,
	bool bSyncHLS
	)
{
	if( bSyncRGB )
	{
		CExtSafeString sR,sG,sB;
		sR.Format( _T("%d"), GetRValue(clr) );
		sG.Format( _T("%d"), GetGValue(clr) );
		sB.Format( _T("%d"), GetBValue(clr) );
		m_EditR.SetWindowText( sR );
		m_EditG.SetWindowText( sG );
		m_EditB.SetWindowText( sB );
	} // if( bSyncRGB )

	if( bSyncCMY )
	{
		COLORREF clrCMY =
			CExtBitmap::stat_RGBtoCMY( clr );
		CExtSafeString sC,sM,sY;
		sC.Format( _T("%d"), GetRValue(clrCMY) );
		sM.Format( _T("%d"), GetGValue(clrCMY) );
		sY.Format( _T("%d"), GetBValue(clrCMY) );
		m_EditC.SetWindowText( sC );
		m_EditM.SetWindowText( sM );
		m_EditY.SetWindowText( sY );
	} // if( bSyncCMY )

	if( bSyncHLS )
	{
		CExtSafeString sH,sS,sL;
		double fH=0.0,fS=0.0,fL=0.0;
		//CExtPaintManager::stat_ConvertRGBToHSL
		CExtBitmap::stat_RGBtoHSL(
			clr, &fH, &fS, &fL );
		int nH,nS,nL;
		nH = min( 255, (UINT)(0.5+fH*255.0) ); // UINT(fH*255.0);
		nS = min( 255, (UINT)(0.5+fS*255.0) ); // UINT(fS*255.0);
		nL = min( 255, (UINT)(0.5+fL*255.0) ); // UINT(fL*255.0);
		sH.Format( _T("%d"), nH );
		sS.Format( _T("%d"), nS );
		sL.Format( _T("%d"), nL );
		m_EditH.SetWindowText( sH );
		m_EditS.SetWindowText( sS );
		m_EditL.SetWindowText( sL );
	} // if( bSyncHLS )

	if( m_wndColorCtrl.m_selClrs.m_clrNew != clr )
	{
//		m_wndColorCtrl.m_selClrs.m_clrNew = clr;
//		m_wndColorCtrl.Invalidate();
//		m_wndColorCtrl.UpdateWindow();
		m_wndColorCtrl.SetupColor( clr, false );
	}
	if( m_wndColorDiff.m_selClrs.m_clrNew != clr )
	{
//		m_wndColorDiff.m_selClrs.m_clrNew = clr;
//		m_wndColorDiff.Invalidate();
//		m_wndColorDiff.UpdateWindow();
		m_wndColorDiff.SetupColor( clr, false );
	}

	m_clrNew = m_wndColorCtrl.m_selClrs.m_clrNew;
}

void CExtColorDlg::SyncFromEditRGB()
{
	if( (! m_bDialogInitColplete ) || m_bSyncingValues )
		return;

CExtLocalResourceHelper _LRH;
CExtSafeString sMsgInvalidRange;
	if( ! g_ResourceManager->LoadString( sMsgInvalidRange, IDS_VALUE_IN_RNG_255 ) )
		sMsgInvalidRange = _T("Value must be in range 0..255");

	m_bSyncingValues = true;
	
CString s;
	m_EditR.GetWindowText( s );
int nR = _ttol(s);
	if( nR < 0 || nR > 255 )
	{
		UINT nType = MB_OK;
		if( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 )
			nType |= MB_RTLREADING;
#if (!defined __EXT_MFC_NO_MSG_BOX)
		::ProfUISMsgBox( GetSafeHwnd(), sMsgInvalidRange, NULL, nType );
#else
		::MessageBox( GetSafeHwnd(), sMsgInvalidRange, NULL, nType );
#endif
		m_EditR.SetFocus();
		m_bSyncingValues = false;
		return;
	}
	
	m_EditG.GetWindowText( s );
int nG = _ttol(s);
	if( nG < 0 || nG > 255 )
	{
		UINT nType = MB_OK;
		if( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 )
			nType |= MB_RTLREADING;
#if (!defined __EXT_MFC_NO_MSG_BOX)
		::ProfUISMsgBox( GetSafeHwnd(), sMsgInvalidRange, NULL, nType );
#else
		::MessageBox( GetSafeHwnd(), sMsgInvalidRange, NULL, nType );
#endif
		m_EditG.SetFocus();
		m_bSyncingValues = false;
		return;
	}

	m_EditB.GetWindowText( s );
int nB = _ttol(s);
	if( nB < 0 || nB > 255 )
	{
		UINT nType = MB_OK;
		if( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 )
			nType |= MB_RTLREADING;
#if (!defined __EXT_MFC_NO_MSG_BOX)
		::ProfUISMsgBox( GetSafeHwnd(), sMsgInvalidRange, NULL, nType );
#else
		::MessageBox( GetSafeHwnd(), sMsgInvalidRange, NULL, nType );
#endif
		m_EditB.SetFocus();
		m_bSyncingValues = false;
		return;
	}

COLORREF clr = RGB(nR,nG,nB);
	SyncToEditRGB( clr, false, true, true );
	m_bSyncingValues = false;
}

void CExtColorDlg::SyncFromEditCMY()
{
	if( (! m_bDialogInitColplete ) || m_bSyncingValues )
		return;

CExtLocalResourceHelper _LRH;
CExtSafeString sMsgInvalidRange;
	if( ! g_ResourceManager->LoadString( sMsgInvalidRange, IDS_VALUE_IN_RNG_255 ) )
		sMsgInvalidRange = _T("Value must be in range 0..255");
	
	m_bSyncingValues = true;
	
CString s;
	m_EditC.GetWindowText( s );
int nC = _ttol(s);
	if( nC < 0 || nC > 255 )
	{
		UINT nType = MB_OK;
		if( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 )
			nType |= MB_RTLREADING;
#if (!defined __EXT_MFC_NO_MSG_BOX)
		::ProfUISMsgBox( GetSafeHwnd(), sMsgInvalidRange, NULL, nType );
#else
		::MessageBox( GetSafeHwnd(), sMsgInvalidRange, NULL, nType );
#endif
		m_EditC.SetFocus();
		m_bSyncingValues = false;
		return;
	}
	
	m_EditM.GetWindowText( s );
int nM = _ttol(s);
	if( nM < 0 || nM > 255 )
	{
		UINT nType = MB_OK;
		if( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 )
			nType |= MB_RTLREADING;
#if (!defined __EXT_MFC_NO_MSG_BOX)
		::ProfUISMsgBox( GetSafeHwnd(), sMsgInvalidRange, NULL, nType );
#else
		::MessageBox( GetSafeHwnd(), sMsgInvalidRange, NULL, nType );
#endif
		m_EditM.SetFocus();
		m_bSyncingValues = false;
		return;
	}

	m_EditY.GetWindowText( s );
int nY = _ttol(s);
	if( nY < 0 || nY > 255 )
	{
		UINT nType = MB_OK;
		if( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 )
			nType |= MB_RTLREADING;
#if (!defined __EXT_MFC_NO_MSG_BOX)
		::ProfUISMsgBox( GetSafeHwnd(), sMsgInvalidRange, NULL, nType );
#else
		::MessageBox( GetSafeHwnd(), sMsgInvalidRange, NULL, nType );
#endif
		m_EditY.SetFocus();
		m_bSyncingValues = false;
		return;
	}

COLORREF clr =
		CExtBitmap::stat_CMYtoRGB(
			RGB( nC, nM, nY )
			);
	SyncToEditRGB( clr, true, false, true );
	m_bSyncingValues = false;
}

void CExtColorDlg::SyncFromEditHSL()
{
	if( (! m_bDialogInitColplete ) || m_bSyncingValues )
		return;

CExtLocalResourceHelper _LRH;
CExtSafeString sMsgInvalidRange;
	if( ! g_ResourceManager->LoadString( sMsgInvalidRange, IDS_VALUE_IN_RNG_255 ) )
		sMsgInvalidRange = _T("Value must be in range 0..255");

	m_bSyncingValues = true;
	
CString s;
	m_EditH.GetWindowText( s );
int nH = _ttol(s);
	if( nH < 0 || nH > 255 )
	{
		UINT nType = MB_OK;
		if( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 )
			nType |= MB_RTLREADING;
#if (!defined __EXT_MFC_NO_MSG_BOX)
		::ProfUISMsgBox( GetSafeHwnd(), sMsgInvalidRange, NULL, nType );
#else
		::MessageBox( GetSafeHwnd(), sMsgInvalidRange, NULL, nType );
#endif
		m_EditH.SetFocus();
		m_bSyncingValues = false;
		return;
	}
	
	m_EditS.GetWindowText( s );
int nS = _ttol(s);
	if( nS < 0 || nS > 255 )
	{
		UINT nType = MB_OK;
		if( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 )
			nType |= MB_RTLREADING;
#if (!defined __EXT_MFC_NO_MSG_BOX)
		::ProfUISMsgBox( GetSafeHwnd(), sMsgInvalidRange, NULL, nType );
#else
		::MessageBox( GetSafeHwnd(), sMsgInvalidRange, NULL, nType );
#endif
		m_EditS.SetFocus();
		m_bSyncingValues = false;
		return;
	}

	m_EditL.GetWindowText( s );
int nL = _ttol(s);
	if( nL < 0 || nL > 255 )
	{
		UINT nType = MB_OK;
		if( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 )
			nType |= MB_RTLREADING;
#if (!defined __EXT_MFC_NO_MSG_BOX)
		::ProfUISMsgBox( GetSafeHwnd(), sMsgInvalidRange, NULL, nType );
#else
		::MessageBox( GetSafeHwnd(), sMsgInvalidRange, NULL, nType );
#endif
		m_EditL.SetFocus();
		m_bSyncingValues = false;
		return;
	}
COLORREF clr =
		//CExtPaintManager::stat_GetRGBFromHLSExtend
		CExtBitmap::stat_HLStoRGB(
			min( 1.0, (double)(0.5+nH)/255.0 ), // double(nH)/255.0
			min( 1.0, (double)(0.5+nL)/255.0 ), // double(nL)/255.0
			min( 1.0, (double)(0.5+nS)/255.0 )  // double(nS)/255.0
			);

	SyncToEditRGB( clr, true, true, false );
	m_bSyncingValues = false;
}
