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

class LevelMeter : public CStatic
{
    DECLARE_DYNAMIC(LevelMeter)

public:
    LevelMeter();
    virtual ~LevelMeter();

    void Monitor(bool monitor);
    void SetCheckBox(CExtCheckBox *checkbox) { _checkbox = checkbox; }

private:
    void PreSubclassWindow() override;
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
    void _OnDraw(CDC *pDC, LPRECT prc);

    int _level;
    int _maxRecentLevel;
    bool _monitor;

    CExtCheckBox *_checkbox;

    DECLARE_MESSAGE_MAP()
};
