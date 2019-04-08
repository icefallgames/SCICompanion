#pragma once


struct NamedPosition
{
    NamedPosition() : View(0), Loop(0), Cel(0), Z(0) {}

    std::string Name;
    point16 Position;
    int16_t Z;

    // Not actually used, I guess, but good to save.
    int16_t View;
    int16_t Loop;
    int16_t Cel;
    int16_t Pri;
};
