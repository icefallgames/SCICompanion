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
#include <stack>
#include <numeric>
#include "SCO.h"
#include "ScriptOM.h"
#include "ScriptOMAll.h"
#include "CompileInterfaces.h"
#include "CompileContext.h"
#include "AppState.h"
#include "SyntaxParser.h"
#include "ClassBrowser.h"
#include <unordered_map>
#include "Text.h"
#include "ResourceEntity.h"
#include "CCrystalTextBuffer.h"
#include "CrystalScriptStream.h"

using namespace sci;
using namespace std;

static const WORD c_TempIndex = 0x1957; // A sentinel marker for an index we don't yet know.

WORD read_word(vector<BYTE> &output, vector<BYTE>::iterator index)
{
    WORD w = *index;
    index++;
    w += ((*index) << 8);
    return w;
}

WORD read_word(vector<BYTE> &output, WORD wOffset)
{
    assert((WORD)output.size() > wOffset);
    return output[wOffset] + (((WORD)output[wOffset + 1]) << 8);
}

//
// Special properties
//
struct IndexAndType
{
    WORD index;
    SpeciesIndex type;
};
const key_value_pair<std::string, IndexAndType> c_specialSelectors[] =
{
    { "objectFunctionArea", { 0xFFFC, DataTypeVoid } },
    { "objectInfo", { 0x0004, DataTypeUInt } },
    { "objectLocal", { 0xFFFA, DataTypeVoid } },
    { "objectName", { 0x0006, DataTypeString } },
    { "objectSize", { 0xFFF6, DataTypeInt } },
    { "objectSpecies", { 0x0000, DataTypeInt} },
    { "objectSuperclass", { 0x0002, DataTypeInt } },
    { "objectTotalProperties", { 0xFFFE, DataTypeInt } },
    { "objectType", { 0xFFF4, DataTypeInt} },
};
bool IsSpecialSelector(const string &str, WORD &wOffset, SpeciesIndex &type)
{
    IndexAndType iat;
    bool fRet = FindKeyValue(c_specialSelectors, ARRAYSIZE(c_specialSelectors), str, iat);
    wOffset = iat.index;
    type = iat.type;
    return fRet;
}

bool CompileTables::Load(SCIVersion version)
{
    // REVIEW: this could be deleted while we're compiling.
    _pVocab = appState->GetResourceMap().GetVocab000();
    const GameFolderHelper &helper = appState->GetResourceMap().Helper();
    return _kernels.Load(helper) && _species.Load(helper) && _selectors.Load(helper);
}

void CompileTables::Save()
{
    _species.Save();
    _selectors.Save();
}

CompileResults::CompileResults(ICompileLog &log) : _log(log), _text(CreateDefaultTextResource(appState->GetVersion())) {}

TextComponent &CompileResults::GetTextComponent()
{
    return _text->GetComponent<TextComponent>();
}

// e.g. Name is "Feature"
void CompileContext::_LoadSCO(const std::string &name, bool fErrorIfNotFound)
{
    assert(!name.empty());
    string scoFileName = appState->GetResourceMap().Helper().GetScriptObjectFileName(name);
    HANDLE hFile = CreateFile(scoFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        scoFileName = appState->GetResourceMap().Helper().GetScriptObjectFileName(name);
        hFile = CreateFile(scoFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    }
    if (hFile != INVALID_HANDLE_VALUE)
    {
        sci::streamOwner streamOwner(hFile);
        CSCOFile scoFile;
        if (scoFile.Load(streamOwner.getReader()))
        {
            _scos[scoFile.GetScriptNumber()] = scoFile;
        }
        else
        {
            ReportError(_pErrorScript, "'%s' is corrupt.", scoFileName.c_str());
        }
        CloseHandle(hFile);
    }
    else if (fErrorIfNotFound)
    {
        char szError[200];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0, szError, ARRAYSIZE(szError), nullptr);
        ReportError(_pErrorScript, "Unable to open '%s': %s", scoFileName.c_str(), szError);
    }
}

// Loads an SCOFile if we don't already have one for this script.
// Doesn't produce an error if we can't get one.  (Maybe it should?)
void CompileContext::_LoadSCOIfNone(WORD wScript)
{
    WordSCOMap::iterator iter = _scos.find(wScript);
    if ((iter == _scos.end()) || (iter->second.IsEmpty()))
    {
        assert((wScript != _wScriptNumber) && (_wScriptNumber != InvalidResourceNumber)); // The "this" script should always be found.
        std::string scriptName = _numberToNameMap[wScript];
        if (scriptName.empty())
        {
            std::stringstream ss;
            ss << "Unable to find name for script #" << wScript;
            ReportResult(CompileResult(ss.str()));
        }
        else
        {
            _LoadSCO(scriptName);
        }
    }
}

CompileContext::CompileContext(SCIVersion version, Script &script, PrecompiledHeaders &headers, CompileTables &tables, ICompileLog &results) :
        _browser(*appState->GetResourceMap().GetClassBrowser()),
        _resourceMap(appState->GetResourceMap()),
        _results(results),
        _tables(tables),
        _headers(headers),
        _script(script),
        _version(version),
        _code(_version)
{
    _pErrorScript = &_script;
    _modifier = VM_None;
    _fErrors = false;
    _pClassProperties = nullptr;
    _wScriptNumber = InvalidResourceNumber;

    //
    // Load all the sco files for the "use" statements.
    //
    const vector<string> &uses = _script.GetUses();
	for (const string &use : uses)
    {
        _LoadSCO(use, true);
    }

    // Get a map of script numbers to script names.  We use this when looking up a species index in the
    // global class table, and then looking in the script for its name.  This is for type checking, and
    // is only needed for the Cpp syntax.
    appState->GetResourceMap().GetNumberToNameMap(_numberToNameMap);

    // We'll always have an accumulator stack context at the top, so just add it now
    PushOutputContext(OC_Accumulator);
}

