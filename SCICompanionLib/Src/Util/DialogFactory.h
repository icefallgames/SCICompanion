
#pragma once

class IDialogFactory
{
public:
    virtual void InvokeDialog(UINT nID, RECT *prcTarget) = 0;
};

