#include "stdafx.h"
#include "CppUnitTest.h"
#include "View.h"
#include "ResourceEntity.h"
#include "ResourceMap.h"
#include "AppState.h"
#include "ResourceContainer.h"
#include "Helper.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
    TEST_CLASS(TestResourceLoad)
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

        TEST_METHOD(TestLoadResources)
        {
            auto container = appState->GetResourceMap().Resources(ResourceTypeFlags::View, ResourceEnumFlags::MostRecentOnly);
            for (auto &blob : *container)
            {
                ResourceEntity *pTest = CreateViewResource(sciVersion0);
                Assert::IsTrue(SUCCEEDED(pTest->InitFromResource(blob.get())), L"Failed to load resource.");
            }
        }

    private:
        static std::string _gameFolder;

    };

    std::string TestResourceLoad::_gameFolder;
}