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
#include "PaletteOperations.h"

// CPaletteDoc document

class CPaletteDoc : public ResourceEntityDocument
{
    DECLARE_DYNCREATE(CPaletteDoc)

public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o

    // Takes ownership:
    void SetResource(std::unique_ptr<ResourceEntity> pPalette, int id = -1);

    // CUndoResource
    void v_OnUndoRedo();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

    virtual ResourceType _GetType() const { return ResourceType::Palette; }

	DECLARE_MESSAGE_MAP()
};
