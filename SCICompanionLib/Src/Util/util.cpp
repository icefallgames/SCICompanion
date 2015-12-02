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

#include "stdafx.h"
#include <math.h>
#include "format.h"
#include "WindowsUtil.h"
#include "TlHelp32.h"
#include <filesystem>

using namespace std::tr2::sys;
using namespace fmt;

__declspec(dllexport) void _cdecl DummyFunctionSoWeBuildALib()
{

}

bool _IsNumber(TCHAR ch, BOOL fHex)
{
    return isdigit(ch) || (fHex && isxdigit(ch));
}

BOOL IsValidResourceType(int iResourceType)
{
    return (iResourceType >= 0) && (iResourceType < NumResourceTypes);
}

BOOL IsValidResourceNumber(int iResourceNum)
{
    return (iResourceNum >= 0) && (iResourceNum < 1000);
}

typedef struct
{
    double h;
    double s;
    double l;
} HSL;

typedef struct
{
    double r;
    double g;
    double b;
} COLOUR;


HSL RGB2HSL(COLORREF color)
{
   double themin,themax,delta;
   HSL c2;
   COLOUR c1 = { ((double)GetRValue(color)) / 255.0, ((double)GetGValue(color)) / 255.0, ((double)GetBValue(color)) / 255.0 };

   themin = min(c1.r,min(c1.g,c1.b));
   themax = max(c1.r,max(c1.g,c1.b));
   delta = themax - themin;
   c2.l = (themin + themax) / 2;
   c2.s = 0;
   if (c2.l > 0 && c2.l < 1)
      c2.s = delta / (c2.l < 0.5 ? (2*c2.l) : (2-2*c2.l));
   c2.h = 0;
   if (delta > 0) {
      if (themax == c1.r && themax != c1.g)
         c2.h += (c1.g - c1.b) / delta;
      if (themax == c1.g && themax != c1.b)
         c2.h += (2 + (c1.b - c1.r) / delta);
      if (themax == c1.b && themax != c1.r)
         c2.h += (4 + (c1.r - c1.g) / delta);
      c2.h /= 6.0;
   }
   return(c2);
}

COLORREF g_rgColorCombos[256];  // EGACOLORS to COLORREFs - can be indexed by EGACOLOR_TO_BYTE
HSL g_rgColorCombosHSL[256];

// Palettes:
EGACOLOR g_rg136ToByte[136];    // The 136 unique EGACOLORS
EGACOLOR g_rg16ToByte[16];      // The 16 pure colors
EGACOLOR g_rgSmoothToByte[136]; // The smooth dithered EGACOLORS
int g_cSmoothEntries = 0;

BOOL g_bFilledCOLORREFArray = FALSE;

int GetNormalizedColorDistance(COLORREF color1, COLORREF color2);

//
// Fills an array with all possible colour combinations of the dithered EGA colors
// rgcolors must be 256 entries long
//
void FillCOLORREFArray()
{
    int iIndex = 0;
    int iIndex16 = 0;
    g_cSmoothEntries = 0;
    for (int i = 0; i < 16; i++)
    {
        for (int j = i; j < 16; j++)
        {
            // Calculate the 136 unique colours
            ASSERT(iIndex < ARRAYSIZE(g_rg136ToByte));
            RGBQUAD rgbq = _Combine(g_egaColors[i], g_egaColors[j]);
            COLORREF color = RGB(rgbq.rgbRed, rgbq.rgbGreen, rgbq.rgbBlue);
            // And the BYTE/EGACOLOR it maps to:
            g_rg136ToByte[iIndex].color1 = i;
            g_rg136ToByte[iIndex].color2 = j;
            if (i == j)
            {
                g_rg16ToByte[iIndex16].color1 = i;
                g_rg16ToByte[iIndex16].color2 = j;
                iIndex16++;
            }
            // See if this is a "smooth" colour
            if (200 > GetNormalizedColorDistance(g_egaColorsCR[i], g_egaColorsCR[j]))
            {
                g_rgSmoothToByte[g_cSmoothEntries].color1 = i;
                g_rgSmoothToByte[g_cSmoothEntries].color2 = j;
                g_cSmoothEntries++;
            }
            iIndex++;
        }
        for (int j = 0; j < 16; j++)
        {
            // This is a mapping of EGACOLORs to COLORREFs
            RGBQUAD rgbq = _Combine(g_egaColors[i], g_egaColors[j]);
            COLORREF color = RGB(rgbq.rgbRed, rgbq.rgbGreen, rgbq.rgbBlue);
            EGACOLOR egaColor = { i, j };
            g_rgColorCombos[EGACOLOR_TO_BYTE(egaColor)] = color; // the COLORREF color
            g_rgColorCombosHSL[EGACOLOR_TO_BYTE(egaColor)] = RGB2HSL(color);
        }
    }
    g_bFilledCOLORREFArray = TRUE;
}


