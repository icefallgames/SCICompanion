#pragma once

enum class NamedPositionType
{
    Full,
    Approach,
    PositionOnly
};

struct NamedPosition
{
    NamedPosition() : Type(NamedPositionType::Full), View(0), Loop(0), Cel(0), Z(0), Pri(-1) {}

    NamedPositionType Type;
    std::string Name;

    point16 Position;
    int16_t Z;
    int16_t View;
    int16_t Loop;
    int16_t Cel;
    int16_t Pri;

    bool operator == (const NamedPosition &src);
    bool operator != (const NamedPosition &src);
};
