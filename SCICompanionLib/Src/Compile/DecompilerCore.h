#pragma once

#include "CompiledScript.h"
#include "scii.h"
#include "ScriptOM.h"
#include <cstdint>

// fwd decl
namespace sci
{
	class FunctionBase;
	class SyntaxNode;
    class RestStatement;
}


//
// Represents whether an instruction consumes or generates stack or accumulator
// In general, values will be 0 or 1, except for cStackConsume which could be
// much larger (e.g. for send or call instruction)
//
struct Consumption
{
    Consumption()
    {
        cAccConsume = 0;
        cStackConsume = 0;
        cAccGenerate = 0;
        cStackGenerate = 0;
    }

    int cAccConsume;
    int cStackConsume;
    int cAccGenerate;
    int cStackGenerate;
};

Consumption _GetInstructionConsumption(scii &inst);

std::string GetBinaryOperatorForInstruction(Opcode b, LangSyntax lang);
std::string GetUnaryOperatorForInstruction(Opcode b, LangSyntax lang);

enum class VarScope : std::uint8_t
{
	Global = 0x00,
	Local = 0x01,
	Temp = 0x02,
	Param = 0x03
};

class CodeNode;

class DecompileLookups : public ICompiledScriptLookups
{
public:
    DecompileLookups(WORD wScript, ICompiledScriptLookups *pLookups, IObjectFileScriptLookups *pOFLookups, ICompiledScriptSpecificLookups *pScriptThings, ILookupNames *pTextResource, IPrivateSpeciesLookups *pPrivateSpecies) :
		_wScript(wScript), _pLookups(pLookups), _pOFLookups(pOFLookups), _pScriptThings(pScriptThings), _pTextResource(pTextResource), _pPrivateSpecies(pPrivateSpecies), PreferLValue(false)
    {
    }

    // ICompiledScriptLookups
    std::string LookupSelectorName(WORD wIndex);
    std::string LookupKernelName(WORD wIndex);
    std::string LookupClassName(WORD wIndex);
    bool LookupSpeciesPropertyList(WORD wIndex, std::vector<WORD> &props);
    bool LookupSpeciesPropertyListAndValues(WORD wIndex, std::vector<WORD> &props, std::vector<WORD> &values);

    // IObjectFileScriptLookups
    std::string ReverseLookupGlobalVariableName(WORD wIndex);
    std::string ReverseLookupPublicExportName(WORD wScript, WORD wIndex);

    // ILookupPropertyName
    std::string LookupPropertyName(WORD wPropertyIndex);

    std::string LookupScriptThing(WORD wName, ICompiledScriptSpecificLookups::ObjectType &type) const;

    std::string LookupParameterName(WORD wIndex); // 1-based

    WORD GetScriptNumber() const { return _wScript; }
    std::string LookupTextResource(WORD wIndex) const;

    void SetPosition(sci::SyntaxNode *pNode);

    void EndowWithProperties(const ILookupPropertyName *pPropertyNames) { _pPropertyNames = pPropertyNames; }
	void EndowWithFunction(sci::FunctionBase *pFunc);

	// Tracking variable usage
	void TrackVariableUsage(VarScope varScope, WORD wIndex, bool isIndexed);
	const std::map<WORD, bool> &GetLocalUsage() { return _localVarUsage; }
	const std::map<WORD, bool> &GetTempUsage(const std::string &functionMatchName) { return _tempVarUsage[functionMatchName]; }
    void TrackRestStatement(sci::RestStatement *rest, uint16_t paramIndex);
    void ResolveRestStatements();

    const sci::ClassDefinition *DecompileLookups::GetClassContext() const;

	bool PreferLValue;
    std::vector<std::unique_ptr<CodeNode>>::iterator BreakExit;
private:
    WORD _wScript;
    ICompiledScriptLookups *_pLookups;
    IObjectFileScriptLookups *_pOFLookups;
    ICompiledScriptSpecificLookups *_pScriptThings;
    ILookupNames *_pTextResource;
    const ILookupPropertyName *_pPropertyNames;
    IPrivateSpeciesLookups *_pPrivateSpecies;
    sci::FunctionBase *_pFunc;
	std::string _functionTrackingName;
    LineCol _fakePosition;

	// Variable usage
	// Need to use map here, because they have to be in order.
	std::map<WORD, bool> _localVarUsage;
	// Then per function
	std::unordered_map<std::string, std::map<WORD, bool>> _tempVarUsage;
    // Rest statements --- these need to be RestStaetment/index pairs. index 1 means first, index 2 means second.
    std::vector<std::pair<sci::RestStatement*, uint16_t>> _restStatementTrack;
};

void DecompileRaw(sci::FunctionBase &func, DecompileLookups &lookups, const BYTE *pBegin, const BYTE *pEnd, WORD wBaseOffset);

struct VariableRange
{
	WORD index;
	WORD arraySize;
};
void CalculateVariableRanges(const std::map<WORD, bool> &usage, WORD variableCount, std::vector<VariableRange> &varRanges);
void AddLocalVariablesToScript(sci::Script &script, DecompileLookups &lookups, const std::vector<WORD> &localVars);

std::string _GetProcNameFromScriptOffset(WORD wOffset);
sci::ValueType _ScriptObjectTypeToPropertyValueType(ICompiledScriptSpecificLookups::ObjectType type);
bool _ObtainInstructionSequence(code_pos branchInstruction, code_pos beginning, code_pos &beginningOfBranchInstructionSequence);



class PreferLValue
{
public:
    PreferLValue(DecompileLookups &lookups, bool prefer) : _lookups(lookups)
    {
        _preferLValueOld = lookups.PreferLValue;
        lookups.PreferLValue = true;
    }
    ~PreferLValue()
    {
        _lookups.PreferLValue = _preferLValueOld;
    }
private:
    bool _preferLValueOld;
    DecompileLookups &_lookups;
};

std::string _GetPublicProcedureName(WORD wScript, WORD wIndex);
void _MassageProcedureCall(sci::ProcedureCall &proc, DecompileLookups &lookups, code_pos pos);
std::string _GetVariableNameFromCodePos(code_pos pos, DecompileLookups &lookups, VarScope *pVarType = nullptr, WORD *pwIndexOut = nullptr);
bool _IsVOIncremented(Opcode bOpcode);
bool _IsVODecremented(Opcode bOpcode);
bool _MaybeAProperty(const std::string &selector);
bool _IsVOStack(Opcode bOpcode);
bool _IsVOPureStack(Opcode bOpcode);
bool _IsVOIndexed(Opcode bOpcode);
bool _IsVOStoreOperation(Opcode bOpcode);
