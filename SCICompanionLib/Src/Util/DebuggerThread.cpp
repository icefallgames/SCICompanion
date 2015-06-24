#include "stdafx.h"
#include "DebuggerThread.h"
#include "format.h"
#include "AppState.h"
#include "MainFrm.h"

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

DebuggerThread::DebuggerThread(const string &gameFolder, int optionalResourceNumber) : _gameFolder(gameFolder), _optionalResourceNumber(optionalResourceNumber), _hwndUI(nullptr) {}

void DebuggerThread::_Start(std::shared_ptr<DebuggerThread> myself)
{
    // Delete the previous debug.log, so the game can create a new one
    try
    {
        deletefile(GetDebugLogFilename(_gameFolder));
        deletefile(GetDebugOnFilename(_gameFolder));
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
        appState->OutputClearResults(OutputPaneType::Debug);
        appState->ShowOutputPane(OutputPaneType::Debug);
        _hwndUI = AfxGetMainWnd()->GetSafeHwnd();
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
    int waitPeriod = 200;
    int waitTime = 4000;
    DWORD waitResult;
    do
    {
        waitResult = WaitForSingleObject(_hAbort.hFile, waitPeriod);
        if (waitResult == WAIT_TIMEOUT)
        {
            // Check for file existance
            if (PathFileExists(debugOnFileName.c_str()))
            {
                // We're good to go
                break;
            }
        }
        waitTime -= waitPeriod;
    } while ((waitResult == WAIT_TIMEOUT) && (waitTime >= 0));

    string errorMessage;

    if ((waitResult == WAIT_TIMEOUT) && (waitTime >= 0))
    {
        ScopedHandle readHandle;
        readHandle.hFile = CreateFile(debugLogFilename.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (readHandle.hFile != INVALID_HANDLE_VALUE)
        {
            size_t validTextInBuffer = 0;
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
                    while (ReadFile(readHandle.hFile, szBuffer + validTextInBuffer, sizeof(szBuffer) - 1 - validTextInBuffer, &cbRead, nullptr) && cbRead)
                    {
                        szBuffer[cbRead] = 0;
                        validTextInBuffer += cbRead;

                        unique_ptr<vector<CompileResult>> results = make_unique<vector<CompileResult>>();
                        // Make a result for each new line.
                        size_t start = 0;
                        for (size_t i = 0; i < validTextInBuffer; i++)
                        {
                            if (szBuffer[i] == '\n')
                            {
                                string textLine(szBuffer + start, (i - start));
                                results->emplace_back(textLine);
                                start = i + 1;
                            }
                        }
                        if (start < validTextInBuffer)
                        {
                            memmove(szBuffer, szBuffer + start, (validTextInBuffer - start));
                        }
                        validTextInBuffer -= start;
                        // TODO: Also, if we're filling up, then process

                        if (!results->empty() && _hwndUI)
                        {
                            // Send to the UI thread.
                            SendMessage(_hwndUI, UWM_RESULTS, (WPARAM)OutputPaneType::Debug, reinterpret_cast<LPARAM>(results.release()));
                        }
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
        else
        {
            errorMessage = GetMessageFromLastError("Debugger");
        }
    }
    else
    {
        // failure, or we were aborted
        if (waitTime < 0)
        {
            errorMessage = "Timed out trying to start debugging functionality. This requires a compatible template game.";
        }
        else
        {
            errorMessage = GetMessageFromLastError("Debugger");
        }
    }

    if (_hwndUI && !errorMessage.empty())
    {
        unique_ptr<vector<CompileResult>> results = make_unique<vector<CompileResult>>();
        results->push_back(CompileResult(errorMessage));
        SendMessage(_hwndUI, UWM_RESULTS, (WPARAM)OutputPaneType::Debug, reinterpret_cast<LPARAM>(results.release()));
    }

    DeleteFile(debugOnFileName.c_str());
    DeleteFile(debugLogFilename.c_str());
    DeleteFile(GetStartDebugFilename(_gameFolder).c_str());

    _thread = nullptr;
}
