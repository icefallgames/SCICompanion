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

#if (!defined __EXT_CMD_ICON_H)
#define __EXT_CMD_ICON_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#include <math.h>

class CExtPaintManager;

#define __EXT_MFC_RGBA(r,g,b,a)		(((COLORREF)RGB(r,g,b))|(((DWORD)(BYTE)(a))<<24))
#define __EXT_MFC_GetAValue(rgba)   ((BYTE)((rgba)>>24))

#define __EXT_MFC_SHADEBLENDCAPS      45

#define __EXT_MFC_SB_NONE             0x00000000
#define __EXT_MFC_SB_CONST_ALPHA      0x00000001
#define __EXT_MFC_SB_PIXEL_ALPHA      0x00000002
#define __EXT_MFC_SB_PREMULT_ALPHA    0x00000004
#define __EXT_MFC_SB_GRAD_RECT        0x00000010
#define __EXT_MFC_SB_GRAD_TRI         0x00000020

/////////////////////////////////////////////////////////////////////////////
// CExtBitmap

#define __EXT_BMP_FLAG_PREMULTIPLIED_RGB_CHANNELS	0x00000001
#define __EXT_BMP_FLAG_NO_RTL_DETECTION				0x00000002

#define __EXT_BMP_FLAG_USER_DEFINED_29				0x00000004
#define __EXT_BMP_FLAG_USER_DEFINED_28				0x00000008
#define __EXT_BMP_FLAG_USER_DEFINED_27				0x00000010
#define __EXT_BMP_FLAG_USER_DEFINED_26				0x00000020
#define __EXT_BMP_FLAG_USER_DEFINED_25				0x00000040
#define __EXT_BMP_FLAG_USER_DEFINED_24				0x00000080
#define __EXT_BMP_FLAG_USER_DEFINED_23				0x00000100
#define __EXT_BMP_FLAG_USER_DEFINED_22				0x00000200
#define __EXT_BMP_FLAG_USER_DEFINED_21				0x00000400
#define __EXT_BMP_FLAG_USER_DEFINED_20				0x00000800
#define __EXT_BMP_FLAG_USER_DEFINED_19				0x00001000
#define __EXT_BMP_FLAG_USER_DEFINED_18				0x00002000
#define __EXT_BMP_FLAG_USER_DEFINED_17				0x00004000
#define __EXT_BMP_FLAG_USER_DEFINED_16				0x00008000
#define __EXT_BMP_FLAG_USER_DEFINED_15				0x00010000
#define __EXT_BMP_FLAG_USER_DEFINED_14				0x00020000
#define __EXT_BMP_FLAG_USER_DEFINED_13				0x00040000
#define __EXT_BMP_FLAG_USER_DEFINED_12				0x00080000
#define __EXT_BMP_FLAG_USER_DEFINED_11				0x00100000
#define __EXT_BMP_FLAG_USER_DEFINED_10				0x00200000
#define __EXT_BMP_FLAG_USER_DEFINED_09				0x00400000
#define __EXT_BMP_FLAG_USER_DEFINED_08				0x00800000
#define __EXT_BMP_FLAG_USER_DEFINED_07				0x01000000
#define __EXT_BMP_FLAG_USER_DEFINED_06				0x02000000
#define __EXT_BMP_FLAG_USER_DEFINED_05				0x04000000
#define __EXT_BMP_FLAG_USER_DEFINED_04				0x08000000
#define __EXT_BMP_FLAG_USER_DEFINED_03				0x10000000
#define __EXT_BMP_FLAG_USER_DEFINED_02				0x20000000
#define __EXT_BMP_FLAG_USER_DEFINED_01				0x40000000
#define __EXT_BMP_FLAG_USER_DEFINED_00				0x80000000

class __PROF_UIS_API CExtBitmap
{
protected:
	LPBYTE m_pImageBuffer;
public:

