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
