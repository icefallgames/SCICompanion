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
#include "SCO.h"
#include "ScriptOM.h"
#include "CompiledScript.h"
#include "GameFolderHelper.h"

using namespace std;
using namespace sci;


//
// SCO format header:
//
// BYTE, BYTE, BYTE
// BYTE majorversion
// BYTE minorversion
// BYTE build
// BYTE sciversion
// BYTE 0 -> align to WORD boundary
// WORD scriptnumber
// WORD cTotalPublicFunctions
// WORD absolute offset in file, of public functions ->
// WORD cTotalClasses
// WORD absolute offset in file of classes ->
// WORD cTotalLocalVars
// WORD offset in file of local vars

// public functions (in sequence, no terminator)
// WORD index of function (e.g. 0, 1, 2)
// n    null terminated string (not sure if things get aligned)

// classes
// WORD cProps
// WORD cMethods
// WORD species
// WORD superclass
// WORD*cProps
//      { WORD selectorIndex, WORD value }
// WORD*cMethods
//      { WORD selectorIndex }

// localvars
// null terminated string


//
// Handy functors
//
template<typename _T>
struct FwdSave : unary_function<_T, void>
{
public:
    FwdSave(vector<BYTE> &output) : _output(output) {}
    void operator()(const _T &thing)
    {
        thing.Save(_output);
    }
private:
    vector<BYTE> &_output;
};

struct PushWord : unary_function<WORD, void>
{
public:
    PushWord(vector<BYTE> &output) : _output(output) {}
    void operator()(const WORD w)
    {
        push_word(_output, w);
    }
private:
    vector<BYTE> &_output;
};


bool CSCOFile::Load(sci::istream &stream)
{
    stream.seekg(stream.tellg() + 3);
    stream >> _bMajorVersion;
    stream >> _bMinorVersion;
    stream >> _bBuild;
    stream >> _bSCIVersion;
    stream >> _bAlignment;
    stream >> _wScriptNumber;

    if (stream.good())
    {
        DWORD dwSavePos = 0;
        // The publics:
        WORD wTotalPublics;
        stream >> wTotalPublics;
        if (stream.good())
        {
            _wOffsetPublics;
            stream >> _wOffsetPublics;
            if (stream.good())
            {
                dwSavePos = stream.tellg();
                stream.seekg(_wOffsetPublics);
                for (WORD i = 0; stream.good() && i < wTotalPublics; i++)
                {
                    CSCOPublicExport procedure;
                    if (procedure.Create(stream))
                    {
                        _publics.push_back(procedure);
                    }
                }
            }
        }

        if (stream.good())
        {
            // Go back
            stream.seekg(dwSavePos);
            if (stream.good())
            {
                WORD wTotalClasses;
                stream >> wTotalClasses;
                if (stream.good())
                {
                    _wOffsetClasses;
                    stream >> _wOffsetClasses;
                    if (stream.good())
                    {
                        dwSavePos = stream.tellg();
                        stream.seekg(_wOffsetClasses);
                        bool fRet = true;
                        for (WORD i = 0; fRet && i < wTotalClasses; i++)
                        {
                            CSCOObjectClass objectClass;
                            fRet = objectClass.Load(stream, _bSCIVersion);
                            if (fRet)
                            {
                                _classes.push_back(objectClass);
                            }
                        }
                    }
                }
            }
        }

        if (stream.good())
        {
            // Go back
            stream.seekg(dwSavePos);
            if (stream.good())
            {
                WORD wTotalVars;
                stream >> wTotalVars;
                if (stream.good())
                {
                    _wOffsetVars;
                    stream >> _wOffsetVars;
                    if (stream.good())
                    {
                        dwSavePos = stream.tellg();
                        stream.seekg(_wOffsetVars);
                        bool fRet = stream.good();
                        _vars.reserve(wTotalVars);
                        for (WORD i = 0; fRet && i < wTotalVars; i++)
                        {
                            CSCOLocalVariable var;
                            fRet = var.Create(stream);
                            if (fRet)
                            {
                                _vars.push_back(var);
                            }
                        }
                    }
                }
            }
        }
    }
    return stream.good();
}