//
// Returns the distance between 2 colours
//
int GetColorDistance(COLORREF color1, COLORREF color2)
{
    int redsq = ((int)GetRValue(color1)) - (int)GetRValue(color2);
    redsq *= redsq;
    int bluesq = ((int)GetBValue(color1)) - (int)GetBValue(color2);
    bluesq *= bluesq;
    int greensq = ((int)GetGValue(color1)) - (int)GetGValue(color2);
    greensq *= greensq;

    return (redsq + bluesq + greensq);
}

int GetColorDistanceRGB(RGBQUAD color1, RGBQUAD color2)
{
    int redsq = (int)color1.rgbRed - (int)color2.rgbRed;
    redsq *= redsq;
    int bluesq = (int)color1.rgbBlue - (int)color2.rgbBlue;
    bluesq *= bluesq;
    int greensq = (int)color1.rgbGreen - (int)color2.rgbGreen;
    greensq *= greensq;

    return (redsq + bluesq + greensq);
}

// (normalized euclidian RGB distance)
int GetNormalizedColorDistance(COLORREF color1, COLORREF color2)
{
    int iDistance = GetColorDistance(color1, color2);
    return (int)sqrt((double)iDistance);
}

//
// Differences between two colours, using HSL and euclidian distance
//
int GetColorDistance2(HSL one, COLORREF color2)
{
    HSL two = (RGB2HSL(color2));
    double hsq = (one.h - two.h);
    hsq *= hsq;
    double lsq = one.l - two.l;
    lsq *= lsq;
    double ssq = one.s - two.s;
    ssq *= ssq;

    return (int)(255.0 * (hsq + lsq + ssq));
}

//
// from http://www.compuphase.com/cmetric.htm
// 
int GetColorDistance3(COLORREF e1, COLORREF e2)
{
  int r,g,b;
  int rmean;

  rmean = ( (int)GetRValue(e1) + (int)GetRValue(e2)) / 2;
  r = (int)GetRValue(e1) - (int)GetRValue(e2);
  g = (int)GetGValue(e1) - (int)GetGValue(e2);
  b = (int)GetBValue(e1) - (int)GetBValue(e2);
  return (((512+rmean)*r*r)>>8) + 4*g*g + (((767-rmean)*b*b)>>8);
}

//
// Weighted RGB
// 
int GetColorDistance4(COLORREF color1, COLORREF color2)
{
    int redsq = ((int)GetRValue(color1)) - (int)GetRValue(color2);
    BOOL fMoreThan128 =  (abs(redsq) >= 128);
    redsq *= redsq;
    int bluesq = ((int)GetBValue(color1)) - (int)GetBValue(color2);
    bluesq *= bluesq;
    int greensq = ((int)GetGValue(color1)) - (int)GetGValue(color2);
    greensq *= greensq;

    if (fMoreThan128)
    {
        return (2 * redsq + 4 * greensq + 3 * bluesq);
    }
    else
    {
        return (3 * redsq + 4 * greensq + 2 * bluesq);
    }
}

//
// Combo of RGB and HSL
//
int GetColorDistance5(HSL one, COLORREF color1, COLORREF color2)
{
    return GetColorDistance(color1, color2) + GetColorDistance2(one, color2);
}



