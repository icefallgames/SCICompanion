#include "stdafx.h"
#include "AppState.h"
#include "Vocab99x.h"
#include "ResourceMap.h"
#include "CompiledScript.h"

const static int VocabClassTable = 996;
const static int VocabSelectorNames = 997;
const static int VocabKernelNames = 999;

using namespace std;

ResourceBlob *_GetVocabData(int iVocab)
{
    return appState->GetResourceMap().MostRecentResource(ResourceType::Vocab, iVocab, false).release();
}

// Default kernal name table, taken from ScummVM source.
// We use vocab999, and if that's not present, then we use this.
static const char *const s_defaultKernelNames[] = {
    /*0x00*/ "Load",
    /*0x01*/ "UnLoad",
    /*0x02*/ "ScriptID",
    /*0x03*/ "DisposeScript",
    /*0x04*/ "Clone",
    /*0x05*/ "DisposeClone",
    /*0x06*/ "IsObject",
    /*0x07*/ "RespondsTo",
    /*0x08*/ "DrawPic",
    /*0x09*/ "Show",
    /*0x0a*/ "PicNotValid",
    /*0x0b*/ "Animate",
    /*0x0c*/ "SetNowSeen",
    /*0x0d*/ "NumLoops",
    /*0x0e*/ "NumCels",
    /*0x0f*/ "CelWide",
    /*0x10*/ "CelHigh",
    /*0x11*/ "DrawCel",
    /*0x12*/ "AddToPic",
    /*0x13*/ "NewWindow",
    /*0x14*/ "GetPort",
    /*0x15*/ "SetPort",
    /*0x16*/ "DisposeWindow",
    /*0x17*/ "DrawControl",
    /*0x18*/ "HiliteControl",
    /*0x19*/ "EditControl",
    /*0x1a*/ "TextSize",
    /*0x1b*/ "Display",
    /*0x1c*/ "GetEvent",
    /*0x1d*/ "GlobalToLocal",
    /*0x1e*/ "LocalToGlobal",
    /*0x1f*/ "MapKeyToDir",
    /*0x20*/ "DrawMenuBar",
    /*0x21*/ "MenuSelect",
    /*0x22*/ "AddMenu",
    /*0x23*/ "DrawStatus",
    /*0x24*/ "Parse",
    /*0x25*/ "Said",
    /*0x26*/ "SetSynonyms",	// Portrait (KQ6 hires)
    /*0x27*/ "HaveMouse",
    /*0x28*/ "SetCursor",
    // FOpen (SCI0)
    // FPuts (SCI0)
    // FGets (SCI0)
    // FClose (SCI0)
    /*0x29*/ "SaveGame",
    /*0x2a*/ "RestoreGame",
    /*0x2b*/ "RestartGame",
    /*0x2c*/ "GameIsRestarting",
    /*0x2d*/ "DoSound",
    /*0x2e*/ "NewList",
    /*0x2f*/ "DisposeList",
    /*0x30*/ "NewNode",
    /*0x31*/ "FirstNode",
    /*0x32*/ "LastNode",
    /*0x33*/ "EmptyList",
    /*0x34*/ "NextNode",
    /*0x35*/ "PrevNode",
    /*0x36*/ "NodeValue",
    /*0x37*/ "AddAfter",
    /*0x38*/ "AddToFront",
    /*0x39*/ "AddToEnd",
    /*0x3a*/ "FindKey",
    /*0x3b*/ "DeleteKey",
    /*0x3c*/ "Random",
    /*0x3d*/ "Abs",
    /*0x3e*/ "Sqrt",
    /*0x3f*/ "GetAngle",
    /*0x40*/ "GetDistance",
    /*0x41*/ "Wait",
    /*0x42*/ "GetTime",
    /*0x43*/ "StrEnd",
    /*0x44*/ "StrCat",
    /*0x45*/ "StrCmp",
    /*0x46*/ "StrLen",
    /*0x47*/ "StrCpy",
    /*0x48*/ "Format",
    /*0x49*/ "GetFarText",
    /*0x4a*/ "ReadNumber",
    /*0x4b*/ "BaseSetter",
    /*0x4c*/ "DirLoop",
    /*0x4d*/ "CanBeHere",       // CantBeHere in newer SCI versions
    /*0x4e*/ "OnControl",
    /*0x4f*/ "InitBresen",
    /*0x50*/ "DoBresen",
    /*0x51*/ "Platform",        // DoAvoider (SCI0)
    /*0x52*/ "SetJump",
    /*0x53*/ "SetDebug",        // for debugging
    /*0x54*/ "InspectObj",      // for debugging
    /*0x55*/ "ShowSends",       // for debugging
    /*0x56*/ "ShowObjs",        // for debugging
    /*0x57*/ "ShowFree",        // for debugging
    /*0x58*/ "MemoryInfo",
    /*0x59*/ "StackUsage",      // for debugging
    /*0x5a*/ "Profiler",        // for debugging
    /*0x5b*/ "GetMenu",
    /*0x5c*/ "SetMenu",
    /*0x5d*/ "GetSaveFiles",
    /*0x5e*/ "GetCWD",
    /*0x5f*/ "CheckFreeSpace",
    /*0x60*/ "ValidPath",
    /*0x61*/ "CoordPri",
    /*0x62*/ "StrAt",
    /*0x63*/ "DeviceInfo",
    /*0x64*/ "GetSaveDir",
    /*0x65*/ "CheckSaveGame",
    /*0x66*/ "ShakeScreen",
    /*0x67*/ "FlushResources",
    /*0x68*/ "SinMult",
    /*0x69*/ "CosMult",
    /*0x6a*/ "SinDiv",
    /*0x6b*/ "CosDiv",
    /*0x6c*/ "Graph",
    /*0x6d*/ "Joystick",
    // End of kernel function table for SCI0
    /*0x6e*/ "ShiftScreen",     // never called?
    /*0x6f*/ "Palette",
    /*0x70*/ "MemorySegment",
    /*0x71*/ "Intersections",	// MoveCursor (SCI1 late), PalVary (SCI1.1)
    /*0x72*/ "Memory",
    /*0x73*/ "ListOps",         // never called?
    /*0x74*/ "FileIO",
    /*0x75*/ "DoAudio",
    /*0x76*/ "DoSync",
    /*0x77*/ "AvoidPath",
    /*0x78*/ "Sort",            // StrSplit (SCI01)
    /*0x79*/ "ATan",            // never called?
    /*0x7a*/ "Lock",
    /*0x7b*/ "StrSplit",
    /*0x7c*/ "GetMessage",      // Message (SCI1.1)
    /*0x7d*/ "IsItSkip",
    /*0x7e*/ "MergePoly",
    /*0x7f*/ "ResCheck",
    /*0x80*/ "AssertPalette",
    /*0x81*/ "TextColors",
    /*0x82*/ "TextFonts",
    /*0x83*/ "Record",          // for debugging
    /*0x84*/ "PlayBack",        // for debugging
    /*0x85*/ "ShowMovie",
    /*0x86*/ "SetVideoMode",
    /*0x87*/ "SetQuitStr",
    /*0x88*/ "DbugStr"          // for debugging
};


