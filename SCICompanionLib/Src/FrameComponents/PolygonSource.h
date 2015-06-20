#pragma once

enum class PolygonType
{
    // These values must match those in sci.h
    TotalAccess = 0,
    NearestAccess = 1,
    BarredAccess = 2,
    ContainedAccess = 3
};

class PolygonSource;

class SCIPolygon
{
public:
    SCIPolygon(PolygonSource *ownerWeak);

    const std::vector<point16> &Points() const { return _points; }
    void AppendPoint(point16 point);
    void DeletePoint(size_t index);
    void SetPoint(size_t index, point16 point);
    PolygonType Type;
    // std::string Name;

private:
    std::vector<point16> _points;
    PolygonSource *_ownerWeak;
};

class PolygonSource
{
public:
    PolygonSource(const std::string &filePath);
    void Commit();

    const std::vector<SCIPolygon> &Polygons() const { return _polygons; }
    SCIPolygon *GetAt(size_t index);
    SCIPolygon *GetBack();
    void AppendPolygon();

    void SetDirty() { _dirty = true;  }

private:
    std::vector<SCIPolygon> _polygons;
    std::string _filePath;
    bool _dirty;
};

std::unique_ptr<PolygonSource> CreatePolygonSource(const std::string &polyFolder, int picNumber);