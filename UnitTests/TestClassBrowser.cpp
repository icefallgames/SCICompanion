#include "stdafx.h"
#include "CppUnitTest.h"
#include "ResourceMap.h"
#include "AppState.h"
#include "ScriptOM.h"
#include "CompileContext.h"
#include "Helper.h"
#include "ClassBrowser.h"
#include "Task.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
    TEST_CLASS(TestClassBrowser)
    {
    public:
        TEST_CLASS_INITIALIZE(ClassSetup)
        {
        }

        TEST_CLASS_CLEANUP(ClassCleanup)
        {
        }

        TEST_METHOD(TestClassBrowserSCI0)
        {
            _gameFolder = SetUpGameSCI0();
            _DoIt();
        }

        TEST_METHOD(TestClassBrowserSCI11)
        {
            _gameFolder = SetUpGameSCI11();
            _DoIt();
        }

        TEST_METHOD_CLEANUP(TestCompileAll_Clean)
        {
            CleanUpGame(_gameFolder);
        }

        void _DoIt()
        {
            SCIClassBrowser *browser = appState->GetResourceMap().GetClassBrowser();

            class TaskStatus : public ITaskStatus
            {
            public:
                bool IsAborted() override { return true; }
            };

            class ClassBrowserEvents : public IClassBrowserEvents
            {
            public:
                ClassBrowserEvents(bool &ok) : _ok(ok) {}
                void NotifyClassBrowserStatus(BrowseInfoStatus status, int iPercent) override
                {
                    if (status == BrowseInfoStatus::Ok)
                    {
                        _ok = true;
                    }
                }
            private:
                bool &_ok;
            };
            bool ok = false;
            ClassBrowserEvents events(ok);;

            Assert::IsTrue(appState->IsBrowseInfoEnabled());
            browser->SetClassBrowserEvents(&events);

            TaskStatus taskStatus;
            browser->ReLoadFromSources(taskStatus);
            Assert::IsTrue(ok);

            // TODO: Now make some queries.
            // pick a script (0) and some points in it, and try to get tooltips, autocopmletes and such.
        }

    private:
        static std::string _gameFolder;
    };

    std::string TestClassBrowser::_gameFolder;
}