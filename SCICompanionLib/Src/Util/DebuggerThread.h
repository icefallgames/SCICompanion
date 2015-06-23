#pragma once

class DebuggerThread
{
public:
    DebuggerThread(const std::string &gameFolder);

    void Abort();

    friend std::shared_ptr<DebuggerThread> CreateDebuggerThread(const std::string &gameFolder);

private:
    void _Start(std::shared_ptr<DebuggerThread> myself);
    static UINT s_DebugThreadWorker(void *pParam);
    void _Main();

    CWinThread *_thread;
    std::shared_ptr<DebuggerThread> _myself;
    std::string _gameFolder;
    ScopedHandle _hAbort;
};

std::shared_ptr<DebuggerThread> CreateDebuggerThread(const std::string &gameFolder);
