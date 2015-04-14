#pragma once

#define COLOREDTOOLTIP_CLASSNAME _T("SCIToolTip")

// CColoredToolTip

//
// Homegrown tooltip that allows for a colored background, and supports "method tooltips", with
// a bold section, or image tooltips.
//

class CColoredToolTip : public CWnd
{
	DECLARE_DYNAMIC(CColoredToolTip)

public:
	CColoredToolTip();
	virtual ~CColoredToolTip();
    void SetColor(COLORREF cr);
    void Show(CPoint ptScreen, std::string pszText);
    void Show(CPoint pt, std::string strMethod, const std::vector<std::string> &strParams, size_t cParams);
    void Show(CPoint ptScreen, HBITMAP hBitmap, CSize size); // Doesn't take ownership
    void Hide();
    void OnPaint();

protected:
	DECLARE_MESSAGE_MAP()

private:
    CBrush _brush;
    COLORREF _cr;

    CFont m_font;
    CFont m_fontBold;

    std::string _strOne;
    std::string _strTwo;
    std::string _strThree;

    UINT _nFormatFlags;

    HBITMAP _hBitmapWeak;
    CSize _size;
};


