#pragma once

enum class PolygonType
{
    TotalAccess,
    NearestAccess,
    BarredAccess,
    ContainedAccess
};

class SCIPolygon
{
public:
    PolygonType Type;
    std::string Name;
    std::vector<point16> Points;
};

class PolygonSource
{
public:
    PolygonSource(const std::string &filePath);
    void Commit();
    std::vector<SCIPolygon> Polygons;

private:
    std::string _filePath;
};

std::unique_ptr<PolygonSource> CreatePolygonSource(const std::string &polyFolder, int picNumber);