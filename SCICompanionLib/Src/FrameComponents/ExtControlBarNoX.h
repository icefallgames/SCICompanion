#pragma once

//
// Subclasses a Prof-UIS ExtControlBar to remove the 'x' that allows the user to close it.
//

class CExtControlBarNoX : public CExtControlBar
{
public:
    void OnNcAreaButtonsReinitialize();
};