bool CVocabWithNames::_Create(sci::istream &byteStream, bool fTruncationOk)
{
    _names.clear();
    uint16_t wMaxIndex;
    byteStream >> wMaxIndex;
    // if the uint16_t read 400, it means there are 401 entries.
    // REVIEW - really? Someone is asking for kernel 113 (Joystick), but they get a garbage character because of this
    // I'm changing this back to <
    // REVIEW: Yes, really - if we leave this as <, then we'll miss the last entry in the selector names, for example.
    // Hmm - only true for some vocabs it seems.  For non essential cases, the caller can pass true
    // in, and we won't try for the extra item.
    if (!fTruncationOk)
    {
        wMaxIndex++;
    }
    for (uint16_t i = 0; byteStream.good() && i < wMaxIndex; i++)
    {
        uint16_t wOffset;
        byteStream >> wOffset;
        uint32_t dwSavePos = byteStream.tellg();
        byteStream.seekg(wOffset);
        if (byteStream.good())
        {
            std::string str;
            // Vocab files strings are run-length encoded.
            byteStream.getRLE(str);
            if (byteStream.good())
            {
                _names.push_back(str);
            }
        }
        byteStream.seekg(dwSavePos); // Go back
    }
    return byteStream.good();
}

uint16_t CVocabWithNames::Add(const string &str)
{
    // Assert that this isn't already in here.
    assert(find(_names.begin(), _names.end(), str) == _names.end());
    _names.push_back(str);
    _fDirty = true;
    return static_cast<uint16_t>(_names.size() - 1);
}

