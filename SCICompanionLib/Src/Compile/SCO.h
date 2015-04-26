#pragma once
#include "Types.h"

//
// Object model for .sco object files.
//

class CSCOFunctionSignature
{
public:
    CSCOFunctionSignature() : _wReturnType(DataTypeAny), _wRequiredParameters(0), _fAdditionalParameters(true) {}
    void Save(std::vector<BYTE> &output) const;
    bool operator==(const CSCOFunctionSignature& value) const;
    bool Create(sci::istream &stream);

    SpeciesIndex GetReturnType() const { return _wReturnType; }
    void SetReturnType(SpeciesIndex w) { _wReturnType = w; }
    const std::vector<SpeciesIndex> &GetParameters() const { return _parameters; }
    void AddParameterType(SpeciesIndex w) { _parameters.push_back(w); }
    WORD GetParameterCount() const { return static_cast<WORD>(_parameters.size()); }
    WORD GetRequiredParameterCount() const { return _wRequiredParameters; }
    void SetRequiredParameterCount(WORD w) { _wRequiredParameters = w; ASSERT(w <= _parameters.size()); }
    bool GetAdditionalParametersOk() const { return _fAdditionalParameters; }
    void SetAdditionalParametersOk(bool b) { _fAdditionalParameters = b; }

private:
    SpeciesIndex _wReturnType;
    std::vector<SpeciesIndex> _parameters;      // If none present, assume anything goes.
    WORD _wRequiredParameters;                  // How many parameters are required
    bool _fAdditionalParameters;                // Are additional unspecified parameters ok
};

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
    WORD GetIndex() const { return _wProcIndex; }
    bool operator==(const CSCOPublicExport& value) const;
    bool operator!=(const CSCOPublicExport& value) const;
    void DebugOut(std::ostream &out) const;

    // Procedures
    const std::vector<CSCOFunctionSignature> &GetSignatures() const { return _signatures; }
    void SetSignatures(const std::vector<CSCOFunctionSignature> &signatures) { _signatures = signatures; }

    // Instances
    void SetInstanceSpecies(SpeciesIndex si);
    bool GetInstanceSpecies(SpeciesIndex &si) const;

private:
    std::string _strName;   // Name
    WORD _wProcIndex;       // Index

    // Cpp only
    // Applies to procedures
    // Also applies to instances.  In that case there is one signature whose return type is the instance species,
    // and the function name is empty.
    std::vector<CSCOFunctionSignature> _signatures;

};

class CSCOObjectProperty
{
public:
    CSCOObjectProperty() : _wType(DataTypeAny) {}
    CSCOObjectProperty(WORD wSelector, WORD wValue, bool fTrackRelocation = false) : _wNameIndex(wSelector), _wValue(wValue), _wType(DataTypeAny), _fNeedsReloc(fTrackRelocation) {}
    CSCOObjectProperty(WORD wSelector, WORD wValue, WORD wType, bool fTrackRelocation = false) : _wNameIndex(wSelector), _wValue(wValue), _wType(wType), _fNeedsReloc(fTrackRelocation) {}
    bool operator==(const CSCOObjectProperty& value) const;
    bool operator!=(const CSCOObjectProperty& value) const;
    void Save(std::vector<BYTE> &output) const;
    void DebugOut(std::ostream &out) const;

    bool Create(sci::istream &stream);
    WORD GetSelector() const { return _wNameIndex; }
    WORD GetValue() const { return _wValue; }
    WORD GetType() const { return _wType; }
    bool NeedsReloc() const { return _fNeedsReloc; }

private:
    WORD _wNameIndex;
    WORD _wValue;
    WORD _wType;    // Cpp only
    bool _fNeedsReloc;
};

class CSCOMethod
{
public:
    bool operator==(const CSCOMethod& value) const;
    bool operator!=(const CSCOMethod& value) const;
    void Save(std::vector<BYTE> &output) const;
    void DebugOut(std::ostream &out) const;
    bool Create(sci::istream &stream);