	class __PROF_UIS_API Filter
	{
	public:
		enum e_filter_type
		{
			box           =  0,
			linear        =  1, // filter width is not used
			bilinear      =  2,
			hermite       =  3, // filter width is not used
			sinc          =  4, // filter width is not used
			catmull_rom   =  5, // filter width is not used
			b_spline      =  6, // filter width is not used
			blackman      =  7,
			lanczos       =  8,
			quadratic     =  9, // filter width is not used
			gaussian      = 10,
			hamming       = 11,
			bicubic       = 12, // filter width is not used
			ft_min_number =  0,
			ft_max_number = 12,
			ft_default    =  0,
		};
	protected:
		e_filter_type m_eFT;
		double m_lfWidth, m_arrParmBuffer[10];
		static double stat_rCalc( double lfV );
	public:
		Filter(
			e_filter_type eFT = ft_default,
			double * pArrParms = NULL,
			INT nParmCount = 0
			);
		virtual ~Filter();
		virtual double Process(
			double lfV
			);
		static double static_DefaultWidth(
			e_filter_type eFT
			);
		virtual e_filter_type FilterTypeGet() const;
		virtual void FilterTypeSet(
			e_filter_type eFT
			);
		virtual double FilterWidthGet() const;
		virtual void FilterWidthSet(
			double lfWidth
			);
		virtual void FilterWidthSetDefault();
		static bool stat_FilterWidthIsUsed(
			e_filter_type eFT
			);
		virtual bool FilterWidthIsUsed() const;
		virtual INT FilterParameterGetCount();
		virtual double FilterParameterGetAt(
			INT nPos
			) const;
		virtual void FilterParameterSetAt(
			INT nPos,
			double lfV
			);
		static __EXT_MFC_SAFE_LPCTSTR stat_FilterNameGet(
			e_filter_type eFT
			);
		virtual __EXT_MFC_SAFE_LPCTSTR FilterNameGet() const;
	}; // class Filter
	
private:
	class __PROF_UIS_API WT
	{
		struct __PROF_UIS_API WE
		{
		public:
			double * m_pWV;
			INT m_nMin, m_nMax;
			WE()
				: m_pWV( NULL )
				, m_nMin( 0 )
				, m_nMax( 0 )
			{
			}
			~WE()
			{
				if( m_pWV != NULL )
					delete [ ] m_pWV;
			}
		}; // struct WE
	protected:
		UINT m_nImageLineSize, m_nReviewSize;
		WE * m_pWT;
	public:
		WT(
			Filter & _f,
			UINT nLineSizeDst,
			UINT nLineSizeSrc
			);
		~WT();
		INT GetMin( INT nPos ) const
		{
			return m_pWT[ nPos ].m_nMin;
		}
		INT GetMax( INT nPos ) const
		{
			return m_pWT[ nPos ].m_nMax;
		}
		double Weight(
			INT nPosDst,
			INT nPosSrc
			) const
		{
			return m_pWT[ nPosDst ].m_pWV[ nPosSrc ];
		}
	}; // class WT
public:	
	enum e_DrawMethod_t
	{
		__EDM_HOLLOW	= 0,
		__EDM_CENTER	= 1,
		__EDM_STRETCH	= 2,
		__EDM_TILE_H	= 3,
		__EDM_TILE_V	= 4,
		__EDM_TILE		= 5,
		__EDM_COUNT		= 6,
		__EDM_DEFAULT	= __EDM_STRETCH,
	};