//
// Returns the closest matching EGACOLOR to color.
// The EGACOLORs are chosen from rgColors
//
EGACOLOR GetClosestEGAColorFromSet(int iAlgorithm, COLORREF color, EGACOLOR *rgColors, int cColors)
{
    if (!g_bFilledCOLORREFArray)
    {
        FillCOLORREFArray();
        ASSERT(g_bFilledCOLORREFArray);
    }

    int closest = INT_MAX;
    int leastdithered = INT_MAX;
    EGACOLOR egacolor = { 0 };
    for (int iIndex = 0; iIndex < cColors; iIndex++)
    {
        BYTE egaIndex = EGACOLOR_TO_BYTE(rgColors[iIndex]);
        int closeness;
        switch (iAlgorithm)
        {
        case 1:
            closeness = GetColorDistance(g_rgColorCombos[egaIndex], color);
            break;
        case 2:
            closeness = GetColorDistance2(g_rgColorCombosHSL[egaIndex], color);
            break;
        case 3:
            closeness = GetColorDistance3(g_rgColorCombos[egaIndex], color);
            break;
        case 4:
            closeness = GetColorDistance4(g_rgColorCombos[egaIndex], color);
            break;
        case 5:
            closeness = GetColorDistance5(g_rgColorCombosHSL[egaIndex], g_rgColorCombos[egaIndex], color);
            break;
        default:
            ASSERT(FALSE);
        }

        if (closeness <= closest)
        {
            BOOL set = FALSE;
            if (closeness == closest)
            {
                // Before going again and setting it, we should check the ega ditherness.
                // We want as close to zero as possible. e.g. we prefer grey/grey to black/white
                // So check the difference between the two components of the dithered EGA color
                int ditherness = GetColorDistance(g_egaColorsCR[rgColors[iIndex].color1], g_egaColorsCR[rgColors[iIndex].color2]);
                if (ditherness < leastdithered)
                {
                    leastdithered = ditherness;
                    set = TRUE;
                }
            }
            else
            {
                set = TRUE;
            }

            if (set)
            {
                closest = closeness;
                egacolor = rgColors[iIndex];
            }
        }
    }
    return egacolor;
}

EGACOLOR GetClosestEGAColor(int iAlgorithm, int iPalette, COLORREF color)
{
    EGACOLOR *pPalette = NULL;
    int cEntries = 0;
    switch (iPalette)
    {
    case 1:
        pPalette = g_rg136ToByte;
        cEntries = ARRAYSIZE(g_rg136ToByte);
        break;
    case 2:
        pPalette = g_rg16ToByte;
        cEntries = ARRAYSIZE(g_rg16ToByte);
        break;
    case 3:
        pPalette = g_rgSmoothToByte;
        cEntries = g_cSmoothEntries;
        break;
    default:
        ASSERT(FALSE);
    }
    return GetClosestEGAColorFromSet(iAlgorithm, color, pPalette, cEntries);
}

EGACOLOR EGAColorFromByte(BYTE b)
{
    EGACOLOR color = { b & 0xf, (b & 0xf0) >> 4 };
    return color;
}

WORD _HexToWord(PCTSTR psz)
{
    WORD wNum = 0;
	for(int i = 0; _IsNumber(psz[i], TRUE); i++)
    {
		wNum <<= 4;
		if (psz[i] >= '0' && psz[i] <= '9')
        {
		    wNum += psz[i]-'0';
        }
		else if (((char)(psz[i] & 0xDF) >= 'A') &&
				((char)(psz[i] & 0xDF) <= 'F'))
        {
		    wNum += ((char)(psz[i] & 0xDF)-'A') + 10;
        }
	}
    return wNum;
}


DWORD GetDllVersion(LPCTSTR lpszDllName)
{
    HINSTANCE hinstDll;
    DWORD dwVersion = 0;

    /* For security purposes, LoadLibrary should be provided with a 
       fully-qualified path to the DLL. The lpszDllName variable should be
       tested to ensure that it is a fully qualified path before it is used. */
    hinstDll = LoadLibrary(lpszDllName);
	
    if(hinstDll)
    {
        DLLGETVERSIONPROC pDllGetVersion;
        pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinstDll, 
                          "DllGetVersion");

        /* Because some DLLs might not implement this function, you
        must test for it explicitly. Depending on the particular 
        DLL, the lack of a DllGetVersion function can be a useful
        indicator of the version. */

        if(pDllGetVersion)
        {
            DLLVERSIONINFO dvi;
            HRESULT hr;

            ZeroMemory(&dvi, sizeof(dvi));
            dvi.cbSize = sizeof(dvi);

            hr = (*pDllGetVersion)(&dvi);

            if(SUCCEEDED(hr))
            {
               dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
            }
        }

        FreeLibrary(hinstDll);
    }
    return dwVersion;
}