// CompileContext
LangSyntax CompileContext::GetLanguage()
{
    return _pErrorScript->Language();
}
OutputContext CompileContext::GetOutputContext()
{
    return _oc.top();
}
WORD CompileContext::GetScriptNumber()
{
    assert(_wScriptNumber != 0xffff);
    return _wScriptNumber;
}
WORD CompileContext::AddStringResourceTuple(const string &str)
{
    _resourceStrings.push_back(str);
    return (WORD)(_resourceStrings.size() - 1);
}
const vector<string> &CompileContext::GetResourceStrings()
{
    return _resourceStrings;
}
WORD CompileContext::LookupSelectorAndAdd(const string &str)
{
    WORD w;
    if (!_tables.Selectors().ReverseLookup(str, w))
    {
        // It doesn't exist ... add it.
        // We can just keep on adding to the selectors list (lots of room)
        w = _tables.Selectors().Add(str);
    }
    return w;
}
bool CompileContext::LookupSelector(const string &str, WORD &wIndex)
{
    return _tables.Selectors().ReverseLookup(str, wIndex);
}
void CompileContext::DefineNewSelector(const std::string &str, WORD &wIndex)
{
    wIndex = _tables.Selectors().Add(str);
}
bool CompileContext::LookupDefine(const std::string &str, WORD &wValue)
{
    bool fRet;
    // One special case
    if (str == "scriptNumber")
    {
        fRet = true;
        wValue = _wScriptNumber;
        assert(_wScriptNumber != 0xffff);
    }
    else
    {
        std::unordered_map<std::string, DefinePtr>::const_iterator nodeIt = _localDefines.find(str);
        fRet = (nodeIt != _localDefines.end());
        if (fRet)
        {
            wValue = nodeIt->second->GetValue();
        }
        if (!fRet)
        {
            // Try the headers.
            fRet = _headers.LookupDefine(str, wValue);
        }
    }
    return fRet;
}
void CompileContext::AddDefine(Define *pDefine)
{
    WORD wDummy;
    bool fDupe = false;
    const string &defineLabel = pDefine->GetLabel();
    if (_localDefines.lower_bound(defineLabel) != _localDefines.end())
    {
        fDupe = true;
    }
    else if (_headers.LookupDefine(defineLabel, wDummy))
    {
        fDupe = true;
    }
    if (fDupe)
    {
        ReportWarning(pDefine, "Duplicate defines: '%s'", defineLabel.c_str());
    }
    _localDefines[defineLabel] = pDefine;
}

