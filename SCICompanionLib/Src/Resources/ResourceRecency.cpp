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

#include "ResourceRecency.h"
#include "ResourceBlob.h"

uint64_t _GetLookupKey(const IResourceIdentifier *pData)
{
    // These should always be already-added resources, so they should never be -1
    assert(pData->GetNumber() != -1);
    assert(pData->GetPackageHint() != -1);
    uint64_t first = pData->GetNumber() * 256 * 256 + pData->GetPackageHint();
    uint64_t low = pData->GetBase36();
    return low + (first << 32);
}

template<typename _T, typename _K>
bool contains(const _T &container, const _K &key)
{
    return container.find(key) != container.end();
}

void ResourceRecency::AddResourceToRecency(const IResourceIdentifier *pData, BOOL fAddToEnd)
{
    _idJustAdded = -1;
    uint64_t iKey = _GetLookupKey(pData);

    ResourceIdArray *pidList;
    RecencyMap::iterator found = _resourceRecency[(int)pData->GetType()].find(iKey);
    if (found != _resourceRecency[(int)pData->GetType()].end())
    {
        pidList = found->second;
#ifdef DEBUG
        // Now that we use a checksum instead of an id, this is not a valid ASSERT...
        // Assert that a resource of this id doesn't not already exist in here.
        /*for (INT_PTR i = 0; i <= pidList->GetUpperBound(); i++)
        {
            ASSERT(pData->GetId() != pidList->GetAt(i));
        }*/
#endif
    }
    else
    {
        // A new one.
        pidList = new ResourceIdArray;
        _resourceRecency[(int)pData->GetType()][iKey] = pidList;
    }
    // Add this resource.
    if (fAddToEnd)
    {
        // This is called when we're enumerating.
        pidList->push_back(pData->GetChecksum());
    }
    else
    {
        // This is called when the user adds a resource.
        _idJustAdded = pData->GetChecksum();
        pidList->insert(pidList->begin(), pData->GetChecksum());
    }
}

void ResourceRecency::DeleteResourceFromRecency(const ResourceBlob *pData)
{
    uint64_t iKey = _GetLookupKey(pData);

    RecencyMap::iterator found = _resourceRecency[(int)pData->GetType()].find(iKey);
    if (found != _resourceRecency[(int)pData->GetType()].end())
    {
        ResourceIdArray *pidList = found->second;
        // Find this item's id.
        for (ResourceIdArray::iterator it = pidList->begin(); it != pidList->end(); ++it)
        {
            if (*it == pData->GetChecksum())
            {
                // Found it. Remove it.
                pidList->erase(it);
                break;
            }
        }
    }
}

//
// Is this the most recent resource of this type.
//
bool ResourceRecency::IsResourceMostRecent(const IResourceIdentifier *pData)
{
    bool fRet;
    if (pData->GetNumber() == -1)
    {
        // The resource is not saved - so consider it the "most recent" version of itself.
        fRet = true;
    }
    else
    {
        fRet = false;
        uint64_t iKey = _GetLookupKey(pData);

        RecencyMap::iterator found = _resourceRecency[(int)pData->GetType()].find(iKey);
        if (found != _resourceRecency[(int)pData->GetType()].end())
        {
            ResourceIdArray *pidList = found->second;
            // (should always have at least one element)
            fRet = !pidList->empty() && (*pidList->begin() == pData->GetChecksum());
        }
        else
        {
            // If we didn't find anything, then consider it the most recent.
            // (for some items, like audio resources, we don't calculate recency)
            fRet = true;
        }
    }
    return fRet;
}

bool ResourceRecency::WasResourceJustAdded(const ResourceBlob *pData)
{
    return (pData->GetChecksum() == _idJustAdded);
}

void ResourceRecency::ClearResourceType(int iType)
{
    ASSERT(iType < NumResourceTypes);

    RecencyMap &map = _resourceRecency[iType];
    // Before removing all, we must de-allocate each array we created.
    for_each(map.begin(), map.end(), delete_map_value());
    map.clear();
}

void ResourceRecency::ClearAllResourceTypes()
{
    for (int i = 0; i < NumResourceTypes; i++)
    {
        ClearResourceType(i);
    }
}
