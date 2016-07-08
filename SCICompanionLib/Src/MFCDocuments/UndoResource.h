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
// ptrdiff_t is any extra info (like cursor position) you wish to include with the undo snapshot
//

#define MAX_UNDO 100
#define AGGRESSIVITY_UNDO 30

template <class _TBase, class _TItem>
class CUndoResource : public _TBase
{
private:
    struct UndoData
    {
        UndoData(std::unique_ptr<_TItem>&& theItem, ptrdiff_t theExtra)
        {
            item = std::move(theItem);
            extra = theExtra;
        }

        std::unique_ptr<_TItem> item;
        ptrdiff_t extra;
    };

protected:
    // Implementors can override to attach extra data to a resource in the undo stack
    virtual ptrdiff_t v_GetExtra() { return 0; }

public:

    typedef std::list<UndoData> _MyListType;

    CUndoResource()
    {
        _pos = _undo.end();
    }

    void AddFirstResource(std::unique_ptr<_TItem> pResource, ptrdiff_t extra = 0)
    {
        ASSERT(_pos == _undo.end());
        _pLastSaved = pResource.get();
        _undo.emplace_back(std::move(pResource), extra);
        _pos = _GetLastUndoFrame();
    }

	void AddNewResourceToUndo(std::unique_ptr<_TItem> pResourceNew, ptrdiff_t extra = 0)
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
        _undo.emplace_back(std::move(pResourceNew), extra);
		_pos = _GetLastUndoFrame();

		// Make sure we don't grow infinitely.
		_TrimUndoStack();
	}

    void SetExtra(ptrdiff_t extra)
    {
        if (_pos != _undo.end())
        {
            _pos->extra = extra;
        }
    }

    void SetLastSaved(const _TItem *pResource)
    {
        _pLastSaved = pResource;
    }

    // Gets the resource at the current location
    const _TItem *GetResource() const
    {
        return (_pos != _undo.end()) ? (*_pos).item.get() : nullptr;
    }

    // Gets the extra data associated with the resource at the current location
    ptrdiff_t GetExtra() const
    {
        return (_pos != _undo.end()) ? (*_pos).extra : 0;
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
            SetModifiedFlag((*_pos).item.get() != _pLastSaved);
        }
    }

    afx_msg void OnRedo()
    {
        if (_pos != _GetLastUndoFrame())
        {
            ++_pos;
            _OnUndoRedo();
            SetModifiedFlag((*_pos).item.get() != _pLastSaved);
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