bool SelectorTable::Load(SCIVersion version)
{
    _version = version;
    bool fRet = false;
    unique_ptr<ResourceBlob> blob(_GetVocabData(VocabSelectorNames));
    if (blob)
    {
        fRet = _Create(blob->GetReadStream());
    }
    if (!fRet)
    {
        appState->LogInfo("Failed to load selector names from vocab resource");
    }
    return fRet;
}

void SelectorTable::Save()
{
    if (_IsDirty())
    {
        // Save ourselves
        const vector<string> &names = GetNames();
        uint16_t cNames = (uint16_t)names.size();
        if (cNames > 0)
        {
            vector<BYTE> output;
            // The max index: (size - 1)
            push_word(output, cNames - 1);
            // Then come the offsets - we can run through the strings to calculate these.
            uint16_t wOffset = (uint16_t)output.size() + cNames * 2; // the strings will start after the offsets.
            vector<string>::const_iterator nameIt = names.begin();
            while (nameIt != names.end())
            {
                push_word(output, wOffset);
                wOffset += (uint16_t)(nameIt->length() + 2); // Increase by size of rle string.
                ++nameIt;
            }
            // Now write the strings
            nameIt = names.begin();
            while (nameIt != names.end())
            {
                push_string_rle(output, *nameIt);
                ++nameIt;
            }
            // Now create a resource data for it and save it.
            appState->GetResourceMap().AppendResource(ResourceBlob(nullptr, ResourceType::Vocab, output, 1, VocabSelectorNames, appState->GetVersion(), ResourceSourceFlags::ResourceMap));
        }
    }
}

string SelectorTable::Lookup(uint16_t wName) const
{
    if (_version.HasOldSCI0ScriptHeader)
    {
        wName >>= 1;
    }
    return __super::Lookup(wName);
}

bool KernelTable::Load()
{
    bool fRet = false;
    unique_ptr<ResourceBlob> blob(_GetVocabData(VocabKernelNames));
    if (blob)
    {
        fRet = _Create(blob->GetReadStream());
        // KQ1 is stored differently... nullptr terminated strings.
        // if the normal kind of create fails, we will try that instead...
        if (!fRet)
        {
            appState->LogInfo("Failed to load kernel names from vocab resource - trying alternate KQ1-style method");
            sci::istream byteStream = blob->GetReadStream();
            while (byteStream.good())
            {
                std::string kernelName;
                byteStream >> kernelName;
                if (byteStream.good())
                {
                    Add(kernelName);
                }
            }
            fRet = true;
        }
    }
    else
    {
        // Kernel names not present. Use the hardcoded list (this is the case in later SCI versions)
        assert(appState->GetVersion().MapFormat != ResourceMapFormat::SCI0); // Shouldn't happen for SCI0
        for (size_t i = 0; i < ARRAYSIZE(s_defaultKernelNames); i++)
        {
            _names.push_back(s_defaultKernelNames[i]);
        }
        fRet = true;
    }
    return fRet;
}

bool GlobalClassTable::Load()
{
    bool fRet = false;
    unique_ptr<ResourceBlob> blob(_GetVocabData(VocabClassTable));
    if (blob)
    {
        fRet = _Create(blob->GetReadStream());
    }
    if (!fRet)
    {
        appState->LogInfo("Failed to load class table from vocab resource");
    }
    return fRet;
}

bool GlobalClassTable::_Create(sci::istream &byteStream)
{
    auto scriptContainer = appState->GetResourceMap().Resources(ResourceTypeFlags::Script, ResourceEnumFlags::MostRecentOnly);
    for (auto &scriptResource : *scriptContainer)
    {
        _scriptNums.push_back((uint16_t)scriptResource->GetNumber());
        // Load the script.
        uint16_t currentScriptNumber = (uint16_t)scriptResource->GetNumber();
        unique_ptr<CompiledScript> compiledScript = make_unique<CompiledScript>(currentScriptNumber);
        if (compiledScript->Load(appState->GetVersion(), currentScriptNumber, scriptResource->GetReadStream()))
        {
            CompiledScript *pCompiledScriptWeak = compiledScript.get();
            _scripts.push_back(move(compiledScript));

            // Add the species in here
            for (auto &compiledObject : pCompiledScriptWeak->GetObjects())
            {
                if (!compiledObject->IsInstance())
                {
                    uint16_t species = compiledObject->GetSpecies();
                    _nameToSpecies[compiledObject->GetName()] = species;
                    _speciesToScriptNumber[species] = currentScriptNumber;
                    _speciesToCompiledObjectWeak[species] = compiledObject.get(); // Owned by _scripts
                }
            }
        }
    }
    return true; // We're done when we run out of stuff to read... it's not failure.
}

