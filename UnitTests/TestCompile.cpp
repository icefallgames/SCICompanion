#include "stdafx.h"
#include "CppUnitTest.h"
#include "ResourceMap.h"
#include "AppState.h"
#include "ScriptOM.h"
#include "CompileContext.h"
#include "Helper.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(TestCompile)
	{
	public:
        TEST_CLASS_INITIALIZE(ClassSetup)
        {
            _gameFolder = SetUpGame();
        }

        TEST_CLASS_CLEANUP(ClassCleanup)
        {
            CleanUpGame(_gameFolder);
        }

        TEST_METHOD(TestCompileAll)
        {
            std::vector<ScriptId> scripts;
            appState->GetResourceMap().GetAllScripts(scripts);
            CompileLog log;
            // TODO: Clear errors?
            CompileTables tables;
            tables.Load(appState->GetVersion());
            PrecompiledHeaders headers(appState->GetResourceMap());
            for (auto &script : scripts)
            {
                NewCompileScript(log, tables, headers, script);
            }
        }

    private:
        static std::string _gameFolder;
	};

    std::string TestCompile::_gameFolder;
}