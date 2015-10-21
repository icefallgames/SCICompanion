
#pragma once

class IVGAPaletteDefinitionCallback
{
public:
    virtual void OnVGAPaletteChanged() = 0;
};

class IEGAPaletteDefinitionCallback
{
public:
    virtual void SetPosition(ptrdiff_t pos) = 0;
};