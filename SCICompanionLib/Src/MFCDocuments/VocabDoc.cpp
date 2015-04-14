// VocabDoc.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "VocabDoc.h"
#include "Vocab000.h"

// CVocabDoc

IMPLEMENT_DYNCREATE(CVocabDoc, CResourceDocument)

CVocabDoc::CVocabDoc()
{
    _pVocab = NULL;
}

CVocabDoc::~CVocabDoc()
{
    if (_pVocab)
    {
        delete _pVocab;
        _pVocab = NULL;
    }
}

void CVocabDoc::v_OnUndoRedo()
{
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(VocabChangeHint::Changed));
}

void CVocabDoc::SetVocabResource(std::unique_ptr<ResourceEntity> pVocab, int id)
{
    _checksum = id;
    AddFirstResource(move(pVocab));
    _UpdateTitle();
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(VocabChangeHint::Changed));
}

Vocab000 *CVocabDoc::GetVocab() const
{
    Vocab000 *pVocab = nullptr;
    const ResourceEntity *pResource = GetResource();
    if (pResource)
    {
        pVocab = pResource->TryGetComponent<Vocab000>();
    }
    return pVocab;
}


void CVocabDoc::PostSuccessfulSave(const ResourceEntity *pResource)
{
    appState->GetResourceMap().ClearVocab000();
}

BEGIN_MESSAGE_MAP(CVocabDoc, TCLASS_2(CUndoResource, CResourceDocument, ResourceEntity))
END_MESSAGE_MAP()

// CVocabDoc diagnostics

#ifdef _DEBUG
void CVocabDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVocabDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG



// CVocabDoc commands
