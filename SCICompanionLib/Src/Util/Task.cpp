#include "stdafx.h"
#include "Task.h"

bool BackgroundTask::IsAborted()
{
    return _schedulerWeak->IsAborted();
}

bool BackgroundScheduler::IsAborted()
{
    CGuard guard(&_cs);
    return _exit;
}

int BackgroundScheduler::SubmitTask(std::unique_ptr<BackgroundTask> task)
{
    int id = -1;
    {
        CGuard guard(&_cs);
        id = _nextId++;
        task->_id = id;
        _queue.push_back(move(task));
    }

    SetEvent(_hWakeUp);
    return id;
}

std::unique_ptr<TaskResponse> BackgroundScheduler::RetrieveResponse(int id)
{
    CGuard guard(&_csResponse);
    // Pop off the front until we find the one we want.
    std::unique_ptr<TaskResponse> response;
    while (!response && !_responseQueue.empty())
    {
        if (_responseQueue.front()->_id == id)
        {
            response = move(_responseQueue.front());
        }
        _responseQueue.pop_front();
    }
    return response;
}

BackgroundScheduler::BackgroundScheduler() : BackgroundScheduler(nullptr, 0)
{
}

BackgroundScheduler::BackgroundScheduler(HWND hwndResponse, UINT msgResponse) : _exit(false), _nextId(0), _hwndResponse(hwndResponse), _msgResponse(msgResponse)
{
    InitializeCriticalSection(&_cs);
    _hWakeUp = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    _pThread = AfxBeginThread(s_ThreadWorker, this, THREAD_PRIORITY_BELOW_NORMAL, 0, 0, nullptr);

    if (_hwndResponse)
    {
        InitializeCriticalSection(&_csResponse);
    }
}

BackgroundScheduler::~BackgroundScheduler()
{
    DeleteCriticalSection(&_cs);
    if (_hwndResponse)
    {
        DeleteCriticalSection(&_csResponse);
    }
}

UINT BackgroundScheduler::s_ThreadWorker(void *pParam)
{
    (reinterpret_cast<BackgroundScheduler*>(pParam))->_DoWork();
    return 0;
}

void BackgroundScheduler::Exit()
{
    {
        CGuard guard(&_cs);
        _exit = true;
    }

    SetEvent(_hWakeUp);

    // We need to wait until the background thread is done.
    WaitForSingleObject(_pThread->m_hThread, INFINITE);
}

void BackgroundScheduler::_DoWork()
{
    bool exit = false;
    while (!exit && (WAIT_OBJECT_0 == WaitForSingleObject(_hWakeUp, INFINITE)))
    {
        bool lookForMoreWork = true;
        while (lookForMoreWork)
        {
            std::unique_ptr<BackgroundTask> task;

            {
                CGuard guard(&_cs);
                if (!_exit && !_queue.empty())
                {
                    task = move(_queue.front());
                    _queue.pop_front();
                }
                else
                {
                    exit = _exit;
                    lookForMoreWork = false;
                }
            }

            if (task)
            {
                task->_schedulerWeak = this;
                std::unique_ptr<TaskResponse> response = task->Execute();
                // If the owner wanted a response, send it now.
                if (response && _hwndResponse)
                {
                    response->_id = task->_id;

                    {
                        CGuard guard(&_csResponse);
                        _responseQueue.push_back(move(response));
                    }

                    PostMessage(_hwndResponse, _msgResponse, 0, 0);
                }
            }
        }
    }
}