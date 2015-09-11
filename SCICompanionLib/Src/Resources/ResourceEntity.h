#pragma once

#include "Components.h"
#include <memory>
#include <typeinfo>
#include <typeindex>

class ResourceEntity;

typedef void(*DeserializeFuncPtr)(ResourceEntity &resource, sci::istream &byteStream);
typedef void(*SerializeFuncPtr)(const ResourceEntity &resource, sci::ostream &byteStream);
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
    ResourceEntity() : ResourceEntity(emptyTraits) {}

    ResourceEntity(const ResourceEntity &src) = delete;
    ResourceEntity &operator=(const ResourceEntity &src) = delete;

    // TODO REVIEW: Redo this
    // We could trap exceptions and then create the default resource instead?
    // Or is the caller responsible?
    HRESULT InitFromResource(const ResourceBlob *prd)
    {
        PackageNumber = prd->GetPackageHint();
        ResourceNumber = prd->HasNumber() ? prd->GetNumber() : -1;
        SourceFlags = prd->GetSourceFlags();
        sci::istream byteStream = prd->GetReadStream();
        byteStream.setThrowExceptions(true);
        Traits.ReadFromFunc(*this, byteStream);
        return S_OK;
    }

    std::unique_ptr<ResourceEntity> Clone() const
    {
        std::unique_ptr<ResourceEntity> pClone = std::make_unique<ResourceEntity>(Traits);
        pClone->ResourceNumber = ResourceNumber;
        pClone->PackageNumber = PackageNumber;
        pClone->SourceFlags = SourceFlags;

        for (auto &pair : components)
        {
            std::unique_ptr<ResourceComponent> pCopy(pair.second->Clone());
            pClone->components[pair.first] = std::move(pCopy);
        }
        return pClone;
    }
    
    int ResourceNumber;
    int PackageNumber;
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

    void ReadFrom(sci::istream byteStream)
    {
        (*Traits.ReadFromFunc)(*this, byteStream);
    }

    void WriteTo(sci::ostream &byteStream, bool fullWrite, int resourceNumber) const
    {
        if (Traits.WriteToFunc)
        {
            (*Traits.WriteToFunc)(*this, byteStream);
        }
        if (fullWrite && Traits.SidecarWriteToFunc)
        {
            (*Traits.SidecarWriteToFunc)(*this, resourceNumber);
        }
    }

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
