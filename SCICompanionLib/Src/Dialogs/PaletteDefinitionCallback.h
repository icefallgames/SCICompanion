
#pragma once

class IPaletteDefinitionCallback
{
public:
    virtual void OnSomethingChanged(BOOL fWriteEntire, EGACOLOR *pPalettes, int iPalette) = 0;
    virtual void OnPreviewOff() = 0;
};


