#include "stdafx.h"
#include "DebuggerThread.h"
#include "format.h"

using namespace std;

shared_ptr<DebuggerThread> CreateDebuggerThread(const std::string &gameFolder, int optionalResourceNumber)
{
    shared_ptr<DebuggerThread> debugger = make_shared<DebuggerThread>(gameFolder, optionalResourceNumber);
    debugger->_Start(debugger);
    return debugger;
}

UINT DebuggerThread::s_DebugThreadWorker(void *pParam)
{
    static_cast<DebuggerThread*>(pParam)->_Main();
    return 0;
}

string GetDebugLogFilename(const string &gameFolder)
{
    return gameFolder + "\\debug.log";
}

string GetDebugOnFilename(const string &gameFolder)
{
    return gameFolder + "\\ndebug.log";
}

string GetStartDebugFilename(const string &gameFolder)
{
    return gameFolder + "\\sdebug.txt";
}

void DebuggerThread::Abort()
{
    SetEvent(_hAbort.hFile);
    // Hmm... not sure about this. Who is responsible for cleanup?
}

DebuggerThread::DebuggerThread(const string &gameFolder, int optionalResourceNumber) : _gameFolder(gameFolder), _optionalResourceNumber(optionalResourceNumber) {}

void DebuggerThread::_Start(std::shared_ptr<DebuggerThread> myself)
{
    // Delete the previous debug.log, so the game can create a new one
    try
    {
        deletefile(GetDebugLogFilename(_gameFolder));
    }
    catch (std::exception &e)
    {
        AfxMessageBox(e.what(), MB_OK | MB_ICONWARNING);
    }

    // Make the startdebug.txt file
    {
        ScopedFile file(GetStartDebugFilename(_gameFolder), GENERIC_WRITE, 0, CREATE_ALWAYS);
        if (_optionalResourceNumber == -1)
        {
            // Standard case
            uint8_t a = ' ';
            file.Write(&a, 1);
        }
        else
        {
            // Put the resource number in here.
            sci::ostream byteStream;
            string number = fmt::format("{0}", _optionalResourceNumber);
            byteStream << number;
            file.Write(byteStream.GetInternalPointer(), byteStream.GetDataSize());
        }
    }

    // Now we can start our watcher thread.
    _thread = AfxBeginThread(s_DebugThreadWorker, this, 0, 0, CREATE_SUSPENDED, nullptr);
    if (_thread)
    {
        //_decompileResults = make_unique<DecompilerDialogResults>(this->GetSafeHwnd());
        //_SyncButtonState();
        _thread->m_bAutoDelete = TRUE;
        _hAbort.hFile = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        //_hThread = _pThread->m_hThread;
        _myself = myself;
        _thread->ResumeThread();
    }

}

void DebuggerThread::_Main()
{
    // Own a reference to ourselves during the lifetime of this method
    shared_ptr<DebuggerThread> myself = _myself;
    _myself.reset();

    string debugLogFilename = GetDebugLogFilename(_gameFolder);
    string debugOnFileName = GetDebugOnFilename(_gameFolder);

    // Wait for the game to create the ndebug.log file
    DWORD waitResult;
    do
    {
        waitResult = WaitForSingleObject(_hAbort.hFile, 500);
        if (waitResult == WAIT_TIMEOUT)
        {
            // Check for file existance
            if (PathFileExists(debugOnFileName.c_str()))
            {
                // We're good to go
                break;
            }
        }
    } while (waitResult == WAIT_TIMEOUT);

    if (waitResult == WAIT_TIMEOUT)
    {
        ScopedHandle readHandle;
        readHandle.hFile = CreateFile(debugLogFilename.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (readHandle.hFile != INVALID_HANDLE_VALUE)
        {
            bool done = false;
            while (!done && (WAIT_TIMEOUT == WaitForSingleObject(_hAbort.hFile, 200)))
            {
                // Now read from the file. We want to know when the process is done writing to it.
                // So try opening the file for write every n milliseconds with exclusive access
                // If we can, that means the process has closed the debug file (and thus exited)
                HANDLE hTest = CreateFile(debugOnFileName.c_str(), GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (hTest == INVALID_HANDLE_VALUE)
                {
                    // Presumably the process still has it open
                    char szBuffer[1024];
                    DWORD cbRead;
                    while (ReadFile(readHandle.hFile, szBuffer, sizeof(szBuffer) - 1, &cbRead, nullptr) && cbRead)
                    {
                        szBuffer[cbRead] = 0;
                        // For now, output to console
                        OutputDebugString(szBuffer);
                    }
                }
                else
                {
                    // We were able to open the file. The process has exited. Close our handles to it and delete it.
                    readHandle.Close();
                    CloseHandle(hTest);
                    done = true;
                }
            }
        }
    }
    // else failure, or we were aborted

    DeleteFile(debugOnFileName.c_str());
    DeleteFile(debugLogFilename.c_str());
    DeleteFile(GetStartDebugFilename(_gameFolder).c_str());

    _thread = nullptr;
}
