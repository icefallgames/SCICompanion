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

//
// A generic undo for sci resources.
// _TBase is the baseclass of the MFC document object to which you want to add this functionality
// _TItem is the resource type
//

#define MAX_UNDO 100
#define AGGRESSIVITY_UNDO 30

template <class _TBase, class _TItem>
class CUndoResource : public _TBase
{
public:
    typedef std::list<std::unique_ptr<_TItem>> _MyListType;

    CUndoResource()
    {
        _pos = _undo.end();
    }

    void AddFirstResource(std::unique_ptr<_TItem> pResource)
    {
        ASSERT(_pos == _undo.end());
        _pLastSaved = pResource.get();
        _undo.push_back(std::move(pResource));
        _pos = _GetLastUndoFrame();
    }

	void AddNewResourceToUndo(std::unique_ptr<_TItem> pResourceNew)
	{
		// Delete all resources after the current one.
        _MyListType::iterator pos = _pos;
		++pos;
		// Now pos points to the position after the current one.
		while (pos != _undo.end())
		{
			// Delete all those resources.
            _MyListType::iterator posToDel = pos;
			++pos;
			_undo.erase(posToDel);
		}

		// Insert after the current pos (which is now the end), and make this our new pos.
		_undo.push_back(std::move(pResourceNew));	// TODO
		_pos = _GetLastUndoFrame();

		// Make sure we don't grow infinitely.
		_TrimUndoStack();
	}

    void SetLastSaved(const _TItem *pResource)
    {
        _pLastSaved = pResource;
    }

    // Gets the resource at the current location
    const _TItem *GetResource() const
    {
        return (_pos != _undo.end()) ? (*_pos).get() : nullptr;
    }

    bool HasUndos()
    {
        return !_undo.empty();
    }

protected:
	DECLARE_MESSAGE_MAP()
    afx_msg void OnUndo()
    {
        if (_pos != _undo.begin())
        {
            --_pos;
            _OnUndoRedo();
            SetModifiedFlag((*_pos).get() != _pLastSaved);
        }
    }

    afx_msg void OnRedo()
    {
        if (_pos != _GetLastUndoFrame())
        {
            ++_pos;
            _OnUndoRedo();
            SetModifiedFlag((*_pos).get() != _pLastSaved);
        }
    }

    afx_msg void OnUpdateRedo(CCmdUI *pCmdUI)
    {
        BOOL fRet = FALSE;
        if (!_undo.empty() && !_PreventUndos())
        {
            fRet = (_pos != _GetLastUndoFrame());
        }
        pCmdUI->Enable(fRet);
    }

    afx_msg void OnUpdateUndo(CCmdUI *pCmdUI)
    {
        BOOL fRet = FALSE;
        if (!_undo.empty() && !_PreventUndos())
        {
            fRet = (_pos != _undo.begin());
        }
        pCmdUI->Enable(fRet);
    }

    // Overrides
    virtual bool v_PreventUndos() const
    {
        return false;
    }

    virtual void v_OnUndoRedo() = 0;

private:
    bool _PreventUndos()
    {
        return v_PreventUndos();
    }

    void _OnUndoRedo()
    {
        v_OnUndoRedo();
    }

    void _TrimUndoStack()
    {
        if (_undo.size() > MAX_UNDO)
        {
            // Remove every other stored frame, up to a certain point.
            _MyListType::iterator pos = _undo.begin();
            BOOL fRemove = FALSE;
            for (int i = 0; i < AGGRESSIVITY_UNDO; i++)
            {
                if (pos != _pos) // Skip it if it's where we currently are!
                {
                    _MyListType::iterator posToMaybeDelete = pos;
                    ++pos;
                    if (fRemove)
                    {
                        _undo.erase(posToMaybeDelete);
                    }
                    fRemove = !fRemove;
                }
            }
        }
    }

    typename _MyListType::iterator _GetLastUndoFrame()
    {
        _MyListType::iterator pos = _undo.end();
        return --pos;
    }

    // Undo buffer.
    _MyListType _undo;
    typename _MyListType::iterator _pos;

    const _TItem *_pLastSaved; // Weak ref
};


BEGIN_TEMPLATE_MESSAGE_MAP_2(CUndoResource, _TBase, _TItem, _TBase)
    ON_COMMAND(ID_EDIT_UNDO, OnUndo)
    ON_COMMAND(ID_EDIT_REDO, OnRedo)
    ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateUndo)
    ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateRedo)
END_MESSAGE_MAP()

