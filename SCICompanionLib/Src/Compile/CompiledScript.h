#pragma once
#include "interfaces.h"
#include "Vocab99x.h"
#include "PMachine.h"
#include "SCO.h"
#include "DisassembleHelper.h"

namespace sci
{
	class Script;
	class FunctionBase;
}
class DecompileLookups;

//
// Information gleaned from the actual script resources.
//
class ICompiledScriptLookups
{
public:
    virtual std::string LookupSelectorName(uint16_t wIndex) = 0;
    virtual std::string LookupKernelName(uint16_t wIndex) = 0;
    virtual std::string LookupClassName(uint16_t wIndex) = 0;
    virtual bool LookupSpeciesPropertyList(uint16_t wIndex, std::vector<uint16_t> &props) = 0;
    virtual bool LookupSpeciesPropertyListAndValues(uint16_t wIndex, std::vector<uint16_t> &props, std::vector<uint16_t> &values) = 0;
};

class ICompiledScriptSpecificLookups
{
public:
    enum ObjectType
    {
        ObjectTypeSaid,
        ObjectTypeString,
        ObjectTypeClass,
    };
    virtual std::string LookupObjectName(uint16_t wOffset, ObjectType &type) const = 0;
};

class IPrivateSpeciesLookups
{
public:
    virtual std::string LookupClassName(uint16_t wIndex) = 0;
    virtual bool LookupSpeciesPropertyList(uint16_t wIndex, std::vector<uint16_t> &props) = 0;
    virtual bool LookupSpeciesPropertyListAndValues(uint16_t wIndex, std::vector<uint16_t> &props, std::vector<uint16_t> &values) = 0;
};


//
// Information gleaned from the .sco object files that the compiler generates for linking.
//
class IObjectFileScriptLookups
{
public:
    virtual std::string ReverseLookupGlobalVariableName(uint16_t wIndex) = 0;
    virtual std::string ReverseLookupPublicExportName(uint16_t wScript, uint16_t wIndex) = 0;
};



class GlobalCompiledScriptLookups : public ICompiledScriptLookups
{
public:
    bool Load(SCIVersion version);
    std::string LookupSelectorName(uint16_t wIndex);
    std::string LookupKernelName(uint16_t wIndex);
    std::string LookupClassName(uint16_t wIndex);
    bool LookupSpeciesPropertyList(uint16_t wIndex, std::vector<uint16_t> &props);
    bool LookupSpeciesPropertyListAndValues(uint16_t wIndex, std::vector<uint16_t> &props, std::vector<uint16_t> &values);

    SelectorTable &GetSelectorTable() { return _selectors; }
    GlobalClassTable &GetGlobalClassTable() { return _classes; }

private:
    SelectorTable       _selectors;
    KernelTable         _kernels;
    GlobalClassTable    _classes;
};

class ObjectFileScriptLookups : public IObjectFileScriptLookups
{
public:
    std::string ReverseLookupGlobalVariableName(uint16_t wIndex);
    std::string ReverseLookupPublicExportName(uint16_t wScript, uint16_t wIndex);

private:
    bool _GetSCOFile(uint16_t wScript, CSCOFile &scoFile);
    bool _LoadSCOFile(uint16_t wScript);
	std::unordered_map<uint16_t, CSCOFile> _mapScriptToObject;
};

class ILookupPropertyName
{
public:
    virtual std::string LookupPropertyName(ICompiledScriptLookups *pLookup, uint16_t wPropertyIndex) const = 0;
};

class CompiledObjectBase : public ILookupPropertyName
{
public:
    CompiledObjectBase() { _fInstance = false; IsPublic = false; }
    bool IsInstance() const { return _fInstance; }
    bool Create(SCIVersion version, sci::istream &stream, BOOL fClass, uint16_t *pwOffset);
    bool Create_SCI1_1(SCIVersion version, sci::istream scriptStream, sci::istream &heapStream, uint16_t *pwOffset);
    std::string GetName() const { return _strName; }
    void SetName(PCTSTR pszName) { _strName = pszName; }
    uint16_t GetSuperClass() const { return _wSuperClass; }
    uint16_t GetSpecies() const
    {
        // In SCI0, and SCI1, _wSpecies and _wSuperClass are the same for instances.
        // In SCI11, _wSpecies is 0xffff for instances. So I think it's reasonable to
        // assume that we can just always use _wSuperClass if _fInstance is true.
        return _fInstance ? _wSuperClass : _wSpeciesIfClass;
    }
    uint16_t GetSpeciesIfClass() const { return _wSpeciesIfClass; }
    uint16_t GetInfo() const { return _wInfo; }

