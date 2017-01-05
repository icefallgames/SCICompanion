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
#include "ScriptConvert.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(TestCompile)
	{
	public:
        TEST_CLASS_INITIALIZE(ClassSetup)
        {
        }

        TEST_CLASS_CLEANUP(ClassCleanup)
        {
        }

        TEST_METHOD(TestCompileAllSCI0)
        {
            _gameFolder = SetUpGameSCI0();
            _DoIt();
        }

        TEST_METHOD(TestCompileAllSCI11)
        {
            _gameFolder = SetUpGameSCI11();
            _DoIt();
        }

        TEST_METHOD_CLEANUP(TestCompileAll_Clean)
        {
            CleanUpGame(_gameFolder);
        }

        void _DoItHelper()
        {
            std::vector<ScriptId> scripts;
            appState->GetResourceMap().GetAllScripts(scripts);
            CompileLog log;
            // TODO: Clear errors?
            CompileTables tables;
            tables.Load(appState->GetVersion());
            PrecompiledHeaders headers(appState->GetResourceMap());
            CompileResults results(log);
            for (auto &script : scripts)
            {
                NewCompileScript(results, log, tables, headers, script);
            }
            Assert::IsFalse(log.HasErrors());
        }

        void _DoIt()
        {
            _DoItHelper();

            // BONUS: Now convert these to Sierra syntax and compile again
            CompileLog log;
            ConvertGame(appState->GetResourceMap(), LangSyntaxSCI, log);
            Assert::IsFalse(log.HasErrors());
            _DoItHelper();
        }

    private:
        static std::string _gameFolder;
	};

    std::string TestCompile::_gameFolder;
}