void CSCOFile::Save(vector<BYTE> &output) const
{
    // Header
    string scoToken = "SCO";
    output.insert(output.end(), scoToken.begin(), scoToken.end());
    output.push_back(_bMajorVersion);
    output.push_back(_bMinorVersion);
    output.push_back(_bBuild);
    output.push_back(_bSCIVersion);
    output.push_back(_bAlignment);
    ASSERT((output.size() % 2) == 0); // alignment should have made it even.

    // Script number
    push_word(output, _wScriptNumber);

    // Offsets to places
    push_word(output, (WORD)_publics.size());
    WORD wOffsetPublics = push_temp(output);
    push_word(output, (WORD)_classes.size());
    WORD wOffsetClasses = push_temp(output);
    push_word(output, (WORD)_vars.size());
    WORD wOffsetVars = push_temp(output);

    // If a particular set of items is empty, then leave the temp offset we wrote at zero...
    if (!_publics.empty())
    {
        write_word(output, wOffsetPublics, (WORD)output.size());
        for_each(_publics.begin(), _publics.end(), FwdSave<CSCOPublicExport>(output));
    }

    if (!_classes.empty())
    {
        write_word(output, wOffsetClasses, (WORD)output.size());
        for (auto &theClass : _classes)
        {
            theClass.Save(output, _bSCIVersion);
        }
    }

    if (!_vars.empty())
    {
        write_word(output, wOffsetVars, (WORD)output.size());
        for_each(_vars.begin(), _vars.end(), FwdSave<CSCOLocalVariable>(output));
    }
}


std::string CSCOFile::GetVariableName(WORD wIndex) const
{
    if (wIndex < _vars.size())
    {
        std::string theName = _vars[wIndex].GetName();
        if (theName == "")
        {
            // TODO: probably an array... search backward for the name?
        }
        return theName;
    }
    else
    {
        return "";
    }
}

std::string CSCOFile::GetExportName(WORD wIndex) const
{
    // The index in the _publics array doesn't necessarily correspond
    // to the actual export index.
    for (const auto &publicExport : _publics)
    {
        if (publicExport.GetIndex() == wIndex)
        {
            return publicExport.GetName();
        }
    }
    return "";
}

std::string CSCOFile::GetClassName(WORD wIndex) const
{
    if (wIndex < _classes.size())
    {
        return _classes[wIndex].GetName();
    }
    else
    {
        return "";
    }
}

//
// Helper to get the index of an item in the script.
//
template<typename _T>
bool GetItemIndex(const vector<_T> &container, const string &name, WORD &wIndex)
{
    vector<_T>::const_iterator position = match_name_dot(container.begin(), container.end(), name);
    if (position != container.end())
    {
        wIndex = (WORD)distance(container.begin(), position);
        return true;
    }
    else
    {
        return false;
    }
}

bool CSCOFile::GetVariableIndex(const std::string &variableName, WORD &wIndex, WORD &wType) const
{
    bool fRet = GetItemIndex(_vars, variableName, wIndex);
    if (fRet)
    {
        wType = _vars[wIndex].GetType();
    }
    return fRet;
}

bool CSCOFile::GetExportIndex(const std::string &exportName, WORD &wIndex) const
{
    uint16_t indexTemp;
    bool fRet = GetItemIndex(_publics, exportName, indexTemp);
    if (fRet)
    {
        wIndex = _publics[indexTemp].GetIndex();
    }
    return fRet;
}

bool CSCOFile::GetPublicExportByName(const std::string &exportName, CSCOPublicExport &theExport) const
{
    WORD wIndex;
    bool fRet = GetExportIndex(exportName, wIndex);
    if (fRet)
    {
		theExport = _publics[wIndex];
    }
    return fRet;
}

std::vector<CSCOFunctionSignature> CSCOFile::GetExportSignatures(WORD wIndex) const
{
    for (auto &theExport : _publics)
    {
        if (theExport.GetIndex() == wIndex)
        {
            return theExport.GetSignatures();
        }
    }
    assert(false);
    return std::vector<CSCOFunctionSignature>();
}

bool CSCOFile::GetClassIndex(std::string className, WORD &wIndex) const
{
    return GetItemIndex(_classes, className, wIndex);
}