    void AdjustName(PCTSTR pszNewName) { ASSERT(!_fInstance); _strName = pszNewName; }
    const std::vector<uint16_t> &GetProperties() const { return _propertySelectors; }
    const std::vector<uint16_t> &GetMethods() const { return _functionSelectors; }
    const std::vector<uint16_t> &GetPropertyValues() const{ return _propertyValues; }
    const std::vector<uint16_t> &GetMethodCodePointersTO() const { return _functionOffsetsTO; }

    // ILookupPropertyName
    std::string LookupPropertyName(ICompiledScriptLookups *pLookup, uint16_t wPropertyIndex) const override;

    uint16_t GetPosInResource() const { return _wPosInResource; }

    int GetNumberOfDefaultSelectors() const { return _version.SeparateHeapResources ? 9 : 4; }

    SCIVersion GetVersion() const { return _version; }

    bool IsPublic;

protected:

    uint16_t _wSpeciesIfClass;
    uint16_t _wSuperClass;
    std::string _strName;
    uint16_t _wInfo;
    // These start from the 4th position (e.g. leave out species, superclass, --info-- and name)
    std::vector<uint16_t> _propertySelectors;
    std::vector<uint16_t> _propertyValues;
    std::vector<uint16_t> _functionSelectors;      // selectors for the methods
    std::vector<uint16_t> _functionOffsetsTO;
    bool _fInstance;
    uint16_t _wPosInResource;
    // If anything else is added, make sure to add it to the = operator.

    SCIVersion _version;
};

struct ScriptSection
{
	uint16_t offset;
	uint16_t type;
	uint16_t length;
};

//
// This represents all the information in a compiled script resources
//
class CompiledScript : /*public ILookupNames, */ public IPrivateSpeciesLookups, public ICompiledScriptSpecificLookups
{
public:
    CompiledScript(const CompiledScript &src) = delete;
    CompiledScript(uint16_t wScript) { _wScript = wScript; }
    bool Load(SCIVersion version, int iScriptNumber, bool quick);
    bool Load(SCIVersion version, int iScriptNumber, sci::istream &byteStream);
    std::vector<std::unique_ptr<CompiledObjectBase>> &GetObjects() { return _objects; }
    const std::vector<std::unique_ptr<CompiledObjectBase>> &GetObjects() const { return _objects; }
    uint16_t GetScriptNumber() const { return _wScript; }

    // ICompiledScriptSpecificLookups
    std::string LookupObjectName(uint16_t wOffset, ObjectType &type) const;

    // IPrivateSpeciesLookups
    std::string LookupClassName(uint16_t wIndex);
    bool LookupSpeciesPropertyList(uint16_t wIndex, std::vector<uint16_t> &props);
    bool LookupSpeciesPropertyListAndValues(uint16_t wIndex, std::vector<uint16_t> &props, std::vector<uint16_t> &values);

    const std::vector<uint8_t> &GetRawBytes() const { return _scriptResource; }
    const uint8_t *GetEndOfRawBytes() const { return &_scriptResource[0] + _scriptResource.size(); }
    bool IsExportAnObject(uint16_t wOffset) const;
    bool IsExportAProcedure(uint16_t wOffset) const;
    std::set<uint16_t> FindInternalCallsTO() const;

    void PopulateSaidStrings(const ILookupNames *pWords) const;

    // TODO: Make these have public names
    std::vector<uint16_t> _localVars;
    std::vector<std::unique_ptr<CompiledObjectBase>> _objects;
    std::vector<uint16_t> _objectsOffsetTO;
    // When loading, we'll also just provide the raw data for stuff. Size, etc...
    std::vector<ScriptSection> _rawScriptSections;
    std::vector<uint16_t> _exportsTO;
    mutable std::vector<std::string> _saidStrings;
    std::vector<std::vector<uint16_t> > _saids;
    std::vector<std::string> _strings;
    std::vector<uint16_t> _stringsOffset;
    std::vector<uint16_t> _saidsOffset;
    std::unordered_map<uint16_t, uint16_t> _synonyms;
    std::vector<CodeSection> _codeSections;
    std::vector<uint16_t> _exportedObjectInstances;

    SCIVersion GetVersion() const { return _version; }

private:
    bool _LoadSCI0_SCI1(sci::istream &byteStream);
    bool _LoadSCI1_1(int iScriptNumber, sci::istream &byteStream);

    bool _ReadExports(sci::istream &stream);
    bool _ReadStrings(sci::istream &stream, uint16_t wDataSize);
    bool _ReadSaids(sci::istream &stream, uint16_t wDataSize);
    CompiledObjectBase *_FindObjectWithSpecies(uint16_t wIndex);

    uint16_t _wScript;
    BOOL _fPreloadText;

    std::vector<BYTE> _scriptResource;

    SCIVersion _version;
};

int GetOperandSize(BYTE bOpcode, OperandType operandType);
uint16_t CalcOffset(uint16_t wOperandStart, uint16_t wRelOffset, bool bByte, BYTE bRawOpcode);