//
// wIndex - index of the item.  Valid for all.
// pwScript - script of the item.  Only valid for ResolvedToken::ExportInstance (wIndex and wScript)
//
ResolvedToken CompileContext::LookupToken(const SyntaxNode *pNodeForError, const string &str, WORD &wIndex, SpeciesIndex &dataType, WORD *pwScript)
{
    // Figure out what this thing is.
    // This is the order in which we should proceed:
    // 1) var/temp/local/string/global
    // 2) class name/instance name
    // 3) class properties
    // e.g. the earlier ones have precedence over the others.
    ResolvedToken tokenType = ResolvedToken::Unknown;
    PropertyValue value;
    if (str == "self")
    {
        tokenType = ResolvedToken::Self; // No index needed
        // Not an error if _className is empty. Can be used in a procedure.
        //if (_className.empty())
        //{
        //    ReportError(pNode, "self can only be used within a class or instance method.");
        //}
        dataType = LookupTypeSpeciesIndex(_fInstance ? _superClassName : _className, pNodeForError);
    }
    else if (str == "super")
    {
        tokenType = ResolvedToken::Class; // We'll return success, and generate any errors right here.
        // This is only valid from within a class.
        if (_className.empty())
        {
            if (pNodeForError)
            {
                ReportError(pNodeForError, "super can only be used within a class or instance method.");
            }
        }
        else if (_superClassName.empty())
        {
            if (pNodeForError)
            {
                // Possible to have no super.
                ReportError(pNodeForError, "super can only be used within a class that has a super class.");
            }
        }
        else
        {
            // Look up the class name
            // Don't check the result - if this fails, it means our class is invalid, and 
            // there will already be an error for that.
            LookupSpeciesIndex(_superClassName, dataType);
            wIndex = dataType.Type();
        }
    }
    else
    {
        // Go in reverse... more recent contexts are searched first.
        auto index = _varContext._Get_container().rbegin();
        while ((tokenType == ResolvedToken::Unknown) && (index != _varContext._Get_container().rend()))
        {
            tokenType = (*index)->LookupVariableName(*this, str, wIndex, dataType);
            index++;   
        }

        if (tokenType == ResolvedToken::Unknown)
        {
            // ResolvedToken::GlobalVariable
            // Keep going - check for global vars (script 0)
            if (_scos[0].GetVariableIndex(str, wIndex)) // May not have a main - that's ok, this will create a dummy empty one.
            {
                dataType = DataTypeAny;
                tokenType = ResolvedToken::GlobalVariable;
            }
        }

        if ((tokenType == ResolvedToken::Unknown) && _pClassProperties)
        {
            tokenType = _pClassProperties->LookupVariableName(*this, str, wIndex, dataType);
        }

        if (tokenType == ResolvedToken::Unknown)
        {
            // ResolvedToken::Class
            if (LookupSpeciesIndex(str, dataType))
            {
                // The data type of a class is the class itself.
                wIndex = dataType.Type();
                tokenType = ResolvedToken::Class;
            }
        }

        // ResolvedToken::Instance
        if (tokenType == ResolvedToken::Unknown)
        {
			for (auto &classDef : _script.GetClasses())
            {
				if (classDef->IsInstance() && (str == classDef->GetName()))
                {
                    // Return a temporary index.
                    wIndex = c_TempIndex;
                    // Then, the caller should call TrackInstanceReference(name, code_pos), and we'll
                    // track his code_pos, and fix it up later when we write the code!
                    dataType = LookupTypeSpeciesIndex(classDef->GetSuperClass(), pNodeForError); // Data type of an instance is its super class. REVIEW: we could change this to be the instance itself.
                    tokenType = ResolvedToken::Instance;
                    break;
                }
            }
        }

        if (tokenType == ResolvedToken::Unknown)
        {
            // ResolvedToken::ClassProperty
            // Then, we need to find the "property index" (not selector index) of the current obect.
            // First, we need to look up the selector# of this property.
            // Then we need a class context.  This context should provide us with a way to lookup and see
            // if the class supports that property.  To do so, we need to get the species # of the class.
            // Then, we need to map that species# to a script/index pair.  From that, we can look up the
            // script in our _scos vector, and ask if class of index # supports that selector, and if so,
            // what is its index?  whew!!!
            // TODO: for now, just fake it if we can get a selector
            if (IsSpecialSelector(str, wIndex, dataType))
            {
                tokenType = ResolvedToken::ClassProperty;
            }
            else
            {
                // Other "normal" properties will be handled by the ClassDefinition's
                // implementation of IVariableLookupContext.
            }
        }
    }
    return tokenType;
}
bool CompileContext::LookupSpeciesIndex(const string &str, SpeciesIndex &wSpeciesIndex)
{
    // First, we'll need to check the scoFiles for this class
    // Then, we'll need to use the global class table to find the "class #"
	for (WordSCOMap::value_type &p : _scos)
    {
        CSCOFile &scoFile = p.second;
        WORD wClassIndexInScript;
        if (scoFile.GetClassIndex(str, wClassIndexInScript))
        {
            if (_tables.Species().GetSpeciesIndex(scoFile.GetScriptNumber(), wClassIndexInScript, wSpeciesIndex))
            {
                return true;
            }
        }
    }
    return false;
}
bool CompileContext::IsDefaultSelector(uint16_t value)
{
    return _tables.Selectors().IsDefaultSelector(value);
}
bool CompileContext::LookupTypeSpeciesIndex(const string &str, SpeciesIndex &wSpeciesIndex)
{
    // This is just like LookupSpeciesIndex, except that we also map standard types.
    bool fRet = GetBuiltInDataTypeSpecies(str, wSpeciesIndex);
    if (!fRet)
    {
        fRet = LookupSpeciesIndex(str, wSpeciesIndex);
    }
    return fRet;
}
SpeciesIndex CompileContext::LookupTypeSpeciesIndex(const string &str, const ISourceCodePosition *pNodeForError)
{
    // Call this one when it's an error if the type doesn't match something.
    SpeciesIndex wType = DataTypeAny;
    if (!str.empty() && !LookupTypeSpeciesIndex(str, wType))
    {
        if (pNodeForError)
        {
            ReportError(pNodeForError, "Unknown type '%s'", str.c_str());
        }
    }
    return wType;
}
std::string CompileContext::SpeciesIndexToDataTypeString(SpeciesIndex wSpeciesIndex)
{
    std::string dataType = GetBuiltInDataTypeString(wSpeciesIndex);
    if (dataType.empty())
    {
        // Nope, not one of the built-in types.
        WORD wScript, wClassIndexInScript;
        if (_tables.Species().GetSpeciesLocation(wSpeciesIndex, wScript, wClassIndexInScript))
        {
            _LoadSCOIfNone(wScript);
            CSCOFile &scoFile = _scos[wScript];
            // Find the name.
            dataType = scoFile.GetClassName(wClassIndexInScript);
        }
    }
 
    if (dataType.empty())
    {
        dataType = "Unknown-type";
    }
    return dataType;
}
SpeciesIndex CompileContext::GetSpeciesSuperClass(SpeciesIndex wSpeciesIndex)
{
    SpeciesIndex ret = DataTypeNone;
    CSCOObjectClass object;
    if (_GetSCOObject(wSpeciesIndex, object))
    {
        ret = object.GetSuperClass();
    }
    return ret;
}


const char* c_rgCommonPropsSCI0[] = { "species", "superClass", "-info-", "name" };
const WORD c_rgCommonPropsTypesSCI0[] = { DataTypeAny, DataTypeAny, DataTypeUInt, DataTypeString };

const char* c_rgCommonPropsSCI1[] = { "-objID-", "-size-","-propDict-","-methDict-","-classScript-", "-script-", "-super-", "-info-", "name" };
const WORD c_rgCommonPropsTypesSCI1[] = { DataTypeAny, DataTypeAny, DataTypeAny, DataTypeAny, DataTypeAny, DataTypeAny, DataTypeAny, DataTypeUInt, DataTypeString };

unordered_set<string> GetDefaultSelectorNames(SCIVersion version)
{
    if (version.SeparateHeapResources)
    {
        return unordered_set<string>(c_rgCommonPropsSCI1, c_rgCommonPropsSCI1 + ARRAYSIZE(c_rgCommonPropsSCI1));
    }
    else
    {
        return unordered_set<string>(c_rgCommonPropsSCI0, c_rgCommonPropsSCI0 + ARRAYSIZE(c_rgCommonPropsSCI0));
    }
}

