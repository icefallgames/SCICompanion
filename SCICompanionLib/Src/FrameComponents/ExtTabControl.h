#pragma once

class ExtTabControl : public CTabCtrl
{
protected:
    DECLARE_MESSAGE_MAP()

private:
    void DrawItem(LPDRAWITEMSTRUCT);
    void DrawItemBorder(LPDRAWITEMSTRUCT);
    void DrawMainBorder(LPDRAWITEMSTRUCT);

    void OnPaint();
    BOOL OnEraseBkgnd(CDC *pDC);
};
