#include "stdafx.h"
#include "PolygonSource.h"
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

class ExtractPolygonsFromHeader : public IExploreNode, public IExploreNodeContext
{
public:
    ExtractPolygonsFromHeader(PolygonSource &polySource) : _polySource(polySource) {}

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
                    _polySource.AppendPolygon();
                }
                else if ((sendParam->GetName() == c_szTypeSelector) && sendParam->HasValue())
                {
                    // The polygon type
                    const ComplexPropertyValue *value = SafeSyntaxNode<ComplexPropertyValue>(sendParam->GetStatements()[0]->GetSyntaxNode());
                    if (value)
                    {
                        auto it = find(begin(AccessType), end(AccessType), value->GetStringValue());
                        auto foo = end(AccessType);
                        size_t index = foo - it;
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

    PolygonSource &_polySource;
};

SCIPolygon::SCIPolygon(PolygonSource *ownerWeak) : _ownerWeak(ownerWeak), Type(PolygonType::BarredAccess) {}

void SCIPolygon::AppendPoint(point16 point)
{
    _points.push_back(point);
    _ownerWeak->SetDirty();
}

void SCIPolygon::DeletePoint(size_t index)
{
    if (index < _points.size())
    {
        _points.erase(_points.begin() + index);
        _ownerWeak->SetDirty();
    }
}

void SCIPolygon::SetPoint(size_t index, point16 point)
{
    if (index < _points.size())
    {
        _points[index] = point;
        _ownerWeak->SetDirty();
    }
}

PolygonSource::PolygonSource(const string &filePath) : _filePath(filePath), _dirty(false)
{
    // Compile this like a script file, but without all the symbol lookups.
    CompileLog log;
    unique_ptr<Script> script = SimpleCompile(log, ScriptId(filePath.c_str()));
    ExtractPolygonsFromHeader extractPolygons(*this);
    script->Traverse(&extractPolygons, extractPolygons);

    _dirty = false; // Since we just loaded (ExtractPolygonsFromHeader will cause it to be true)
}

// The Polygon:new
unique_ptr<SingleStatement> _MakeNewPolygonStatement(const SCIPolygon &poly)
{
    unique_ptr<SingleStatement> statement = make_unique<SingleStatement>();

    unique_ptr<SendCall> polygonNew = make_unique<SendCall>();
    polygonNew->SetName("Polygon");

    polygonNew->AddSendParam(make_unique<SendParam>("new", true));

    if ((int)poly.Type < ARRAYSIZE(AccessType))
    {
        unique_ptr<SendParam> typeParam = make_unique<SendParam>(c_szTypeSelector);
        typeParam->AddStatement(_MakeTokenStatement(AccessType[(int)poly.Type]));
        polygonNew->AddSendParam(move(typeParam));
    }

    unique_ptr<SendParam> initParam = make_unique<SendParam>(c_szInitSelector);
    for (const auto &point : poly.Points())
    {
        initParam->AddStatement(_MakeNumberStatement(point.x));
        initParam->AddStatement(_MakeNumberStatement(point.y));
    }
    polygonNew->AddSendParam(move(initParam));

    polygonNew->AddSendParam(make_unique<SendParam>("yourself", true));

    statement->SetSyntaxNode(move(polygonNew));
    return statement;
}

// The addObstacle call
unique_ptr<SingleStatement> _MakeAddPolygonCode(const SCIPolygon &poly)
{
    unique_ptr<SingleStatement> statement = make_unique<SingleStatement>();
    unique_ptr<SendCall> roomAddObstacble = make_unique<SendCall>();

    unique_ptr<LValue> roomNameLValue = make_unique<LValue>();
    roomNameLValue->SetName(c_szRoomName);
    roomAddObstacble->SetLValue(move(roomNameLValue));

    unique_ptr<SendParam> addObstacleParam = make_unique<SendParam>();
    addObstacleParam->SetName(c_szAddObstacleSelector);
    addObstacleParam->AddStatement(_MakeNewPolygonStatement(poly));
    roomAddObstacble->AddSendParam(move(addObstacleParam));
    statement->SetSyntaxNode(move(roomAddObstacble));
    return statement;
}

void PolygonSource::Commit()
{
    if (_dirty)
    {
        // Construct the script om
        Script script(ScriptId(_filePath.c_str()));

        unique_ptr<ProcedureDefinition> proc = make_unique<ProcedureDefinition>();
        proc->SetName(c_szProcedureName);
        proc->SetScript(&script);
        proc->AddSignature(make_unique<FunctionSignature>());
        for (auto &poly : _polygons)
        {
            proc->AddStatement(move(_MakeAddPolygonCode(poly)));
        }
        script.AddProcedure(move(proc));

        std::stringstream ss;
        SourceCodeWriter out(ss, script.Language());
        out.pszNewLine = "\r\n";

        PCSTR pszFilename = PathFindFileName(_filePath.c_str());
        ss << fmt::format("// {0} -- Produced by SCI Companion{1}", pszFilename, out.pszNewLine);
        ss << fmt::format("// This file should only be edited with the SCI Companion polygon editor{0}", out.pszNewLine);

        // Now the meat of the script
        script.OutputSourceCode(out);
        MakeTextFile(ss.str().c_str(), _filePath);

        _dirty = false;
    }
}

void PolygonSource::AppendPolygon()
{
    _dirty = true;
    _polygons.emplace_back(this);
}

SCIPolygon *PolygonSource::GetAt(size_t index)
{
    SCIPolygon *poly = nullptr;
    if (index < _polygons.size())
    {
        poly = &_polygons[index];
    }
    return poly;
}

SCIPolygon *PolygonSource::GetBack()
{
    return &_polygons.back();
}

unique_ptr<PolygonSource> CreatePolygonSource(const string &polyFolder, int picNumber)
{
    string polyFilename = fmt::format("{0}.shp", picNumber);
    string polyFilePath = fmt::format("{0}\\{1}", polyFolder, polyFilename);
    return make_unique<PolygonSource>(polyFilePath);
}