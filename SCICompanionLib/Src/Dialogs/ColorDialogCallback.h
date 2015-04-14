
#pragma once

class IColorDialogCallback
{
public:
    virtual void OnColorClick(BYTE bIndex, int nID, BOOL fLeft) = 0;
    virtual void OnColorHover(BYTE bIndex) {};  // Provide default impl, since this is uncommon
};