// The properties returned here, include the 4 default ones.  Even if we can't find the
// species, we'll return the 4 default ones.
vector<species_property> CompileContext::GetSpeciesProperties(const string &speciesNames)
{
    vector<species_property> propertiesRet;
    if (!speciesNames.empty())
    {
        // Find the scofile that contains this species.
		for (WordSCOMap::value_type &p : _scos)
        {
            CSCOFile &scoFile = p.second;
            const CSCOObjectClass *pClass = nullptr;
            if (scoFile.GetClass(speciesNames, &pClass))
            {
                // We have the class.
                const vector<CSCOObjectProperty> &properties = pClass->GetProperties();
				for (auto &theProp : properties)
                {
                    species_property specProp = { theProp.GetSelector(), theProp.GetValue(), DataTypeAny, false };
                    propertiesRet.push_back(specProp);
                }
                break;
            }
        }
    }
    if (propertiesRet.empty())
    {
        const char** commonProps = this->_version.SeparateHeapResources ? c_rgCommonPropsSCI1 : c_rgCommonPropsSCI0;
        const uint16_t* commonPropsTypes = this->_version.SeparateHeapResources ? c_rgCommonPropsTypesSCI1 : c_rgCommonPropsTypesSCI0;
        int commonPropsCount = this->_version.SeparateHeapResources ? ARRAYSIZE(c_rgCommonPropsSCI1) : ARRAYSIZE(c_rgCommonPropsSCI0);
        // Either we have inherited from nothing (which is fine), or we couldn't find the species.
        // In any case, return the default four properties.
        // REVIEW: can we better describe species and superClass?  They are sort of Obj's
        for (int i = 0; i < commonPropsCount; i++)
        {
            WORD wSelector = 0;
            _tables.Selectors().ReverseLookup(commonProps[i], wSelector);
            species_property commonProp = { wSelector, 0, commonPropsTypes[i], false };
            propertiesRet.push_back(commonProp);
        }
    }
    return propertiesRet;
}
void CompileContext::SetClassContext(const string &className, const string &superClass, bool fInstance)
{
    _superClassName = superClass;
    _className = className;
    _fInstance = fInstance;
}
string CompileContext::GetClassName()
{
    return _className;
}
bool CompileContext::IsInstance()
{
    return _fInstance;
}
string CompileContext::GetSuperClassName()
{
    return _superClassName;
}
const ClassDefinition *CompileContext::LookupClassDefinition(const std::string &name)
{
	return match_name(_script.GetClasses(), name);
}

// Look up a string and map it to a procedure.  Return the script and index of the procedure, where appropraite
// Script are looked up in this order:
// ProcedureKernel:     wIndex
// ProcedureLocal:      classOwner
// ProcedureMain:       wIndex
// ProcedureExternal:   wScript, wIndex
//
// pSignatures - optional: accepts the list of function signatures for this call.
ProcedureType CompileContext::LookupProc(const string &str, WORD &wScript, WORD &wIndex, string &classOwner)
{
    ProcedureType type = ProcedureUnknown;
    // First try kernel.
    if (_tables.Kernels().ReverseLookup(str, wIndex))
    {
        type = ProcedureKernel;
    }
    else
    {
        // Then try local procedures first.
        if (_localProcs.find(str) != _localProcs.end())
        {
            type = ProcedureLocal;
            // Also return the class owner for this procedure.
            classOwner = _localProcClassOwner[str];
            wIndex = c_TempIndex; // A temporary index
        }
        else
        {
            // Then main
            if (_scos[0].GetExportIndex(str, wIndex))
            {
                // Found a proc in main.
                type = ProcedureMain;
                wScript = 0x3419; // Sentinel, to ensure no one uses the wScript value.
            }
            else
            {
                // Then other sco files.
                for (WordSCOMap::value_type &p : _scos)
                {
                    CSCOFile &scoFile = p.second;
                    if (scoFile.GetExportIndex(str, wIndex))
                    {
                        // Found it.
                        wScript = scoFile.GetScriptNumber();
                        type = ProcedureExternal;
                    }
                }
            }
        }
    }
    return type;
}
ProcedureType CompileContext::LookupProc(const std::string &str)
{
    // Version where caller doesn't care about the information... just wants to know if its a proc
    WORD wScript, wIndex;
    string classOwner;
    return LookupProc(str, wScript, wIndex, classOwner);
}
bool CompileContext::_GetSCOObject(SpeciesIndex wSpecies, CSCOObjectClass &scoObject)
{
    bool fRet = false;
    if (!IsPODType(wSpecies))
    {
        WORD wScript, wClassIndexInScript;
        if (_tables.Species().GetSpeciesLocation(wSpecies, wScript, wClassIndexInScript))
        {
            _LoadSCOIfNone(wScript);
            CSCOFile &scoFile = _scos[wScript];
            scoObject = scoFile.GetObjectBySpecies(wSpecies);
            fRet = true;
        }
    }
    return fRet;
}
bool CompileContext::LookupSpeciesMethodOrProperty(SpeciesIndex wCallee, WORD wSelector, SpeciesIndex &propertyType, bool &fMethod)
{
    bool fRet = false;
    fMethod = false;
    // Find the CSCOObject for wCallee.
    CSCOObjectClass object;
    while (!fRet && _GetSCOObject(wCallee, object))
    {
        for(uint16_t method : object.GetMethods())
        {
            fRet = (method == wSelector);
            if (fRet)
            {
                fMethod = true;
                break;
            }
        }
        if (!fRet)
        {
            for(const CSCOObjectProperty &property : object.GetProperties())
            {
                fRet = (property.GetSelector() == wSelector);
                if (fRet)
                {
                    propertyType = DataTypeAny;
                    break;
                }
            }
        }
        wCallee = object.GetSuperClass(); // Try the super class
    }
    return fRet;
}
void CompileContext::PushOutputContext(OutputContext outputContext)
{
    _oc.push(outputContext);
}
void CompileContext::PopOutputContext()
{
    _oc.pop();
}
bool CompileContext::HasMeaning() { return _meaning.top(); }
void CompileContext::PushMeaning(bool fMeaning) { _meaning.push(fMeaning); }
void CompileContext::PopMeaning() { _meaning.pop(); }
bool CompileContext::InConditional() { return _conditional.top(); }
void CompileContext::PushConditional(bool fConditional) { _conditional.push(fConditional); }
void CompileContext::PopConditional() { _conditional.pop(); }
void CompileContext::PushRestFrame() { _restBanned.push(false); }
void CompileContext::NotifyProcOrSend()
{
    if (!_restBanned.empty())
    {
        _restBanned.top() = true;
    }
}
void CompileContext::ErrorIfRestBannedHere(const ISourceCodePosition *pos)
{
    if (!_restBanned.empty() && _restBanned.top())
    {
        ReportError(pos, "&rest cannot be used if the send target itself contains nested procedure calls or sends. Assign the result of the procedure call or send to a temporary variable and use that instead.");
    }
}
void CompileContext::PopRestFrame() { _restBanned.pop(); }
bool CompileContext::SupportTypeChecking()
{
    // Used to be used for experimental cpp syntax.
    return false;
}
bool CompileContext::LookupWord(const string word, WORD &wWordGroup)
{
    Vocab000::WordGroup group;
    bool fRet = _tables.Vocab()->LookupWord(word.c_str(), group);
    wWordGroup = (WORD)group;
    return fRet;
}
sci::Script *CompileContext::SetErrorContext(sci::Script *pScript)
{
    sci::Script *pOld = _pErrorScript;
    _pErrorScript = pScript;
    return pOld;
}
void CompileContext::ReportResult(const CompileResult &result)
{
    _results.ReportResult(result);
}
void CompileContext::ReportWarning(const ISourceCodePosition *pPos, const char *pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    _ReportThing(false, pPos, pszFormat, argList);
    va_end(argList);
}
void CompileContext::ReportError(const ISourceCodePosition *pPos, const char *pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    _ReportThing(true, pPos, pszFormat, argList);
    va_end(argList);
    _fErrors = true;
}
void CompileContext::ReportTypeError(const ISourceCodePosition *pPos, SpeciesIndex w1, SpeciesIndex w2, const char *pszFormat)
{
    std::string s1 = SpeciesIndexToDataTypeString(w1);
    std::string s2 = SpeciesIndexToDataTypeString(w2);
    ReportError(pPos, pszFormat, s1.c_str(), s2.c_str());
}
void CompileContext::_ReportThing(bool fError, const ISourceCodePosition *pPos, const char *pszFormat, va_list argList)
{
    char szMessage[300];
    StringCchVPrintf(szMessage, ARRAYSIZE(szMessage), pszFormat, argList);
    assert(pPos);
    char sz[300];
    // Add one to line number, since they are reported from a 0-base (parser), but displayed from a 1-base (script editor)
    int line = pPos->GetLineNumber() + 1;
    StringCchPrintf(sz, ARRAYSIZE(sz), "  [%s]: %s  Line: %d, col: %d", fError ? "Error" : "Warning", szMessage, line, pPos->GetColumnNumber());
    _results.ReportResult(CompileResult(sz, ScriptId(_pErrorScript->GetPath().c_str()), line, pPos->GetColumnNumber(), fError ? CompileResult::CRT_Error : CompileResult::CRT_Warning));
}
bool CompileContext::HasErrors() { return _fErrors; }

