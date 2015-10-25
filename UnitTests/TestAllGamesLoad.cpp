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
#include "View.h"
#include "ResourceEntity.h"
#include "ResourceUtil.h"
#include "ResourceMap.h"
#include "AppState.h"
#include "ResourceContainer.h"
#include "Helper.h"
#include "format.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// Place Sierra game folders under the following folder (or change based on your needs):
const char SierraGameFolder[] = "e:\\SierraGames\\";

// We can't easily identify the game, so just identify type/number pairs that are known to fail.
std::pair<ResourceType, int> KnownFailures[]
{
    { ResourceType::View, 111 },        // LSL6
    { ResourceType::View, 140 },        // PQ2
    { ResourceType::View, 17 },         // SQ4
};

namespace UnitTests
{
    TEST_CLASS(TestAllGamesLoad)
    {
    public:
        TEST_CLASS_INITIALIZE(ClassSetup)
        {
        }

        TEST_CLASS_CLEANUP(ClassCleanup)
        {
            delete appState;
        }

        TEST_METHOD(TestAllGames)
        {
            std::string findString = SierraGameFolder;
            findString += "*";
            // Collect the file names
            std::vector<std::string> folders;
            WIN32_FIND_DATA findData = { 0 };
            HANDLE hFolder = FindFirstFile(findString.c_str(), &findData);
            if (hFolder != INVALID_HANDLE_VALUE)
            {
                BOOL ok = TRUE;
                while (ok)
                {
                    if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    {
                        if (*findData.cFileName != '.')
                        {
                            folders.push_back(findData.cFileName);
                        }
                    }

                    ok = FindNextFile(hFolder, &findData);
                }
                FindClose(hFolder);
            }

            if (folders.empty())
            {
                Logger::WriteMessage(L"Found no games.");
            }

            for (auto folder : folders)
            {
                std::string finalPath = SierraGameFolder;
                finalPath += folder;
                _LoadAllResources(finalPath);
            }
        }

        void _LoadAllResources(const std::string &gameFolder)
        {
            char szPath[MAX_PATH];
            GetCurrentDirectory(MAX_PATH, szPath);

            std::wstring message = fmt::format(L"Loading game in {0}.", gameFolder);
            Logger::WriteMessage(message.c_str());

            appState = new AppState(nullptr);
            appState->GetResourceMap().SetGameFolder(gameFolder);
            Assert::IsTrue(appState->GetResourceMap().IsGameLoaded());

            // Normally ResourceMap uses the module filename for this. But unit tests are run from another exe.
            std::string exeFolder = szPath;
            exeFolder += "\\";
            appState->GetResourceMap().SetIncludeFolderForTest(exeFolder);

            ResourceTypeFlags flags = ResourceTypeFlags::AllCreatable;
            flags &= ~ResourceTypeFlags::Sound;     // Leave sounds out for now, we still don't load SCI10 sounds properly.
            flags &= ~ResourceTypeFlags::Vocab;     // Vocabs can't just be "created", we need to follow more specific logic. TODO
            auto container = appState->GetResourceMap().Resources(flags, ResourceEnumFlags::None);
            int count = 0;
            for (auto &blob : *container)
            {
                try
                {
                    std::unique_ptr<ResourceEntity> resource = CreateResourceFromResourceData(*blob, false);
                    count++;
                }
                catch (std::exception)
                {
                    auto itFind = std::find_if(std::begin(KnownFailures), std::end(KnownFailures),
                        [&blob](std::pair<ResourceType, int> &pair) { return pair.first == blob->GetType() && pair.second == blob->GetNumber(); }
                        );

                    if (itFind == std::end(KnownFailures))
                    {
                        std::wstring message = fmt::format(L"Unexpected: failed to load resource {0} of type {1}.", blob->GetNumber(), (int)blob->GetType());
                        Assert::IsTrue(false, message.c_str());
                    }
                }
            }
            
            message = fmt::format(L"Loaded {0} resources.", count);
            Logger::WriteMessage(message.c_str());

            delete appState;
            appState = nullptr;
        }

    };
}