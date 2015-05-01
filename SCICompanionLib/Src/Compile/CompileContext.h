#pragma once

#include "scii.h"
#include "SCO.h"
#include "Vocab000.h"
#include "Vocab99x.h"
#include "ScriptOMSmall.h"

class ResourceEntity;
struct TextComponent;

//
// Indicates where code should puts its value (stack or accumulator)
//
enum OutputContext
{
    OC_Stack        = 0,
    OC_Accumulator  = 1,
    OC_Unknown      = 2,
};

enum VariableModifier
{
    VM_None,
    VM_PreIncrement,
    VM_PreDecrement,
};

// fwd decl's
namespace sci
{
	class SyntaxNode;
	class Define;
	class ClassDefinition;
	class Script;
}
class CResourceMap;
class IOutputByteCode;
class CompileTables;
class PrecompiledHeaders;
class ICompileLog;
class SCIClassBrowser;
class ISourceCodePosition;

typedef std::unordered_map<std::string, sci::Define*> defines_map;
typedef std::multimap<std::string, code_pos> ref_multimap;
typedef std::map<std::string, const ISourceCodePosition*> stringcode_map; // Strings and associated syntax nodes (for line #). Important that they have order.
typedef std::pair<code_pos, WORD> call_pair;
typedef std::multimap<std::string, call_pair> ref_and_index_multimap;

class CompileContext : public ICompileLog
{
private:
    SCIClassBrowser &_browser;
    CResourceMap &_resourceMap;
    sci::Script &_script;       // Script being compiled
    sci::Script *_pErrorScript;  // Current script used for error reporting (could be header file)

    typedef std::unordered_map<WORD, CSCOFile> WordSCOMap;
    WordSCOMap _scos;
    std::unordered_map<WORD, std::string> _numberToNameMap;
    std::vector<CSCOObjectClass> _instances;
    WORD _wScriptNumber;
    ICompileLog &_results;
    bool _fErrors;
    CompileTables &_tables;

    // The headers (and/or script) own the defines.
    PrecompiledHeaders &_headers;
    defines_map _localDefines; // defines in *this* script (not the headers)

    SCIVersion _version;

    // e.g. Name is "Feature"
    void _LoadSCO(const std::string &name, LangSyntax langPref, bool fErrorIfNotFound = false);
    // Loads an SCOFile if we don't already have one for this script.
    // Doesn't produce an error if we can't get one.  (Maybe it should?)
    void _LoadSCOIfNone(WORD wScript);

public:
    CompileContext(SCIVersion version, sci::Script &script, PrecompiledHeaders &headers, CompileTables &tables, ICompileLog &results);
    LangSyntax GetLanguage();
    OutputContext GetOutputContext();
    WORD GetScriptNumber();
    WORD AddStringResourceTuple(const std::string &str);
    const std::vector<std::string> &GetResourceStrings();
    WORD LookupSelectorAndAdd(const std::string &str);
    bool LookupSelector(const std::string &str, WORD &wIndex);
    void DefineNewSelector(const std::string &str, WORD &wIndex);
    bool LookupDefine(const std::string &str, WORD &wValue);
    void AddDefine(sci::Define *pDefine);
    SCIVersion GetVersion() { return _version; }
    //
    // wIndex - index of the item.  Valid for all.
    // pwScript - script of the item.  Only valid for ResolvedToken::ExportInstance (wIndex and wScript)
    //
    ResolvedToken LookupToken(const sci::SyntaxNode *pNode, const std::string &str, WORD &wIndex, SpeciesIndex &dataType, WORD *pwScript = nullptr);
    bool LookupSpeciesIndex(const std::string &str, SpeciesIndex &wSpeciesIndex);
    SpeciesIndex LookupTypeSpeciesIndex(const std::string &str, const ISourceCodePosition *pPos);
    bool LookupTypeSpeciesIndex(const std::string &str, SpeciesIndex &wSpeciesIndex);
    std::string SpeciesIndexToDataTypeString(SpeciesIndex wSpeciesIndex);
    SpeciesIndex GetSpeciesSuperClass(SpeciesIndex wSpeciesIndex);
    // The properties returned here, include the 4 default ones.  Even if we can't find the
    // species, we'll return the 4 default ones.
    std::vector<species_property> GetSpeciesProperties(const std::string &speciesNames);
    void SetClassContext(const std::string &className, const std::string &superClass, bool fInstance);
    std::string GetClassName();
    bool IsInstance();
    std::string GetSuperClassName();
    const sci::ClassDefinition *LookupClassDefinition(const std::string &name);
    // Look up a string and map it to a procedure.  Return the script and index of the procedure, where appropraite
    // Script are looked up in this order:
    // ProcedureKernel:     wIndex
    // ProcedureLocal:      classOwner
    // ProcedureMain:       wIndex
    // ProcedureExternal:   wScript, wIndex
    //
    // pSignatures - optional: accepts the list of function signatures for this call.
    ProcedureType LookupProc(const std::string &str, WORD &wScript, WORD &wIndex, std::string &classOwner, std::vector<CSCOFunctionSignature> *pSignatures = nullptr);
    ProcedureType LookupProc(const std::string &str);
    bool _GetSCOObject(SpeciesIndex wSpecies, CSCOObjectClass &scoObject);
    bool LookupSpeciesMethodOrProperty(SpeciesIndex wCallee, WORD wSelector, std::vector<CSCOFunctionSignature> &signatures, SpeciesIndex &propertyType, bool &fMethod);
    void PushOutputContext(OutputContext outputContext);
    void PopOutputContext();
    bool HasMeaning();
    void PushMeaning(bool fMeaning);
    void PopMeaning();
    bool InConditional();
    void PushConditional(bool fConditional);
    void PopConditional();
    bool SupportTypeChecking();
    bool LookupWord(const std::string word, WORD &wWordGroup);
    sci::Script *SetErrorContext(sci::Script *pScript);
    void ReportResult(const CompileResult &result);
    void ReportWarning(const ISourceCodePosition *pPos, const char *pszFormat, ...);
    void ReportError(const ISourceCodePosition *pPos, const char *pszFormat, ...);
    void ReportTypeError(const ISourceCodePosition *pPos, SpeciesIndex w1, SpeciesIndex w2, const char *pszFormat);
    void _ReportThing(bool fError, const ISourceCodePosition *pPos, const char *pszFormat, va_list argList);
    bool HasErrors();