// Try to figure out which script, if any, this identifier is exported from.
// This is just used for error reporting.
string CompileContext::ScanForIdentifiersScriptName(const std::string &identifier)
{
    string strRet;
    const VariableDeclVector *globals = _browser.GetMainGlobals();
    if (globals)
    {
        if (matches_name(globals->begin(), globals->end(), identifier))
        {
            strRet = "main";
        }
    }
    if (strRet.empty())
    {
        const Script *pContainerScript = nullptr;
        // Try exported procedures.
        const RawProcedureVector &procs = _browser.GetPublicProcedures();
        auto procIt = match_name(procs.begin(), procs.end(), identifier);
        if (procIt != procs.end())
        {
            pContainerScript = (*procIt)->GetOwnerScript();
        }
        if (pContainerScript == nullptr)
        {
            // Try classes.
			const RawClassVector &classes = _browser.GetAllClasses();
			auto classIt = match_name(classes.begin(), classes.end(), identifier);
            if (classIt != classes.end())
            {
                pContainerScript = (*classIt)->GetOwnerScript();
            }
        }
        if (pContainerScript)
        {
            strRet = pContainerScript->GetName();
            // Trim the ".sc" off.
            auto it = strRet.find('.');
            if (it != string::npos)
            {
                strRet.erase(it);
            }
        }
    }
    return strRet;
}

scicode &CompileContext::code() { return _code; }

