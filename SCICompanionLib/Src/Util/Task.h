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

#include <deque>

class ITaskStatus
{
public:
    virtual bool IsAborted() = 0;
};

template<typename _TPayload, typename _TResponse = int>
class BackgroundScheduler : public ITaskStatus
{
public:
    BackgroundScheduler() : BackgroundScheduler(nullptr, 0) {}

    BackgroundScheduler(HWND hwndResponse, UINT msgResponse) : _exit(false), _nextId(0), _hwndResponse(hwndResponse), _msgResponse(msgResponse)
    {
        _thread = std::thread(s_ThreadWorker, this);
    }

    ~BackgroundScheduler()
    {
        Exit();
    }

    int SubmitTask(std::unique_ptr<_TPayload> task, std::function<std::unique_ptr<_TResponse>(ITaskStatus&, _TPayload&)> func)
    {
        int id;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            id = _nextId++;
            _queue.emplace_back(id, std::move(task), func);
        }

        _conditionWakeUp.notify_one();
        return id;
    }
    int SubmitTask(HWND hwnd, UINT msg, std::unique_ptr<_TPayload> task, std::function<std::unique_ptr<_TResponse>(ITaskStatus&, _TPayload&)> func)
    {
        // This allows us to keep the same scheduler around for different windows
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _hwndResponse = hwnd;
            _msgResponse = msg;
        }

        return SubmitTask(std::move(task), func);
    }

    std::unique_ptr<_TResponse> RetrieveResponse(int id)
    {

        std::lock_guard<std::mutex> lock(_mutexResponse);
        // Pop off the front until we find the one we want.
        std::unique_ptr<_TResponse> response;
        while (!response && !_responseQueue.empty())
        {
            if (_responseQueue.front().id == id)
            {
                response = move(_responseQueue.front().response);
            }
            _responseQueue.pop_front();
        }
        return response;
    }

    void DeactivateHWND(HWND hwndNoMore)
    {
        // Since multiple windows may use the same scheduler, when a window that submits
        // as task is destroyed, we want to clear the response hwnd out so that we don't
        // post to an invalid hwnd.
        if (_hwndResponse == hwndNoMore)
        {
            _hwndResponse = nullptr;
        }
    }

    bool IsAborted() override
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _exit;
    }

    // Waits until background thread exits.
    void Exit()
    {
        bool deleteThings = false;

        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (!_exit)
            {
                deleteThings = true;
                _exit = true;
            }
        }

        if (deleteThings)
        {
            _conditionWakeUp.notify_one();

            // We need to wait until the background thread is done.
            _thread.join();
        }
    }

private:
    static UINT s_ThreadWorker(void *pParam)
    {
        (reinterpret_cast<BackgroundScheduler*>(pParam))->_DoWork();
        return 0;
    }

    void _DoWork()
    {
        while (!_exit)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _conditionWakeUp.wait(lock, [&]() { return this->_exit || !this->_queue.empty(); });
            if (!_exit)
            {
                // Now the mutex is locked again.
                assert(!this->_queue.empty());
                std::unique_ptr<_TPayload> payload = std::move(_queue.front().payload);
                std::function<std::unique_ptr<_TResponse>(ITaskStatus&, _TPayload&)> func = _queue.front().func;
                int id = _queue.front().id;
                _queue.pop_front();
                // But we'll unlock it while we do our heavy work.
                _mutex.unlock();

                if (payload)
                {
                    std::unique_ptr<_TResponse> response = func(*this, *payload);
                    // If the owner wanted a response, send it now.
                    if (response)
                    {
                        HWND hwnd;
                        UINT msg;
                        {
                            std::lock_guard<std::mutex> lock(_mutexResponse);
                            hwnd = _hwndResponse;
                            msg = _msgResponse;
                            if (_hwndResponse)
                            {
                                _responseQueue.emplace_back(id, std::move(response));
                            }
                        }
                        if (hwnd)
                        {
                            PostMessage(hwnd, msg, 0, 0);
                        }
                    }
                }
                // Now lock it again before we loop
                _mutex.lock();
            }
        }
    }

    std::thread _thread;

    // REVIEW: these were auto reset...
    std::condition_variable _conditionWakeUp;

    bool _exit;

    struct TaskInfo
    {
        TaskInfo(int id, std::unique_ptr<_TPayload> payload, std::function<std::unique_ptr<_TResponse>(ITaskStatus&, _TPayload&)> func) : id(id), payload(std::move(payload)), func(func) {}

        int id;
        std::function<std::unique_ptr<_TResponse>(ITaskStatus&, _TPayload&)> func;
        std::unique_ptr<_TPayload> payload;
    };

    struct TaskResponse
    {
        TaskResponse(int id, std::unique_ptr<_TResponse> response) : id(id), response(std::move(response)) {}

        int id;
        std::unique_ptr<_TResponse> response;
    };

    std::mutex _mutex;
    int _nextId;
    std::deque<TaskInfo> _queue;

    std::mutex _mutexResponse;
    HWND _hwndResponse;
    UINT _msgResponse;
    std::deque<TaskResponse> _responseQueue;
};

#include <future>

template<typename _TResponse, typename _TInnerFunc>
_TResponse HWNDTaskWrapper(_TInnerFunc innerFunc, HWND hwnd, UINT message)
{
    assert(hwnd != nullptr);
    _TResponse response = innerFunc();
    if (hwnd)
    {
        PostMessage(hwnd, message, 0, 0);
    }
    return response;
}

// A more generic mechanism?
template<typename _TResponse>
class CWndTaskSink
{
public:
    // pwnd guaranteed to exist as long as CWndTaskSink does.
    CWndTaskSink(CWnd *pwnd, UINT message) : _pwnd(pwnd), _message(message) {}

    ~CWndTaskSink() { Abandon(); }

    template<typename _TFunc>
    void StartTask(_TFunc func)
    {
        // TODO: add futures to a queue, so we can instantiate new ones.
        _future = std::make_unique<std::future<_TResponse>>(std::async(std::launch::async, HWNDTaskWrapper<_TResponse, _TFunc>, func, _pwnd->GetSafeHwnd(), _message));
    }

    void Abandon()
    {
        // TODO
    }

    _TResponse GetResponse()
    {
        // Ok to block, since we posted the message just as we were about to be done. get blocks
        //if (future_status::ready == _future.wait_for(std::chrono::seconds(0)))
        return _future->get();
    }

private:
    std::unique_ptr<std::future<_TResponse>> _future;

    CWnd *_pwnd;
    UINT _message;
};