bool GlobalClassTable::LookupSpecies(const std::string &className, uint16_t &species)
{
    
    auto it = _nameToSpecies.find(className);
    bool success = (it != _nameToSpecies.end());
    if (success)
    {
        species = it->second;
    }
    return success;
}

std::string GlobalClassTable::Lookup(uint16_t wIndex) const
{
    string name;
    auto it = _speciesToCompiledObjectWeak.find(wIndex);
    if (it != _speciesToCompiledObjectWeak.end())
    {
        name = it->second->GetName();
    }
    return name;
}

std::vector<CompiledScript*> GlobalClassTable::GetAllScripts()
{
    std::vector<CompiledScript*> scripts;
    for (auto &script : _scripts)
    {
        scripts.push_back(script.get());
    }
    return scripts;
}

bool GlobalClassTable::GetSpeciesPropertySelector(uint16_t wSpeciesIndex, std::vector<uint16_t> &props, std::vector<uint16_t> &values)
{
    auto it = _speciesToCompiledObjectWeak.find(wSpeciesIndex);
    bool success = (it != _speciesToCompiledObjectWeak.end());
    if (success)
    {
        CompiledObjectBase *object = it->second;
        values = object->GetPropertyValues();
        props = object->GetProperties();
        assert(values.size() == props.size());
    }
    return success;
}

std::vector<uint16_t> GlobalClassTable::GetSubclassesOf(uint16_t baseClass)
{
    vector<uint16_t> subclasses;
    for (auto &speciesObjectPair : _speciesToCompiledObjectWeak)
    {
        std::vector<uint16_t> potentialChildChain;
        potentialChildChain.reserve(15);
        CompiledObjectBase *object = speciesObjectPair.second;
        potentialChildChain.push_back(object->GetSpecies());
        // Follow the chain up 
        while (object->GetSuperClass() != 0xffff)
        {
            if (object->GetSuperClass() == baseClass)
            {
                copy(potentialChildChain.begin(), potentialChildChain.end(), back_inserter(subclasses));
                break;
            }
            object = _speciesToCompiledObjectWeak[object->GetSuperClass()];
            potentialChildChain.push_back(object->GetSpecies());
        }
    }
    return subclasses;
}

bool SpeciesTable::Load()
{
    bool fRet = false;
    unique_ptr<ResourceBlob> blob(_GetVocabData(VocabClassTable));
    if (blob)
    {
        fRet = _Create(blob->GetReadStream());
    }
    if (!fRet)
    {
        appState->LogInfo("Failed to load class table from vocab resource");
    }
    return fRet;
}

void SpeciesTable::Save()
{
    if (_fDirty)
    {
        // Save ourselves
        vector<BYTE> output;
        output.reserve(4 * _direct.size());
        for (vector<uint16_t>::const_iterator speciesIt = _direct.begin(); speciesIt != _direct.end(); ++speciesIt)
        {
            push_word(output, 0);
            push_word(output, *speciesIt);
        }
        // Now create a resource data for it
        appState->GetResourceMap().AppendResource(ResourceBlob(nullptr, ResourceType::Vocab, output, 1, VocabClassTable, appState->GetVersion(), ResourceSourceFlags::ResourceMap));
    }
}

bool SpeciesTable::_Create(sci::istream &byteStream)
{
    _wNewSpeciesIndex = 0;
    while (byteStream.good())
    {
        uint16_t offset;
        byteStream >> offset;   // Don't know what this is.
        uint16_t wScriptNum;
        byteStream >> wScriptNum;
        if (byteStream.good())
        {
            _direct.push_back(wScriptNum);
            _map[wScriptNum].push_back(_wNewSpeciesIndex++);
        }
    }
    return true;
}

bool SpeciesTable::GetSpeciesIndex(uint16_t wScript, uint16_t wClassIndexInScript, SpeciesIndex &wSpeciesIndex) const
{
    bool fRet = false;
    uint16_t wClassIndex = 0;
    species_map::const_iterator script = _map.find(wScript);
    if (script != _map.end())
    {
        const vector<uint16_t> &classesInScript = (*script).second;
        if (wClassIndexInScript < (uint16_t)classesInScript.size())
        {
            fRet = true;
            wSpeciesIndex = classesInScript[wClassIndexInScript];
        }
    }
    return fRet;
}

