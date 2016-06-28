/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
#include "stdafx.h"
#include "ResourceEntity.h"
#include "RasterOperations.h"
#include "AppState.h"
#include "ResourceBlob.h"

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
Base36Number(NoBase36),
SourceFlags(ResourceSourceFlags::Invalid)
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

// TODO REVIEW: Redo this
// We could trap exceptions and then create the default resource instead?
// Or is the caller responsible?
HRESULT ResourceEntity::InitFromResource(const ResourceBlob *prd)
{
    PackageNumber = prd->GetPackageHint();
    ResourceNumber = prd->HasNumber() ? prd->GetNumber() : -1;
    Base36Number = prd->GetHeader().Base36Number;
    SourceFlags = prd->GetSourceFlags();
    sci::istream byteStream = prd->GetReadStream();
    byteStream.setThrowExceptions(true);
    Traits.ReadFromFunc(*this, byteStream, prd->GetPropertyBag());
    return S_OK;
}

std::unique_ptr<ResourceEntity> ResourceEntity::Clone() const
{
    std::unique_ptr<ResourceEntity> pClone = std::make_unique<ResourceEntity>(Traits);
    pClone->ResourceNumber = ResourceNumber;
    pClone->PackageNumber = PackageNumber;
    pClone->Base36Number = Base36Number;
    pClone->SourceFlags = SourceFlags;

    for (auto &pair : components)
    {
        std::unique_ptr<ResourceComponent> pCopy(pair.second->Clone());
        pClone->components[pair.first] = std::move(pCopy);
    }
    return pClone;
}