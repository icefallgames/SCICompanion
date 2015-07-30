#pragma once
#include "UndoResource.h"
#include "ResourceDocument.h"
#include "ResourceEntity.h"

void DoNothing(ResourceEntity &resource);

//
// Modifications to resource documents should be done via ApplyChanges. The functions passed to
// ApplyChanges are give components from a clone of the resource. This means that undo functionality
// is taken care of automatically.
//
class ResourceEntityDocument : public CUndoResource<CResourceDocument, ResourceEntity>
{
public:
    //
    // Usage:
    //
    // document->ApplyChanges<MyComponent1, MyComponent2...>(
    //   [] (MyComponent1 &comp1, MyComponent2 &comp2 ...)
    //   {
    //        // Make changes to components
    //        // etc...
    //        return WrapHint(MyComponentChangeHint); // (Return a 0 hint to indicate no change)
    //   }
    //  );
    //
    template<typename... _TComponentArgs, typename _Func>
    bool ApplyChanges(_Func f)
    {
        return ApplyChanges<_TComponentArgs ...>(f, DoNothing, DoNothing);
    }

    // Similar to ApplyChanges, but the new item is added to the undo stack before it's committed
    // so it can be accessed by views/docs. If no changes are made, it is removed from the undo stack.
    template<typename... _TComponentArgs, typename _Func>
    bool PreviewChanges(_Func f)
    {
        return ApplyChanges<_TComponentArgs ...>(f, DoNothing, true);
    }

    template<typename... _TComponentArgs, typename _Func, typename _PostFunc>
    bool ApplyChangesWithPost(_Func f, _PostFunc postf)
    {
        return ApplyChanges<_TComponentArgs ...>(f, DoNothing, postf);
    }

    template<typename... _TComponentArgs, typename _Func, typename _PreFunc>
    bool ApplyChanges(_Func f, _PreFunc pf, bool preview = false)
    {
        return ApplyChanges<_TComponentArgs ...>(f, pf, DoNothing, preview);
    }

    // 
    // Usage as above, but allows for a second lambda to be passed, which receives the ResourceEntity
    // This is called prior to the first lamba. You would use this to add/remove components from the resource.
    // 
    template<typename... _TComponentArgs, typename _Func, typename _PreFunc, typename _PostFunc>
    bool ApplyChanges(_Func f, _PreFunc pf, _PostFunc postf, bool preview = false)
    {
        bool modified = false;
        const ResourceEntity *pEntity = static_cast<const ResourceEntity*>(GetResource());
        if (pEntity)
        {
            std::unique_ptr<ResourceEntity> pNew = pEntity->Clone();
            ResourceEntity *temp = pNew.get();
            if (preview)
            {
                // If previewing, add it now...
                AddNewResourceToUndo(std::move(pNew));
            }

            // Allows modification of the ResourceEntity itself
            pf(*temp);
            // Allows modification of the components attached to it
            auto updateHint = f(temp->GetComponent<_TComponentArgs>()...);

            static_assert(std::is_base_of<CObject, decltype(updateHint)>::value, "The value returned by the function must be a subclass of CObject. Use WrapHint.");

            uint32_t hintNumber = GetHint<uint32_t>(&updateHint);
            // We assume that a hint of zero means nothing changed. So don't set the modified flag.
            modified = (hintNumber != 0);
            if (modified)
            {
                if (!preview)
                {
                    AddNewResourceToUndo(std::move(pNew));
                }

                postf(*temp);
                // This needs to happen before we update views:
                PostApplyChanges(&updateHint);

                // Go ahead and notify things...
                UpdateAllViewsAndNonViews(nullptr, 0, &updateHint);

                if (modified)
                {
                    SetModifiedFlag(TRUE);
                }
            }
            else
            {
                if (preview)
                {
                    // Undo the preview we added to the stack.
                    OnUndo();
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

    virtual void PostApplyChanges(CObject *pObj) {}
};