const std::string MakeFile(PCSTR pszContent, const std::string &filename)
{
    std::string path;
    char szTempPath[MAX_PATH];
    if (GetTempPath(ARRAYSIZE(szTempPath), szTempPath))
    {
        strncat_s(szTempPath, filename.c_str(), ARRAYSIZE(szTempPath));
#pragma warning(push)
#pragma warning(disable: 4996)
        FILE *pFile = fopen(szTempPath, "wt");
#pragma warning(pop)
        if (pFile)
        {
            fputs(pszContent, pFile);
            fclose(pFile);
            path = szTempPath;
        }
    }
    return path;
}

std::string MakeTextFile(PCSTR pszContent, const std::string &filename)
{
    char szTempPath[MAX_PATH];
    bool fOk = false;
    if (PathIsRelative(filename.c_str()))
    {
        fOk = !!GetTempPath(ARRAYSIZE(szTempPath), szTempPath);
        strncat_s(szTempPath, filename.c_str(), ARRAYSIZE(szTempPath));
    }
    else
    {
        // Just use the path as is.
        strncpy_s(szTempPath, filename.c_str(), ARRAYSIZE(szTempPath));
        fOk = true;
    }
    if (fOk)
    {
        bool fError = true;

#pragma warning(push)
#pragma warning(disable: 4996)
        FILE *pFile = fopen(szTempPath, "wt");
#pragma warning(pop)
        if (pFile)
        {
            fputs(pszContent, pFile);
            fclose(pFile);
        }
        return szTempPath;
    }
    else
    {
        return "";
    }
}

//
// Given a stringstream, put it into a temporary text file and show it.
//
void ShowTextFile(PCSTR pszContent, const std::string &filename)
{
    std::string actualPath = MakeTextFile(pszContent, filename);
    ShowFile(actualPath);
}

void ShowFile(const std::string &actualPath)
{
    if (!actualPath.empty())
    {
        bool fError = true;
        if (((INT_PTR)ShellExecute(AfxGetMainWnd()->GetSafeHwnd(), "open", actualPath.c_str(), NULL, NULL, SW_SHOWNORMAL)) > 32)
        {
            fError = false;
        }

        if (fError)
        {
            char szMsg[200];
            DWORD_PTR arg = (DWORD_PTR)actualPath.c_str();
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY, 0, GetLastError(), 0, szMsg, ARRAYSIZE(szMsg), (va_list*)&arg);
            AfxMessageBox(szMsg, MB_OK | MB_APPLMODAL);
        }
    }
}

bool Save8BitBmp(const std::string &filename, const BITMAPINFO &info, BYTE *pBits, DWORD id)
{
    std::ofstream bmpFile(filename.c_str(), std::ios::out | std::ios::binary);
    if (info.bmiHeader.biBitCount != 8)
    {
        return false; // We don't support that right now.
    }
    if (bmpFile.is_open())
    {
        // Calc the image size
        DWORD dwSize = info.bmiHeader.biSizeImage;
        if (dwSize == 0)
        {
            // Calculate it.
            ASSERT(info.bmiHeader.biCompression == BI_RGB);
            dwSize = (info.bmiHeader.biBitCount / 8) * CX_ACTUAL(info.bmiHeader.biWidth) * info.bmiHeader.biHeight;
        }

        DWORD cbPalette = sizeof(RGBQUAD) * 256;

        BITMAPFILEHEADER bmfh;
        bmfh.bfType = 'B'+ ('M' << 8);
        int nBitsOffset = info.bmiHeader.biSize + cbPalette;
        LONG lImageSize = dwSize;
        LONG lFileSize = nBitsOffset + lImageSize;
        bmfh.bfOffBits = nBitsOffset + sizeof(bmfh);
        bmfh.bfSize = lFileSize;
        bmfh.bfReserved1 = (WORD)(id & 0x0000FFFF);
        bmfh.bfReserved2 = (WORD)((id & 0xFFFF0000) >> 16);

        // Write the file header.
        bmpFile.write((const char *)&bmfh, sizeof(bmfh)); // 14

        // Write the bitmapinfo
        bmpFile.write((const char *)&info.bmiHeader, info.bmiHeader.biSize);

        // Write the palette
        bmpFile.write((const char *)info.bmiColors, cbPalette); // 2 ^ 8

        // Write the bits.
        bmpFile.write((const char *)pBits, dwSize);
        return true;
    }
    return false;
}

