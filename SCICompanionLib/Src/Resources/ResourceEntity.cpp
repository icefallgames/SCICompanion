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
