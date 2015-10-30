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
#pragma once

#include "Resource.h"
#include "NoFlickerStatic.h"

// CGameVersionDialog dialog

class CGameVersionDialog : public CExtNCW<CExtResizableDialog>
{
public:
    CGameVersionDialog(SCIVersion &version, CWnd* pParent = NULL);   // standard constructor

    // Dialog Data
    enum { IDD = IDD_DIALOGVERSION };

    SCIVersion &_version;


private:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK() override;
    afx_msg void OnViewResourceMap();
    afx_msg void OnViewMessageMap();
    DECLARE_MESSAGE_MAP()

    void _Sync();
    SCIVersion _ReverseSync();

    int _fHasPalette;
    int _fGrayscaleCursors;
    int _fCodeSCI1;
    int _fSeparateHeapResources;
    int _fVocab900;
    int _fEarlySCI0Script;
    int _fSCI11Palettes;
    int _fSupportsMessages;
    int _fSupportsMessageAudio;
    int _fAudioIsWavFormat;

    int _viewFormat;
    int _picFormat;
    int _audioVolume;
    int _resourceMapVersion;
    int _resourcePackVersion;
    int _compressionVersion;
    int _soundVersion;
    int _mainAudioMapFormat;
    int _base36AudioMapFormat;
    int _audioMapNumberIndex;
    int _messageMapSource;
    int _resolution;

    CExtButton m_wndOk;
    CExtButton m_wndCancel;
    CExtButton m_wndViewResMap;
    CExtButton m_wndViewMessageMap;

    CExtGroupBox m_wndGroupResourceMap;
    CExtRadioButton m_wndRadioResourceMapSCI0;
    CExtRadioButton m_wndRadioResourceMapSCI0_SCI1;
    CExtRadioButton m_wndRadioResourceMapSCI1;
    CExtRadioButton m_wndRadioResourceMapSCI11;
    CExtRadioButton m_wndRadioResourceMapSCI2;

    CExtGroupBox m_wndGroupResourcePack;
    CExtRadioButton m_wndRadioResourcePackSCI0;
    CExtRadioButton m_wndRadioResourcePackSCI1;
    CExtRadioButton m_wndRadioResourcePackSCI11;
    CExtRadioButton m_wndRadioResourcePackSCI2;

    CExtGroupBox m_wndGroupSound;
    CExtRadioButton m_wndRadioSoundSCI0;
    CExtRadioButton m_wndRadioSoundSCI1;

    CExtGroupBox m_wndGroupCompression;
    CExtRadioButton m_wndRadioCompressionSCI0;
    CExtRadioButton m_wndRadioCompressionSCI1;

    CExtComboBox m_wndViewCombo;
    CExtComboBox m_wndPicCombo;
    CExtComboBox m_wndAudioCombo;

    CExtLabel m_wndLabel10;
    CExtComboBox m_wndMainAudioMapCombo;
    CExtLabel m_wndLabel11;
    CExtComboBox m_wndBase36AudioMapCombo;
    CExtLabel m_wndLabel12;
    CExtComboBox m_wndAudioMapNumberCombo;
    CExtLabel m_wndLabel13;
    CExtComboBox m_wndMessageMapSourceCombo;
    CExtLabel m_wndLabel14;
    CExtComboBox m_wndResolution;

    CExtLabel m_wndLabel4;
    CExtLabel m_wndLabel5;
    CExtLabel m_wndLabel7;

    CExtCheckBox m_wndHasPalette;
    CExtCheckBox m_wndGrayscaleCursors;
    CExtCheckBox m_wndSCI1Code;
    CExtCheckBox m_wndSepHeap;
    CExtCheckBox m_wndParserVocab900;
    CExtCheckBox m_wndEarlySCI0Script;
    CExtCheckBox m_wndSCI11Palettes;
    CExtCheckBox m_wndSupportsMessages;
    CExtCheckBox m_wndSupportsMessageAudio;
    CExtCheckBox m_wndAudioIsWavFormat;
};