bool EnsureFolderExists(const std::string &folderName, bool throwException)
{
    if (!PathFileExists(folderName.c_str()))
    {
        if (!CreateDirectory(folderName.c_str(), nullptr))
        {
            if (throwException)
            {
                std::string error = GetMessageFromLastError(folderName);
                throw std::exception(error.c_str());
            }
            return false;
        }
    }
    return true;
}
 
std::string GetRandomTempFolder()
{ 
    std::string final;
    char szTempPath[MAX_PATH];
    GetTempPath(ARRAYSIZE(szTempPath), szTempPath);
    char szPath[MAX_PATH];
    GetTempFileName(szTempPath, "SCI", 0, szPath);
    DeleteFile(szPath);
    if (CreateDirectory(szPath, nullptr))
    {
        final = szPath;
    }
    return final;
}

std::string GetBinaryDataVisualization(const uint8_t *data, size_t length, int columns)
{
    uint32_t position = 0;

    MemoryWriter writer;

    while (position < length)
    {
        writer << "0x" << pad(hexu(position), 4, '0') << " ";
        int currentColumns = min(columns, (int)(length - position));
        for (int i = 0; i < currentColumns; i++)
        {
            writer << " " << pad(hexu(data[position + i]), 2, '0');
        }
        // Add a gap
        int spaces = (columns - currentColumns) * 3 + 2;
        for (int i = 0; i < spaces; i++)
        {
            writer << ' ';
        }

        for (int i = 0; i < currentColumns; i++)
        {
            byte c = data[position + i];
            if (isprint(c))
            {
                writer << (char)c;
            }
            else
            {
                writer << ".";
            }
        }
        writer << "\r\n";
        position += currentColumns;
    }

    return writer.str();
}

//
// ScriptId implementation
// (unique identifier for a script)
//
std::string ScriptId::GetTitle() const
{
    TCHAR szFileName[MAX_PATH];
    StringCchCopy(szFileName, ARRAYSIZE(szFileName), _strFileNameOrig.c_str());
    PTSTR pszExt = PathFindExtension(szFileName);
    *pszExt = 0; // Chop off file extension
    return szFileName;
}
std::string ScriptId::GetTitleLower() const
{
    TCHAR szFileName[MAX_PATH];
    StringCchCopy(szFileName, ARRAYSIZE(szFileName), _strFileName.c_str());
    PTSTR pszExt = PathFindExtension(szFileName);
    *pszExt = 0; // Chop off file extension
    return szFileName;
}

std::string ScriptId::GetFullPath() const
{
    std::string fullPath = _strFolder;
    fullPath += "\\";
    fullPath += _strFileNameOrig;
    return fullPath;
}

bool ScriptId::IsHeader() const
{
    PCSTR pszExt = PathFindExtension(_strFileName.c_str());
    return (strcmp(TEXT(".sh"), pszExt) == 0) ||
        (strcmp(TEXT(".shm"), pszExt) == 0) ||
        (strcmp(TEXT(".shp"), pszExt) == 0);
}

void ScriptId::_Init(PCTSTR pszFullFileName, WORD wScriptNum)
{
    _wScriptNum = wScriptNum;
    if (pszFullFileName)
    {
        path fullPath(pszFullFileName);
        _strFileName = fullPath.filename();
        _strFolder = fullPath.parent_path();
        /*
        CString str = pszFullFileName;
        int iIndexBS = str.ReverseFind('\\');
        _strFolder = str.Left(iIndexBS);
        _strFileName = str.Right(str.GetLength() - iIndexBS - 1);*/
        _strFileNameOrig = _strFileName;
        _MakeLower();
        _DetermineLanguage();
    }
}

ScriptId::ScriptId() : _language(LangSyntaxUnknown) { _wScriptNum = InvalidResourceNumber; };

