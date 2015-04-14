#include "stdafx.h"
#include "SCO.h"

using namespace std;

bool _IsSCOPFile(const std::string &scoFileName)
{
    return (scoFileName.find(".scop") != std::string::npos);
}


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
    FwdSave(bool fIncludesTypeInfo, vector<BYTE> &output) : _output(output), _fIncludesTypeInfo(fIncludesTypeInfo) {}
    void operator()(const _T &thing)
    {
        thing.Save(_fIncludesTypeInfo, _output);
    }
private:
    vector<BYTE> &_output;
    bool _fIncludesTypeInfo;
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


bool CSCOFile::Create(bool fIncludesTypeInfo, sci::istream &stream)
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
                    if (procedure.Create(fIncludesTypeInfo, stream))
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
                            fRet = objectClass.Create(fIncludesTypeInfo, stream);
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
                        for (WORD i = 0; fRet && i < wTotalVars; i++)
                        {
                            CSCOLocalVariable var;
                            fRet = var.Create(fIncludesTypeInfo, stream);
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

void CSCOFile::Save(bool fIncludesTypeInfo, vector<BYTE> &output) const
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
        for_each(_publics.begin(), _publics.end(), FwdSave<CSCOPublicExport>(fIncludesTypeInfo, output));
    }

    if (!_classes.empty())
    {
        write_word(output, wOffsetClasses, (WORD)output.size());
        for_each(_classes.begin(), _classes.end(), FwdSave<CSCOObjectClass>(fIncludesTypeInfo, output));
    }

    if (!_vars.empty())
    {
        write_word(output, wOffsetVars, (WORD)output.size());
        for_each(_vars.begin(), _vars.end(), FwdSave<CSCOLocalVariable>(fIncludesTypeInfo, output));
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
    if (wIndex < _publics.size())
    {
        return _publics[wIndex].GetName();
    }
    else
    {
        return "";
    }
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
    bool fRet = GetItemIndex(_publics, exportName, wIndex);
    /*if (fRet)
    {
        ASSERT(wIndex == _publics[wIndex].GetIndex()); // Does index in _publics ever != the index in the object itself?
    }*/
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
    ASSERT(wIndex < _publics.size());
    return _publics[wIndex].GetSignatures();
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
void CSCOPublicExport::Save(bool fIncludesTypeInfo, std::vector<BYTE> &output) const
{
    // 1) index in this script
    // 2) name
    push_word(output, _wProcIndex);
    push_string(output, _strName);
    if (fIncludesTypeInfo)
    {
        // 3) number of signatures
        // 4) n signatures...
        push_word(output, static_cast<WORD>(_signatures.size()));
        for_each(_signatures.begin(), _signatures.end(), FwdSave<CSCOFunctionSignature>(fIncludesTypeInfo, output));
    }
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

bool CSCOPublicExport::Create(bool fIncludesTypeInfo, sci::istream &stream)
{
    stream >> _wProcIndex;
    stream.getRLE(_strName);
    if (fIncludesTypeInfo)
    {
        WORD cSigs;
        stream >> cSigs;
        for (WORD i = 0; i < cSigs; ++i)
        {
            CSCOFunctionSignature sig;
            sig.Create(fIncludesTypeInfo, stream);
            _signatures.push_back(sig);
        }
    }
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



bool CSCOLocalVariable::Create(bool fIncludesTypeInfo, sci::istream &stream)
{
    stream.getRLE(_strName);
    if (fIncludesTypeInfo)
    {
        stream >> _wType;
    }
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
void CSCOLocalVariable::Save(bool fIncludesTypeInfo, std::vector<BYTE> &output) const
{
    push_string(output, _strName);
    if (fIncludesTypeInfo)
    {
        push_word(output, _wType);
    }
}

bool CSCOObjectProperty::Create(bool fIncludesTypeInfo, sci::istream &stream)
{
    stream >> _wNameIndex;
    stream >> _wValue;
    if (fIncludesTypeInfo)
    {
        stream >> _wType;
    }
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
void CSCOObjectProperty::Save(bool fIncludesTypeInfo, std::vector<BYTE> &output) const
{
    push_word(output, _wNameIndex);
    push_word(output, _wValue);
    if (fIncludesTypeInfo)
    {
        push_word(output, _wType);
    }
}


bool CSCOObjectClass::Create(bool fIncludesTypeInfo, sci::istream &stream)
{
    stream.getRLE(_strName);

    WORD wNumProps;
    WORD wNumMethods;
    stream >> wNumProps;
    stream >> wNumMethods;
    stream >> _wSpecies;
    stream >> _wSuperClass;
    bool fRet = stream.good();
    if (fRet)
    {
        // Put in the default 4 properties.
        _properties.push_back(CSCOObjectProperty(0, _wSpecies));        // 0) species
        _properties.push_back(CSCOObjectProperty(1, _wSuperClass));     // 1) superclass
        _properties.push_back(CSCOObjectProperty(2, 0x8000));           // 2) -info- always 0x8000 for classes
        _properties.push_back(CSCOObjectProperty(23, 0));               // 3) Name - meaningless here
        // TODO: look up the name selector?
        for (WORD i = 0; fRet && i < wNumProps; i++)
        {
            CSCOObjectProperty property;
            fRet = property.Create(fIncludesTypeInfo, stream);
            if (fRet)
            {
                _properties.push_back(property);
            }
        }
    }
    if (fRet)
    {
        for (WORD i = 0; stream.good() && i < wNumMethods; i++)
        {
            CSCOMethod method;
            fRet = method.Create(fIncludesTypeInfo, stream);
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
#define NUM_DEFAULT_PROPS 4
void CSCOObjectClass::Save(bool fIncludesTypeInfo, std::vector<BYTE> &output) const
{
    push_string(output, _strName);

    ASSERT(_properties.size() >= NUM_DEFAULT_PROPS);
    push_word(output, (WORD)(_properties.size() - NUM_DEFAULT_PROPS));
    push_word(output, (WORD)_methods.size());
    push_word(output, _wSpecies);
    push_word(output, _wSuperClass);

    // Skip the 4 default properties in _properties, and write the rest.
    for_each(_properties.begin() + NUM_DEFAULT_PROPS, _properties.end(), FwdSave<CSCOObjectProperty>(fIncludesTypeInfo, output));
    // Write the method selectors.
    for_each(_methods.begin(), _methods.end(), FwdSave<CSCOMethod>(fIncludesTypeInfo, output));
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
void CSCOMethod::Save(bool fIncludesTypeInfo, std::vector<BYTE> &output) const
{
    push_word(output, _wSelector);
    if (fIncludesTypeInfo)
    {
        push_word(output, static_cast<WORD>(_signatures.size()));
        for_each(_signatures.begin(), _signatures.end(), FwdSave<CSCOFunctionSignature>(fIncludesTypeInfo, output));
    }
}
void CSCOMethod::DebugOut(std::ostream &out) const
{
    out << _wSelector << endl;
}
bool CSCOMethod::Create(bool fIncludesTypeInfo, sci::istream &stream)
{
    // 1) selector
    stream >> _wSelector;

    if (fIncludesTypeInfo)
    {
        // 2) # of method signatures
        // 3) n method signatures
        WORD cSignatures;
        stream >> cSignatures;
        for (WORD i = 0; i < cSignatures; ++i)
        {
            CSCOFunctionSignature sig;
            sig.Create(fIncludesTypeInfo, stream);
            _signatures.push_back(sig);
        }
    }
    return stream.good();
}

bool CSCOFunctionSignature::operator==(const CSCOFunctionSignature& value) const
{
    return ((_wReturnType == value._wReturnType) &&
            (_parameters == value._parameters));
}
void CSCOFunctionSignature::Save(bool fIncludesTypeInfo, std::vector<BYTE> &output) const
{
    if (fIncludesTypeInfo)
    {
        // 1) Return type
        // 2) Number of parameters
        // 3) Parameter types (c 2)
        // 4) The number of required parameters
        // 5) Are additional parameters ok (0 or !0, WORD)
        push_word(output, _wReturnType);
        push_word(output, static_cast<WORD>(_parameters.size()));
        for(WORD wParameter : _parameters)
        {
            push_word(output, wParameter);
        }
        push_word(output, _wRequiredParameters);
        push_word(output, _fAdditionalParameters ? 0xffff : 0);
    }
}
bool CSCOFunctionSignature::Create(bool fIncludesTypeInfo, sci::istream &stream)
{
    if (fIncludesTypeInfo)
    {
        stream >> _wReturnType;
        WORD cParameters;
        stream >> cParameters;
        for (WORD i = 0; i < cParameters; ++i)
        {
            WORD w;
            stream >> w;
            _parameters.push_back(w);
        }
        stream >> _wRequiredParameters;
        WORD wAdditionalOk;
        stream >> wAdditionalOk;
        _fAdditionalParameters = (wAdditionalOk != 0);
        ASSERT((wAdditionalOk == 0) || (wAdditionalOk == 0xffff));
    }
    return stream.good();
}

