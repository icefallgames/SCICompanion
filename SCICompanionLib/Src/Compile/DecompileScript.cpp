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
#include "DecompileScript.h"
#include "CompiledScript.h"
#include "DecompilerCore.h"
#include "ScriptOMAll.h"
#include "AutoDetectVariableNames.h"
#include "SCO.h"
#include "GameFolderHelper.h"
#include "DecompilerResults.h"
#include "format.h"
#include "DecompilerConfig.h"

using namespace sci;
using namespace std;

const char InvalidLookupError[] = "LOOKUP_ERROR";

void DecompileObject(const CompiledObject &object,
    sci::Script &script,
    DecompileLookups &lookups,
    const std::vector<BYTE> &scriptResource,
    const std::vector<CodeSection> &codeSections,
    const std::set<uint16_t> &codePointersTO)
{
    lookups.EndowWithProperties(&object);

    uint16_t superClassScriptNum;
    if (lookups.GetSpeciesScriptNumber(object.GetSuperClass(), superClassScriptNum))
    {
        lookups.TrackUsingScript(superClassScriptNum);
    }

    unique_ptr<ClassDefinition> pClass = std::make_unique<ClassDefinition>();
    pClass->SetScript(&script);
    pClass->SetInstance(object.IsInstance());
    pClass->SetName(object.GetName());
    pClass->SetSuperClass(lookups.LookupClassName(object.GetSuperClass()));
    pClass->SetPublic(object.IsPublic);
    vector<uint16_t> propertySelectorList;
    vector<CompiledVarValue> speciesPropertyValueList;
    bool fSuccess = lookups.LookupSpeciesPropertyListAndValues(object.GetSpecies(), propertySelectorList, speciesPropertyValueList);
    if (!fSuccess && !object.IsInstance())
    {
        // We're a class - our species is ourself
        propertySelectorList = object.GetProperties();
        speciesPropertyValueList = object.GetPropertyValues();
        fSuccess = true;
    }
    if (fSuccess)
    {
        assert(propertySelectorList.size() == speciesPropertyValueList.size());
        size_t size1 = propertySelectorList.size();
        size_t size2 = object.GetPropertyValues().size();
        if (size1 == size2)
        {
            for (size_t i = object.GetNumberOfDefaultSelectors(); i < object.GetPropertyValues().size(); i++)
            {
                const CompiledVarValue &propValue = object.GetPropertyValues()[i];
                // If this is an instance, look up the species values, and only
                // include those that are different.
                if (!object.IsInstance() || (propValue.value != speciesPropertyValueList[i].value))
                {
                    unique_ptr<ClassProperty> prop = make_unique<ClassProperty>();
                    prop->SetName(lookups.LookupSelectorName(propertySelectorList[i]));
                    PropertyValue value;
                    ICompiledScriptSpecificLookups::ObjectType type;
                    std::string saidOrString;
                    if (!propValue.isObjectOrString || !lookups.LookupScriptThing(propValue.value, type, saidOrString))
                    {
                        assert(!propValue.isObjectOrString && "We should have resolved some token for this property value");

                        // Just give it a number
                        uint16_t number = propValue.value;
                        value.SetValue(number);
                        if (lookups.GetDecompilerConfig()->IsBitfieldProperty(prop->GetName()))
                        {
                            value._fHex = true;
                        }
                        else if (number >= 32768)
                        {
                            // A good bet that it's negative
                            value.Negate();
                        }
                    }
                    else
                    {
                        // REVIEW: we could provide a hit here when we shouldn't... oh well.
                        // Use ValueType::Token, since the ' or " is already provided in the string.
                        value.SetValue(saidOrString, _ScriptObjectTypeToPropertyValueType(type));
                    }
                    prop->SetValue(value);
                    pClass->AddProperty(move(prop));
                }
            }
        }
        // else -> make ERROR by adding script comment.
    } // else make ERROR

    // Methods
    const vector<uint16_t> &functionSelectors = object.GetMethods();
    const vector<uint16_t> &functionOffsetsTO = object.GetMethodCodePointersTO();
    assert(functionSelectors.size() == functionOffsetsTO.size());
    for (size_t i = 0; i < functionSelectors.size() && !lookups.DecompileResults().IsAborted(); i++)
    {
        // Now the code.
        set<uint16_t>::const_iterator functionIndex = find(codePointersTO.begin(), codePointersTO.end(), functionOffsetsTO[i]);
        if (functionIndex != codePointersTO.end())
        {
            const BYTE *pStartCode = &scriptResource[*functionIndex];
            const BYTE *pEndCode = &scriptResource[0] + scriptResource.size();

            std::unique_ptr<MethodDefinition> pMethod = std::make_unique<MethodDefinition>();
            pMethod->SetOwnerClass(pClass.get());
            pMethod->SetScript(&script);
            pMethod->SetName(lookups.LookupSelectorName(functionSelectors[i]));
            DecompileRaw(*pMethod, lookups, pStartCode, pEndCode, functionOffsetsTO[i]);
            pClass->AddMethod(std::move(pMethod));
        }
    }

    script.AddClass(std::move(pClass));
    lookups.EndowWithProperties(nullptr);
}

