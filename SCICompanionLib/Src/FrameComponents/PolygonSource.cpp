#include "stdafx.h"
#include "PolygonSource.h"
#include "format.h"

using namespace std;

PolygonSource::PolygonSource(const string &filePath) : _filePath(filePath)
{
    // TODO Load it. If the file doesn't exist, that's fine.
}

void PolygonSource::Commit()
{
    // TODO save it.
}

unique_ptr<PolygonSource> CreatePolygonSource(const string &polyFolder, int picNumber)
{
    string polyFilename = fmt::format("{0}.shp", picNumber);
    string polyFilePath = fmt::format("{0}\\{1}", polyFolder, polyFilename);
    return make_unique<PolygonSource>(polyFilePath);
}