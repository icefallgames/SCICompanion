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
#include "Text.h"

// CTextDoc document

class CTextDoc : public ResourceEntityDocument
{
	DECLARE_DYNCREATE(CTextDoc)

public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o

    // Takes ownership:
    void SetTextResource(std::unique_ptr<ResourceEntity> pText, int id = -1);

    // CUndoResource
    void v_OnUndoRedo();

    bool v_DoPreResourceSave() override;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

    virtual ResourceType _GetType() const
    {
        const ResourceEntity *pResource = static_cast<const ResourceEntity*>(GetResource());
        return pResource->GetType();
    }


	DECLARE_MESSAGE_MAP()
};
