#include "stdafx.h"
#include "Polygon.h"
#include "format.h"
#include "ScriptOMAll.h"
#include "CompileContext.h"
#include "ScriptMakerHelper.h"

using namespace std;
using namespace sci;

enum class WindingOrder
{
    CW,
    CCW
};

WindingOrder DetermineWindingOrder(const std::vector<point16> &points)
{
    long totalProduct = 0;
    for (size_t i = 0; i < points.size(); i++)
    {
        point16 cur = points[i];
        point16 next = points[(i + 1) % points.size()];
        int product = (next.x - cur.x) * (next.y + cur.y);
        totalProduct += product;
    }
    return (totalProduct < 0) ? WindingOrder::CW : WindingOrder::CCW;
}

void FixupPolygon(SCIPolygon &polygon)
{
    WindingOrder desiredWindingOrder = WindingOrder::CW;
    if (polygon.Type == PolygonType::ContainedAccess)
    {
        desiredWindingOrder = WindingOrder::CCW;
    }
    WindingOrder actualWindingOrder = DetermineWindingOrder(polygon.Points());
    if (actualWindingOrder != desiredWindingOrder)
    {
        // Flip the points
        reverse(polygon.Points().begin(), polygon.Points().end());
    }
}

const char c_szDefaultPolyName[] = "P_Default";                 // P_Default[nnn] where [nnn] is the pic number.
const char c_szAddPolysToRoomFunction[] = "AddPolygonsToRoom";  // The export in main for SCI1.1 template game.

const string AccessType[] =
{
    "PTotalAccess",
    "PNearestAccess",
    "PBarredAccess",
    "PContainedAccess",
};

unique_ptr<ProcedureCall> GetSetUpPolyProcedureCall(int picResource)
{
    unique_ptr<ProcedureCall> procCall = make_unique<ProcedureCall>(c_szAddPolysToRoomFunction);
    _AddStatement(*procCall, make_unique<PropertyValue>(fmt::format("{0}{1}", c_szDefaultPolyName, picResource), ValueType::Pointer));
    return procCall;
}

void _ExtractPolygonsFromStatements(const string &name, PolygonComponent &polySource, const SingleStatementVector &statements)
{
    auto it = statements.begin();
    if (it != statements.end())
    {
        const PropertyValue *pValue = SafeSyntaxNode<PropertyValue>((*it)->GetSyntaxNode());
        int polyCount = 0;
        if (pValue->GetType() == ValueType::Number)
        {
            ++it;
            polyCount = pValue->GetNumberValue();
        }
        for (int i = 0; (it != statements.end()) && (i < polyCount); i++)
        {
            pValue = SafeSyntaxNode<PropertyValue>((*it)->GetSyntaxNode());
            ++it;
            if (pValue)
            {
                auto itType = find(begin(AccessType), end(AccessType), pValue->GetStringValue());
                if (itType != end(AccessType))
                {
                    SCIPolygon polygon;
                    polygon.Name = name;
                    polygon.Type = (PolygonType)(itType - begin(AccessType));

                    pValue = SafeSyntaxNode<PropertyValue>((*it)->GetSyntaxNode());
                    ++it;
                    if (pValue)
                    {
                        uint16_t pointCount = pValue->GetNumberValue();

                        // Now the points
                        while (pointCount && (it != statements.end()))
                        {
                            pValue = SafeSyntaxNode<PropertyValue>((*it)->GetSyntaxNode());
                            ++it;
                            if (pValue)
                            {
                                int16_t x = (int16_t)pValue->GetNumberValue();
                                if (it != statements.end())
                                {
                                    pValue = SafeSyntaxNode<PropertyValue>((*it)->GetSyntaxNode());
                                    ++it;
                                    if (pValue)
                                    {
                                        int16_t y = (int16_t)pValue->GetNumberValue();
                                        polygon.AppendPoint(point16(x, y));
                                    }
                                }
                            }
                            pointCount--;
                        }

                        polySource.AppendPolygon(polygon);
                    }
                }
            }
        }
    }
}

bool startsWith(const std::string &text, const std::string &prefix)
{
    return text.length() > prefix.length() && std::equal(prefix.begin(), prefix.end(), text.begin());
}

class ExtractPolygonsFromHeader : public IExploreNode, public IExploreNodeContext
{
public:
    ExtractPolygonsFromHeader(PolygonComponent &polySource) : _polySource(polySource) {}

    void ExploreNode(IExploreNodeContext *pContext, SyntaxNode &node, ExploreNodeState state) override
    {
        if (state == ExploreNodeState::Pre)
        {
            VariableDecl *varDecl = SafeSyntaxNode<VariableDecl>(&node);
            if (varDecl)
            {
                if (startsWith(varDecl->GetName(), c_szDefaultPolyName))
                {
                    // It's the default un-named polygons
                    _ExtractPolygonsFromStatements("", _polySource, varDecl->GetStatements());
                }
                else
                {
                    // Named poly
                    _ExtractPolygonsFromStatements(varDecl->GetName(), _polySource, varDecl->GetStatements());
                }
            }
        }
    }

    PolygonComponent &_polySource;
};

SCIPolygon::SCIPolygon() : Type(PolygonType::BarredAccess) {}

void SCIPolygon::AppendPoint(point16 point)
{
    _points.push_back(point);
}

void SCIPolygon::DeletePoint(size_t index)
{
    if (index < _points.size())
    {
        _points.erase(_points.begin() + index);
    }
}

void SCIPolygon::SetPoint(size_t index, point16 point)
{
    if (index < _points.size())
    {
        _points[index] = point;
    }
}