ScriptId::ScriptId(const std::string &fullPath) : _language(LangSyntaxUnknown)
{
    _Init(fullPath.c_str());
}
ScriptId::ScriptId(PCTSTR pszFullFileName) : _language(LangSyntaxUnknown)
{
    _Init(pszFullFileName);
}
ScriptId::ScriptId(PCTSTR pszFileName, PCTSTR pszFolder) : _language(LangSyntaxUnknown)
{
    assert(StrChr(pszFileName, '\\') == nullptr); // Ensure file and path are not mixed up.
    _strFileName = pszFileName;
    _strFolder = pszFolder;
    _MakeLower();
    _wScriptNum = InvalidResourceNumber;
}

ScriptId::ScriptId(const ScriptId &src)
{
    _strFolder = src.GetFolder();
    _strFileName = src.GetFileName();
    _strFileNameOrig = src._strFileNameOrig;
    _MakeLower();
    _wScriptNum = src.GetResourceNumber();
    _language = src._language;
}

ScriptId& ScriptId::operator=(const ScriptId& src)
{
    _strFolder = src.GetFolder();
    _strFileName = src.GetFileName();
    _strFileNameOrig = src._strFileNameOrig;
    _MakeLower();
    _wScriptNum = src.GetResourceNumber();
    _language = src._language;
    return(*this);
}

// Set the path w/o changing the resource number.
void ScriptId::SetFullPath(const std::string &fullPath)
{
    _Init(fullPath.c_str(), GetResourceNumber());
}


void ScriptId::SetLanguage(LangSyntax lang) { _language = lang; }

BOOL ScriptId::IsNone() const { return _strFileName.empty(); }
const std::string &ScriptId::GetFileName() const { return _strFileName; }
const std::string &ScriptId::GetFolder() const { return _strFolder; }
const std::string &ScriptId::GetFileNameOrig() const { return _strFileNameOrig; }

PCSTR g_codeFileEndings[] =
{
    ".sh",
    ".sc",
    ".shm",
    ".shp",
};

bool IsCodeFile(const std::string &text)
{
    for (PCSTR ending : g_codeFileEndings)
    {
        if (lstrcmpi(ending, PathFindExtension(text.c_str())) == 0)
        {
            return true;
        }
    }
    return false;
}

const std::string SCILanguageMarker = "SCI Script";

LangSyntax _DetermineLanguage(const std::string &firstLine)
{
    LangSyntax langSniff = LangSyntaxStudio;
    size_t pos = firstLine.find(';');
    if (pos != std::string::npos)
    {
        if (firstLine.find(SCILanguageMarker) != std::string::npos)
        {
            langSniff = LangSyntaxSCI;
        }
    }
    return langSniff;
}

void ScriptId::_DetermineLanguage()
{
    if (_language == LangSyntaxUnknown)
    {
        if ((0 == strcmp(".scp", PathFindExtension(_strFileName.c_str()))))
            // (0 == strcmp(".shp", PathFindExtension(_strFileName.c_str()))))
        {
            _language = LangSyntaxCpp;
        }
        else
        {
            // Sniff the file
            LangSyntax langSniff = LangSyntaxUnknown;
            std::ifstream file(GetFullPath());
            std::string line;
            if (std::getline(file, line))
            {
                langSniff = ::_DetermineLanguage(line);
            }
            else
            {
                // This can happen if the file doesn't exist.
                _language = LangSyntaxStudio; // For compat reasons.
            }
            _language = langSniff;
        }
    }
}

LangSyntax ScriptId::Language() const
{
    assert(_language != LangSyntaxUnknown);
    return _language;
}

void ScriptId::_MakeLower()
{
    transform(_strFolder.begin(), _strFolder.end(), _strFolder.begin(), tolower);
    transform(_strFileName.begin(), _strFileName.end(), _strFileName.begin(), tolower);
}

void ScriptId::SetResourceNumber(WORD wScriptNum)
{
    ASSERT((_wScriptNum == InvalidResourceNumber) || (_wScriptNum == wScriptNum));
    _wScriptNum = wScriptNum;
}

bool operator==(const ScriptId& script1, const ScriptId& script2)
{
    bool fRet;
    fRet = (script1.GetFileName() == script2.GetFileName());
    if (fRet)
    {
        // Don't think we need to check _wScriptNum, in case it isn't set?
        fRet = (script1.GetFolder() == script2.GetFolder());
    }
    return fRet;
}

