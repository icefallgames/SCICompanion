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
    nullptr
};

ResourceEntity::ResourceEntity(const ResourceTraits &traits) :
Traits(traits),
ResourceNumber(-1),
PackageNumber(-1),
CanSave(true)
{
}
