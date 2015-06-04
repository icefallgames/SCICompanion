#pragma once

#include "Components.h"

class ResourceEntity;

// rawDataStream is used if the image bits are stored separately from the RLE encoding opcodes.
void ReadImageData(sci::istream &byteStream, Cel &cel, bool isVGA);
void ReadImageData(sci::istream &byteStreamRLE, Cel &cel, bool isVGA, sci::istream &byteStreamLiteral);
void WriteImageData(sci::ostream &byteStream, const Cel &cel, bool isVGA);
void WriteImageData(sci::ostream &rleStream, const Cel &cel, bool isVGA, sci::ostream &literalStream, bool writeZero);
void ReadCelFromVGA11(sci::istream &byteStream, Cel &cel, bool isPic);

extern uint8_t g_vgaPaletteMapping[256];

struct Cel
{
    Cel(size16 size, point16 placement, uint8_t transparentColor)
    {
        this->size = size;
        this->placement = placement;
        this->TransparentColor = transparentColor;
    }

    Cel() = default;
    Cel(const Cel &cel) = default;
    Cel &operator=(const Cel &cel) = default;

    size_t GetDataSize() const
    {
        return (CX_ACTUAL(size.cx) * size.cy);
    }

    // REVIEW: std::vector is 16 bytes. We could change this to 4 bytes if we wish
    // (thus reducing size of a cel from 24 to 12?) 
    // But we'd need to write a manual copy/assignment constructor.
    sci::array<uint8_t> Data;
    size16 size;
    point16 placement;
    uint8_t TransparentColor;
};

struct Loop
{
    Loop() : UnknownData(0), IsMirror(false), MirrorOf(0xff) {}
    Loop(const Loop &loop) = default;
    Loop &operator=(const Loop &loop) = default;
    int GetMirrorOf()
    {
        return IsMirror ? (int)MirrorOf : -1;

    }

    std::vector<Cel> Cels;
    uint16_t UnknownData;
    bool IsMirror;
    uint8_t MirrorOf;
};


enum class RasterCaps : uint16_t
{
    None = 0x0000,
    Transparency = 0x0001,
    Placement = 0x0002,
    Resize = 0x0004,
    Reorder = 0x0008,
    Mirror = 0x0010,
    Animate = 0x0020,
    SCI0CursorPlacement = 0x0040,
    EightBitPlacement = 0x0080,
};
DEFINE_ENUM_FLAGS(RasterCaps, uint16_t)

enum class OriginStyle : uint16_t
{
    None,
    BottomCenter,
    TopLeft,
};


enum class PaletteType : uint8_t
{
    EGA_Two,
    EGA_Four,
    EGA_Sixteen,
    VGA_256,
};


struct RasterTraits
{
    // Instances of these should be unique global objects, and people should
    // only hold onto references to them.
    RasterTraits(const RasterTraits &src) = delete;
    RasterTraits& operator=(const RasterTraits &src) = delete;

    RasterCaps Caps;
    OriginStyle OriginStyle;
    uint16_t MaxWidth;
    uint16_t MaxHeight;
    PaletteType PaletteType;
    const uint8_t *PaletteMapping;
    const RGBQUAD *Palette;
    int PreviewCel;
    GetItemLabelFuncPtr GetItemLabelFunc;
    bool SupportsScaling;
};

struct RasterSettings
{
    // Instances of these should be unique global objects, and people should
    // only hold onto references to them.
    RasterSettings(const RasterSettings &src) = delete;
    RasterSettings& operator=(const RasterSettings &src) = delete;

    int DefaultZoom;
};

union CelIndex
{
    CelIndex(const CelIndex& celIndex) = default;
    CelIndex() = default;
    CelIndex(int loopIn, int celIn)
    {
        loop = (uint16_t)loopIn;
        cel = (uint16_t)celIn;
    }
    CelIndex(DWORD indexIn)
    {
        index = indexIn;
    }

    struct
    {
        uint16_t loop;
        uint16_t cel;
    };
    DWORD index;
};



struct RasterComponent : ResourceComponent
{
    RasterComponent();
    RasterComponent(const RasterComponent &src) = default;
    RasterComponent(const RasterTraits &traits, RasterSettings &settings) : Traits(traits), Settings(settings), UnknownData(0), ScaleFlags(0) {}
    ResourceComponent *Clone() const override
    {
        return new RasterComponent(*this);
    }

    // Helper functions. None of these are bounds checked.
    int LoopCount() const { return (int)Loops.size(); }
    int CelCount(int nLoop) const { return (int)Loops[nLoop].Cels.size(); }
    Cel& GetCel(CelIndex index);
    const Cel& RasterComponent::GetCel(CelIndex index) const;

    static RasterComponent CreateDegenerate();

    const RasterTraits &Traits;
    RasterSettings &Settings;
    std::vector<Loop> Loops;
    uint32_t UnknownData;
    uint8_t ScaleFlags;             // Scaling flags for VGA 1.1 views (0x0 is scalable, 0x1 is not)
};

#include <pshpack1.h>
struct CelHeader_VGA11
{
    size16 size;
    point16 placement;
    uint8_t transparentColor;
    // "Unknown":
    uint8_t always_0xa;
    uint8_t temp2, temp3;
    uint32_t totalCelDataSize;
    uint32_t rleCelDataSize;
    uint32_t paletteOffset;     // Used for pic cels only, apparently.
    uint32_t offsetRLE;
    uint32_t offsetLiteral;
    uint32_t something;
};
#include <poppack.h>

ResourceEntity *CreateViewResource(SCIVersion version);
ResourceEntity *CreateDefaultViewResource(SCIVersion version);