const uint16_t BogusSQ5Export = 0x3af;

void DecompileFunction(const CompiledScript &compiledScript, ProcedureDefinition &func, DecompileLookups &lookups, uint16_t wCodeOffsetTO, const set<uint16_t> &sortedCodePointersTO)
{
    lookups.EndowWithProperties(lookups.GetPossiblePropertiesForProc(wCodeOffsetTO));
    set<uint16_t>::const_iterator codeStartIt = sortedCodePointersTO.find(wCodeOffsetTO);
    assert(codeStartIt != sortedCodePointersTO.end());
    bool isValidFunctionPointer = (*codeStartIt < compiledScript.GetRawBytes().size()) && (*codeStartIt != BogusSQ5Export);
    if (isValidFunctionPointer)
    {
        const BYTE *pBegin = &compiledScript.GetRawBytes()[*codeStartIt];
        const BYTE *pEnd = compiledScript.GetEndOfRawBytes();
        DecompileRaw(func, lookups, pBegin, pEnd, wCodeOffsetTO);
        if (lookups.WasPropertyRequested() && lookups.GetPossiblePropertiesForProc(wCodeOffsetTO))
        {
            const CompiledObject *object = static_cast<const CompiledObject *>(lookups.GetPossiblePropertiesForProc(wCodeOffsetTO));
            // This procedure is "of" this object
            func.SetClass(object->GetName());
        }
    }
    else
    {
        // Make a function stub
        func.AddSignature(make_unique<FunctionSignature>());
        lookups.DecompileResults().AddResult(DecompilerResultType::Warning, fmt::format("Invalid function offset: {0:04x}", *codeStartIt));
    }
    lookups.EndowWithProperties(nullptr);
}

void InsertHeaders(Script &script)
{
    // For decompiling, we don't need game.sh yet (since we're not creating it is still TBD)
    script.AddInclude("sci.sh");
}

void DetermineAndInsertUsings(const GameFolderHelper &helper, Script &script, DecompileLookups &lookups)
{
    for (uint16_t usingScript : lookups.GetUsings())
    {
        script.AddUse(helper.FigureOutName(ResourceType::Script, usingScript, NoBase36));
    }
}

// e.g. of the form "proc255_3", or "localproc_0b2a", where localproc_0b2a is actually an exported procedure.
// If true, returns the script number and export index so we can look up the real name.
bool _IsUndeterminedPublicProc(const CompiledScript &compiledScript, const std::string &procName, uint16_t &script, uint16_t &index)
{
    script = 0;
    index = 0;
    if (0 == procName.compare(0, 4, "proc"))
    {
        string rest = procName.substr(4, string::npos);
        // This needs to be of the form [number]_[number]
        size_t position = 0;
        int scriptNumber = stoi(rest, &position);
        if ((position > 0) && (position < rest.size()))
        {
            if (rest[position] == '_')
            {
                rest = rest.substr(position + 1, string::npos);
                int indexNumber = stoi(rest, &position);
                script = (uint16_t)scriptNumber;
                index = (uint16_t)indexNumber;
                return true;
            }
        }
    }
    else if (0 == procName.compare(0, 10, "localproc_"))
    {
        string rest = procName.substr(10, string::npos);
        size_t position = 0;
        int offset = stoi(rest, &position, 16);
        if (position == rest.size())    // Consumed whole thing
        {
            // Find the offset of this proc. If it's also a public export, count it as so.
            int indexNumber;
            if (compiledScript.IsExportAProcedure((uint16_t)offset, &indexNumber))
            {
                script = compiledScript.GetScriptNumber();
                index = (uint16_t)indexNumber;
                return true;
            }
        }
    }
    return false;
}