bool operator<(const ScriptId& script1, const ScriptId& script2)
{
    bool fRet;
    fRet = (script1.GetFileName() < script2.GetFileName());
    if (fRet)
    {
        // Don't think we need to check _wScriptNum, in case it isn't set?
        fRet = (script1.GetFolder() < script2.GetFolder());
    }
    return fRet;
}


void throw_if(bool value, const char *message)
{
    if (value)
    {
        throw std::exception(message);
    }
}

// Ugly code straight off MSDN
std::string GetMessageFromLastError(const std::string &details)
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)details.c_str()) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        details.c_str(), dw, lpMsgBuf);

    std::string message = (LPCTSTR)lpDisplayBuf;

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);

    return message;
}

ScopedFile::ScopedFile(const std::string &filename, DWORD desiredAccess, DWORD shareMode, DWORD creationDisposition) : filename(filename)
{
    hFile = CreateFile(filename.c_str(), desiredAccess, shareMode, nullptr, creationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        std::string details = "Opening ";
        details += filename;
        throw std::exception(GetMessageFromLastError(details).c_str());
    }
}

void ScopedFile::Write(const uint8_t *data, uint32_t length)
{
    DWORD cbWritten = 0;
    if (length > 0)
    {
        if (!WriteFile(hFile, data, length, &cbWritten, nullptr) || (cbWritten != length))
        {
            std::string details = "Writing to ";
            details += filename;
            throw std::exception(GetMessageFromLastError(details).c_str());
        }
    }
}

uint32_t ScopedFile::SeekToEnd()
{
    uint32_t position = SetFilePointer(hFile, 0, nullptr, FILE_END);
    if (position == INVALID_SET_FILE_POINTER)
    {
        throw std::exception("Can't seek to end");
    }
    return position;
}

uint32_t ScopedFile::GetLength()
{
    DWORD upperSize;
    uint32_t size = GetFileSize(hFile, &upperSize);
    if (upperSize > 0)
    {
        throw std::exception("File too large.");
    }
    return size;
}

void testopenforwrite(const std::string &filename)
{
    ScopedFile file(filename, GENERIC_WRITE, 0, OPEN_ALWAYS);
}

void movefile(const std::string &from, const std::string &to)
{
    if (!MoveFile(from.c_str(), to.c_str()))
    {
        std::string details = "Moving ";
        details += from;
        details += " to ";
        details += to;
        throw std::exception(GetMessageFromLastError(details).c_str());
    }
}

struct convert {
    void operator()(char& c) { c = toupper((unsigned char)c); }
};
void ToUpper(std::string &aString)
{
    std::for_each(aString.begin(), aString.end(), convert());
}

std::string GetExeSubFolder(const char *subFolder)
{
    std::string templateFolder;
    HRESULT hr = E_FAIL;
    TCHAR szPath[MAX_PATH];
    if (GetModuleFileName(nullptr, szPath, ARRAYSIZE(szPath)))
    {
        PSTR pszFileName = PathFindFileName(szPath);
        *pszFileName = 0; // null it
        hr = StringCchCat(szPath, ARRAYSIZE(szPath), subFolder);
        if (SUCCEEDED(hr))
        {
            templateFolder = szPath;
        }
    }
    return templateFolder;
}

bool CopyFilesOver(HWND hwnd, const std::string &from, const std::string &to)
{
    char sz[MAX_PATH];
    StringCchCopy(sz, ARRAYSIZE(sz), from.c_str());
    size_t iDblNullTerm = strlen(sz) + 1;
    bool success = iDblNullTerm < ARRAYSIZE(sz);
    if (success)
    {
        sz[iDblNullTerm] = 0; // pFrom needs to have double null terminate...
        // 2) copy the files over.
        SHFILEOPSTRUCT fileOp = { 0 };
        fileOp.hwnd = hwnd;
        fileOp.wFunc = FO_COPY;
        fileOp.pFrom = sz;
        fileOp.pTo = to.c_str();
        fileOp.fFlags = FOF_NOCONFIRMMKDIR | FOF_NOCOPYSECURITYATTRIBS;
        success = (SHFileOperation(&fileOp) == 0);
    }
    return success;
}