// Tell someone about where we have 1) exports, 2) saids, and 3) internal strings
void CompileContext::TrackExport(code_pos where)
{
    _exports.push_back(where);
}
void CompileContext::TrackPublicInstance(WORD wOffset)
{
    _publicInstances.push_back(wOffset);
}
bool CompileContext::PreScanLocalProc(const string &name, const std::string &ownerClass)
{
    // Inform the context about a local procedure name.
    if (_localProcs.find(name) == _localProcs.end())
    {
        _localProcs[name] = code().get_undetermined();
        _localProcClassOwner[name] = ownerClass;
        return true;
    }
    else
    {
        return false; // Already exists.
    }
}
void CompileContext::TrackLocalProc(const string &name, code_pos where)
{
    // Tell the context the code position of the implemented procedure.
    assert(_localProcs.find(name) != _localProcs.end());
    _localProcs[name] = where;
}
void CompileContext::TrackInstanceReference(const string &name)
{
    _instanceReferences.insert(ref_and_index_multimap::value_type(name, call_pair(code().get_cur_pos(), 0)));
}
void CompileContext::TrackStringReference(const string &name)
{
    _stringReferences.insert(ref_and_index_multimap::value_type(name, call_pair(code().get_cur_pos(), 0)));
}
void CompileContext::TrackStringTokenReference(const string &tokenName, WORD wIndex)
{
    _stringTokenReferences.insert(ref_and_index_multimap::value_type(tokenName, call_pair(code().get_cur_pos(), wIndex)));
}
void CompileContext::TrackSaidReference(const string &name)
{
    _saidReferences.insert(ref_and_index_multimap::value_type(name, call_pair(code().get_cur_pos(), 0)));
}
void CompileContext::FixupAsmLabelBranches()
{
    // Tell the branch instructions where they are going.
    for (auto labelRef : _labelReferences)
    {
        code_pos labelLocation = _labelLocations[labelRef.first];
        _code.set_call_target(labelRef.second, labelLocation);
    }
}
void CompileContext::TrackAsmLabelLocation(const std::string &label)
{
    _labelLocations[label] = code().get_cur_pos();
}
void CompileContext::TrackAsmLabelReference(const std::string &label)
{
    _labelReferences.insert(ref_multimap::value_type(label, code().get_cur_pos()));
}
void CompileContext::ReportLabelName(ISourceCodePosition *position, const std::string &labelName)
{
    if (_labelNames.find(labelName) == _labelNames.end())
    {
        _labelNames.insert(labelName);
    }
    else
    {
        this->ReportError(position, "Duplicate label '%s'", labelName.c_str());
    }
}
bool CompileContext::DoesLabelExist(const std::string &label)
{
    return _labelNames.find(label) != _labelNames.end();
}
const ref_and_index_multimap &CompileContext::GetInstanceReferences() { return _instanceReferences; }
const ref_and_index_multimap &CompileContext::GetStringReferences() { return _stringReferences; }
const ref_and_index_multimap &CompileContext::GetStringTokenReferences() { return _stringTokenReferences; }
const ref_and_index_multimap &CompileContext::GetSaidReferences() { return _saidReferences; }
bool CompileContext::TrackMethod(const string &name, code_pos where)
{
    if (_localProcs.find(name) == _localProcs.end())
    {
        _localProcs[name] = where;
        return true;
    }
    else
    {
        return false; // Already exists.
    }
}
void CompileContext::TrackLocalProcCall(const string &name)
{
    assert(_localProcs.find(name) != _localProcs.end());
    _localProcCalls.insert(ref_multimap::value_type(name, code().get_cur_pos()));
}
code_pos CompileContext::GetLocalProcPos(const string &name)
{
    assert(_localProcs.find(name) != _localProcs.end());
    return _localProcs[name];
}
void CompileContext::TrackRelocation(WORD wOffset)
{
    // This is used for instance names and saids/strings that are property values.
    // Their addresses are put into the relocation table for the script.
    _relocations.push_back(wOffset);
}
void CompileContext::FixupLocalCalls()
{
    // Tell all the calls where they're calling to.
    multimap<string, code_pos>::const_iterator theCall = _localProcCalls.begin();
    while (theCall != _localProcCalls.end())
    {
        assert(_localProcs.find((*theCall).first) != _localProcs.end());
        code_pos callTarget = _localProcs[(*theCall).first];
        _code.set_call_target((*theCall).second, callTarget);
        theCall++;
    }
}
void CompileContext::TrackCallOffsetInstruction(WORD wProcIndex)
{
    _calls.push_back(call_pair(_code.get_cur_pos(), wProcIndex));
}
void CompileContext::PreScanSaid(const string &theSaid, const ISourceCodePosition *pPos)
{
    _allSaids[theSaid] = pPos;
}
WORD CompileContext::GetSaidTempOffset(const string &theSaid)
{
    stringcode_map::iterator saidIt = _allSaids.find(theSaid);
    assert(saidIt != _allSaids.end());
    size_t index = distance(_allSaids.begin(), saidIt);
    return (WORD)index;
}
void CompileContext::SpecifyFinalSaidOffset(const string &theString, WORD wFinalOffset)
{
    WORD wTempOffset = GetSaidTempOffset(theString);
    _finalSaidOffsets[wTempOffset] = wFinalOffset;
}
void CompileContext::PreScanString(string &theString, const ISourceCodePosition *pPos)
{
    // Strings are unescaped during parsing.
    _allInCodeStrings[theString] = pPos;
}
void CompileContext::AddStringToken(const string &token, const string &theString)
{
    // Note that these strings may contain invalid chars and nulls and stuff
    _allScriptStringDecls[token] = theString;
}
WORD CompileContext::GetStringTempOffset(const string &theString)
{
    stringcode_map::iterator stringIt = _allInCodeStrings.find(theString);
    assert(stringIt != _allInCodeStrings.end());
    size_t index = distance(_allInCodeStrings.begin(), stringIt);
    return ((WORD)index) + ImaginaryStringOffset; // Strings start at 0x8000 imaginary offset.
}
WORD CompileContext::GetStringTokenTempOffset(const string &theString)
{
    auto stringIt = _allScriptStringDecls.find(theString);
    assert(stringIt != _allScriptStringDecls.end());
    size_t index = distance(_allScriptStringDecls.begin(), stringIt);
    // in code strings come first. So add the size of that to our index.
    index += _allInCodeStrings.size();
    return ((WORD)index) + ImaginaryStringOffset; // Strings start at 0x8000 imaginary offset.
}
void CompileContext::SpecifyFinalStringOffset(const string &theString, WORD wFinalOffset)
{
    WORD wTempOffset = GetStringTempOffset(theString);
    _finalStringOffsets[wTempOffset] = wFinalOffset;
}
void CompileContext::SpecifyFinalStringTokenOffset(const string &theToken, WORD wFinalOffset)
{
    WORD wTempOffset = GetStringTokenTempOffset(theToken);
    _finalStringOffsets[wTempOffset] = wFinalOffset;
}
WORD CompileContext::LookupFinalStringOrSaidOffset(WORD wTempOffset)
{
    if (wTempOffset >= ImaginaryStringOffset)
    {
        assert(!_finalStringOffsets.empty());
        return _finalStringOffsets[wTempOffset];
    }
    else
    {
        assert(!_finalSaidOffsets.empty());
        return _finalSaidOffsets[wTempOffset];
    }
}
void CompileContext::SpecifyOffsetIndexResolvedValue(WORD wOffsetIndex, WORD wOffset)
{
    _offsetIndexToOffset[wOffsetIndex] = wOffset;    
}
void CompileContext::PushVariableLookupContext(const IVariableLookupContext *pVarContext)
{
    _varContext.push(pVarContext);
}
void CompileContext::PopVariableLookupContext()
{
    _varContext.pop();
}
void CompileContext::SetClassPropertyLookupContext(const IVariableLookupContext *pVarContext)
{
    _pClassProperties = pVarContext;
}
void CompileContext::SetReturnValueTypes(const std::vector<SpeciesIndex> &types)
{
    _allowedReturnValues = types;
}
std::vector<SpeciesIndex> CompileContext::GetReturnValueTypes()
{
    return _allowedReturnValues;
}
void CompileContext::SetVariableModifier(VariableModifier modifier)
{
    _modifier = modifier;
}
VariableModifier CompileContext::GetVariableModifier()
{
    return _modifier;
}
void CompileContext::PushSendCallType(SpeciesIndex wType)
{
    _sendCallType.push(wType);
}
void CompileContext::PopSendCallType()
{
    _sendCallType.pop();
}
SpeciesIndex CompileContext::GetSendCalleeType()
{
    return _sendCallType.top();
}

