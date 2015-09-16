#pragma once

#include <deque>

class BackgroundScheduler;

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
    virtual void Execute() = 0;
    bool IsAborted() override;

private:
    BackgroundScheduler *_schedulerWeak;
    friend BackgroundScheduler;
};

class BackgroundScheduler
{
public:
    BackgroundScheduler();
    ~BackgroundScheduler();
    void SubmitTask(std::unique_ptr<BackgroundTask> task);
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
    std::deque<std::unique_ptr<BackgroundTask>> _queue;
};