// A bit of a hack
void CSCOFile::ReplaceObject(const CSCOObjectClass &object)
{
    for (size_t i = 0; i < _classes.size(); ++i)
    {
        if (_classes[i].GetName() == object.GetName())
        {
            // Replace it.
            _classes[i] = object;
            return;
        }
    }
    ASSERT(FALSE); // Should always be found.
}

bool CSCOFile::GetClass(std::string className, const CSCOObjectClass **ppClass) const
{
    *ppClass = nullptr;
    std::vector<CSCOObjectClass>::const_iterator position = match_name_dot(_classes.begin(), _classes.end(), className);
    if (position != _classes.end())
    {
        *ppClass = &(*position);
        return true;
    }
    else
    {
        return false;
    }
}

CSCOObjectClass CSCOFile::GetObjectBySpecies(WORD species) const
{
    for (auto &theClass : _classes)
    {
        if (theClass.GetSpecies() == species)
        {
            return theClass;
        }
    }
    return CSCOObjectClass();
}

bool CSCOFile::operator==(const CSCOFile& value) const
{
    return (_bMajorVersion == value._bMajorVersion) &&
        (_bMinorVersion == value._bMinorVersion) &&
        (_bBuild == value._bBuild) &&
        (_bSCIVersion == value._bSCIVersion) &&
        (_bAlignment == value._bAlignment) &&
        (_wScriptNumber == value._wScriptNumber) &&
        (_publics == value._publics) &&
        (_classes == value._classes) &&
        (_vars == value._vars);

}
bool CSCOFile::operator!=(const CSCOFile& value) const
{
    return !((*this) == value);
}



void CSCOPublicExport::DebugOut(std::ostream &out) const
{
    out << _strName << " at " << _wProcIndex << endl;
}
void CSCOPublicExport::Save(std::vector<BYTE> &output) const
{
    // 1) index in this script
    // 2) name
    push_word(output, _wProcIndex);
    push_string(output, _strName);
}


void CSCOFile::DebugOut(std::ostream &out) const
{
    out << "SCO object file.\n";
    out << "version " << (WORD)_bMajorVersion << "-" << (WORD)_bMinorVersion << "-" << (WORD)_bBuild << "-" << (WORD)_bSCIVersion << "\n";
    out << "script " << _wScriptNumber << "\n\n";

    out << endl;
    out << dec << (WORD)_publics.size() << " exports at $" << hex << _wOffsetPublics << "\n";
    for (const CSCOPublicExport &publicExport : _publics)
    {
        publicExport.DebugOut(out);
    }

    out << endl;
    out << dec << (WORD)_classes.size() << " classes at $" << hex << _wOffsetClasses << "\n";
    for (const CSCOObjectClass &objectClass : _classes)
    {
        objectClass.DebugOut(out);
    }

    out << endl;
    out << dec << (WORD)_vars.size() << " variables at $" << hex << _wOffsetVars << "\n";
    for (const CSCOLocalVariable &localVar : _vars)
    {
        localVar.DebugOut(out);
    }
}

bool CSCOPublicExport::Create(sci::istream &stream)
{
    stream >> _wProcIndex;
    stream.getRLE(_strName);
    return stream.good();
}

void CSCOPublicExport::SetInstanceSpecies(SpeciesIndex si)
{
    ASSERT(_signatures.empty());
    CSCOFunctionSignature sig;
    sig.SetReturnType(si);
    _signatures.push_back(sig);
}

bool CSCOPublicExport::GetInstanceSpecies(SpeciesIndex &si) const
{
    if (_signatures.size() == 1) // REVIEW -> need a way to determine if this is an instance or procedure
    {
        si = _signatures[0].GetReturnType();
        return true;
    }
    return false; // Not an instance.
}

bool CSCOPublicExport::operator==(const CSCOPublicExport& value) const
{
    return (_strName == value._strName) &&
           (_wProcIndex == value._wProcIndex);
}
bool CSCOPublicExport::operator!=(const CSCOPublicExport& value) const
{
    return !((*this) == value);
}



