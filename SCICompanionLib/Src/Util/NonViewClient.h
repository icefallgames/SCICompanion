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

// fwd deck
class CView;
class CObject;

class INonViewClient
{
public:
    virtual void UpdateNonView(CObject *pObject) = 0;
};

class INonViewHost
{
public:
    // The semantics of AddNonViewClient are such that a client is allowed to add itself
    // multiple times (but should only be called once).
    // Also, the client's life-time must exceed that of the host.
    virtual void AddNonViewClient(INonViewClient *pClient) = 0;
    virtual void UpdateAllViewsAndNonViews(CView *pSender, LPARAM lHint, CObject *pObject = NULL) = 0;
};

// stl helper
struct _UpdateNonView : public std::binary_function<INonViewClient*, CObject*, void>
{
    void operator()(INonViewClient *pClient, CObject *pObjec) const
    {
        pClient->UpdateNonView(pObjec);
    }
};