void SCIPolygon::InsertPoint(size_t index, point16 point)
{
    if (index < _points.size())
    {
        _points.insert(_points.begin() + (index + 1), point);
    }
}

PolygonComponent::PolygonComponent(const string &polyFolder, int picNumber) : _polyFolder(polyFolder), _picNumber(picNumber)
{
    // Compile this like a script file, but without all the symbol lookups.
    if (picNumber != -1)
    {
        CompileLog log;
        unique_ptr<Script> script = SimpleCompile(log, ScriptId(GetPolyFile().c_str()));
        ExtractPolygonsFromHeader extractPolygons(*this);
        script->Traverse(&extractPolygons, extractPolygons);
    }
}

// The Polygon:new
unique_ptr<SingleStatement> _MakeNewPolygon()
{
    unique_ptr<SingleStatement> statement = make_unique<SingleStatement>();

    unique_ptr<SendCall> polygonnew = make_unique<SendCall>();
    polygonnew->SetName("Polygon");
    polygonnew->AddSendParam(make_unique<SendParam>("new", true));

    statement->SetSyntaxNode(move(polygonnew));
    return statement;
}

std::string PolygonComponent::GetPolyFile() const
{
    return fmt::format("{0}\\{1}.shp", _polyFolder, _picNumber);
}

void _ApplyPolygonToVarDecl(VariableDecl &varDecl, const SCIPolygon &poly)
{
    // Type, point count, points
    varDecl.AddSimpleInitializer(PropertyValue(AccessType[(int)poly.Type], ValueType::Token));
    varDecl.AddSimpleInitializer(PropertyValue((uint16_t)poly.Points().size()));
    for (point16 point : poly.Points())
    {
        varDecl.AddSimpleInitializer(PropertyValue(point.x));
        varDecl.AddSimpleInitializer(PropertyValue(point.y));
    }
}

void _ApplyPolygonsToScript(int picNumber, Script &script, const vector<SCIPolygon> &polygons)
{
    // Start with the defaut polygons
    vector<SCIPolygon> defaultPolygons;
    copy_if(polygons.begin(), polygons.end(), back_inserter(defaultPolygons),
        [](const SCIPolygon &poly){ return poly.Name.empty(); }
        );

    // e.g. P_110
    string defaultPolyVarName = fmt::format("{0}{1}", c_szDefaultPolyName, picNumber);
    unique_ptr<VariableDecl> defaultPolyVarDecl = make_unique<VariableDecl>();
    defaultPolyVarDecl->SetName(defaultPolyVarName);

    // The count
    defaultPolyVarDecl->AddSimpleInitializer(PropertyValue((uint16_t)defaultPolygons.size()));
    for (const SCIPolygon &poly : defaultPolygons)
    {
        _ApplyPolygonToVarDecl(*defaultPolyVarDecl, poly);
    }
    defaultPolyVarDecl->SetSize((uint16_t)defaultPolyVarDecl->GetStatements().size());
    script.AddVariable(move(defaultPolyVarDecl));

    // Now the named ones
    for (const SCIPolygon &poly : polygons)
    {
        if (!poly.Name.empty())
        {
            unique_ptr<VariableDecl> namedPolyVarDecl = make_unique<VariableDecl>();
            namedPolyVarDecl->SetName(poly.Name);
            namedPolyVarDecl->AddSimpleInitializer(PropertyValue(1));   // 1: single polygon
            _ApplyPolygonToVarDecl(*namedPolyVarDecl, poly);
            namedPolyVarDecl->SetSize((uint16_t)namedPolyVarDecl->GetStatements().size());
            script.AddVariable(move(namedPolyVarDecl));
        }
    }
}

void PolygonComponent::Commit(int picNumber)
{
    _picNumber = picNumber;
    if (_picNumber != -1)
    {
        string polyFile = GetPolyFile();
        // Construct the script om
        Script script(ScriptId(polyFile.c_str()));

        std::stringstream ss;
        SourceCodeWriter out(ss, script.Language());
        out.pszNewLine = "\n";

        PCSTR pszFilename = PathFindFileName(polyFile.c_str());
        ss << fmt::format("// {0} -- Produced by SCI Companion{1}", pszFilename, out.pszNewLine);
        ss << fmt::format("// This file should only be edited with the SCI Companion polygon editor{0}", out.pszNewLine);

        _ApplyPolygonsToScript(picNumber, script, _polygons);

        // Now the meat of the script
        script.OutputSourceCode(out);
        string bakPath = polyFile + ".bak";
        MakeTextFile(ss.str().c_str(), bakPath);
        deletefile(polyFile);
        movefile(bakPath, polyFile);
    }
}

void PolygonComponent::AppendPolygon(const SCIPolygon &polygon)
{
    _polygons.push_back(polygon);
}

void PolygonComponent::DeletePolygon(size_t index)
{
    if (index < _polygons.size())
    {
        _polygons.erase(_polygons.begin() + index);
    }
}

SCIPolygon *PolygonComponent::GetAt(size_t index)
{
    SCIPolygon *poly = nullptr;
    if (index < _polygons.size())
    {
        poly = &_polygons[index];
    }
    return poly;
}

const SCIPolygon *PolygonComponent::GetAt(size_t index) const
{
    return const_cast<PolygonComponent*>(this)->GetAt(index);
}

SCIPolygon *PolygonComponent::GetBack()
{
    return &_polygons.back();
}

unique_ptr<PolygonComponent> CreatePolygonComponent(const string &polyFolder, int picNumber)
{
    // Create the directory if it doesn't exist
    if (!PathFileExists(polyFolder.c_str()))
    {
        CreateDirectory(polyFolder.c_str(), nullptr);
    }
    return make_unique<PolygonComponent>(polyFolder, picNumber);
}