bool CSCOLocalVariable::Create(sci::istream &stream)
{
    stream.getRLE(_strName);
    return stream.good();
}
bool CSCOLocalVariable::operator==(const CSCOLocalVariable& value) const
{
    return (_strName == value._strName);
}
bool CSCOLocalVariable::operator!=(const CSCOLocalVariable& value) const
{
    return !((*this) == value);
}
void CSCOLocalVariable::DebugOut(std::ostream &out) const
{
    out << _strName << "\n";
}
void CSCOLocalVariable::Save(std::vector<BYTE> &output) const
{
    push_string(output, _strName);
}

bool CSCOObjectProperty::Create(sci::istream &stream)
{
    stream >> _wNameIndex;
    stream >> _wValue;
    return stream.good();
}

bool CSCOObjectProperty::operator==(const CSCOObjectProperty& value) const
{
    return (_wNameIndex == value._wNameIndex) &&
        (_wValue == value._wValue);
}
bool CSCOObjectProperty::operator!=(const CSCOObjectProperty& value) const
{
    return !((*this) == value);
}
void CSCOObjectProperty::DebugOut(std::ostream &out) const
{
    out << _wNameIndex << " = " << _wValue << endl;
}
void CSCOObjectProperty::Save(std::vector<BYTE> &output) const
{
    push_word(output, _wNameIndex);
    push_word(output, _wValue);
}


bool CSCOObjectClass::Load(sci::istream &stream, SCOVersion version)
{
    stream.getRLE(_strName);

    WORD wNumPropsExcludingCore;
    WORD wNumMethods;
    stream >> wNumPropsExcludingCore;
    stream >> wNumMethods;
    stream >> _wSpecies;
    stream >> _wSuperClass;
    bool fRet = stream.good();
    if (fRet)
    {
        if (version == SCOVersion::SCI0)
        {
            // Put in the default 4 properties.
            _properties.push_back(CSCOObjectProperty(0, _wSpecies));        // 0) species
            _properties.push_back(CSCOObjectProperty(1, _wSuperClass));     // 1) superclass
            _properties.push_back(CSCOObjectProperty(2, 0x8000));           // 2) -info- always 0x8000 for classes
            _properties.push_back(CSCOObjectProperty(23, 0));               // 3) Name - meaningless here
            // TODO: look up the name selector?
        }
        else
        {
            // Put in the default 9 properties.
            _properties.push_back(CSCOObjectProperty(4096, 0));             // -objID-
            _properties.push_back(CSCOObjectProperty(4097, 0));             // -size-
            _properties.push_back(CSCOObjectProperty(4098, 0));             // -propDict-
            _properties.push_back(CSCOObjectProperty(4099, 0));             // -methDict-
            _properties.push_back(CSCOObjectProperty(4100, 0));             // -classScript-
            _properties.push_back(CSCOObjectProperty(4101, _wSpecies));     // -script- (but seems to be species)
            _properties.push_back(CSCOObjectProperty(4102, _wSuperClass));  // -super-
            _properties.push_back(CSCOObjectProperty(4103, 0x8000));        // -info-
            _properties.push_back(CSCOObjectProperty(20, 0));               // Name
        }
        if (wNumPropsExcludingCore != 0xffff)   // In case there are less props than the core. Some SCI1 are missing name.
        {
            for (WORD i = 0; fRet && i < wNumPropsExcludingCore; i++)
            {
                CSCOObjectProperty property;
                fRet = property.Create(stream);
                if (fRet)
                {
                    _properties.push_back(property);
                }
            }
        }
    }
    if (fRet)
    {
        for (WORD i = 0; stream.good() && i < wNumMethods; i++)
        {
            CSCOMethod method;
            fRet = method.Create(stream);
            if (fRet)
            {
                _methods.push_back(method);
            }
        }
    }
    return stream.good();
}

bool CSCOObjectClass::operator==(const CSCOObjectClass& value) const
{
    return (_strName == value._strName) &&
        (_wSpecies == value._wSpecies) &&
        (_wSuperClass == value._wSuperClass) &&
        (_methods == value._methods) &&
        (_properties == value._properties);
}
bool CSCOObjectClass::operator!=(const CSCOObjectClass& value) const
{
    return !((*this) == value);
}
void CSCOObjectClass::DebugOut(std::ostream &out) const
{
    out << "class " << _strName << endl;
    out << "    species: " << _wSpecies << "  super: " << _wSuperClass << endl;

    out << (WORD)_properties.size() << " properties:" << endl;
    for (const CSCOObjectProperty &prop : _properties)
    {
        prop.DebugOut(out);
    }
    out << endl;

    out << (WORD)_methods.size() << " methods:" << endl;
    for (const CSCOMethod &method : _methods)
    {
        method.DebugOut(out);
    }
   
    out << endl;
}

