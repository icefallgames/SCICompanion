#include "stdafx.h"
#include "CppUnitTest.h"
#include "ResourceMap.h"
#include "AppState.h"
#include "ScriptOM.h"
#include "CompileContext.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(TestCompile)
	{
	public:

        TEST_CLASS_INITIALIZE(ClassSetup)
        {
            char szPath[MAX_PATH];
            GetCurrentDirectory(MAX_PATH, szPath);
            std::string gameFolder = szPath;
            gameFolder += "\\TemplateGame";

            appState = new AppState(nullptr);
            appState->GetResourceMap().SetGameFolder(gameFolder);

            // Normally ResourceMap uses the module filename for this. But unit tests are run from another exe.
            std::string exeFolder = szPath;
            exeFolder += "\\";
            appState->GetResourceMap().SetIncludeFolderForTest(exeFolder);
        }

        TEST_CLASS_CLEANUP(ClassCleanup)
        {
            delete appState;
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

	};
}