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

#include "Components.h"
#include <memory>
#include <typeinfo>
#include <typeindex>

class ResourceBlob;
class ResourceEntity;
enum class BlobKey;
enum class ResourceSourceFlags;

typedef void(*DeserializeFuncPtr)(ResourceEntity &resource, sci::istream &byteStream, const std::map<BlobKey, uint32_t> &propertyBag);
typedef void(*SerializeFuncPtr)(const ResourceEntity &resource, sci::ostream &byteStream, std::map<BlobKey, uint32_t> &propertyBag);
typedef void(*SidecarSerializeFuncPtr)(const ResourceEntity &resource, int resourceNumber);
typedef bool(*ValidationFuncPtr)(const ResourceEntity &resource);

struct ResourceTraits
{
    //ResourceTraits() : Type(ResourceType::None), ReadFromFunc(nullptr), WriteToFunc(nullptr), ValidationFunc(nullptr), SidecarWriteToFunc(nullptr) {}

    // Instances of these should be unique global objects, and people should
    // only hold onto references to them.
    ResourceTraits(const ResourceTraits &src) = delete;
    ResourceTraits& operator=(const ResourceTraits &src) = delete;

    ResourceType Type;
    DeserializeFuncPtr ReadFromFunc;
    SerializeFuncPtr WriteToFunc;               // This is null if saving isn't supported for this resource.
    ValidationFuncPtr ValidationFunc;
    SidecarSerializeFuncPtr SidecarWriteToFunc; // Null for most resources. For resources with associated sidecar files.
};

extern ResourceTraits emptyTraits;

bool NoValidationFunc(const ResourceEntity &resource);

class ResourceEntity final
{
public:
    ResourceEntity(const ResourceTraits &traits);
    ResourceEntity();

    ResourceEntity(const ResourceEntity &src) = delete;
    ResourceEntity &operator=(const ResourceEntity &src) = delete;

    // TODO REVIEW: Redo this
    // We could trap exceptions and then create the default resource instead?
    // Or is the caller responsible?
    HRESULT InitFromResource(const ResourceBlob *prd);
    std::unique_ptr<ResourceEntity> Clone() const;
    
    int ResourceNumber;
    int PackageNumber;
    uint32_t Base36Number;
    ResourceSourceFlags SourceFlags;
    const ResourceTraits &Traits;

    template<typename _T>
    const _T &GetComponent() const
    {
        const std::type_info& r2 = typeid(_T);
        auto result = components.find(std::type_index(r2));
        if (result != components.end())
        {
            return static_cast<_T&>(*(result->second));
        }
        throw std::exception("No component of this type exists");
    }

    template<typename _T>
    _T &GetComponent()
    {
        const std::type_info& r2 = typeid(_T);
        auto result = components.find(std::type_index(r2));
        if (result != components.end())
        {
            return static_cast<_T&>(*(result->second));
        }
        throw std::exception("No component of this type exists");
    }

    template<typename _T>
    _T *TryGetComponent()
    {
        const std::type_info& r2 = typeid(_T);
        auto result = components.find(std::type_index(r2));
        if (result != components.end())
        {
            return static_cast<_T*>(result->second.get());
        }
        return nullptr;
    }

    template<typename _T>
    const _T *TryGetComponent() const
    {
        const std::type_info& r2 = typeid(_T);
        auto result = components.find(std::type_index(r2));
        if (result != components.end())
        {
            return static_cast<_T*>(result->second.get());
        }
        return nullptr;
    }

    template<typename _T>
    void AddComponent(std::unique_ptr<_T> pComponent)
    {
        std::unique_ptr<ResourceComponent> pTemp(pComponent.release());
        const std::type_info& r2 = typeid(_T);
        components[std::type_index(r2)] = std::move(pTemp);
    }

    template<typename _T>
    void RemoveComponent()
    {
        const std::type_info& r2 = typeid(_T);
        auto it = components.find(std::type_index(r2));
        if (it != components.end())
        {
            components.erase(it);
        }
    }

    void ReadFrom(sci::istream byteStream, const std::map<BlobKey, uint32_t> &propertyBag);
    void WriteToTest(sci::ostream &byteStream, bool fullWrite, int resourceNumber) const;
    void WriteTo(sci::ostream &byteStream, bool fullWrite, int resourceNumber, std::map<BlobKey, uint32_t> &propertyBag) const;

    bool CanWrite() const
    {
        return Traits.WriteToFunc != nullptr;
    }

    bool PerformChecks() const
    {
        return (*Traits.ValidationFunc)(*this);
    }

    ResourceType GetType() const { return Traits.Type; }

private:
    std::unordered_map<std::type_index, std::unique_ptr<ResourceComponent>> components;
};
