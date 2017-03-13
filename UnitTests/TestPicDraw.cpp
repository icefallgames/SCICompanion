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

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


#include "ImageUtil.h"
#include "PaletteOperations.h"
#include "View.h"
#include "ResourceContainer.h"
#include "ResourceSources.h"
#include "ResourceMapOperations.h"
#include "PatchResourceSource.h"
#include "PicDrawManager.h"
#include "Pic.h"
#include "ResourceEntity.h"
#include "ResourceSourceFlags.h"
#include "format.h"
#include "Helper.h"
#include "GameFolderHelper.h"

std::unique_ptr<Cel> CelFromBitmapFile(const std::string &filename)
{
    std::unique_ptr<Cel> cel;
    // GDI+ only deals with unicode.
    int a = filename.length();
    BSTR unicodestr = SysAllocStringLen(nullptr, a);
    MultiByteToWideChar(CP_ACP, 0, filename.c_str(), a, unicodestr, a);
    std::unique_ptr<Gdiplus::Bitmap> bitmap(Gdiplus::Bitmap::FromFile(unicodestr, TRUE));
    if (bitmap)
    {
        // T color doesn't matter.
        std::vector<Cel> cels;
        PaletteComponent palette;
        if (GetCelsAndPaletteFromGdiplus(*bitmap, 255, cels, palette))
        {
            cel = std::make_unique<Cel>(cels[0]);
            FlipImageData(&cel->Data[0], cel->size.cx, cel->size.cy, cel->GetStride());
        }
    }
    return cel;
}

bool CompareCels(Cel &one, Cel &two, size_t &index, uint8_t &oneFound, uint8_t &twoFound)
{
    index = 0xffffffff;
    if (one.Data.size() != two.Data.size())
    {
        oneFound = twoFound = 0;
        return false;
    }
    else
    {
        for (index = 0; index < one.Data.size(); index++)
        {
            if (one.Data[index] != two.Data[index])
            {
                oneFound = one.Data[index];
                twoFound = two.Data[index];
                return false;
            }
        }
    }
    return true;
}

void VerifyPic(PicDrawManager &pdm, PicScreen screen, const std::string &fileName)
{
    std::unique_ptr<Cel> celCreated = pdm.MakeCelFromPic(screen, PicPosition::Final);
    std::unique_ptr<Cel> celBitmap = CelFromBitmapFile(fileName);
    if (!celBitmap)
    {
        std::wstring message = fmt::format(L"Unable to load bitmap file {}.", fileName);
        Logger::WriteMessage(message.c_str());
    }
    Assert::IsNotNull(celBitmap.get());
    if (celBitmap)
    {
        size_t offset;
        uint8_t found, expected;
        bool result = CompareCels(*celCreated, *celBitmap, offset, found, expected);
        if (!result)
        {
            std::wstring message = fmt::format(L"Difference in offset {0} ({2},{3}) of pic {1}.\nExpected {4:02x} and got {5:02x}", offset, fileName, offset % celCreated->size.cx, offset / celCreated->size.cy, (int)expected, (int)found);
            Logger::WriteMessage(message.c_str());
        }
        Assert::IsTrue(result);
    }
}

void VerifyFileWorker(ResourceEntity &resource, const std::string &filenameRaw)
{
    PicDrawManager pdm(resource.TryGetComponent<PicComponent>(), resource.TryGetComponent<PaletteComponent>());

    VerifyPic(pdm, PicScreen::Visual, filenameRaw + "-vis.bmp");
    VerifyPic(pdm, PicScreen::Priority, filenameRaw + "-pri.bmp");
    std::string filenameCtl = filenameRaw + "-ctl.bmp";
    // SCI2 doesn't have ctl, so check first.
    if (PathFileExists(filenameCtl.c_str()))
    {
        VerifyPic(pdm, PicScreen::Control, filenameCtl);
    }
}