bool SpeciesTable::GetSpeciesLocation(SpeciesIndex wSpeciesIndex, uint16_t &wScript, uint16_t &wClassIndexInScript) const
{
    uint16_t wType = wSpeciesIndex.Type();
    bool fRet = (wType < static_cast<uint16_t>(_direct.size()));
    if (fRet)
    {
        wScript = _direct[wType];
        const vector<uint16_t> &classesInScript = _map.at(wScript);
        for (size_t i = 0; i < classesInScript.size(); i++)
        {
            if (classesInScript[i] == wSpeciesIndex)
            {
                fRet = true;
                wClassIndexInScript = (uint16_t)i;
                break;
            }
        }
    }
    return fRet;
}

SpeciesIndex SpeciesTable::MaybeAddSpeciesIndex(uint16_t wScript, uint16_t wClassIndexInScript)
{
    SpeciesIndex wSpeciesIndex;
    if (GetSpeciesIndex(wScript, wClassIndexInScript, wSpeciesIndex))
    {
        return wSpeciesIndex;
    }
    else
    {
        // REVIEW: this code is not exception safe.
        // For adding classes in the middle of other classes.
        // REVIEW -> what we're doing here is totally busted.  script numbers do not need to be
        // sequential.  Look at what SCIStudio does - it adds it at the end.  The only thing is,
        // the class #'s change for classes within the current file only.  That could still
        // be catastrophic for some files for the type system.
        // If this script didn't exist in the map before, it should now.
        uint16_t wNewSpeciesIndex = _wNewSpeciesIndex;
        _map[wScript].push_back(wNewSpeciesIndex);

        // Ok, not sure what this garbage was for. Let's just add it
        _direct.push_back(wScript); // Easy peasy
        /*
        // We need to search for this script number in _direct.
        std::vector<uint16_t>::iterator it = find(_direct.begin(), _direct.end(), wScript);
        if (it == _direct.end())
        {
            // This is the first time this script appears in the species table.  Add it.
            _direct.push_back(wScript);
        }
        else
        {
            // Find the next one that isn't this script.
            std::vector<uint16_t>::iterator itEnd = find_if(it, _direct.end(), bind2nd(not_equal_to<uint16_t>(), wScript));
            // Now insert this script here.
            _direct.insert(itEnd, wScript);
        }*/

        _wNewSpeciesIndex++;
        assert(_wNewSpeciesIndex == (uint16_t)_direct.size());
        _fDirty = true;
        return wNewSpeciesIndex;
    }
}

// Textual form of all the script names.
std::vector<std::string> SpeciesTable::GetNames() const
{
    std::vector<std::string> names;
    for (size_t i = 0; i < _direct.size(); ++i)
    {
        std::stringstream stream;
        stream << "species " << static_cast<DWORD>(i) << ": script " << _direct[i];
        names.push_back(stream.str());
    }
    return names;
}

bool CVocabWithNames::ReverseLookup(std::string name, uint16_t &wIndex) const
{
    // PERF: use a more efficient container for looking up words
    std::vector<std::string>::const_iterator position = find(_names.begin(), _names.end(), name);
    bool fRet = (position != _names.end());
    if (fRet)
    {
        wIndex = (uint16_t)distance(_names.begin(), position);
    }
    return fRet;
}

std::string CVocabWithNames::Lookup(uint16_t wName) const
{
    std::string strRet;
    if ((size_t)wName < _names.size())
    {
        strRet = _names[wName];
    }
    else
    {
        strRet = _GetMissingName(wName);
    }
    return strRet;
}

std::string KernelTable::_GetMissingName(uint16_t wName) const
{
    char sz[100];
    StringCchPrintf(sz, ARRAYSIZE(sz), "kernel[%d]", wName);
    return sz;
}

//
// For some reason, SCI0 games are missing the "Joystick" kernel in the kernel
// vocab resource (or rather, the name is garbage characters). Fix that here.
//
const char szMissingKernel[] = "Joystick";
uint16_t wMissingKernel = 113;
std::string KernelTable::Lookup(uint16_t wName) const
{
    if (wName == wMissingKernel)
    {
        return szMissingKernel;
    }
    else
    {
        return __super::Lookup(wName);
    }
}
bool KernelTable::ReverseLookup(std::string name, uint16_t &wIndex) const
{
    if (name == szMissingKernel)
    {
        wIndex = wMissingKernel;
        return true;
    }
    else
    {
        return __super::ReverseLookup(name, wIndex);
    }
}