class ResolveProcedureCalls : public IExploreNodeContext, public IExploreNode
{
public:
    ResolveProcedureCalls(const GameFolderHelper &helper, const CompiledScript &compiledScript, unordered_map<int, unique_ptr<CSCOFile>> &scoMap) : _scoMap(scoMap), _compiledScript(compiledScript), _helper(helper) {}

    void ExploreNode(IExploreNodeContext *pContext, SyntaxNode &node, ExploreNodeState state) override
    {
        if (state == ExploreNodeState::Pre)
        {
            ProcedureCall *procCall = SafeSyntaxNode<ProcedureCall>(&node);
            if (procCall)
            {
                uint16_t scriptNumber, index;
                if (_IsUndeterminedPublicProc(_compiledScript, procCall->GetName(), scriptNumber, index))
                {
                    CSCOFile *sco = _EnsureSCO(scriptNumber);
                    if (sco)
                    {
                        string newProcName = sco->GetExportName(index);
                        assert(!newProcName.empty());
                        if (!newProcName.empty())
                        {
                            procCall->SetName(newProcName);
                        }
                    }
                    else
                    {
                        // In the case when we don't have an .sco file yet (first compile), if this
                        // was actually a local call instruction (but is an exported public proc, as we now know),
                        // use the public proc name for it temporarily
                        if (procCall->GetName() != _GetPublicProcedureName(scriptNumber, index))
                        {
                            procCall->SetName(_GetPublicProcedureName(scriptNumber, index));
                        }
                    }
                }
            }

            // We need to handle this for ASM too
            Asm *asmStatement = SafeSyntaxNode<Asm>(&node);
            if (asmStatement)
            {
                string instruction = asmStatement->GetName();
                if (instruction == "call" || instruction == "callb" || instruction == "calle")
                {
                    SyntaxNode *procNameNode = asmStatement->GetStatements()[0]->GetSyntaxNode();
                    PropertyValue *value = SafeSyntaxNode<PropertyValue>(procNameNode);
                    uint16_t scriptNumber, index;
                    assert(value->GetType() == ValueType::Token);
                    if (_IsUndeterminedPublicProc(_compiledScript, value->GetStringValue(), scriptNumber, index))
                    {
                        CSCOFile *sco = _EnsureSCO(scriptNumber);
                        if (sco)
                        {
                            string newProcName = sco->GetExportName(index);
                            assert(!newProcName.empty());
                            if (!newProcName.empty())
                            {
                                value->SetValue(newProcName, ValueType::Token);
                            }
                        }
                    }
                }
            }
        }
    }

private:
    CSCOFile *_EnsureSCO(uint16_t script)
    {
        if (_scoMap.find(script) == _scoMap.end())
        {
            _scoMap[script] = move(GetExistingSCOFromScriptNumber(_helper, script));
        }
        return _scoMap.at(script).get();
    }
        
    const GameFolderHelper &_helper;
    unordered_map<int, unique_ptr<CSCOFile>> &_scoMap;
    const CompiledScript &_compiledScript;
};

// This pulls in the required .sco files to find the public procedure names
void ResolvePublicProcedureCalls(const GameFolderHelper &helper, Script &script, const CompiledScript &compiledScript)
{
    unordered_map<int, unique_ptr<CSCOFile>> scoMap;
    scoMap[script.GetScriptNumber()] = move(GetExistingSCOFromScriptNumber(helper, script.GetScriptNumber()));

    // First let's resolve the exports
    CSCOFile *thisSCO = scoMap.at(script.GetScriptNumber()).get();
    if (thisSCO)
    {
        for (auto &proc : script.GetProceduresNC())
        {
            uint16_t scriptNumber, index;
            if (proc->IsPublic() && _IsUndeterminedPublicProc(compiledScript, proc->GetName(), scriptNumber, index))
            {
                assert(scriptNumber == script.GetScriptNumber());
                string newProcName = thisSCO->GetExportName(index);
                assert(!newProcName.empty());
                if (newProcName.empty())
                {
                    proc->SetName(newProcName);
                }
            }
        }
    }

    // Now the actual calls, which could be to any script
    ResolveProcedureCalls resolveProcCalls(helper, compiledScript, scoMap);
    script.Traverse(&resolveProcCalls, resolveProcCalls);
}