	CExtBitmap();
	CExtBitmap(
		const CExtBitmap & other
		);
	virtual ~CExtBitmap();
	CExtBitmap & operator = (
		const CExtBitmap & other
		);
	virtual bool AssignFromHICON(
		HICON hIcon,
		bool bForceLowColorMode = false
		);
	virtual bool AssignFromOther(
		const CExtBitmap & other,
		bool bEnableThrowException = false
		);
	virtual void Empty();
	virtual bool IsEmpty() const;
	virtual void SerializeUnpacked( CArchive & ar );
	virtual bool Clipboard_Copy(
		HWND hWnd = NULL,
		bool bProvideWinFormat = true,
		bool bProvideExtFormat = true
		) const;
	virtual bool Clipboard_Paste(
		HWND hWnd = NULL,
		bool bProvideWinFormat = true,
		bool bProvideExtFormat = true
		);
	virtual bool Clipboard_CanCopy(
		bool bProvideWinFormat = true,
		bool bProvideExtFormat = true
		) const;
	virtual bool Clipboard_CanPaste(
		HWND hWnd = NULL,
		bool bProvideWinFormat = true,
		bool bProvideExtFormat = true
		) const;
	virtual CLIPFORMAT Clipboard_GetExtFormat() const;
	static __EXT_MFC_SAFE_LPCTSTR g_strExtBitmapClipboardFormatName;
	virtual CExtSafeString Clipboard_GetExtFormatName() const;
	bool Process(
		LONG * pMatrix,
		LONG nMatrixSize,
		LONG nFactor,
		LONG nOffset,
		LPCRECT pRect = NULL
		);
	virtual bool Flip(
		bool bHorizontal,
		bool bVertVertical,
		LPCRECT pRectFlip = NULL
		);
	bool FlipHorizontal(
		LPCRECT pRectFlip = NULL
		);
	bool FlipVertical(
		LPCRECT pRectFlip = NULL
		);
	virtual bool Scale(
		INT nWidth,
		INT nHeight,
		Filter & _f
		);
	bool Scale(
		INT nWidth,
		INT nHeight
		);
	bool LoadBMP_Resource(
		__EXT_MFC_SAFE_LPCTSTR sResourceID,
		__EXT_MFC_SAFE_LPCTSTR sResourceType = RT_BITMAP, // default bitmap section
		HINSTANCE hInst = NULL, // find it automatically
		bool bMake32 = false,
		bool bEnableThrowException = false
		);
	bool LoadBMP_Resource(
		HINSTANCE hInst,
		HRSRC hRsrc,
		bool bMake32 = false,
		bool bEnableThrowException = false
		);
	bool LoadBMP_Buffer(
		LPCVOID pBuffer,
		UINT nByteCount,
		bool bMake32 = false,
		bool bEnableThrowException = false,
		bool bResourceFormat = false
		);
	bool LoadBMP_File(
		__EXT_MFC_SAFE_LPCTSTR strFilePath,
		bool bMake32 = false,
		bool bEnableThrowException = false,
		bool bResourceFormat = false
		);
	bool LoadBMP_File(
		CFile & _file,
		bool bMake32 = false,
		bool bEnableThrowException = false,
		bool bResourceFormat = false
		);
	bool LoadBMP(
		CArchive & ar,
		bool bMake32 = false,
		bool bEnableThrowException = false,
		bool bResourceFormat = false
		);
	bool SaveBMP_File(
		__EXT_MFC_SAFE_LPCTSTR strFilePath,
		bool bMake32 = false,
		bool bEnableThrowException = false,
		bool bUseIndexedRLE = true
		);
	bool SaveBMP_File(
		CFile & _file,
		bool bMake32 = false,
		bool bEnableThrowException = false,
		bool bUseIndexedRLE = true
		);
	bool SaveBMP(
		CArchive & ar,
		bool bMake32 = false,
		bool bEnableThrowException = false,
		bool bUseIndexedRLE = true
		);
#ifndef _AFX_NO_OLE_SUPPORT
	static bool stat_OlePictureLoad(
		IStream * pStream, DWORD dwImageSize, IPicture ** ppPicture,
		CSize * pSizeImage, OLE_XSIZE_HIMETRIC * p_nWidthHM, OLE_YSIZE_HIMETRIC * p_nHeightHM,
		bool bShowMsgBoxes = false
		);
	static bool stat_OlePictureLoad(
		LPCTSTR strPictureFilePath, IPicture ** ppPicture,
		CSize * pSizeImage, OLE_XSIZE_HIMETRIC * p_nWidthHM, OLE_YSIZE_HIMETRIC * p_nHeightHM,
		bool bShowMsgBoxes = false
		);
	bool OlePictureLoad(
		__EXT_MFC_SAFE_LPCTSTR sResourceID,
		__EXT_MFC_SAFE_LPCTSTR sResourceType = RT_BITMAP, // default bitmap section
		HINSTANCE hInst = NULL, // find it automatically
		bool bMake32 = false,
		bool bEnableThrowException = false,
		bool bShowMsgBoxes = false
		);
	bool OlePictureLoad(
		HINSTANCE hInst,
		HRSRC hRsrc,
		bool bMake32 = false,
		bool bEnableThrowException = false,
		bool bShowMsgBoxes = false
		);
	bool OlePictureLoad(
		CFile & _file,
		bool bMake32 = false,
		bool bEnableThrowException = false,
		bool bShowMsgBoxes = false
		);
	bool OlePictureLoad(
		CArchive & ar,
		bool bMake32 = false,
		bool bEnableThrowException = false,
		bool bShowMsgBoxes = false
		);
	bool OlePictureLoad(
		IStream * pStream,
		bool bMake32 = false,
		bool bEnableThrowException = false,
		bool bShowMsgBoxes = false
		);
	bool OlePictureLoad(
		IStream * pStream,
		DWORD dwImageSize,
		bool bMake32 = false,
		bool bEnableThrowException = false,
		bool bShowMsgBoxes = false
		);
	bool OlePictureLoad(
		LPCTSTR strPictureFilePath,
		bool bMake32,
		bool bEnableThrowException = false,
		bool bShowMsgBoxes = false
		);
#endif // _AFX_NO_OLE_SUPPORT
	bool Make32(
		bool bEnableThrowException = false
		);
	bool FromColor(
		COLORREF clr,
		const SIZE & _dimension,
		BYTE nAlpha = BYTE(255),
		bool bEnableThrowException = false
		);
	void ColorRect(
		COLORREF clr,
		const RECT & rcFill,
		BYTE nAlpha = BYTE(255)
		);
	bool FromGradient(
		COLORREF clrLeft,
		COLORREF clrRight,
		bool bHorz,
		const SIZE & _dimension,
		UINT nCountOfSteps = 256,
		BYTE nAlphaLeft = BYTE(255),
		BYTE nAlphaRight = BYTE(255),
		bool bEnableThrowException = false
		);
	void GradientRect(
		COLORREF clrLeft,
		COLORREF clrRight,
		bool bHorz,
		const RECT & rcFill,
		UINT nCountOfSteps = 256,
		BYTE nAlphaLeft = BYTE(255),
		BYTE nAlphaRight = BYTE(255)
		);
	bool FromSurface(
		HDC hDC,
		const RECT & rcSrc,
		bool bEnableThrowException = false
		);
	bool FromBitmap(
		HBITMAP hBitmap,
		bool bEnableThrowException = false,
		bool bNoAdjustAlpha = false
		);
	bool FromBitmap(
		HBITMAP hBitmap,
		LPCRECT pRectBitmapSrc, // = NULL - entire bitmap
		bool bEnableThrowException,
		bool bNoAdjustAlpha
		);
	bool FromBitmap(
		const CExtBitmap & _other,
		LPCRECT pRectBitmapSrc, // = NULL - entire bitmap
		bool bEnableThrowException = false
		);
	virtual bool ExtractEditableBitmap(
		HBITMAP & hBitmap,
		COLORREF & clrTransparent,
		CArray <COLORREF,COLORREF> * pArr = NULL, // recommended palette, item 0 is clrTransparent
		COLORREF ** ppClrSurface = NULL // if not NULL - force 32 bit
		) const;
	bool CreateRotated9xStack(
		const CExtBitmap & bmpSrcStack,
		int nAngleCw90, // 0, 90, 180, 270, 360 clockwise
		int nStackItemCount, // >= 1
		int nSrcItemWidth,
		int nSrcItemHeight,
		bool bSrcStackIsHorizontal,
		bool bDstStackIsHorizontal
		);
	virtual HICON CreateHICON(
		bool bReplaceSysColors = true,
		COLORREF clrTransparent = COLORREF(-1L)
		) const;
	HBITMAP CreateBitmap( 
		HDC hDC, 
		bool bCreateDDB
		) const;
	HBITMAP CreateBitmap(
		bool bCreateDDB
		) const;
	HBITMAP CreateBitmap( 
		HDC hDC, 
		COLORREF ** ppClrSurface,
		COLORREF clrFillBk = COLORREF(-1L)
		) const;
	HBITMAP CreateBitmap(
		COLORREF ** ppClrSurface,
		COLORREF clrFillBk = COLORREF(-1L)
		) const;
	static bool g_bIsAutoPreMultiplyRGBChannels;
	virtual bool IsAutoPreMultiplyRGBChannels() const;
	virtual void PreMultiplyRGBChannels(
		bool bForward = true
		);
	virtual bool PreMultipliedRGBChannelsGet() const;
	virtual void PreMultipliedRGBChannelsSet( bool bPreMultiplied );
	void MakeMono(
		COLORREF clrMono = COLORREF(-1L),
		LPCRECT pRect = NULL
		);
	void AlphaColor(
		COLORREF clr,
		COLORREF clrTolerance,
		BYTE nAlpha,
		LPCRECT pRect = NULL
		);
	void AlphaColorDistance(
		COLORREF clr,
		INT nDistance,
		BYTE nAlpha,
		LPCRECT pRect = NULL
		);
	void AlphaRect(
		const RECT & rcFrame,
		BYTE nAlpha,
		COLORREF clrTransparent = COLORREF(-1L),
		COLORREF clrOpaque = COLORREF(-1L)
		);
	void AlphaFrame(
		const RECT & rcFrame,
		const RECT & rcPadding,
		BYTE nAlpha,
		COLORREF clrTransparent = COLORREF(-1L),
		COLORREF clrOpaque = COLORREF(-1L)
		);
	virtual int DrawImpl(
		HDC hDC,
		int nDstX,
		int nDstY,
		int nDstWidth,
		int nDstHeight,
		int nSrcX,
		int nSrcY,
		int nSrcWidth,
		int nSrcHeight,
		DWORD dwRop = SRCCOPY
		) const;
	virtual int Draw(
		HDC hDC,
		int nDstX,
		int nDstY,
		int nDstWidth,
		int nDstHeight,
		int nSrcX,
		int nSrcY,
		int nSrcWidth,
		int nSrcHeight,
		DWORD dwRop = SRCCOPY
		) const;
	int Draw(
		HDC hDC,
		int nDstX,
		int nDstY,
		int nDstWidth,
		int nDstHeight,
		DWORD dwRop = SRCCOPY
		) const;
	int Draw(
		HDC hDC,
		const RECT & rcDst,
		const RECT & rcSrc,
		DWORD dwRop = SRCCOPY
		) const;
	int Draw(
		HDC hDC,
		const RECT & rcDst,
		DWORD dwRop = SRCCOPY
		) const;
	virtual bool DrawNcFrame(
		HDC hDC,
		const RECT & rcDstDraw,
		const RECT & rcSrcDraw,
		const RECT & rcPadding,
		INT nCaptionHeightDst = 0,
		INT nCaptionHeightSrc = 0,
		bool bDrawMiddlePart = false,
		bool bSmootherAsPossible = true,
		DWORD dwRop = SRCCOPY
		) const;
	bool DrawNcFrame(
		HDC hDC,
		const RECT & rcDstDraw,
		const RECT & rcPadding,
		INT nCaptionHeightDst = 0,
		INT nCaptionHeightSrc = 0,
		bool bDrawMiddlePart = false,
		bool bSmootherAsPossible = true,
		DWORD dwRop = SRCCOPY
		) const;
	virtual bool DrawSkinParts(
		HDC hDC,
		const RECT & rcDstDraw,
		const RECT & rcSrcDraw,
		const RECT & rcPadding,
		e_DrawMethod_t eDM,
		bool bDrawMiddlePart = true,
		bool bSmootherAsPossible = false,
		DWORD dwRop = SRCCOPY
		) const;
	bool DrawSkinParts(
		HDC hDC,
		const RECT & rcDstDraw,
		const RECT & rcPadding,
		e_DrawMethod_t eDM,
		bool bDrawMiddlePart = true,
		bool bSmootherAsPossible = false,
		DWORD dwRop = SRCCOPY
		) const;
	virtual int AlphaBlend(
		HDC hDC,
		int nDstX,
		int nDstY,
		int nDstWidth,
		int nDstHeight,
		int nSrcX,
		int nSrcY,
		int nSrcWidth,
		int nSrcHeight,
		BYTE nSCA = 0xFF
		) const;
	int AlphaBlend(
		HDC hDC,
		int nDstX,
		int nDstY,
		int nDstWidth,
		int nDstHeight,
		BYTE nSCA = 0xFF
		) const;
	int AlphaBlend(
		HDC hDC,
		const RECT & rcDst,
		const RECT & rcSrc,
		BYTE nSCA = 0xFF
		) const;
	int AlphaBlend(
		HDC hDC,
		const RECT & rcDst,
		BYTE nSCA = 0xFF
		) const;
	virtual bool AlphaBlendSkinParts(
		HDC hDC,
		const RECT & rcDstDraw,
		const RECT & rcSrcDraw,
		const RECT & rcPadding,
		e_DrawMethod_t eDM,
		bool bDrawMiddlePart = true,
		bool bSmootherAsPossible = false,
		BYTE nSCA = 0xFF
		) const;
	bool AlphaBlendSkinParts(
		HDC hDC,
		const RECT & rcDstDraw,
		const RECT & rcPadding,
		e_DrawMethod_t eDM,
		bool bDrawMiddlePart = true,
		bool bSmootherAsPossible = false,
		BYTE nSCA = 0xFF
		) const;
	virtual bool GetPixel(
		INT nX,
		INT nY,
		RGBQUAD & _pixel
		) const;
	COLORREF GetPixel(
		INT nX,
		INT nY,
		BYTE * p_nAlpha = NULL
		) const;
	virtual bool SetPixel(
		INT nX,
		INT nY,
		const RGBQUAD & _pixel,
		bool bSetRed = true,
		bool bSetGreen = true,
		bool bSetBlue = true,
		bool bSetAlpha = true
		);
	bool SetPixel(
		INT nX,
		INT nY,
		COLORREF clr, // if COLORREF( -1L ) - do not set color
		INT nAlpha = -1 // less than zero - do not set alpha
		);
	bool ColorizeMono(
		COLORREF clrTransparent,
		COLORREF clrTarget,
		LPCRECT pRect = NULL
		);
	bool AdjustAlpha(
		double percentAlpha,
		LPCRECT pRect = NULL
		);
	bool AdjustHLS(
		COLORREF clrTransparent = RGB(0,0,0),
		COLORREF clrTolerance = RGB(0,0,0),
		double percentH = 0.0,
		double percentL = 0.0,
		double percentS = 0.0,
		LPCRECT pRect = NULL
		);
	HGLOBAL GenerateHGLOBAL(
		HPALETTE hPalDst = NULL
		) const;
	static HGLOBAL stat_HBITMAPtoHGLOBAL(
		HBITMAP hBmpSrc,
		HPALETTE hPalDst = NULL
		);
	static bool stat_CloneBitmap(
		CBitmap & bmpDst,
		const CBitmap & bmpSrc,
		const RECT * pRcPart = NULL,
		COLORREF ** ppSurface = NULL // if not NULL - create 32 bit surface
		);
	static HBITMAP stat_CloneBitmap(
		HBITMAP hBmpSrc,
		const RECT * pRcPart = NULL,
		COLORREF ** ppSurface = NULL // if not NULL - create 32 bit surface
		);
	HRGN GenerateColorHRGN(
		bool bRTL,
		COLORREF clrTransparent,
		COLORREF clrTolerance = RGB(0,0,0)
		) const;
	HRGN GenerateSkinPartColorRGN(
		bool bRTL,
		const RECT & rcDst,
		const RECT & rcSrc,
		const RECT & rcPadding,
		COLORREF clrTransparent,
		bool bLeft,
		bool bTop
		) const;
	HRGN GenerateSkinPartColorRGN(
		bool bRTL,
		const RECT & rcDst,
		const RECT & rcPadding,
		COLORREF clrTransparent,
		bool bLeft,
		bool bTop
		) const;
	HRGN GenerateSkinFrameColorRGN(
		bool bRTL,
		const RECT & rcDst,
		const RECT & rcSrc,
		const RECT & rcPadding,
		COLORREF clrTransparent
		) const;
	HRGN GenerateSkinFrameColorRGN(
		bool bRTL,
		const RECT & rcDst,
		const RECT & rcPadding,
		COLORREF clrTransparent
		) const;
	HRGN GenerateSkinPartAlphaRGN(
		bool bRTL,
		const RECT & rcDst,
		const RECT & rcSrc,
		const RECT & rcPadding,
		bool bLeft,
		bool bTop,
		BYTE nAlphaMin = BYTE(0),
		BYTE nAlphaMax = BYTE(0)
		) const;
	HRGN GenerateSkinPartAlphaRGN(
		bool bRTL,
		const RECT & rcDst,
		const RECT & rcPadding,
		bool bLeft,
		bool bTop,
		BYTE nAlphaMin = BYTE(0),
		BYTE nAlphaMax = BYTE(0)
		) const;
	HRGN GenerateSkinFrameAlphaRGN(
		bool bRTL,
		const RECT & rcDst,
		const RECT & rcSrc,
		const RECT & rcPadding,
		BYTE nAlphaMin = BYTE(0),
		BYTE nAlphaMax = BYTE(0)
		) const;
	HRGN GenerateSkinFrameAlphaRGN(
		bool bRTL,
		const RECT & rcDst,
		const RECT & rcPadding,
		BYTE nAlphaMin = BYTE(0),
		BYTE nAlphaMax = BYTE(0)
		) const;
	CSize GetSize() const;
	ULONG GetBPP() const;
	static ULONG stat_RcLine(
		ULONG nWidth,
		ULONG nBPP
		);
	static ULONG stat_RcScanLineSize(
		ULONG nLineIndex
		);
	static ULONG stat_RcScanLineSize(
		ULONG nWidth,
		ULONG nBPP
		);
	static ULONG stat_RcUPE(
		ULONG nBPP
		);
	static LPBYTE stat_RcOffsetToScanLine(
		LPBYTE pBitsStart,
		ULONG nScanLineSize,
		ULONG nLineIndex
		);
	static ULONG stat_RcImageBufferSize(
		ULONG nWidth,
		ULONG hHeight,
		ULONG nBPP
		);
	static LPBITMAPINFOHEADER stat_RcInfoHeader(
		LPBYTE pImageBuffer
		);
	static LPBITMAPINFO stat_RcInfo(
		LPBYTE pImageBuffer
		);
	static LPBYTE stat_RcScanLine(
		LPBYTE pImageBuffer,
		ULONG nLineIndex,
		ULONG nScanLineSize
		);
	static LPBYTE stat_RcSurface(
		LPBYTE pImageBuffer
		);
	static LPVOID stat_AllocLL(
		__EXT_MFC_ULONG_PTR amount,
		__EXT_MFC_ULONG_PTR alignment
		);
	static void stat_FreeLL(
		LPVOID pMemToFree
		);
	static LPBYTE stat_AllocHL(
		ULONG nWidth,
		ULONG hHeight,
		ULONG nBPP,
		ULONG nMaskRed,
		ULONG nMaskGreen,
		ULONG nMaskBlue,
		ULONG nMaskAlpha,
		ULONG nFlags
		);
	static void stat_FreeHL(
		LPVOID pMemToFree
		);
	static LPBYTE stat_BmpLoad(
		CArchive & ar,
		bool bEnableThrowException,
		bool bResourceFormat
		);
	static bool stat_BmpSave(
		LPBYTE pImageBuffer,
		CArchive & ar,
		bool bEnableThrowException,
		bool bUseIndexedRLE
		);
	static LPBYTE stat_RcTo32(
		LPBYTE pImageBuffer,
		bool bEnableThrowException
		);
	static LPBYTE stat_rcResize(
		LPBYTE pImageBuffer,
		ULONG nWidth,
		ULONG nHeight,
		Filter & _f,
		bool bEnableAlphaMarginAdjustment
		);
	static void stat_rcResizeH(
		LPBYTE pImageBufferSrc,
		ULONG nWidthSrc,
		LPBYTE pImageBufferDst,
		ULONG nWidthDst,
		ULONG nHeightDst,
		Filter & _f,
		bool bEnableAlphaMarginAdjustment
		);
	static void stat_rcResizeV(
		LPBYTE pImageBufferSrc,
		ULONG nHeightSrc,
		LPBYTE pImageBufferDst,
		ULONG nWidthDst,
		ULONG nHeightDst,
		Filter & _f,
		bool bEnableAlphaMarginAdjustment
		);
	static ULONG stat_MaskRed( LPBYTE pImageBuffer );
	static ULONG stat_MaskGreen( LPBYTE pImageBuffer );
	static ULONG stat_MaskBlue( LPBYTE pImageBuffer );
	static ULONG stat_MaskAlpha( LPBYTE pImageBuffer );
	static ULONG stat_RunTimeFlagsGet( LPBYTE pImageBuffer );
	static void stat_RunTimeFlagsSet( LPBYTE pImageBuffer, ULONG nFlags );
	static bool stat_PreMultipliedRGBChannelsGet( LPBYTE pImageBuffer );
	static void stat_PreMultipliedRGBChannelsSet( LPBYTE pImageBuffer, bool PreMultiplied );
	const BYTE * GetBuffer() const;
	ULONG MaskRed() const;
	ULONG MaskGreen() const;
	ULONG MaskBlue() const;
	ULONG MaskAlpha() const;
	ULONG RunTimeFlagsGet() const;
	void RunTimeFlagsSet( ULONG nFlags ) const;
	virtual void OnLoad();
	virtual void OnContentChanged();
	static __EXT_MFC_SAFE_LPCTSTR stat_GetDrawMethodName(
		e_DrawMethod_t eDM
		);
	static e_DrawMethod_t stat_GetDrawMethodFromString(
		__EXT_MFC_SAFE_LPCTSTR str
		);
	INT GetBitmapColorMap( // returns bitmap's color bit count or zero if error
		CMap < COLORREF, COLORREF, DWORD, DWORD > & _map // key is color, value is count of color entries in bitmap
		) const;
	INT GetBitmapColorArray( // returns bitmap's color bit count or zero if error
		CArray < COLORREF, COLORREF > & arr
		) const;
	static COLORREF stat_RGBtoCMY( COLORREF clr );
	static COLORREF stat_CMYtoRGB( COLORREF clr );
	//----------------------------------------------------------------------
	// Conversion between the HSL (Hue, Saturation, and Luminosity) 
	// and RBG color model.
	//----------------------------------------------------------------------
	// The conversion algorithms presented here come from the book by 
	// Fundamentals of Interactive Computer Graphics by Foley and van Dam. 
	// In the example code, HSL values are represented as floating point 
	// number in the range 0 to 1. RGB tridrants use the Windows convention 
	// of 0 to 255 of each element. 
	//----------------------------------------------------------------------
	static double stat_HuetoRGB(double m1, double m2, double h );
	static COLORREF stat_HLStoRGB( double H, double L, double S );
	static void stat_RGBtoHSL( COLORREF rgb, double *H, double *S, double *L );
	static BYTE stat_GetRGBFromHue(float rm1, float rm2, float rh);
	static COLORREF stat_GetRGBFromHLSExtend( double H, double L, double S );
	static COLORREF stat_HLS_Adjust(
		COLORREF clr,
		double percentH = 0.0,
		double percentL = 0.0,
		double percentS = 0.0
		);
	static COLORREF stat_RGB_Enlight( 
		COLORREF clrLight, 
		COLORREF clrDark, 
		INT nEnlighten
		);
	static COLORREF stat_RGB_Blend( 
		COLORREF clrFrom, 
		COLORREF clrTo, 
		INT nAlpha
		);
}; // class CExtBitmap

