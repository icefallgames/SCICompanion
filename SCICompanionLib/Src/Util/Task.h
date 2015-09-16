#pragma once

#include <deque>

class BackgroundScheduler;
class TaskResponse;

class ITask
{
public:
    virtual bool IsAborted() = 0;

};

class BackgroundTask : public ITask
{
    // TODO: Consider automatic de-duping support
public:
    virtual ~BackgroundTask() {}
    virtual std::unique_ptr<TaskResponse> Execute() = 0;
    bool IsAborted() override;

private:
    BackgroundScheduler *_schedulerWeak;
    int _id;
    friend BackgroundScheduler;
};

class TaskResponse
{
public:
    TaskResponse() : _id(-1) {}
    int GetId() { return _id; }
private:
    int _id;
    friend BackgroundScheduler;
};

class BackgroundScheduler
{
public:
    BackgroundScheduler();
    BackgroundScheduler(HWND hwndResponse, UINT msgResponse);
    ~BackgroundScheduler();
    int SubmitTask(std::unique_ptr<BackgroundTask> task);
    std::unique_ptr<TaskResponse> RetrieveResponse(int id);
    bool IsAborted();
    
    // Waits until background thread exits.
    void Exit();

private:
    static UINT s_ThreadWorker(void *pParam);
    void _DoWork();

    CWinThread *_pThread;
    HANDLE _hWakeUp;

    bool _exit;

    CRITICAL_SECTION _cs;
    int _nextId;
    std::deque<std::unique_ptr<BackgroundTask>> _queue;

    CRITICAL_SECTION _csResponse;
    HWND _hwndResponse;
    UINT _msgResponse;
    std::deque<std::unique_ptr<TaskResponse>> _responseQueue;
};