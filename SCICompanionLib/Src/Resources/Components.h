#pragma once

struct size16
{
    size16() : size16(0, 0) {}
    size16(uint16_t width, uint16_t height) : cx(width), cy(height) {}

    uint16_t cx;
    uint16_t cy;

    bool operator==(const size16 &other) const
    {
        return cx == other.cx && cy == other.cy;
    }
    bool operator!=(const size16 &other) const
    {
        return cx != other.cx || cy != other.cy;
    }
    size16 operator+(size16 other) const
    {
        return size16(cx + other.cx, cy + other.cy);
    }
    size16 operator-(size16 other) const
    {
        return size16(cx - other.cx, cy - other.cy);
    }
};

int PaddedSize(size16 size);
size16 CSizeToSize(CSize size);
CSize SizeToCSize(size16 size);

struct point16
{
    point16() : point16(0, 0) {}
    point16(int16_t xIn, int16_t yIn) : x(xIn), y(yIn) {}

    int16_t x;
    int16_t y;

    bool operator==(const point16 &other) const
    {
        return x == other.x && y == other.y;
    }
    bool operator!=(const point16 &other) const
    {
        return x != other.x || y != other.y;
    }
    point16 operator+(point16 point) const
    {
        return point16(x + point.x, y + point.y);
    }
    point16 operator-(point16 point) const
    {
        return point16(x - point.x, y - point.y);
    }
};

point16 CPointToPoint(CPoint pt);
CPoint PointToCPoint(point16 pt);
point16 CPointToPointClamp(CPoint pt);

struct ResourceComponent
{
    virtual ResourceComponent* Clone() = 0;

    // This is necessary, or else lists of ResourceComponents won't be properly destroyed.
    virtual ~ResourceComponent() {}
};

typedef void(*GetItemLabelFuncPtr)(PTSTR  pszLabel, size_t cch, int nCel);

extern uint8_t g_egaPaletteMapping[16];
