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
#include "CompiledScript.h"
#include "DecompilerCore.h"
#include "DecompilerConfig.h"
#include "DecompilerResults.h"
#include "PMachine.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

class TestDecompilerResults : public IDecompilerResults
{
public:
    void AddResult(DecompilerResultType type, const std::string &message) override {}
    bool IsAborted() override { return false; }
    void InformStats(bool functionSuccessful, int byteCount) override { (functionSuccessful ? successCount : fallbackCount)++; }
    void SetGlobalVarsUpdated(const std::vector<std::pair<std::string, std::string>> &mainDirtyRenames) override {}

    int successCount = 0;
    int fallbackCount = 0;
};

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

        // Smoke test: decompile every script the template games compile to, which runs the control
        // flow graph and its code_pos maps. TestCodePosLessHandlesEnd covers the end() key case.
        TEST_METHOD(TestDecompileAllSCI0)
        {
            _gameFolder = SetUpGameSCI0();
            _DoItHelper();
            _DecompileAll();
        }

        TEST_METHOD(TestDecompileAllSCI11)
        {
            _gameFolder = SetUpGameSCI11();
            _DoItHelper();
            _DecompileAll();
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
            for (auto &script : scripts)
            {
                CompileResults results(log);
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

        void _DecompileAll()
        {
            const GameFolderHelper &helper = appState->GetResourceMap().Helper();
            GlobalCompiledScriptLookups lookups;
            Assert::IsTrue(lookups.Load(helper));
            uint16_t dummy;
            lookups.GetSelectorTable().ReverseLookup("", dummy);  // Prime the selector name cache, as DecompileDialog does.
            std::unique_ptr<IDecompilerConfig> config = CreateDecompilerConfig(helper, lookups.GetSelectorTable());

            std::vector<ScriptId> scripts;
            appState->GetResourceMap().GetAllScripts(scripts);
            TestDecompilerResults results;
            int decompiledScripts = 0;
            for (auto &script : scripts)
            {
                uint16_t scriptNum = script.GetResourceNumber();
                CompiledScript compiledScript(0, CompiledScriptFlags::RemoveBadExports);
                if (compiledScript.Load(helper, helper.Version, scriptNum))
                {
                    std::unique_ptr<sci::Script> decompiled = DecompileScript(config.get(), lookups, helper, scriptNum, compiledScript, results);
                    Assert::IsTrue(decompiled != nullptr);
                    decompiledScripts++;
                }
            }
            Assert::IsTrue(decompiledScripts > 0);
            Assert::IsTrue(results.successCount > 0);
        }

    private:
        static std::string _gameFolder;
	};

    std::string TestCompile::_gameFolder;

    // Needs no game, so it lives outside TestCompile, whose per-test cleanup tears down a game folder.
    TEST_CLASS(TestCodePos)
    {
    public:
        // code_pos maps (compiler branch fixups, decompiler control flow) can be keyed on the code list's
        // end(). The comparator must not dereference it: the debug STL asserts, and it is undefined anyway.
        TEST_METHOD(TestCodePosLessHandlesEnd)
        {
            std::list<scii> code;
            code.emplace_back(sciVersion0, Opcode::RET, 0);
            code.emplace_back(sciVersion0, Opcode::RET, 0);
            std::map<code_pos, int, CodePosLess> positions(CodePosLess{ code.end() });
            positions[code.end()] = 0;
            positions[code.begin()] = 1;
            positions[std::next(code.begin())] = 2;
            positions[code.end()] = 3;
            Assert::AreEqual((size_t)3, positions.size());
            Assert::AreEqual(3, positions[code.end()]);
            Assert::AreEqual(1, positions[code.begin()]);
            Assert::AreEqual(2, positions[std::next(code.begin())]);
        }
    };
}