class ResolveVariableValues : public IExploreNodeContext, public IExploreNode
{
public:
    ResolveVariableValues(const IDecompilerConfig &config) : _config(config) {}

    void ExploreNode(IExploreNodeContext *pContext, SyntaxNode &node, ExploreNodeState state) override
    {
        if (state == ExploreNodeState::Pre)
        {
            SwitchStatement *switchStatement = SafeSyntaxNode<SwitchStatement>(&node);
            if (switchStatement)
            {
                _config.ResolveSwitchStatementValues(*switchStatement);
            }
            BinaryOp *binaryOp = SafeSyntaxNode<BinaryOp>(&node);
            if (binaryOp)
            {
                _config.ResolveBinaryOpValues(*binaryOp);
            }
        }
    }

private:
    const IDecompilerConfig &_config;
};

Script *Decompile(const GameFolderHelper &helper, const CompiledScript &compiledScript, DecompileLookups &lookups, const ILookupNames *pWords)
{
    unique_ptr<Script> pScript = std::make_unique<Script>();
    pScript->SyntaxVersion = 2;
    ScriptId scriptId;
    scriptId.SetResourceNumber(compiledScript.GetScriptNumber());
    pScript->SetScriptId(scriptId);

    compiledScript.PopulateSaidStrings(pWords);

    // Synonyms
    if (compiledScript._synonyms.size() > 0)
    {
        for (const auto &syn : compiledScript._synonyms)
        {
            unique_ptr<Synonym> pSynonym = std::make_unique<Synonym>();
            ICompiledScriptSpecificLookups::ObjectType type;
            pSynonym->MainWord = InvalidLookupError;
            lookups.LookupScriptThing(syn.first, type, pSynonym->MainWord);
            pSynonym->Replacement = InvalidLookupError;
            lookups.LookupScriptThing(syn.second, type, pSynonym->Replacement);
            pScript->AddSynonym(std::move(pSynonym));
        }
    }

    // Now its time for code.
    // Make an index of code pointers by looking at the object methods
    set<uint16_t> codePointersTO;
    for (auto &object : compiledScript._objects)
    {
        const vector<uint16_t> &methodPointersTO = object->GetMethodCodePointersTO();
        codePointersTO.insert(methodPointersTO.begin(), methodPointersTO.end());
    }

    // and the exported procedures
    for (size_t i = 0; i < compiledScript._exportsTO.size(); i++)
    {
        uint16_t wCodeOffset = compiledScript._exportsTO[i];
        // Export offsets could point to objects too - we're only interested in code pointers, so
        // check that it's not an object
        if (compiledScript.IsExportAProcedure(wCodeOffset))
        {
            codePointersTO.insert(wCodeOffset);
        }
    }

    // and finally, the most difficult of all, we'll need to scan though for any call calls...
    // those would be our internal procs
    set<uint16_t> internalProcOffsetsTO = compiledScript.FindInternalCallsTO();
    // Before adding these though, remove any exports from the internalProcOffsets.
    for (const auto &exporty : compiledScript._exportsTO)
    {
        set<uint16_t>::iterator internalsIndex = find(internalProcOffsetsTO.begin(), internalProcOffsetsTO.end(), exporty);
        if (internalsIndex != internalProcOffsetsTO.end())
        {
            // Remove this guy.
            internalProcOffsetsTO.erase(internalsIndex);
        }
    }
    // Now add the internal guys to the full list
    codePointersTO.insert(internalProcOffsetsTO.begin(), internalProcOffsetsTO.end());
    // Now we know the length of each code segment (assuming none overlap)

    // Spit out code segments:
    // First, the objects (instances, classes)
    for (auto &object : compiledScript._objects)
    {
        DecompileObject(*object, *pScript, lookups, compiledScript.GetRawBytes(), compiledScript._codeSections, codePointersTO);
        if (lookups.DecompileResults().IsAborted())
        {
            break;
        }
    }

    map<int, string> exportSlotToName;

    // Now the exported procedures.
    for (size_t i = 0; i < compiledScript._exportsTO.size() && !lookups.DecompileResults().IsAborted(); i++)
    {
        // _exportsTO, in addition to containing code pointers for public procedures, also
        // contain the Rm class.  Filter these out by ignoring code pointers which point outside
        // the codesegment.
        uint16_t exportPointer = compiledScript._exportsTO[i];
        if (compiledScript.IsExportAProcedure(exportPointer))
        {
            std::unique_ptr<ProcedureDefinition> pProc = std::make_unique<ProcedureDefinition>();
            pProc->SetScript(pScript.get());
            pProc->SetName(lookups.ReverseLookupPublicExportName(compiledScript.GetScriptNumber(), (uint16_t)i));
            exportSlotToName[i] = pProc->GetName();
            pProc->SetPublic(true);
            DecompileFunction(compiledScript, *pProc, lookups, exportPointer, codePointersTO);
            pScript->AddProcedure(std::move(pProc));
        }
        else if (exportPointer == 0)
        {
            // Valid.
        }
        else 
        {
            // It should be an object
            CompiledObject *object = compiledScript.GetObjectForExport(exportPointer);
            if (object)
            {
                exportSlotToName[i] = object->GetName();
            }
            assert(object);
        }
    }

    // Now the internal procedures (REVIEW - possibly overlap with exported ones)
    for (uint16_t offset : internalProcOffsetsTO)
    {
        if (lookups.DecompileResults().IsAborted())
        {
            break;
        }
        std::unique_ptr<ProcedureDefinition> pProc = make_unique<ProcedureDefinition>();
        pProc->SetScript(pScript.get());
        pProc->SetName(_GetProcNameFromScriptOffset(offset));
        pProc->SetPublic(false);
        DecompileFunction(compiledScript, *pProc, lookups, offset, codePointersTO);
        pScript->AddProcedure(std::move(pProc));
    }

    if (!lookups.DecompileResults().IsAborted())
    {
        AddLocalVariablesToScript(*pScript, compiledScript, lookups, compiledScript._localVars);

        // Load this script's SCO, and main's SCO (assuming this isn't main)
        unique_ptr<CSCOFile> mainSCO;
        if (compiledScript.GetScriptNumber() != 0)
        {
            mainSCO = GetExistingSCOFromScriptNumber(helper, 0);
        }
        unique_ptr<CSCOFile> oldScriptSCO = GetExistingSCOFromScriptNumber(helper, compiledScript.GetScriptNumber());

        bool mainDirty = false;
        AutoDetectVariableNames(*pScript, lookups.GetDecompilerConfig(), mainSCO.get(), oldScriptSCO.get(), mainDirty);

        ResolvePublicProcedureCalls(helper, *pScript, compiledScript);

        MassageProcedureCalls(lookups, *pScript);

        if (lookups.GetDecompilerConfig())
        {
            ResolveVariableValues resolveVariableValues(*lookups.GetDecompilerConfig());
            pScript->Traverse(&resolveVariableValues, resolveVariableValues);
        }

        InsertHeaders(*pScript);

        DetermineAndInsertUsings(helper, *pScript, lookups);

        for (auto &pair : exportSlotToName)
        {
            unique_ptr<ExportEntry> entry = make_unique<ExportEntry>(pair.first, pair.second);
            pScript->GetExports().push_back(move(entry));
        }

        // Decompiling always generates an SCO. Any pertinent info from the old SCO should be transfered
        // to the new one based extracting info from the script.
        std::unique_ptr<CSCOFile> scoFile = SCOFromScriptAndCompiledScript(*pScript, compiledScript);
        SaveSCOFile(helper, *scoFile);

        // We may have added some global info to main's SCO. Save that now.
        if (mainDirty)
        {
            lookups.DecompileResults().AddResult(DecompilerResultType::Important, "Updating global variables in script 0");
            SaveSCOFile(helper, *mainSCO);
        }
    }
    return pScript.release();
}
