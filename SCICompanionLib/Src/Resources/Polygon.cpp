#include "stdafx.h"
#include "Polygon.h"
#include "format.h"
#include "ScriptOMAll.h"
#include "CompileContext.h"
#include "ScriptMakerHelper.h"

using namespace std;
using namespace sci;

const char c_szAddObstacleSelector[] = "addObstacle";
const char c_szTypeSelector[] = "type";
const char c_szInitSelector[] = "init";
const char c_szProcedureName[] = "SetUpPolys";
const char c_szRoomName[] = "gRoom";

const string AccessType[] =
{
    "PTotalAccess",
    "PNearestAccess",
    "PBarredAccess",
    "PContainedAccess",
};

std::string GetSetUpPolyProcedureName(int picResource)
{
    return fmt::format("SetUpPolys_{0}", picResource);
}

class ExtractPolygonsFromHeader : public IExploreNode, public IExploreNodeContext
{
public:
    ExtractPolygonsFromHeader(PolygonComponent &polySource) : _polySource(polySource) {}

    void ExploreNode(IExploreNodeContext *pContext, SyntaxNode &node, ExploreNodeState state) override
    {
        // REVIEW: We could do more validation here (procedure name, etc...), but for now we can just look for things we're interested in.
        if (state == ExploreNodeState::Pre)
        {
            SendParam *sendParam = SafeSyntaxNode<SendParam>(&node);
            if (sendParam)
            {
                if (sendParam->GetName() == c_szAddObstacleSelector)
                {
                    // New polygon
                    _polySource.AppendPolygon(SCIPolygon());
                }
                else if ((sendParam->GetName() == c_szTypeSelector) && sendParam->HasValue())
                {
                    // The polygon type
                    const ComplexPropertyValue *value = SafeSyntaxNode<ComplexPropertyValue>(sendParam->GetStatements()[0]->GetSyntaxNode());
                    if (value)
                    {
                        auto it = find(begin(AccessType), end(AccessType), value->GetStringValue());
                        auto foo = begin(AccessType);
                        size_t index = it - foo;
                        _polySource.GetBack()->Type = (PolygonType)index;
                    }
                }
                else if (sendParam->GetName() == c_szInitSelector)
                {
                    SCIPolygon *polygon = _polySource.GetBack();
                    point16 newPoint;
                    bool startNewPoint = true;
                    // The polygon points
                    for (auto &statement : sendParam->GetStatements())
                    {
                        const ComplexPropertyValue *value = SafeSyntaxNode<ComplexPropertyValue>(statement->GetSyntaxNode());
                        if (value)
                        {
                            int16_t numberValue = (int16_t)value->GetNumberValue();
                            if (startNewPoint)
                            {
                                newPoint.x = numberValue;
                            }
                            else
                            {
                                newPoint.y = numberValue;
                                polygon->AppendPoint(newPoint);
                            }
                            startNewPoint = !startNewPoint;
                        }
                    }
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
        unique_ptr<Script> script = SimpleCompile(log, ScriptId(polyFolder.c_str()));
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

// The values set on the new polygon
unique_ptr<SingleStatement> _MakeThePolygonStatement(const SCIPolygon &poly)
{
    unique_ptr<SingleStatement> statement = make_unique<SingleStatement>();
    unique_ptr<SendCall> polygonStuff = make_unique<SendCall>();

    polygonStuff->SetStatement1(move(_MakeNewPolygon()));

    if ((int)poly.Type < ARRAYSIZE(AccessType))
    {
        unique_ptr<SendParam> typeParam = make_unique<SendParam>(c_szTypeSelector);
        typeParam->AddStatement(_MakeTokenStatement(AccessType[(int)poly.Type]));
        polygonStuff->AddSendParam(move(typeParam));
    }

    unique_ptr<SendParam> initParam = make_unique<SendParam>(c_szInitSelector);
    for (const auto &point : poly.Points())
    {
        initParam->AddStatement(_MakeNumberStatement(point.x));
        initParam->AddStatement(_MakeNumberStatement(point.y));
    }
    polygonStuff->AddSendParam(move(initParam));

    polygonStuff->AddSendParam(make_unique<SendParam>("yourself", true));

    statement->SetSyntaxNode(move(polygonStuff));
    return statement;
}

// The addObstacle call
unique_ptr<SingleStatement> _MakeAddPolygonCode(const SCIPolygon &poly)
{
    unique_ptr<SingleStatement> statement = make_unique<SingleStatement>();
    unique_ptr<SendCall> roomAddObstacble = make_unique<SendCall>();

    roomAddObstacble->SetLValue(make_unique<LValue>(c_szRoomName));

    unique_ptr<SendParam> addObstacleParam = make_unique<SendParam>();
    addObstacleParam->SetName(c_szAddObstacleSelector);
    addObstacleParam->AddStatement(_MakeThePolygonStatement(poly));
    roomAddObstacble->AddSendParam(move(addObstacleParam));
    statement->SetSyntaxNode(move(roomAddObstacble));
    return statement;
}

std::string PolygonComponent::_GetPolyFile()
{
    return fmt::format("{0}\\{1}.shp", _polyFolder, _picNumber);
}

void PolygonComponent::Commit(int picNumber)
{
    _picNumber = picNumber;
    if (_picNumber != -1)
    {
        string polyFile = _GetPolyFile();
        // Construct the script om
        Script script(ScriptId(polyFile.c_str()));

        unique_ptr<ProcedureDefinition> proc = make_unique<ProcedureDefinition>();
        proc->SetName(GetSetUpPolyProcedureName(_picNumber));
        proc->SetScript(&script);
        proc->AddSignature(make_unique<FunctionSignature>());
        for (auto &poly : _polygons)
        {
            proc->AddStatement(move(_MakeAddPolygonCode(poly)));
        }
        script.AddProcedure(move(proc));

        std::stringstream ss;
        SourceCodeWriter out(ss, script.Language());
        //out.pszNewLine = "\r\n";
        out.pszNewLine = "\n";

        PCSTR pszFilename = PathFindFileName(polyFile.c_str());
        ss << fmt::format("// {0} -- Produced by SCI Companion{1}", pszFilename, out.pszNewLine);
        ss << fmt::format("// This file should only be edited with the SCI Companion polygon editor{0}", out.pszNewLine);

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