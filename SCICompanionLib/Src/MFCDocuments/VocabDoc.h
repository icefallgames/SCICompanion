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