/////////////////////////////////////////////////////////////////////////////
// CExtBitmapCache

class __PROF_UIS_API CExtBitmapCache : public CExtBitmap
{
	HBITMAP m_hBitmapCache;
	void _InitCache( const CExtBitmap & _bmp );
public:
	CExtBitmapCache();
	CExtBitmapCache(
		const CExtBitmap & other
		);
	virtual ~CExtBitmapCache();
	virtual bool AssignFromOther(
		const CExtBitmap & other,
		bool bEnableThrowException = false
		);

	virtual void Empty();
	virtual bool IsEmpty() const;

	virtual int AlphaBlend(
		HDC hDC,
		int nDstX,
		int nDstY,
		int nDstWidth,
		int nDstHeight,
		int nSrcX,
		int nSrcY,
		int nSrcWidth,
		int nSrcHeight,
		BYTE nSCA = 0xFF
		) const;
	virtual void OnContentChanged();
}; // class CExtBitmapCache

/////////////////////////////////////////////////////////////////////////////
// CExtCmdIcon
// command icon class

// CExtCmdIcon::m_dwFlagsValues
#define __EXT_ICON_PERSISTENT_BITMAP_DISABLED	0x00000001
#define __EXT_ICON_PERSISTENT_BITMAP_HOVER		0x00000002
#define __EXT_ICON_PERSISTENT_BITMAP_PRESSED	0x00000004
#define __EXT_ICON_FLAGS_DEFAULT				0x00000000