void VerifyFilesInFolder(bool saveAndReload, SCIVersion version, const std::string &folder)
{
    std::unique_ptr<ResourceSourceArray> mapAndVolumes = std::make_unique<ResourceSourceArray>();
    mapAndVolumes->push_back(std::make_unique<PatchFilesResourceSource>(ResourceTypeFlags::Pic, version, folder, ResourceSourceFlags::PatchFile));
    std::unique_ptr<ResourceContainer> resourceContainer(
        new ResourceContainer(
        folder,
        move(mapAndVolumes),
        ResourceTypeFlags::Pic,
        ResourceEnumFlags::None,
        nullptr)
        );

    bool foundSome = false;
    for (auto blob : *resourceContainer)
    {
        foundSome = true;
        std::unique_ptr<ResourceEntity> resource = CreateResourceFromResourceData(*blob);
        std::string filenameRaw = folder + "\\" + GetFileNameFor(*blob);
        if (saveAndReload)
        {
            // Save it to a stream
            sci::ostream savedStream;
            std::map<BlobKey, uint32_t> propertyBag;
            resource->WriteTo(savedStream, true, resource->ResourceNumber, propertyBag);
            // Load it back
            sci::istream loadStream(savedStream.GetInternalPointer(), savedStream.GetDataSize());
            ResourceBlob blob;
            GameFolderHelper dummyHelper;
            blob.CreateFromBits(dummyHelper, "whatever", resource->GetType(), &loadStream, resource->PackageNumber, resource->ResourceNumber, resource->Base36Number, version, ResourceSourceFlags::PatchFile);
            std::unique_ptr<ResourceEntity> resourceReloaded = CreateResourceFromResourceData(blob, false);
            // Veirfy
            VerifyFileWorker(*resourceReloaded, filenameRaw);
        }
        else
        {
            VerifyFileWorker(*resource, filenameRaw);
        }
    }
    if (!foundSome)
    {
        std::wstring message = fmt::format(L"Found no test files in {0}", folder);
        Logger::WriteMessage(message.c_str());
        Assert::IsTrue(foundSome);
    }
}

void TestPicsHelper(bool saveAndReload)
{
    std::string folder = GetTestFileDirectory("Pics");
    VerifyFilesInFolder(saveAndReload, sciVersion0, folder + "\\SCI0");
    VerifyFilesInFolder(saveAndReload, sciVersion1_EarlyEGA, folder + "\\SCI1.0\\EGA");
    VerifyFilesInFolder(saveAndReload, sciVersion1_Early, folder + "\\SCI1.0\\Early");
    VerifyFilesInFolder(saveAndReload, sciVersion1_Mid, folder + "\\SCI1.0\\Mid");
    VerifyFilesInFolder(saveAndReload, sciVersion1_1, folder + "\\SCI1.1");
    VerifyFilesInFolder(saveAndReload, sciVersion2, folder + "\\SCI2");
}

namespace UnitTests
{
    TEST_CLASS(TextPicDraw)
    {
    public:
        TEST_CLASS_INITIALIZE(ClassSetup)
        {
            if (Gdiplus::Ok != Gdiplus::GdiplusStartup(&_gdiplusToken, &_gdiplusStartupInput, nullptr))
            {
                Assert::IsFalse(true);
            }
        }

        TEST_CLASS_CLEANUP(ClassCleanup)
        {
            Gdiplus::GdiplusShutdown(_gdiplusToken);
        }

        TEST_METHOD(TestPics)
        {
            TestPicsHelper(true);
        }

        TEST_METHOD(TestPicsSaveReload)
        {
            TestPicsHelper(false);
        }

    private:
        static Gdiplus::GdiplusStartupInput _gdiplusStartupInput;
        static ULONG_PTR _gdiplusToken;
    };

    Gdiplus::GdiplusStartupInput TextPicDraw::_gdiplusStartupInput;
    ULONG_PTR TextPicDraw::_gdiplusToken;
}