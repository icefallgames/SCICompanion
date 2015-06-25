#pragma once

#include "Components.h"

enum class PolygonType
{
    // These values must match those in sci.h
    TotalAccess = 0,
    NearestAccess = 1,
    BarredAccess = 2,
    ContainedAccess = 3
};

class PolygonComponent;

class SCIPolygon
{
public:
    SCIPolygon();
    const std::vector<point16> &Points() const { return _points; }
    std::vector<point16> &Points() { return _points; }
    void AppendPoint(point16 point);
    void DeletePoint(size_t index);
    void SetPoint(size_t index, point16 point);
    void InsertPoint(size_t index, point16 point);
    PolygonType Type;
    // std::string Name;

private:
    std::vector<point16> _points;
};

class PolygonComponent : public ResourceComponent
{
public:
    PolygonComponent(const std::string &polyFolder, int picNumber);
    void Commit(int picNumber);

    PolygonComponent *Clone() const override
    {
        return new PolygonComponent(*this);
    }

    const std::vector<SCIPolygon> &Polygons() const { return _polygons; }
    SCIPolygon *GetAt(size_t index);
    const SCIPolygon *GetAt(size_t index) const;
    SCIPolygon *GetBack();
    void AppendPolygon(const SCIPolygon &polygon);
    void DeletePolygon(size_t index);

private:
    std::string _GetPolyFile();

    std::vector<SCIPolygon> _polygons;
    std::string _polyFolder;
    int _picNumber;
};

std::unique_ptr<PolygonComponent> CreatePolygonComponent(const std::string &polyFolder, int picNumber);
std::string GetSetUpPolyProcedureName(int picResource);