void CSCOObjectClass::Save(std::vector<BYTE> &output, SCOVersion version) const
{
    push_string(output, _strName);

    size_t numDefaultProps = (version == SCOVersion::SCI0) ? 4 : 9;

    // assert(_properties.size() >= numDefaultProps);
    // Sometimes the name property is left off (e.g. two classes in SQ5, script 948).
    // So instead, just trim:
    // numDefaultProps = min(numDefaultProps, _properties.size());
    // Oh wait, we can't, because we write the number of non-default props as the first thing
    // in the object section. So that would be negative.
    // Ok, well let's allow for -ve number there.
    assert(_properties.size() >= (numDefaultProps - 1));
    size_t numNonDefaultProps;
    if (_properties.size() < numDefaultProps)
    {
        numNonDefaultProps = 0xffff;    // -1
    }
    else
    {
        numNonDefaultProps = _properties.size() - numDefaultProps;
    }

    push_word(output, (WORD)numNonDefaultProps);
    push_word(output, (WORD)_methods.size());
    push_word(output, _wSpecies);
    push_word(output, _wSuperClass);

    // Skip the 4 default properties in _properties, and write the rest.
    if (numNonDefaultProps != 0xffff)
    {
        for_each(_properties.begin() + numDefaultProps, _properties.end(), FwdSave<CSCOObjectProperty>(output));
    }
    // Write the method selectors.
    for_each(_methods.begin(), _methods.end(), FwdSave<CSCOMethod>(output));
}


bool CSCOMethod::operator==(const CSCOMethod& value) const
{
    return (_wSelector == value._wSelector) &&
           (_signatures == value._signatures);
}
bool CSCOMethod::operator!=(const CSCOMethod& value) const
{
    return !((*this) == value);
}
void CSCOMethod::Save(std::vector<BYTE> &output) const
{
    push_word(output, _wSelector);
}
void CSCOMethod::DebugOut(std::ostream &out) const
{
    out << _wSelector << endl;
}
bool CSCOMethod::Create(sci::istream &stream)
{
    // 1) selector
    stream >> _wSelector;
    return stream.good();
}

bool CSCOFunctionSignature::operator==(const CSCOFunctionSignature& value) const
{
    return ((_wReturnType == value._wReturnType) &&
            (_parameters == value._parameters));
}
void CSCOFunctionSignature::Save(std::vector<BYTE> &output) const
{
}
bool CSCOFunctionSignature::Create(sci::istream &stream)
{
    return true;
}

void SaveSCOFile(const GameFolderHelper &helper, const CSCOFile &sco)
{
    // Ask the question
    std::string keyName = default_reskey(sco.GetScriptNumber(), NoBase36);
    std::string scriptTitle = helper.GetIniString("Script", keyName, keyName.c_str());
    ScriptId script = helper.GetScriptId(scriptTitle);
    SaveSCOFile(helper, sco, script);
}

void SaveSCOFile(const GameFolderHelper &helper, const CSCOFile &sco, ScriptId script)
{
    vector<BYTE> scoOutput;
    // First save the .sco file
    sco.Save(scoOutput);
    // Copy these bytes to a stream...
    std::string scoFileName = helper.GetScriptObjectFileName(script.GetTitle(), LangSyntaxStudio);
    ofstream scoFile(scoFileName.c_str(), ios::out | ios::binary);
    // REVIEW: yucky
    scoFile.write((const char *)&scoOutput[0], (std::streamsize)scoOutput.size());
    scoFile.close();
}

