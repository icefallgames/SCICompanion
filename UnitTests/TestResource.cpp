#include "stdafx.h"
#include "CppUnitTest.h"
#include "View.h"
#include "ResourceEntity.h"
#include "ResourceMap.h"
#include "AppState.h"
#include "ResourceContainer.h"
#include "RasterOperations.h"
#include "format.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

std::wstring ToString(const RasterChangeHint& q)
{
    return fmt::format(L"RasterChangeHint:{0:08x}", (int)q);
}

namespace UnitTests
{		
    TEST_CLASS(TestResource)
    {
    public:
        TEST_CLASS_INITIALIZE(ClassSetup)
        {
        }

        TEST_CLASS_CLEANUP(ClassCleanup)
        {
        }

        TEST_METHOD(TestCreateDefaultResources)
        {
	        // Create the default resources.
            ResourceEntity *pTest = CreateDefaultViewResource(sciVersion0);
        }

        TEST_METHOD(TestViewMirror)
        {
            RasterChange change;
            ResourceEntity *pTest = CreateDefaultViewResource(sciVersion0);
            Assert::IsNotNull(pTest);
            RasterComponent *raster = pTest->TryGetComponent<RasterComponent>();
            Assert::IsNotNull(raster);
            // Insert second loop:
            change = InsertLoop(*raster, 0, false);
            Assert::AreEqual(change.hint, RasterChangeHint::NewView);
            // Make it a mirror of the first:
            change = MakeMirrorOf(*raster, 1, 0);
            Assert::AreEqual(change.hint, RasterChangeHint::NewView);
            Loop &loopOrig = raster->Loops[0];
            Assert::IsFalse(loopOrig.IsMirror);
            Assert::AreEqual(loopOrig.MirrorOf, (uint8_t)0xff);
            Loop &loopMirror = raster->Loops[1];
            Assert::IsTrue(loopMirror.IsMirror);
            Assert::AreEqual(loopMirror.MirrorOf, (uint8_t)0);
            // Un-mirror it
            change = MakeMirrorOf(*raster, 1, -1);
            Assert::AreEqual(change.hint, RasterChangeHint::NewView);
            Assert::IsTrue(!loopMirror.IsMirror);
            Assert::AreEqual(loopMirror.MirrorOf, (uint8_t)0xff);
        }

	};
}