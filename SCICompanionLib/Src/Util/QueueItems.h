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
// This template implements a worker thread that can be used to perform background tasks.
//
// TITEM is a class that represents the data to work with.
// TRESULT is a class that represents the results you get back.
// TRESULT needs a static function of the form:
// static TRESULT *CreateFromWorkItem(TITEM *pWorkItem);
//

//
// QueueItems should be held in a std::shared_ptr.
// The UI thread should call Abort when done:
//  _pQueue->Abort();
//
template <class TITEM, class TRESULT>
class QueueItems : public std::enable_shared_from_this<QueueItems<TITEM, TRESULT>>
{
public:
    //
    // uMessage is posted to hwndView when there are (potentially multiple) results ready.
    //
    QueueItems(HWND hwndView, UINT uMessage) : _fAbort(false), _uResultReadyMessage(uMessage), _hwndView(hwndView)
    {
    }

    ~QueueItems()
    {
        Abort();
    }

    bool Init()
    {
        bool fRet = false;
        try
        {
            _thread = std::thread(s_ThreadWorker, this->shared_from_this());
            _thread.detach();
            fRet = true;
        }
        catch (std::system_error)
        {
        }
        return fRet;
    }

    void GiveWorkItem(std::unique_ptr<TITEM> pWorkItem)
    {
        // Insert things at the beginning of the list.
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _workItems.push_front(std::move(pWorkItem));
        }

        _condition.notify_one();
    }

    bool TakeWorkResult(TRESULT **ppWorkResult)
    {
        bool fRet = false;
        std::lock_guard<std::mutex> lock(_mutexResponse);
        if (!_workResults.empty())
        {
            fRet = true;
            *ppWorkResult = _workResults.front().release();
            _workResults.pop_front();
        }
        return fRet;
    }

    void Abort()
    {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _workItems.clear();
            _fAbort = true;
        }

        _condition.notify_one();
    }

    bool HasAborted()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _fAbort;
    }

private:
    bool _TakeWorkItem(TITEM **ppWorkItem)
    {
        bool fRet = false;
        std::lock_guard<std::mutex> lock(_mutex);
        if (!_workItems.empty())
        {
            fRet = true;
            *ppWorkItem = _workItems.back().release();
            _workItems.pop_back();
        }
        // else No work items left.
        return fRet;
    }

    void _GiveWorkResult(std::unique_ptr<TRESULT> pWorkResult)
    {
        {
            std::lock_guard<std::mutex> lock(_mutexResponse);
            _workResults.push_back(std::move(pWorkResult));
        }
        PostMessage(_hwndView, _uResultReadyMessage, 0, 0);
    }

    void _ThreadWorker()
    {
        while (!_fAbort)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _condition.wait(lock, [&]() { return _fAbort || !_workItems.empty(); });
            if (!_fAbort)
            {
                std::unique_ptr<TITEM> workItem = std::move(_workItems.back());
                _workItems.pop_back();
                lock.unlock(); // While we do heavy work
                std::unique_ptr<TRESULT> pResult(TRESULT::CreateFromWorkItem(workItem.get()));
                if (pResult)
                {
                    _GiveWorkResult(std::move(pResult));
                }
            }
        }
    }

    static UINT s_ThreadWorker(std::shared_ptr<QueueItems<TITEM, TRESULT>> me)
    {
        me->_ThreadWorker();
        return 0;
    }

    HWND _hwndView;
    UINT _uResultReadyMessage;
    std::mutex _mutex;
    std::mutex _mutexResponse;

    std::condition_variable _condition;

    std::thread _thread;
    bool _fAbort;

    std::list<std::unique_ptr<TITEM>> _workItems;
    std::list<std::unique_ptr<TRESULT>> _workResults;
};