// Other public functions
vector<call_pair> &CompileContext::GetCalls() { return _calls; }
vector<code_pos> &CompileContext::GetExports() { return _exports; }
vector<WORD> &CompileContext::GetPublicInstanceOffsets() { return _publicInstances; }
stringcode_map &CompileContext::GetSaids() { return _allSaids; }
stringcode_map &CompileContext::GetInCodeStrings() { return _allInCodeStrings; }
std::map<std::string, std::string> &CompileContext::GetDeclaredStrings() { return _allScriptStringDecls; }

const unordered_map<WORD, WORD> &CompileContext::GetOffsetFixups()
{
    return _offsetIndexToOffset;
}
void CompileContext::SetScriptNumber()
{
    _wScriptNumber = _script.GetScriptNumber();
    if (_wScriptNumber == InvalidResourceNumber)
    {
        _wScriptNumber = 0; // Safe, because we'll report an error if we leave it at zero.
        if (!LookupDefine(_script.GetScriptNumberDefine(), _wScriptNumber))
        {
            ReportError(&_script, "Couldn't determine script number: '%s' is undefined.", _script.GetScriptNumberDefine().c_str());
        }
    }
    if (_wScriptNumber > _version.GetMaximumResourceNumber())
    {
        ReportError(&_script, "Script number must be less than %d: %d", _version.GetMaximumResourceNumber(), _wScriptNumber);
    }
    _scos[_wScriptNumber].SetScriptNumber(_wScriptNumber);
}
WORD CompileContext::EnsureSpeciesTableEntry(WORD wIndexInScript)
{
    // This won't work unless we have a valid script number
    assert(_wScriptNumber != InvalidResourceNumber);
    return _tables.Species().MaybeAddSpeciesIndex(_wScriptNumber, wIndexInScript);
}
void CompileContext::LoadIncludes()
{
    _headers.Update(*this, _script);
}
void CompileContext::AddSCOClass(CSCOObjectClass scoClass, bool fInstance)
{
    // TODO - if it's a class, add it to some list that gets used when looking classnames up, etc...
    // If it's an instance, add it to another list.
    if (fInstance)
    {
        _instances.push_back(scoClass);
    }
    else
    {
        assert(_wScriptNumber != InvalidResourceNumber);
        assert(_scos[_wScriptNumber].GetScriptNumber() != 0xffff); // Script number not supplied yet.
        _scos[_wScriptNumber].AddObject(scoClass);
    }
}
void CompileContext::ReplaceSCOClass(CSCOObjectClass scoClass)
{
    // This is a bit of a hack.
    assert(_wScriptNumber != InvalidResourceNumber);
    _scos[_wScriptNumber].ReplaceObject(scoClass);
}
void CompileContext::AddSCOVariable(CSCOLocalVariable scoVar)
{
    assert(_wScriptNumber != InvalidResourceNumber);
    _scos[_wScriptNumber].AddVariable(scoVar);
}
void CompileContext::AddSCOPublics(CSCOPublicExport scoPublic)
{
    assert(_wScriptNumber != InvalidResourceNumber);
    _scos[_wScriptNumber].AddPublic(scoPublic);
}
std::vector<CSCOObjectClass> &CompileContext::GetInstanceSCOs()
{
    return _instances;
}
CSCOFile &CompileContext::GetScriptSCO()
{
    assert(_wScriptNumber != InvalidResourceNumber);
    return _scos[_wScriptNumber];
}
std::string CompileContext::LookupSelectorName(WORD wIndex) const
{
    return _tables.Selectors().Lookup(wIndex);
}
const vector<WORD> &CompileContext::GetRelocations()
{
    // This is used for instance names and saids/strings that are property values.
    // Their addresses are put into the relocation table for the script.
    return _relocations;
}

PrecompiledHeaders::PrecompiledHeaders(CResourceMap &resourceMap) : _resourceMap(resourceMap), _fValid(false), _versionCompiled(resourceMap.Helper().Version) {}

std::vector<std::string> g_defaultCPPHeaders;
std::vector<std::string> g_defaultSCIStudioHeaders;
std::vector<std::string> &GetDefaultHeaders(Script &script)
{
    // No default headers for SCI Studio syntax, sorry!
    return g_defaultSCIStudioHeaders; // empty
}

PrecompiledHeaders::~PrecompiledHeaders() {}