    // Try to figure out which script, if any, this identifier is exported from.
    // This is just used for error reporting.
    std::string ScanForIdentifiersScriptName(const std::string &identifier);
    scicode &code();
    // Tell someone about where we have 1) exports, 2) saids, and 3) internal strings
    void TrackExport(code_pos where);
    void TrackPublicInstance(WORD wOffset);
    bool PreScanLocalProc(const std::string &name, const std::string &ownerClass);
    void TrackLocalProc(const std::string &name, code_pos where);
    void TrackInstanceReference(const std::string &name);
    void TrackStringReference(const std::string &name);
    void TrackStringTokenReference(const std::string &tokenName, WORD wIndex);
    void TrackSaidReference(const std::string &name);
    void TrackAsmLabelReference(const std::string &label);
    void TrackAsmLabelLocation(const std::string &label);
    void ReportLabelName(ISourceCodePosition *position, const std::string &labelName);
    bool DoesLabelExist(const std::string &label);
    const ref_and_index_multimap &GetInstanceReferences();
    const ref_and_index_multimap &GetStringReferences();
    const ref_and_index_multimap &GetStringTokenReferences();
    const ref_and_index_multimap &GetSaidReferences();
    bool TrackMethod(const std::string &name, code_pos where);

    void TrackLocalProcCall(const std::string &name);
    code_pos GetLocalProcPos(const std::string &name);
    void TrackRelocation(WORD wOffset);
    void FixupLocalCalls();
    void FixupAsmLabelBranches();
    void TrackCallOffsetInstruction(WORD wProcIndex);
    void PreScanSaid(const std::string &theSaid, const ISourceCodePosition *pPos);
    WORD GetSaidTempOffset(const std::string &theSaid);
    void SpecifyFinalSaidOffset(const std::string &theString, WORD wFinalOffset);
    void PreScanStringAndUnescape(std::string &theString, const ISourceCodePosition *pPos);
    void AddStringToken(const std::string &token, const std::string &theString);
    WORD GetStringTempOffset(const std::string &theString);
    WORD GetStringTokenTempOffset(const std::string &theString);
    void SpecifyFinalStringOffset(const std::string &theString, WORD wFinalOffset);
    void SpecifyFinalStringTokenOffset(const std::string &theToken, WORD wFinalOffset);
    WORD LookupFinalStringOrSaidOffset(WORD wTempOffset);
    void SpecifyOffsetIndexResolvedValue(WORD wOffsetIndex, WORD wOffset);
    void PushVariableLookupContext(const IVariableLookupContext *pVarContext);
    void PopVariableLookupContext();
    void SetClassPropertyLookupContext(const IVariableLookupContext *pVarContext);
    void SetReturnValueTypes(const std::vector<SpeciesIndex> &types);
    std::vector<SpeciesIndex> GetReturnValueTypes();
    void SetVariableModifier(VariableModifier modifier);
    VariableModifier GetVariableModifier();
    void PushSendCallType(SpeciesIndex wType);
    void PopSendCallType();
    SpeciesIndex GetSendCalleeType();

