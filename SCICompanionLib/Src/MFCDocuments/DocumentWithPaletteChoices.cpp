#include "stdafx.h"
#include "DocumentWithPaletteChoices.h"
#include "AppState.h"
#include "ResourceEntity.h"
#include "format.h"

const int EmbeddedPaletteId = -1;
const int EmbeddedPaletteOnlyId = -2;

DocumentWithPaletteChoices::DocumentWithPaletteChoices() : _currentPaletteIndex(0) {}

void DocumentWithPaletteChoices::SwitchToEmbeddedPalette()
{
    if (_currentPaletteIndex < (int)_paletteChoices.size())
    {
        int id = _paletteChoices[_currentPaletteIndex];
        if ((id == EmbeddedPaletteId) || (id == EmbeddedPaletteOnlyId))
        {
            // We're good
            return;
        }
    }

    for (int i = 0; i < (int)_paletteChoices.size(); i++)
    {
        if (_paletteChoices[i] == EmbeddedPaletteOnlyId)
        {
            SetPaletteChoice(i, false);
            break;
        }
    }
}

const PaletteComponent *DocumentWithPaletteChoices::_GetEmbeddedPalette()
{
    const PaletteComponent *palette = nullptr;
    const ResourceEntity *resource = v_GetResource();
    if (resource)
    {
        palette = resource->TryGetComponent<PaletteComponent>();
    }
    return palette;
}

void DocumentWithPaletteChoices::_SetInitialPalette()
{
    if (v_IsVGA())
    {
        SetPaletteChoice(_currentPaletteIndex, true);
    }
}

void DocumentWithPaletteChoices::RefreshPaletteOptions()
{
    _paletteChoices = appState->GetResourceMap().GetPaletteList();
    if (_GetEmbeddedPalette())
    {
        _paletteChoices.insert(_paletteChoices.begin(), EmbeddedPaletteId);
        _paletteChoices.push_back(EmbeddedPaletteOnlyId);
    }
    _SetInitialPalette();
    v_OnUpdatePaletteOptions();
}


void DocumentWithPaletteChoices::SetPaletteChoice(int index, bool force)
{
    if ((force || (index != _currentPaletteIndex)) && (index >= 0) && (index < (int)_paletteChoices.size()))
    {
        _currentPaletteIndex = index;
        int choice = _paletteChoices[index];
        if (choice == EmbeddedPaletteId)
        {
            _currentPaletteComponent = appState->GetResourceMap().GetMergedPalette(*v_GetResource(), 999);
        }
        else if (choice == EmbeddedPaletteOnlyId)
        {
            const PaletteComponent *temp = _GetEmbeddedPalette();
            if (temp)
            {
                _currentPaletteComponent = std::make_unique<PaletteComponent>(*temp);
            }
            else
            {
                _currentPaletteComponent.reset(nullptr);
            }
        }
        else
        {
            _currentPaletteComponent = appState->GetResourceMap().GetPalette(choice);
        }
        v_OnUpdatePaletteChoice();
    }
}

std::string DocumentWithPaletteChoices::GetPaletteChoiceName(int index)
{
    std::string name;
    if (index < (int)_paletteChoices.size())
    {
        if (_paletteChoices[index] == EmbeddedPaletteId)
        {
            name = "Embedded";
        }
        else if (_paletteChoices[index] == EmbeddedPaletteOnlyId)
        {
            name = "Embedded only";
        }
        else
        {
            name = fmt::format("{0}", _paletteChoices[index]);
        }
    }
    return name;
}

const PaletteComponent *DocumentWithPaletteChoices::GetCurrentPaletteComponent() const
{
    return _currentPaletteComponent.get();
}