    void SetSelector(WORD wSelector) { _wSelector = wSelector; }
    WORD GetSelector() const { return _wSelector; }

    const std::vector<CSCOFunctionSignature> &GetSignatures() const { return _signatures; }
    void SetSignatures(const std::vector<CSCOFunctionSignature> &signatures) { _signatures = signatures; }
    
private:
    WORD _wSelector;
    std::vector<CSCOFunctionSignature> _signatures;
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
    void Save(std::vector<BYTE> &output) const;
    void DebugOut(std::ostream &out) const;

    const std::string &GetName() const { return _strName; }
    bool Create(sci::istream &stream);
    const std::vector<CSCOMethod> &GetMethods() const { return _methods; }

    // The properties include the default four properties (redundant, a bit)
    const std::vector<CSCOObjectProperty> &GetProperties() const { return _properties; }
    std::vector<CSCOObjectProperty> &GetPropertiesNonConst() { return _properties; }

    void SetName(std::string name) { _strName = name; }
    void SetSpecies(WORD wSpecies) { _wSpecies = wSpecies; }
    WORD GetSpecies() const { return _wSpecies; }
    void SetSuperClass(WORD wSuper) { _wSuperClass = wSuper; }
    WORD GetSuperClass() const { return _wSuperClass; }
    void SetMethods(const std::vector<CSCOMethod> &methods) { _methods = methods; }
    void SetProperties(std::vector<CSCOObjectProperty> &properties) { _properties = properties; }
    void SetPublic(bool fPublic) { _fPublic = fPublic; }
    bool IsPublic() { return _fPublic; }
 
private:
    // Real data
    std::vector<CSCOMethod> _methods;
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
    CSCOLocalVariable() : _wType(DataTypeAny) {}
    CSCOLocalVariable(const std::string &name) : _strName(name), _wType(DataTypeAny) {}
    CSCOLocalVariable(const std::string &name, WORD wType) : _strName(name), _wType(wType) {}
    bool operator==(const CSCOLocalVariable& value) const;
    bool operator!=(const CSCOLocalVariable& value) const;
    void Save(std::vector<BYTE> &output) const;
    void DebugOut(std::ostream &out) const;
    const std::string &GetName() const { return _strName; }
    const WORD GetType() const { return _wType; }
    bool Create(sci::istream &stream);

private:
    std::string _strName;
    WORD _wType;    // Cpp only
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
        _bSCIVersion = 0;
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

    bool Create(sci::istream &stream);
    std::string GetVariableName(WORD wIndex)  const;
    std::string GetExportName(WORD wIndex)  const;
    std::string GetClassName(WORD wIndex) const;
    bool GetVariableIndex(const std::string &name, WORD &wIndex, WORD &wType) const;
    bool GetExportIndex(const std::string &name, WORD &wIndex) const;
    bool GetPublicExportByName(const std::string &exportName, CSCOPublicExport &theExport) const;
    std::vector<CSCOFunctionSignature> GetExportSignatures(WORD wIndex) const;
    bool GetClassIndex(std::string, WORD &wIndex) const;
    bool GetClass(std::string className, const CSCOObjectClass **ppClass) const;
    WORD GetScriptNumber() const { return _wScriptNumber; }
    const std::vector<CSCOObjectClass> &GetObjects() const { return _classes; }
    std::vector<CSCOObjectClass> &GetObjects() { return _classes; }
    CSCOObjectClass GetObjectBySpecies(WORD species) const;
    bool IsEmpty() const { return _wScriptNumber == InvalidResourceNumber; }
    const std::vector<CSCOPublicExport> &GetExports() const { return _publics; }

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
    BYTE _bSCIVersion;
    BYTE _bAlignment; // ARCHITECTURE: SCIStudio doesn't check the alignment - we could use this as a clue
                      // that there is extra info (e.g. data type)

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