    // Other public functions
    std::vector<call_pair> &GetCalls();
    std::vector<code_pos> &GetExports();
    std::vector<WORD> &GetPublicInstanceOffsets();
    stringcode_map &GetSaids();
    stringcode_map &GetInCodeStrings();
    std::map<std::string, std::string> &GetDeclaredStrings();
    const std::unordered_map<WORD, WORD> &GetOffsetFixups();
    void SetScriptNumber();
    WORD EnsureSpeciesTableEntry(WORD wIndexInScript);
    void LoadIncludes();
    void AddSCOClass(CSCOObjectClass scoClass, bool fInstance);
    void ReplaceSCOClass(CSCOObjectClass scoClass);
    void AddSCOVariable(CSCOLocalVariable scoVar);
    void AddSCOPublics(CSCOPublicExport scoPublic);
    void AddFakeSCOPublic(CSCOPublicExport scoNotActuallyPublic);
    std::vector<CSCOObjectClass> &GetInstanceSCOs();
    CSCOFile &GetScriptSCO();
    std::string LookupSelectorName(WORD wIndex) const;
    const std::vector<WORD> &GetRelocations();

private:

    // Our code
    scicode _code;

    // The current output context
    std::stack<OutputContext> _oc;
    std::stack<bool> _meaning;
    std::stack<bool> _conditional;

    // Local, script vars
    std::stack<const IVariableLookupContext *> _varContext;
    // class properties
    const IVariableLookupContext *_pClassProperties;
    // Type of the target object of the current send call
    std::stack<SpeciesIndex> _sendCallType;

    std::vector<std::string> _errors;

    // List of code positions of public procedure exports - order is determined implicitly
    std::vector<code_pos> _exports;
    // List of offsets of public instance exports.
    std::vector<WORD> _publicInstances;
    // List of local procedures
    std::map<std::string, code_pos> _localProcs;
    typedef std::map<std::string, CSCOPublicExport> LocalProcsMap;
    LocalProcsMap _localProcs2;
    std::map<std::string, std::string> _localProcClassOwner;
    ref_multimap _localProcCalls;
    // MultiMap of instance/string/said references in code (string -> code_pos)
    ref_and_index_multimap _instanceReferences;
    ref_and_index_multimap _stringReferences;
    ref_and_index_multimap _stringTokenReferences;
    ref_and_index_multimap _saidReferences;
    ref_multimap _labelReferences;
    std::map<std::string, code_pos> _labelLocations;
    std::set<std::string> _labelNames;  // Pre-scan

    const uint16_t ImaginaryStringOffset = 0x8000;

    // These are a set of all the saids/strings in the resource.  We use the index of a particular
    // said/string as a temporary value to write in code (not needed)
    stringcode_map _allSaids;      // Indicies start from 0
    stringcode_map _allInCodeStrings;    // Indicies start from ImaginaryStringOffset
    // This must be an ordered map. Order matters.
    std::map<std::string, std::string> _allScriptStringDecls;

    // Mapping of our imaginary indices to actually offsets within the generated resource
    std::unordered_map<WORD, WORD> _finalSaidOffsets;
    std::unordered_map<WORD, WORD> _finalStringOffsets;

    std::unordered_map<WORD, WORD> _offsetIndexToOffset;

    std::vector<call_pair> _calls;

    // List of resource strings we'll need to write to a text resource
    std::vector<std::string> _resourceStrings;

    std::vector<WORD> _relocations;

    std::string _className;
    std::string _superClassName;
    bool _fInstance;

    VariableModifier _modifier; // Increment or decrement modifier

    std::vector<SpeciesIndex> _allowedReturnValues; // Current allowed return values
};

template<typename T>
class GenericOutputByteCode2 : public std::unary_function<std::unique_ptr<T>, void>
{
public:
	GenericOutputByteCode2(CompileContext &context) : _context(context) {}

	void operator()(const std::unique_ptr<T> &proc)
	{
		_results.push_back(proc->OutputByteCode(_context));
	}

	WORD GetByteCount() const
	{
		WORD wBytes = 0;
		for (const CodeResult &result : _results)
		{
			wBytes += result.GetBytes();
		}
		return wBytes;
	}
	std::vector<SpeciesIndex> GetTypes() const
	{
		std::vector<SpeciesIndex> types;
		for(const CodeResult &result : _results)
		{
			types.push_back(result.GetType());
		}
		return types;
	}
	SpeciesIndex GetLastType() const
	{
		SpeciesIndex type = DataTypeVoid;
		if (!_results.empty())
		{
			type = _results[_results.size() - 1].GetType(); // Return last type.
		}
		return type;
	}
private:
	std::vector<CodeResult> _results;
	CompileContext &_context;
};