void PrecompiledHeaders::Update(CompileContext &context, Script &script)
{
    assert(context.GetVersion() == _versionCompiled);

    // Here, we look at the headers included by this script, and add them to a scanlist
    // We look to see if they've already been parsed.
    //          If so, look in the parsed version, and add all its includes to the scanlist
    //          If not, parse it, and add all its includes to the scanlist
    // Now go through the scanlist again, and repeat, until nothing gets added to the scanlist.
    set<string> headerScanList;
    headerScanList.insert(GetDefaultHeaders(script).begin(), GetDefaultHeaders(script).end());
    headerScanList.insert(script.GetIncludes().begin(), script.GetIncludes().end());
    set<string> nonHeadersEncountered;
    // Now also include any headers that *those* headers include.  To do so, we'll need to parse
    // the header - ideally we can use the pre-parsed versions.
    bool fDone = false;
    while (!fDone)
    {
        set<string> newHeaders; // The additional ones we'll pick up
        set<string>::iterator curHeaderIt = headerScanList.begin();
        for (; curHeaderIt != headerScanList.end(); ++curHeaderIt)
        {
            // Get the script for this header.
            header_map::iterator oldHeader = _allHeaders.find(*curHeaderIt);
            if (oldHeader == _allHeaders.end())
            {
                auto encounteredIt = nonHeadersEncountered.find(*curHeaderIt);
                if (encounteredIt == nonHeadersEncountered.end())
                {
                    // It's a header we have not yet encountered. Parse it.
                    ScriptId scriptId(_resourceMap.GetIncludePath(*curHeaderIt));
                    CCrystalTextBuffer buffer;
                    if (buffer.LoadFromFile(scriptId.GetFullPath().c_str()))
                    {
                        CScriptStreamLimiter limiter(&buffer);
                        CCrystalScriptStream stream(&limiter);
                        unique_ptr<Script> pNewHeader = std::make_unique<Script>(scriptId);
                        if (SyntaxParser_Parse(*pNewHeader, stream, PreProcessorDefinesFromSCIVersion(context.GetVersion()), &context))
                        {
                            if (pNewHeader->IsHeader())
                            {
                                // Look for any includes in here, and add them to our set.
                                newHeaders.insert(pNewHeader->GetIncludes().begin(), pNewHeader->GetIncludes().end());
                                // And now that we've parsed something, add it to the master list
                                _allHeaders[*curHeaderIt] = move(pNewHeader);
                            }
                            else
                            {
                                // This is an include which is not a header. Merge it into our Script
                                MergeScripts(script, *pNewHeader);
                                nonHeadersEncountered.insert(*curHeaderIt);
                            }
                        }
                        else
                        {
                            std::stringstream ss;
                            ss << "Parsing errors while loading " << scriptId.GetFullPath() << ".";
                            context.ReportResult(CompileResult(ss.str(), CompileResult::CRT_Error));
                        }
                        buffer.FreeAll();
                    }
                    else
                    {
                        std::stringstream ss;
                        ss << "Unable to load " << scriptId.GetFullPath() << ".";
                        context.ReportResult(CompileResult(ss.str(), CompileResult::CRT_Error));
                    }
                }
            }
            else
            {
                Script *pOldHeader = oldHeader->second.get();
                newHeaders.insert(pOldHeader->GetIncludes().begin(), pOldHeader->GetIncludes().end());
            }
        }
        // Add them to our master list for this particular script - we're done when we didn't add any new ones.
        size_t oldSize = headerScanList.size();
        headerScanList.insert(newHeaders.begin(), newHeaders.end());
        fDone = (oldSize == headerScanList.size());
    }

    // Once we're here, we should have:
    // 1) A complete map of all Script objects we need (and possibly others), in _allHeaders
    // 2) A complete set of the headers used in *this* script in headerScanList
    // If headerScanList is the same as the last set of headers (_currentHeaders), then we're done.
    // Otherwise, we have to re-evaluate all our defines.
    // We can use equality to compare the header lists, since they are in a std::set, which contains
    // ordered and unique values.
    if (headerScanList != _curHeaderList)
    {
        _curHeaderList = headerScanList;
        // Clear out our defines
        _defines.clear();
        // And generate them anew
        set<string>::iterator nameIt = _curHeaderList.begin();
        while (nameIt != _curHeaderList.end())
        {
            header_map::iterator headerIt = _allHeaders.find(*nameIt);
            if (headerIt != _allHeaders.end())
            {
                Script *pHeaderScript = headerIt->second.get();

                // Get the script for this.  We know it exists, so no need to check for failure
                Script *pOldError = context.SetErrorContext(pHeaderScript);    // All errors henceforth are in this header.

                const DefineVector &defines = pHeaderScript->GetDefines();
                DefineVector::const_iterator defineIt = defines.begin();
                for (; defineIt != defines.end(); ++defineIt)
                {
                    const string &defineLabel = (*defineIt)->GetLabel();
                    if (_defines.lower_bound(defineLabel) != _defines.end())
                    {
                        context.ReportWarning((*defineIt).get(), "Duplicate defines: '%s'", defineLabel.c_str());
                    }
                    _defines[defineLabel] = (*defineIt).get(); // This is risky... I hope the container lifetime outlasts _defines.
                }

                context.SetErrorContext(pOldError);   // Now they're in the main script again.
            }
            ++nameIt;
        }
    }
    _fValid = true;
    _versionCompiled = context.GetVersion();
}

bool PrecompiledHeaders::LookupDefine(const std::string &str, WORD &wValue)
{
    assert(_fValid);
    bool fRet = false;
    defines_map::const_iterator nodeIt = _defines.find(str);
    fRet = (nodeIt != _defines.end());
    if (fRet)
    {
        wValue = nodeIt->second->GetValue();
    }
    return fRet;
}

void GenericOutputByteCode::operator()(const IOutputByteCode* proc)
{
	_results.push_back(proc->OutputByteCode(_context));
}

// This merges scriptToBeMerged into mainScript.
// Note: The items merged from scriptToBeMerged are removed from it.
void MergeScripts(sci::Script &mainScript, sci::Script &scriptToBeMerged)
{
    // For now, just support procedures and local variables
    auto &procs = scriptToBeMerged.GetProceduresNC();
    for (auto &proc : procs)
    {
        mainScript.AddProcedure(move(proc));
    }
    auto &localVars = scriptToBeMerged.GetScriptVariables();
    for (auto &localVar : localVars)
    {
        mainScript.AddVariable(move(localVar));
    }
}
