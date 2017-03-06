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
#include "Pic.h"
#include "Polygon.h"
#include "ResourceEntity.h"
#include "ResourceMap.h"
#include "AppState.h"
#include "ResourceContainer.h"
#include "Helper.h"
#include "format.h"
#include "CompileContext.h"
#include "ScriptConvert.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
    TEST_CLASS(TestPolygonLoad)
    {
    public:
        TEST_CLASS_INITIALIZE(ClassSetup)
        {
        }

        TEST_CLASS_CLEANUP(ClassCleanup)
        {
        }

        TEST_METHOD(TestLoadPolygonSCI11)
        {
            _gameFolder = SetUpGameSCI11();
            _DoIt();
        }

        TEST_METHOD_CLEANUP(TestLoadResources_Clean)
        {
            CleanUpGame(_gameFolder);
        }

        void _DoItHelper(int picNumber)
        {
            // Open it (if it exists, which it shouldn't)
            std::string polyFolder = appState->GetResourceMap().Helper().GetPolyFolder();
            std::unique_ptr<PolygonComponent> poly = CreatePolygonComponent(polyFolder, picNumber);
            SCIPolygon polygon;
            polygon.Type = PolygonType::ContainedAccess;
            polygon.AppendPoint(point16(0, 0));
            polygon.AppendPoint(point16(1, 0));
            polygon.AppendPoint(point16(1, 1));
            // Append a polygon and save
            poly->AppendPolygon(polygon);
            poly->Commit(picNumber);

            // Reload and verify
            poly = CreatePolygonComponent(polyFolder, picNumber);
            Assert::IsTrue(poly->Polygons().size() == 1);
            Assert::IsTrue(poly->Polygons()[0].Points().size() == 3);
        }

        void _DoIt()
        {
            _DoItHelper(777);

            appState->GetResourceMap().SetGameLanguage(LangSyntax::LangSyntaxSCI);

            _DoItHelper(778); // Different number, since we want a fresh file.
        }

    private:
        static std::string _gameFolder;

    };

    std::string TestPolygonLoad::_gameFolder;
}
