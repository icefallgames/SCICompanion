#include "stdafx.h"
#include "DecompileScript.h"
#include "CompiledScript.h"
#include "DecompilerCore.h"
#include "ScriptOMAll.h"
#include "AutoDetectVariableNames.h"

using namespace sci;
using namespace std;

void DecompileObject(const CompiledObjectBase &object, sci::Script &script,
    DecompileLookups &lookups,
    const std::vector<BYTE> &scriptResource,
    const std::vector<CodeSection> &codeSections,
    const std::set<uint16_t> &codePointersTO)
{
    lookups.EndowWithProperties(&object);

    unique_ptr<ClassDefinition> pClass = std::make_unique<ClassDefinition>();
    pClass->SetScript(&script);
    pClass->SetInstance(object.IsInstance());
    pClass->SetName(object.GetName());
    pClass->SetSuperClass(lookups.LookupClassName(object.GetSuperClass()));
    pClass->SetPublic(object.IsPublic);
    vector<uint16_t> propertySelectorList;
    vector<uint16_t> speciesPropertyValueList;
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
                // If this is an instance, look up the species values, and only
                // include those that are different.
                if (!object.IsInstance() || (object.GetPropertyValues()[i] != speciesPropertyValueList[i]))
                {
                    ClassProperty prop;
                    prop.SetName(lookups.LookupSelectorName(propertySelectorList[i]));
                    PropertyValue value;
                    ICompiledScriptSpecificLookups::ObjectType type;
                    std::string saidOrString = lookups.LookupScriptThing(object.GetPropertyValues()[i], type);
                    if (saidOrString.empty())
                    {
                        // Just give it a number
                        uint16_t number = object.GetPropertyValues()[i];
                        value.SetValue(number);
                        if (number == 65535)
                        {
                            // A good bet that it's -1
                            value.Negate();
                        }
                    }
                    else
                    {
                        // REVIEW: we could provide a hit here when we shouldn't... oh well.
                        // Use ValueType::Token, since the ' or " is already provided in the string.
                        value.SetValue(saidOrString, _ScriptObjectTypeToPropertyValueType(type));
                    }
                    prop.SetValue(value);
                    pClass->AddProperty(prop);
                }
            }
        }
        // else -> make ERROR by adding script comment.
    } // else make ERROR

    // Methods
    const vector<uint16_t> &functionSelectors = object.GetMethods();
    const vector<uint16_t> &functionOffsetsTO = object.GetMethodCodePointersTO();
    assert(functionSelectors.size() == functionOffsetsTO.size());
    for (size_t i = 0; i < functionSelectors.size(); i++)
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

void DecompileFunction(const CompiledScript &compiledScript, ProcedureDefinition &func, DecompileLookups &lookups, uint16_t wCodeOffsetTO, const set<uint16_t> &sortedCodePointersTO)
{
    lookups.EndowWithProperties(lookups.GetPossiblePropertiesForProc(wCodeOffsetTO));
    set<uint16_t>::const_iterator codeStartIt = sortedCodePointersTO.find(wCodeOffsetTO);
    ASSERT(codeStartIt != sortedCodePointersTO.end());
    const BYTE *pBegin = &compiledScript.GetRawBytes()[*codeStartIt];
    const BYTE *pEnd = compiledScript.GetEndOfRawBytes();
    DecompileRaw(func, lookups, pBegin, pEnd, wCodeOffsetTO);
    if (lookups.WasPropertyRequested() && lookups.GetPossiblePropertiesForProc(wCodeOffsetTO))
    {
        const CompiledObjectBase *object = static_cast<const CompiledObjectBase *>(lookups.GetPossiblePropertiesForProc(wCodeOffsetTO));
        // This procedure is "of" this object
        func.SetClass(object->GetName());
    }
    lookups.EndowWithProperties(nullptr);
}

Script *Decompile(const CompiledScript &compiledScript, DecompileLookups &lookups, const ILookupNames *pWords)
{
    unique_ptr<Script> pScript = std::make_unique<Script>();
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
            pSynonym->MainWord = lookups.LookupScriptThing(syn.first, type);
            pSynonym->Replacement = lookups.LookupScriptThing(syn.second, type);
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
    }

    // Now the exported procedures.
    for (size_t i = 0; i < compiledScript._exportsTO.size(); i++)
    {
        // _exportsTO, in addition to containing code pointers for public procedures, also
        // contain the Rm class.  Filter these out by ignoring code pointers which point outside
        // the codesegment.
        if (compiledScript.IsExportAProcedure(compiledScript._exportsTO[i]))
        {
            std::unique_ptr<ProcedureDefinition> pProc = std::make_unique<ProcedureDefinition>();
            pProc->SetScript(pScript.get());
            pProc->SetName(lookups.ReverseLookupPublicExportName(compiledScript.GetScriptNumber(), (uint16_t)i));
            pProc->SetPublic(true);
            DecompileFunction(compiledScript, *pProc, lookups, compiledScript._exportsTO[i], codePointersTO);
            pScript->AddProcedure(std::move(pProc));
        }
    }

    // Now the internal procedures (REVIEW - possibly overlap with exported ones)
    for (uint16_t offset : internalProcOffsetsTO)
    {
        std::unique_ptr<ProcedureDefinition> pProc = make_unique<ProcedureDefinition>();
        pProc->SetScript(pScript.get());
        pProc->SetName(_GetProcNameFromScriptOffset(offset));
        pProc->SetPublic(false);
        DecompileFunction(compiledScript, *pProc, lookups, offset, codePointersTO);
        pScript->AddProcedure(std::move(pProc));
    }

    AddLocalVariablesToScript(*pScript, lookups, compiledScript._localVars);

    AutoDetectVariableNames(*pScript);

    return pScript.release();
}
