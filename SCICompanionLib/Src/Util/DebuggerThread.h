#pragma once

class DebuggerThread
{
public:
    DebuggerThread(const std::string &gameFolder, int optionalResourceNumber);

    void Abort();

    friend std::shared_ptr<DebuggerThread> CreateDebuggerThread(const std::string &gameFolder, int optionalResourceNumber);

private:
    void _Start(std::shared_ptr<DebuggerThread> myself);
    static UINT s_DebugThreadWorker(void *pParam);
    void _Main();

    CWinThread *_thread;
    HWND _hwndUI;
    std::shared_ptr<DebuggerThread> _myself;
    std::string _gameFolder;
    int _optionalResourceNumber;
    ScopedHandle _hAbort;
};

std::shared_ptr<DebuggerThread> CreateDebuggerThread(const std::string &gameFolder, int optionalResourceNumber);
