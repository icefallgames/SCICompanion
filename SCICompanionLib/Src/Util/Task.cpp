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

void BackgroundScheduler::SubmitTask(std::unique_ptr<BackgroundTask> task)
{
    {
        CGuard guard(&_cs);
        _queue.push_back(move(task));
    }

    SetEvent(_hWakeUp);
}

BackgroundScheduler::BackgroundScheduler() : _exit(false)
{
    InitializeCriticalSection(&_cs);
    _hWakeUp = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    _pThread = AfxBeginThread(s_ThreadWorker, this, THREAD_PRIORITY_BELOW_NORMAL, 0, 0, nullptr);
}

BackgroundScheduler::~BackgroundScheduler()
{
    DeleteCriticalSection(&_cs);
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
                task->Execute();
            }
        }
    }
}