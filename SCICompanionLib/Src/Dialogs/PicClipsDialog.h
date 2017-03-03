/***************************************************************************
Copyright (c) 2017 Philip Fortier

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

struct PaletteComponent;
class ResourceEntity;
class PicCommand;
struct sRECT;

struct PicClip
{
    PicClip() {}
    PicClip(const std::string &name, const sRECT &bounds, std::unique_ptr<std::vector<PicCommand>> &&Commands);


    std::string Name;
    std::unique_ptr<std::vector<PicCommand>> Commands;
    CRect Bounds;
};

class PicClipsDialog : public CExtResizableDialog
{
public:
    PicClipsDialog(CWnd* pParent = nullptr);   // standard constructor

    // Dialog Data
    enum { IDD = IDD_PICCLIPS };

    BOOL PreTranslateMessage(MSG* pMsg) override;
    void SetNumber(int number) { _number = number; }

    const std::vector<PicCommand> &GetCurrentClip(CRect &boundsOut);
    void MoveSelection(int delta);


private:
    void _PopulateFilesList();
    void OnLvnBeginlabeleditListscripts(NMHDR *pNMHDR, LRESULT *pResult);
    void OnLvnEndlabeleditListscripts(NMHDR *pNMHDR, LRESULT *pResult);
    void OnCbnSelchangeFiles();

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

    CExtButton m_wndSave;
    CListCtrl m_wndClips;
    CExtComboBox m_wndFiles;

    // Visuals
    CExtButton m_wndOK;

    std::vector<std::unique_ptr<PicClip>> _clips;

    std::vector<std::string> _fileNames;

    void _InitClipsList();
    void _UpdateList();

    HACCEL _hAccel;

    bool _initialized;
    int _number;
    bool _inLabelEdit;

public:
    afx_msg void OnUpdateCopyPaste(CCmdUI *pCmd);
    afx_msg void OnBnClickedSave();
    afx_msg void OnPaste();
};
