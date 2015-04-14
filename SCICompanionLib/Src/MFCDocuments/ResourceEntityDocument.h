#pragma once
#include "UndoResource.h"
#include "ResourceDocument.h"
#include "ResourceEntity.h"

class ResourceEntityDocument : public CUndoResource<CResourceDocument, ResourceEntity>
{
public:

    // Crazy variadic templates.
    template<typename... _TComponentArgs, typename _Func>
    bool ApplyChanges(_Func f)
    {
        bool modified = false;
        const ResourceEntity *pEntity = static_cast<const ResourceEntity*>(GetResource());
        if (pEntity)
        {
            std::unique_ptr<ResourceEntity> pNew = pEntity->Clone();
            auto updateHint = f(pNew->GetComponent<_TComponentArgs>()... );

            static_assert(std::is_base_of<CObject, decltype(updateHint)>::value, "The value returned by the function must be a subclass of CObject. Use WrapHint.");

            uint32_t hintNumber = GetHint<uint32_t>(&updateHint);
            // We assume that a hint of zero means nothing changed. So don't set the modified flag.
            modified = (hintNumber != 0);

            // This needs to happen before we update views:
            PostApplyChanges();

            if (modified)
            {
                AddNewResourceToUndo(std::move(pNew));

                // Go ahead and notify things...
                UpdateAllViewsAndNonViews(nullptr, 0, &updateHint);

                if (modified)
                {
                    SetModifiedFlag(TRUE);
                }
            }
        }
        return modified;
    }

    void _OnSuccessfulSave(const ResourceEntity *pResource) override
    {
        SetLastSaved(pResource);
        PostSuccessfulSave(pResource);
    }

    // Any custom functionality a document requires after a resource is saved.
    virtual void PostSuccessfulSave(const ResourceEntity *pResource) {}

    virtual void PostApplyChanges() {}
};

