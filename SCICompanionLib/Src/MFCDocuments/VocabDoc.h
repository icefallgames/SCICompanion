#pragma once

#include "ResourceEntityDocument.h"
// CVocabDoc document

// FWD decl
struct Vocab000;

class CVocabDoc : public ResourceEntityDocument
{
	DECLARE_DYNCREATE(CVocabDoc)

public:
	CVocabDoc();
	virtual ~CVocabDoc();

    void SetVocabResource(std::unique_ptr<ResourceEntity> pVocab, int id);
    const Vocab000 *GetVocab() const;

    void v_OnUndoRedo() override;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
    virtual ResourceType _GetType() const { return ResourceType::Vocab; }

    DECLARE_MESSAGE_MAP()
    void PostSuccessfulSave(const ResourceEntity *pResource) override;

private:
    Vocab000 *_pVocab;
};

