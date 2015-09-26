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