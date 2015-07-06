#pragma once

#include "PaletteOperations.h"

class DocumentWithPaletteChoices
{
public:
    DocumentWithPaletteChoices();

protected:
    virtual bool v_IsVGA() = 0;
    virtual void v_OnUpdatePaletteOptions() = 0;
    virtual const ResourceEntity *v_GetResource() = 0;
    virtual void v_OnUpdatePaletteChoice() = 0;

    void _SetInitialPalette();
    const PaletteComponent *_GetEmbeddedPalette();

public:
    std::vector<int> &GetPaletteChoices() { return _paletteChoices; }
    std::string GetPaletteChoiceName(int index);
    int GetPaletteChoice() { return _currentPaletteIndex; }
    void RefreshPaletteOptions();
    void SetPaletteChoice(int index, bool force);
    void SwitchToEmbeddedPalette();
    const PaletteComponent *GetCurrentPaletteComponent() const;

protected:
    // Keep around one of these too:
    std::unique_ptr<PaletteComponent> _currentPaletteComponent;
    std::vector<int> _paletteChoices;
    int _currentPaletteIndex;  // index into _paletteChoices
};