#pragma once

#include <deque>
#include <thread>
#include <mutex>

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
        InitializeCriticalSection(&_cs);
        _hWakeUp = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        _thread = std::thread(s_ThreadWorker, this);
        InitializeCriticalSection(&_csResponse);
    }

    ~BackgroundScheduler()
    {
        Exit();
        DeleteCriticalSection(&_cs);
        DeleteCriticalSection(&_csResponse);
    }

    int SubmitTask(std::unique_ptr<_TPayload> task, std::function<std::unique_ptr<_TResponse>(ITaskStatus&, _TPayload&)> func)
    {
        int id;
        {
            CGuard guard(&_cs);
            id = _nextId++;
            _queue.emplace_back(id, std::move(task), func);
        }

        SetEvent(_hWakeUp);
        return id;
    }
    int SubmitTask(HWND hwnd, UINT msg, std::unique_ptr<_TPayload> task, std::function<std::unique_ptr<_TResponse>(ITaskStatus&, _TPayload&)> func)
    {
        // This allows us to keep the same scheduler around for different windows
        {
            CGuard guard(&_cs);
            _hwndResponse = hwnd;
            _msgResponse = msg;
        }

        return SubmitTask(std::move(task), func);
    }

    std::unique_ptr<_TResponse> RetrieveResponse(int id)
    {
        CGuard guard(&_csResponse);
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
        CGuard guard(&_cs);
        return _exit;
    }

    // Waits until background thread exits.
    void Exit()
    {
        bool deleteThings = false;

        {
            CGuard guard(&_cs);
            if (!_exit)
            {
                deleteThings = true;
                _exit = true;
            }
        }

        if (deleteThings)
        {
            SetEvent(_hWakeUp);

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
        bool exit = false;
        while (!exit && (WAIT_OBJECT_0 == WaitForSingleObject(_hWakeUp, INFINITE)))
        {
            bool lookForMoreWork = true;
            while (lookForMoreWork)
            {
                std::unique_ptr<_TPayload> payload;
                std::function<std::unique_ptr<_TResponse>(ITaskStatus&, _TPayload&)> func;
                int id;

                {
                    CGuard guard(&_cs);
                    if (!_exit && !_queue.empty())
                    {
                        payload = std::move(_queue.front().payload);
                        func = _queue.front().func;
                        id = _queue.front().id;
                        _queue.pop_front();
                    }
                    else
                    {
                        exit = _exit;
                        lookForMoreWork = false;
                    }
                }

                if (payload)
                {
                    std::unique_ptr<_TResponse> response = func(*this, *payload);
                    // If the owner wanted a response, send it now.
                    if (response)
                    {
                        HWND hwnd;
                        UINT msg;
                        {
                            CGuard guard(&_csResponse);
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
            }
        }
    }

    std::thread _thread;
    HANDLE _hWakeUp;

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

    CRITICAL_SECTION _cs;
    int _nextId;
    std::deque<TaskInfo> _queue;

    CRITICAL_SECTION _csResponse;
    HWND _hwndResponse;
    UINT _msgResponse;
    std::deque<TaskResponse> _responseQueue;
};

#if 0
// A more generic mechanism?
template<typename _TResponse>
class CWndTaskSink
{
public:
    // pwnd guaranteed to exist as long as CWndTaskSink does.
    CWndTaskSink(CWnd *pwnd) : _pwnd(pwnd) {}

    ~CWndTaskSink() { Abandon(); }

    void Abandon()
    {
        _communicator->Unhook();
    }

    std::unique_ptr<_TResponse> GetResponse()
    {
        return std::move(_communicator->GetResponse());
    }

private:

    class TaskCommunicator
    {
    public:
        TaskCommunicator(HWND hwnd, UINT message) : _hwnd(hwnd) {}

        void Unhook()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _hwnd = nullptr;
            _response = nullptr;
        }

        std::unique_ptr<_TResponse> GetResponse()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_hwnd)
            {
                return std::move(_response);
            }
        }

        void ProvideResponse(std::unique_ptr<_TResponse> response)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_hwnd)
            {
                _response = std::move(response);
                PostMessage(_hwnd, _message, 0, 0);
            }
        }

    private:
        // TODO: Maybe this is sharedptr?
        HWND _hwnd;
        UINT _message;
        std::unique_ptr<_TResponse> _response;
        std::mutex _mutex;
    };

    std::shared_ptr<TaskCommunicator> _communicator;

    CWnd *_pwnd;
};
#endif


