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
#pragma once
#include "Types.h"

namespace sci
{
    class Script;
}


enum SCOVersion : uint8_t
{
    SCI0 = 0,
    SeparateHeap = 1,
};

//
// Object model for .sco object files.
//

//
// Includes both public procedures and public instances
//
class CSCOPublicExport
{
public:
    CSCOPublicExport() : _wProcIndex(0) {}
    CSCOPublicExport(const std::string &name, WORD wIndex) : _wProcIndex(wIndex), _strName(name) {}
    void Save(std::vector<BYTE> &output) const;
    bool Create(sci::istream &stream);
    const std::string &GetName() const { return _strName; }
    void SetName(const std::string &name) { _strName = name; }
    WORD GetIndex() const { return _wProcIndex; }
    bool operator==(const CSCOPublicExport& value) const;
    bool operator!=(const CSCOPublicExport& value) const;
    void DebugOut(std::ostream &out) const;

private:
    std::string _strName;   // Name
    WORD _wProcIndex;       // Index
};

class CSCOObjectProperty
{
public:
    CSCOObjectProperty() {}
    CSCOObjectProperty(WORD wSelector, WORD wValue, bool fTrackRelocation = false) : _wNameIndex(wSelector), _wValue(wValue), _fNeedsReloc(fTrackRelocation) {}
    bool operator==(const CSCOObjectProperty& value) const;
    bool operator!=(const CSCOObjectProperty& value) const;
    void Save(std::vector<BYTE> &output) const;
    void DebugOut(std::ostream &out) const;

    bool Create(sci::istream &stream);
    WORD GetSelector() const { return _wNameIndex; }
    WORD GetValue() const { return _wValue; }
    bool NeedsReloc() const { return _fNeedsReloc; }

private:
    WORD _wNameIndex;
    WORD _wValue;
    bool _fNeedsReloc;
};

class CSCOObjectClass
{
public:
    CSCOObjectClass()
    {
        // Initialize to degenerate case.
        _wSpecies = DataTypeNone;
        _wSuperClass = DataTypeNone;
        _fPublic = false;
    }
    bool operator==(const CSCOObjectClass& value) const;
    bool operator!=(const CSCOObjectClass& value) const;
    void Save(std::vector<BYTE> &output, SCOVersion version) const;
    void DebugOut(std::ostream &out) const;

    const std::string &GetName() const { return _strName; }
    bool Load(sci::istream &stream, SCOVersion version);
    const std::vector<uint16_t> &GetMethods() const { return _methods; }
    std::vector<uint16_t> &GetMethods() { return _methods; }

    // The properties include the default four properties (redundant, a bit)
    const std::vector<CSCOObjectProperty> &GetProperties() const { return _properties; }
    std::vector<CSCOObjectProperty> &GetPropertiesNonConst() { return _properties; }

    void SetName(std::string name) { _strName = name; }
    void SetSpecies(WORD wSpecies) { _wSpecies = wSpecies; }
    WORD GetSpecies() const { return _wSpecies; }
    void SetSuperClass(WORD wSuper) { _wSuperClass = wSuper; }
    WORD GetSuperClass() const { return _wSuperClass; }
    void SetMethods(const std::vector<uint16_t> &methods) { _methods = methods; }
    void SetProperties(std::vector<CSCOObjectProperty> &properties) { _properties = properties; }
    void SetPublic(bool fPublic) { _fPublic = fPublic; }
    bool IsPublic() const { return _fPublic; }
 
private:
    // Real data
    std::vector<uint16_t> _methods;
    std::vector<CSCOObjectProperty> _properties;
    std::string _strName;
    WORD _wSpecies;
    WORD _wSuperClass;

    // Handy things:
    bool _fPublic;
};

