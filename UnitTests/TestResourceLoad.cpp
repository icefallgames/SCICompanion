#include "stdafx.h"
#include "CppUnitTest.h"
#include "View.h"
#include "ResourceEntity.h"
#include "ResourceMap.h"
#include "AppState.h"
#include "ResourceContainer.h"
#include "Helper.h"
#include "format.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
    TEST_CLASS(TestResourceLoad)
    {
    public:
        TEST_CLASS_INITIALIZE(ClassSetup)
        {
        }

        TEST_CLASS_CLEANUP(ClassCleanup)
        {
        }

        TEST_METHOD(TestLoadResourcesSCI0)
        {
            _gameFolder = SetUpGameSCI0();
            _DoIt();
        }

        TEST_METHOD(TestLoadResourcesSCI11)
        {
            _gameFolder = SetUpGameSCI11();
            _DoIt();
        }

        TEST_METHOD_CLEANUP(TestLoadResources_Clean)
        {
            CleanUpGame(_gameFolder);
        }

        void _DoIt()
        {
            auto container = appState->GetResourceMap().Resources(ResourceTypeFlags::View, ResourceEnumFlags::MostRecentOnly);
            for (auto &blob : *container)
            {
                try
                {
                    ResourceEntity *pTest = CreateViewResource(sciVersion0);
                }
                catch (std::exception)
                {
                    std::wstring message = fmt::format(L"Failed to load resource %d of type %d.", blob->GetNumber(), (int)blob->GetType());
                    Assert::IsTrue(false, message.c_str());
                }
            }
        }

    private:
        static std::string _gameFolder;

    };

    std::string TestResourceLoad::_gameFolder;
}
