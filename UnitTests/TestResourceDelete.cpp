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
#include "ResourceContainer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
    TEST_CLASS(TestResourceDelete)
    {
    public:
        TEST_CLASS_INITIALIZE(ClassSetup)
        {
        }

        TEST_CLASS_CLEANUP(ClassCleanup)
        {
        }

        TEST_METHOD(TestDeleteSCI0)
        {
            _gameFolder = SetUpGameSCI0();
            _DoIt();
        }

        TEST_METHOD(TestDeleteSCI11)
        {
            _gameFolder = SetUpGameSCI11();
            _DoIt();
        }

        TEST_METHOD_CLEANUP(TestDelete_Clean)
        {
            CleanUpGame(_gameFolder);
        }

        std::unique_ptr<ResourceBlob> _Count(int retrieveNth, int &count)
        {
            // Enumerate
            std::unique_ptr<ResourceBlob> retrieve;
            count = 0;
            // Only include a subset of resource types, since things like audio/sync are dealt with differently.
            auto resourceContainer1 = appState->GetResourceMap().Resources(ResourceTypeFlags::View | ResourceTypeFlags::Pic | ResourceTypeFlags::Sound, ResourceEnumFlags::None | ResourceEnumFlags::AddInDefaultEnumFlags);
            for (auto &blob : *resourceContainer1)
            {
                if ((count == retrieveNth) || (retrieveNth == -1))
                {
                    retrieve = std::make_unique<ResourceBlob>(*blob);
                }
                count++;
            }
            return retrieve;
        }

        void _DoIt()
        {
            int count;
            std::unique_ptr<ResourceBlob> first = _Count(0, count);
            Assert::IsNotNull(first.get());

            // Delete first guy
            appState->GetResourceMap().DeleteResource(first.get());

            // Count again, and this time retrieve the second item
            int count2;
            std::unique_ptr<ResourceBlob> second = _Count(2, count2);

            Assert::AreEqual(count - 1, count2);

            // Delete second guy
            appState->GetResourceMap().DeleteResource(second.get());

            // Count again, and this time retrieve the last item
            int count3;
            std::unique_ptr<ResourceBlob> last = _Count(-1, count3);

            Assert::AreEqual(count - 2, count3);

            // Delete last item
            appState->GetResourceMap().DeleteResource(last.get());

            // Count again
            int count4;
            _Count(0, count4);

            Assert::AreEqual(count - 3, count4);
        }

    private:
        static std::string _gameFolder;
    };

    std::string TestResourceDelete::_gameFolder;
}