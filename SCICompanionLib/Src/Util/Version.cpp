#include "stdafx.h"
#include "Version.h"

SCIVersion sciVersion0 =
{
    ResourceMapFormat::SCI0,
    ResourcePackageFormat::SCI0,
    SoundFormat::SCI0,
    CompressionFormat::SCI0,
    false,
    ViewFormat::EGA,
    PicFormat::EGA,
    false,
    false,
    false,
    0,
    1,
    false,
    false
};

SCIVersion sciVersion1_Late =
{
    ResourceMapFormat::SCI1,
    ResourcePackageFormat::SCI1,
    SoundFormat::SCI1,
    CompressionFormat::SCI1,
    true,
    ViewFormat::VGA1,
    PicFormat::VGA1_1,
    false,
    true,
    false,
    900,
    0,
    false,
    false
};

SCIVersion sciVersion1_1 =
{
    ResourceMapFormat::SCI11,
    ResourcePackageFormat::SCI11,
    SoundFormat::SCI1,
    CompressionFormat::SCI1,
    true,
    ViewFormat::VGA1,
    PicFormat::VGA1_1,
    false,
    true,
    true,
    900,
    0,
    false,
    true
};

bool SCIVersion::operator == (const SCIVersion &src)
{
    return 0 == memcmp(this, &src, sizeof(*this));
}
bool SCIVersion::operator != (const SCIVersion &src)
{
    return 0 != memcmp(this, &src, sizeof(*this));
}