bool DeleteDirectory(HWND hwnd, const std::string &folder)
{
    char szPath[MAX_PATH];
    StringCchCopy(szPath, ARRAYSIZE(szPath), folder.c_str());
    szPath[folder.length() + 1] = 0;   // double null term

    SHFILEOPSTRUCT fileOp = { 0 };
    fileOp.hwnd = nullptr;
    fileOp.wFunc = FO_DELETE;
    fileOp.pFrom = szPath;
    fileOp.pTo = nullptr;
    fileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;
    return (SHFileOperation(&fileOp) == 0);
}

#define VK_A        65
#define VK_C        67
#define VK_V        86
#define VK_X        88
#define VK_Z        90

BOOL HandleEditBoxCommands(MSG* pMsg, CEdit &wndEdit)
{
    BOOL fRet = FALSE;
    if (!fRet)
    {
        if ((pMsg->message >= WM_KEYFIRST) && (pMsg->message <= WM_KEYLAST))
        {
            // Fwd the delete key to the edit control
            if ((pMsg->message != WM_CHAR) && (pMsg->wParam == VK_DELETE))
            {
                ::SendMessage(wndEdit.GetSafeHwnd(), pMsg->message, pMsg->wParam, pMsg->lParam);
                fRet = TRUE; // Don't dispatch message, we handled it.
            }
        }
    }
    if (!fRet)
    {
        if (pMsg->message == WM_KEYDOWN)
        {
            if (GetKeyState(VK_CONTROL) & 0x8000)
            {
                if (pMsg->wParam == VK_C)
                {
                    wndEdit.Copy();
                    fRet = TRUE;
                }
                if (pMsg->wParam == VK_V)
                {
                    wndEdit.Paste();
                    fRet = TRUE;
                }
                if (pMsg->wParam == VK_X)
                {
                    wndEdit.Cut();
                    fRet = TRUE;
                }
                if (pMsg->wParam == VK_Z)
                {
                    wndEdit.Undo();
                    fRet = TRUE;
                }
                if (pMsg->wParam == VK_A)
                {
                    wndEdit.SetSel(0xffff0000);
                    fRet = TRUE;
                }
            }
        }
    }
    return fRet;
}

bool TerminateProcessTree(HANDLE hProcess, DWORD retCode)
{
    bool success = true;
    DWORD killId = GetProcessId(hProcess);

    // Error handling removed for brevity
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process;
    ZeroMemory(&process, sizeof(process));
    process.dwSize = sizeof(process);
    Process32First(snapshot, &process);
    std::unordered_map<DWORD, DWORD> childToParent;
    do
    {
        childToParent[process.th32ProcessID] = process.th32ParentProcessID;
    } while (Process32Next(snapshot, &process));

    std::set<DWORD> killIds;
    killIds.insert(killId);
    for (auto &pair : childToParent)
    {
        bool kill = false;
        std::vector<DWORD> childrenToKill;
        auto itParent = childToParent.find(pair.first);

        while ((itParent != childToParent.end()) && itParent->first)
        {
            childrenToKill.push_back(itParent->first);
            if (itParent->second == killId)
            {
                kill = true;
                break;
            }
            DWORD temp = itParent->first;
            itParent = childToParent.find(itParent->second);
            if ((itParent != childToParent.end()) && (temp == itParent->second))
            {
                // REVIEW: Got into an infinite loop here when we had a mapping of:
                // [508]->[524], and
                // [524]->[508]
                // This may be a race condition where new processes were created?
                // This has happened twice now.
                // Let's detect and bail.
                success = false;
                break;
            }
        }
        if (kill)
        {
            killIds.insert(childrenToKill.begin(), childrenToKill.end());
        }
    }

    for (DWORD killPid : killIds)
    {
        HANDLE killHandle = OpenProcess(PROCESS_TERMINATE, TRUE, killPid);
        if (killHandle)
        {
            TerminateProcess(killHandle, 0);
            CloseHandle(killHandle);
        }
    }
    return success;
}

// Very basic function that turns a string into an integer, with no exceptions, no error-checking, etc...
int string_to_int(const std::string &word)
{
    int value = 0;
    for (char c : word)
    {
        int digitValue = c - '0';
        if ((digitValue < 0) || (digitValue > 9))
        {
            break;
        }
        value *= 10;
        value += digitValue;
    }
    return value;
}