class GenericOutputByteCode : public std::unary_function<IOutputByteCode*, void>
{
public:
    GenericOutputByteCode(CompileContext &context) : _context(context) {}
	void operator()(const IOutputByteCode* proc);

	WORD GetByteCount() const
    {
        WORD wBytes = 0;
        for(const CodeResult &result : _results)
        {
            wBytes += result.GetBytes();
        }
        return wBytes;
    }
    std::vector<SpeciesIndex> GetTypes() const
    {
        std::vector<SpeciesIndex> types;
        for(const CodeResult &result : _results)
        {
            types.push_back(result.GetType());
        }
        return types;
    }
    SpeciesIndex GetLastType() const
    {
        SpeciesIndex type = DataTypeVoid;
        if (!_results.empty())
        {
            type = _results[_results.size() - 1].GetType(); // Return last type.
        }
        return type;
    }
private:
    std::vector<CodeResult> _results;
    CompileContext &_context;
};

//
// Some important tables used for compiling.  When doing multiple compiles at once, this
// is kept across individual compiles.
// Both the species and selector tables may be modified.  They will be updated in the game
// when Save is called.
//
class CompileTables
{
public:
    bool Load(SCIVersion version);
    void Save();
    const Vocab000 *Vocab() { return _pVocab; }
    const KernelTable &Kernels() { return _kernels; }
    SpeciesTable &Species() { return _species; }
    SelectorTable &Selectors() { return _selectors; }
private:
    const Vocab000 *_pVocab;
    KernelTable _kernels;
    SpeciesTable _species;
    SelectorTable _selectors;
};

//
// Encapsulates the result of a single compilation, including:
// 1) the generate script resource, as a vector of BYTEs
// 2) the sco file
// 3) the script number
// 4) a reference to an ICompileLog, for logging errors and warnings.
//
class CompileResults
{
public:
    CompileResults(ICompileLog &log);
    std::vector<BYTE> &GetScriptResource() { return _outputScr; }
    std::vector<BYTE> &GetHeapResource() { return _outputHep; }
    CSCOFile &GetSCO() { return _sco; }
    WORD GetScriptNumber() const { return _wScriptNumber; }
    void SetScriptNumber(WORD wNum) { _wScriptNumber = wNum; }
    ICompileLog &GetLog() { return _log; }
    ResourceEntity &GetTextResource() { return *_text; }
    TextComponent &GetTextComponent();
    
private:
    std::vector<BYTE> _outputScr;
    std::vector<BYTE> _outputHep;
    WORD _wScriptNumber;
    CSCOFile _sco;
    ICompileLog &_log;
    std::unique_ptr<ResourceEntity> _text;
};


class CompileLog : public ICompileLog
{
public:
    CompileLog() { _cErrors = 0; _cWarnings = 0; }
    void ReportResult(const CompileResult &result)
    {
        _compileResults.push_back(result);
    }
    void SummarizeAndReportErrors();
    void Clear() { _compileResults.clear(); }
    std::vector<CompileResult> &Results() { return _compileResults; }
    void CalculateErrors();

private:
    std::vector<CompileResult> _compileResults;
    int _cErrors;
    int _cWarnings;
};


//
// Maintains a set of pre-compiled headers across compilations
// For each script that is comiled, call Update.  That will ensure the defines
// returned by GetDefines() are appropriate.
//
class PrecompiledHeaders
{
public:
    PrecompiledHeaders(CResourceMap &resourceMap);
    ~PrecompiledHeaders();
    // Call this each time you compile a new script
    void Update(CompileContext &context, sci::Script &script);

    bool LookupDefine(const std::string &str, WORD &wValue);
private:
    typedef std::unordered_map<std::string, sci::Define*> defines_map;
	typedef std::unordered_map<std::string, sci::Script* > header_map;

    // Filename (not full path) which maps a header to its Script object.
    header_map _allHeaders; 

    // A set of the names of all the last script's header includes.
    std::set<std::string> _curHeaderList;   

    // The define hashtable which corresponds to the _curHeaderList
    // Someone else has ownership of these defines.
    defines_map _defines;

    // Whether or not the _defines table is valid
    bool _fValid;
    CResourceMap &_resourceMap;
};  

//
// The be-all end-all function for compiling a script.
// Returns true if there were no errors.
//
bool GenerateScriptResource_SCI0(sci::Script &script, PrecompiledHeaders &headers, CompileTables &tables, CompileResults &results);

void ErrorHelper(CompileContext &context, const ISourceCodePosition *pPos, const std::string &text, const std::string &identifier);

bool NewCompileScript(CompileLog &log, CompileTables &tables, PrecompiledHeaders &headers, ScriptId &script);
