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
#pragma once

class ResourceBlob;
enum class AppendBehavior;

//
// Clients can implement this interface to be notified when
// resources have been added or removed.
//
class IResourceMapEvents
{
public:
    virtual void OnResourceAdded(const ResourceBlob *pData, AppendBehavior appendBehavior) = 0;
    virtual void OnResourceDeleted(const ResourceBlob *pData) = 0;
    virtual void OnResourceMapReloaded(bool isInitialLoad) = 0;
    virtual void OnResourceTypeReloaded(ResourceType iType) = 0;
};