class CSCOLocalVariable
{
public:
    CSCOLocalVariable() {}
    CSCOLocalVariable(const std::string &name) : _strName(name) {}
    bool operator==(const CSCOLocalVariable& value) const;
    bool operator!=(const CSCOLocalVariable& value) const;
    void Save(std::vector<BYTE> &output) const;
    void DebugOut(std::ostream &out) const;
    const std::string &GetName() const { return _strName; }
    bool Create(sci::istream &stream);
    void SetName(const std::string &name) { _strName = name; }

private:
    std::string _strName;
};


//
// This represents all the information in an .sco file
// 
class CSCOFile
{
public:
    CSCOFile()
    {
        _bMajorVersion = 1;
        _bMinorVersion = 0;
        _bBuild = 0;
        _bSCIVersion = SCOVersion::SCI0;
        _wScriptNumber = InvalidResourceNumber;
        _wOffsetPublics = 0;
        _wOffsetClasses = 0;
        _wOffsetVars = 0;
        _bAlignment = 0;
    }
    bool operator==(const CSCOFile& value) const;
    bool operator!=(const CSCOFile& value) const;
    void DebugOut(std::ostream &out) const;
    void Save(std::vector<BYTE> &output) const;

    bool Load(sci::istream &stream);
    void SetVersion(SCOVersion version) { _bSCIVersion = version; }
    std::string GetVariableName(WORD wIndex)  const;
    std::string GetExportName(WORD wIndex)  const;
    std::string GetClassName(WORD wIndex) const;
    bool GetVariableIndex(const std::string &name, WORD &wIndex) const;
    bool GetExportIndex(const std::string &name, WORD &wIndex) const;
    bool GetPublicExportByName(const std::string &exportName, CSCOPublicExport &theExport) const;
    bool GetClassSpecies(std::string, SpeciesIndex &species) const;
    bool GetClass(std::string className, const CSCOObjectClass **ppClass) const;
    WORD GetScriptNumber() const { return _wScriptNumber; }
    const std::vector<CSCOObjectClass> &GetObjects() const { return _classes; }
    std::vector<CSCOObjectClass> &GetObjects() { return _classes; }
    CSCOObjectClass GetObjectBySpecies(WORD species) const;
    bool IsEmpty() const { return _wScriptNumber == InvalidResourceNumber; }
    const std::vector<CSCOPublicExport> &GetExports() const { return _publics; }
    std::vector<CSCOPublicExport> &GetExports() { return _publics; }
    const std::vector<CSCOLocalVariable> &GetVariables() const { return _vars; }
    std::vector<CSCOLocalVariable> &GetVariables() { return _vars; }

    // Modifiers
    void AddObject(const CSCOObjectClass &object) { _classes.push_back(object); }
    void AddPublic(const CSCOPublicExport &proc) { _publics.push_back(proc); }
    void AddVariable(const CSCOLocalVariable &var) { _vars.push_back(var); }
    void SetScriptNumber(WORD w) { _wScriptNumber = w; }

    // A bit of a hack
    void ReplaceObject(const CSCOObjectClass &object);

private:
    BYTE _bMajorVersion;
    BYTE _bMinorVersion;
    BYTE _bBuild;
    SCOVersion _bSCIVersion;
    BYTE _bAlignment;

    WORD _wScriptNumber;

    // Publics index is contained with the CSCOPublicExport.
    std::vector<CSCOPublicExport> _publics;
    std::vector<CSCOObjectClass> _classes;
    // Variable index is implied by order.
    std::vector<CSCOLocalVariable> _vars;

    // "Irrelevant"
    WORD _wOffsetPublics;
    WORD _wOffsetClasses;
    WORD _wOffsetVars;
};

class GameFolderHelper;
void SaveSCOFile(const GameFolderHelper &helper, const CSCOFile &sco, ScriptId script);
void SaveSCOFile(const GameFolderHelper &helper, const CSCOFile &sco);

class CompiledScript;
std::unique_ptr<CSCOFile> SCOFromScriptAndCompiledScript(const sci::Script &script, const CompiledScript &compiledScript);
std::unique_ptr<CSCOFile> GetExistingSCOFromScriptNumber(const GameFolderHelper &helper, uint16_t number);