unique_ptr<CSCOFile> SCOFromScriptAndCompiledScript(const Script &script, const CompiledScript &compiledScript)
{
    unique_ptr<CSCOFile> sco = make_unique<CSCOFile>();
    sco->SetVersion(compiledScript.GetVersion().SeparateHeapResources ? SCOVersion::SeparateHeap : SCOVersion::SCI0);

    sco->SetScriptNumber(script.GetScriptNumber());

    // Local vars are easy
    std::vector<CSCOLocalVariable> &localVars = sco->GetVariables();
    for (auto &localVar : script.GetScriptVariables())
    {
        CSCOLocalVariable scoLocalVar;
        scoLocalVar.SetName(localVar->GetName());
        localVars.push_back(scoLocalVar);
    }

    // Classes
    std::vector<CSCOObjectClass> &scoObjects = sco->GetObjects();
    const vector<unique_ptr<CompiledObject>> &compiledObjects = compiledScript.GetObjects();
    unordered_map<string, CompiledObject*> nameToCompiledObject;
    for (const auto &compiledObject : compiledObjects)
    {
        nameToCompiledObject[compiledObject->GetName()] = compiledObject.get();
    }
    for (auto &object : script.GetClasses())
    {
        // We only care about classes
        if (!object->IsInstance())
        {
            CSCOObjectClass newSCOObject;
            CompiledObject *compiledObject = nameToCompiledObject[object->GetName()];
            // With object and compiledObject, we should have everything we need?
            newSCOObject.SetName(object->GetName());
            newSCOObject.SetPublic(compiledObject->IsPublic);   // REVIEW: When is a class not public?
            newSCOObject.SetSpecies(compiledObject->GetSpecies());
            newSCOObject.SetSuperClass(compiledObject->GetSuperClass());

            // Now the methods. All we care about are the selectors for the methods defined here.
            vector<CSCOMethod> &methods = newSCOObject.GetMethods();
            for (uint16_t methodSelector : compiledObject->GetMethods())
            {
                methods.emplace_back(methodSelector);
            }

            // And finally properties.
            vector<CSCOObjectProperty> &properties = newSCOObject.GetPropertiesNonConst();
            for (size_t i = 0; i < compiledObject->GetProperties().size(); i++)
            {
                properties.emplace_back(compiledObject->GetProperties()[i], compiledObject->GetPropertyValues()[i].value);
            }

            // Our object is complete.
            scoObjects.push_back(newSCOObject);
        }
    }

    // Now public procedures and instances. Get their names from the script first.
    // We assume the ordering in the script corresponds to the ordering in the compiled script.
    uint16_t exportIndex = 0;
    vector<string> publicInstanceNames;
    for (const auto &classDef : script.GetClasses())
    {
        //if (classDef->IsInstance() && classDef->IsPublic())
        if (classDef->IsPublic())   // SCI1.1 seems to have public classes too. Like the main game class.
        {
            publicInstanceNames.push_back(classDef->GetName());
        }
    }
    vector<string> publicProcNames;
    for (const auto &procDef : script.GetProcedures())
    {
        if (procDef->IsPublic())
        {
            publicProcNames.push_back(procDef->GetName());
        }
    }

    size_t instanceIndex = 0;
    size_t procIndex = 0;
    for (uint16_t exportOffset : compiledScript._exportsTO)
    {
        string exportName;
        if (compiledScript.IsExportAnObject(exportOffset))
        {
            if (instanceIndex < publicInstanceNames.size())
            {
                exportName = publicInstanceNames[instanceIndex++];
                sco->GetExports().emplace_back(exportName, exportIndex);
            }
            else
            {
                // SQ5, script 209 hits this. The same public instance appears multiple near
                // the end of the export table. No sense in writing it to the SCO multiple times.
                //exportName = publicInstanceNames.back();
            }
        }
        else if (compiledScript.IsExportAProcedure(exportOffset))
        {
            exportName = publicProcNames[procIndex++];
            sco->GetExports().emplace_back(exportName, exportIndex);
        }
        // Exports may be zero too. We won't write those to the SCO though.
        exportIndex++;
    }

    return sco;

}

unique_ptr<CSCOFile> GetExistingSCOFromScriptNumber(const GameFolderHelper &helper, uint16_t number)
{
    unique_ptr<CSCOFile> sco;
    string objectFilename = helper.GetScriptObjectFileName(number);
    if (!objectFilename.empty() && PathFileExists(objectFilename.c_str()))
    {
        ScopedFile scoped(objectFilename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
        sci::streamOwner streamOwner(scoped.hFile);
        sco = make_unique<CSCOFile>();
        sco->Load(streamOwner.getReader());
    }
    return sco;
}