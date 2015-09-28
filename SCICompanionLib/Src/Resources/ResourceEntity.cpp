#include "stdafx.h"
#include "ResourceEntity.h"
#include "RasterOperations.h"
#include "AppState.h"

bool NoValidationFunc(const ResourceEntity &resource)
{
    return true;
}

ResourceTraits emptyTraits =
{
    ResourceType::None,
    nullptr,
    nullptr,
    nullptr,
    nullptr
};

ResourceEntity::ResourceEntity(const ResourceTraits &traits) :
Traits(traits),
ResourceNumber(-1),
PackageNumber(-1),
Base36Number(NoBase36)
{
}

ResourceEntity::ResourceEntity() : ResourceEntity(emptyTraits) {}

void ResourceEntity::WriteToTest(sci::ostream &byteStream, bool fullWrite, int resourceNumber) const
{
    std::map<BlobKey, uint32_t> propertyBag;
    WriteTo(byteStream, fullWrite, resourceNumber, propertyBag);
}

void ResourceEntity::WriteTo(sci::ostream &byteStream, bool fullWrite, int resourceNumber, std::map<BlobKey, uint32_t> &propertyBag) const
{
    if (Traits.WriteToFunc)
    {
        (*Traits.WriteToFunc)(*this, byteStream, propertyBag);
    }
    if (fullWrite && Traits.SidecarWriteToFunc)
    {
        (*Traits.SidecarWriteToFunc)(*this, resourceNumber);
    }
}

void ResourceEntity::ReadFrom(sci::istream byteStream, const std::map<BlobKey, uint32_t> &propertyBag)
{
    (*Traits.ReadFromFunc)(*this, byteStream, propertyBag);
}
