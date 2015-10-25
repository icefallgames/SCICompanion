/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
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