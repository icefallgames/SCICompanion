#include "stdafx.h"
#include "CppUnitTest.h"
#include "View.h"
#include "ResourceEntity.h"
#include "ResourceMap.h"
#include "AppState.h"
#include "ResourceContainer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{		
    TEST_CLASS(TestResource)
    {
    public:
        TEST_CLASS_INITIALIZE(ClassSetup)
        {
            appState = new AppState(nullptr);
            std::string gameFolder = "E:\\Documents\\Visual Studio 2013\\Projects\\SCICompanion\\Debug\\TemplateGame";
            appState->GetResourceMap().SetGameFolder(gameFolder);
        }

        TEST_CLASS_CLEANUP(ClassCleanup)
        {
            delete appState;
        }
        

        TEST_METHOD(TestCreateDefaultResources)
        {
	        // Create the default resources.
            ResourceEntity *pTest = CreateDefaultViewResource(sciVersion0);
        }

        TEST_METHOD(TestLoadResources)
        {
            for (auto &blob : *appState->GetResourceMap().Resources(ResourceTypeFlags::View, ResourceEnumFlags::MostRecentOnly))
            {
                ResourceEntity *pTest = CreateViewResource(sciVersion0);
                Assert::IsTrue(SUCCEEDED(pTest->InitFromResource(blob.get())), L"Failed to load resource.");
            }
        }

	};
}