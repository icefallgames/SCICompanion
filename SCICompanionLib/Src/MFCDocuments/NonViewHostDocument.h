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

#include "NonViewClient.h"

// CNonViewHostDocument document

class CNonViewHostDocument : public CDocument, public INonViewHost
{
public:
    // INonViewHost
    void AddNonViewClient(INonViewClient *pClient) override;
    void UpdateAllViewsAndNonViews(CView *pSender, LPARAM lHint, CObject *pObject = NULL) override;

private:
    // Use a set, to ensure we don't add the same client twice.
    std::set<INonViewClient*> _nonViewClients;
};
