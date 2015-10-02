#pragma once

class LevelMeter : public CStatic
{
    DECLARE_DYNAMIC(LevelMeter)

public:
    LevelMeter();
    virtual ~LevelMeter();

    void Monitor(bool monitor);

private:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
    void _OnDraw(CDC *pDC, LPRECT prc);

    int _level;

    DECLARE_MESSAGE_MAP()
};