class __PROF_UIS_API CExtCmdIcon
{
public:
	enum e_paint_type_t
	{
		__PAINT_NORMAL      = 0,
		__PAINT_DISABLED    = 1,
		__PAINT_HOVER       = 2,
		__PAINT_PRESSED     = 3,
		__PAINT_INVISIBLE   = 4,
		__PAINT_VAL_MIN     = 0,
		__PAINT_VAL_MAX     = 4,
		__PAINT_VAL_COUNT   = 5,
	};
	CExtBitmap m_bmpNormal, m_bmpDisabled, m_bmpHover, m_bmpPressed;
	DWORD m_dwFlags;
	CExtCmdIcon();
// 	CExtCmdIcon(
// 		HICON hIcon,
// 		bool bCreateCopy // if false - delete hIcon
// 		);
// 	CExtCmdIcon(
// 		HBITMAP hBitmap,
// 		COLORREF clrTransparent = RGB( 0, 0, 0 ),
// 		LPCRECT pRectBitmapSrc = NULL,
// 		bool bCloneBitmap = true // if false - pRectBitmapSrc must be NULL and will delete hBitmap
// 		);
	CExtCmdIcon( const CExtCmdIcon & other );
	virtual ~CExtCmdIcon();
	CExtCmdIcon & operator = ( const CExtCmdIcon & other );
	virtual void AssignFromOther( const CExtCmdIcon & other );
	virtual void AssignFromHICON(
		HICON hIcon,
		bool bCreateCopy, // if false - delete hIcon
		bool bForceLowColorMode = false
		);
	virtual void AssignFromHBITMAP(
		HBITMAP hBitmap,
		COLORREF clrTransparent = COLORREF(-1L), // COLORREF(-1L) - no force transparent pixels
		LPCRECT pRectBitmapSrc = NULL,
		bool bCloneBitmap = true // if false - pRectBitmapSrc must be NULL and will delete hBitmap
		);
	virtual bool IsEmpty() const;
	virtual void Empty();
	virtual void Serialize( CArchive & ar );
	virtual CSize GetSize() const;
	operator CSize () const;
	virtual void OnGlobalPaintManagerChanged();
	virtual void OnEmptyGeneratedBitmaps();
	virtual void OnContentChanged();
	virtual void SyncSysColors(
		CExtPaintManager * pPM
		);
	virtual void OnSysColorChange(
		CExtPaintManager * pPM
		);
	virtual void OnSettingChange(
		CExtPaintManager * pPM,
		UINT uFlags,
		__EXT_MFC_SAFE_LPCTSTR lpszSection
		);
	virtual void OnDisplayChange(
		CExtPaintManager * pPM,
		INT nDepthBPP,
		CPoint ptSizes
		);
	virtual void OnThemeChanged(
		CExtPaintManager * pPM,
		WPARAM wParam,
		LPARAM lParam
		);
	virtual bool Scale(
		const SIZE & newSize
		);
	virtual bool CreateScaledCopy(
		const CExtCmdIcon & other,
		const SIZE & newSize
		);
	virtual bool CreateCheckMarkIcon(
		CExtPaintManager * pPM,
		COLORREF clrMark = COLORREF(-1L),
		COLORREF clrTransparent = COLORREF(-1L),
		CSize sizeIcon = CSize(16,16)
		);
	virtual bool CreateIndeterminatedSquareIcon(
		CExtPaintManager * pPM,
		COLORREF clrMark = COLORREF(-1L),
		COLORREF clrTransparent = COLORREF(-1L),
		CSize sizeIcon = CSize(16,16)
		);
	virtual bool CreateRadioMarkIcon(
		CExtPaintManager * pPM,
		COLORREF clrMark = COLORREF(-1L),
		COLORREF clrTransparent = COLORREF(-1L),
		CSize sizeIcon = CSize(16,16)
		);
	virtual bool CreateEmptyIcon(
		CExtPaintManager * pPM,
		COLORREF clrTransparent = COLORREF(-1L),
		CSize sizeIcon = CSize(16,16)
		);
	virtual HICON ExtractHICON(
		bool bReplaceSysColors = true,
		COLORREF clrTransparent = COLORREF(-1L)
		) const;
	virtual CExtBitmap & GetBitmap(
		e_paint_type_t ePT = __PAINT_NORMAL,
		CExtPaintManager * pPM = NULL
		);
	const CExtBitmap & GetBitmap(
		e_paint_type_t ePT = __PAINT_NORMAL,
		CExtPaintManager * pPM = NULL
		) const;
	void Paint(
		CExtPaintManager * pPM,
		HDC hDC,
		const RECT & rc,
		e_paint_type_t ePT = __PAINT_NORMAL,
		BYTE nSCA = BYTE(255)
		) const;
	virtual void Paint(
		CExtPaintManager * pPM,
		HDC hDC,
		INT nX,
		INT nY,
		INT nDX, // -1 use default size
		int nDY, // -1 use default size
		e_paint_type_t ePT = __PAINT_NORMAL,
		BYTE nSCA = BYTE(255)
		) const;
	virtual void PaintAccentEmboss(
		CExtPaintManager * pPM,
		HDC hDC,
		COLORREF clrAccent,
		INT nX,
		INT nY,
		INT nDX = -1, // -1 use default size
		int nDY = -1,  // -1 use default size
		BYTE nSCA = BYTE(255)
		);
	virtual void PaintAccentMono(
		CExtPaintManager * pPM,
		HDC hDC,
		COLORREF clrAccent,
		INT nX,
		INT nY,
		INT nDX = -1, // -1 use default size
		int nDY = -1,  // -1 use default size
		BYTE nSCA = BYTE(255)
		);
	virtual void PaintAccentColorized(
		CExtPaintManager * pPM,
		HDC hDC,
		COLORREF clrAccent,
		INT nX,
		INT nY,
		INT nDX = -1, // -1 use default size
		int nDY = -1,  // -1 use default size
		BYTE nSCA = BYTE(255)
		);
	virtual bool ExtractEditableBitmap(
		HBITMAP & hBitmap,
		COLORREF & clrTransparent,
		CArray <COLORREF,COLORREF> * pArr = NULL, // recommended palette, item 0 is clrTransparent
		COLORREF ** ppClrSurface = NULL // if not NULL - force 32 bit
		) const;
}; // class CExtCmdIcon

#endif // __EXT_CMD_ICON_H
