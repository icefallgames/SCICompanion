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

#include "interfaces.h"
#include "CompileCommon.h"

class SpeciesIndex;
class CompiledObject;
class GameFolderHelper;

//
// This can represent any vocab resource with names
//
class CVocabWithNames : public ILookupNames
{
public:
    CVocabWithNames() { _fDirty = false; }
    virtual std::string Lookup(uint16_t wName) const;
    virtual bool ReverseLookup(std::string name, uint16_t &wIndex) const;
    const std::vector<std::string> &GetNames() const { return _names; }
    virtual uint16_t Add(const std::string &str);
    bool Create(sci::istream *pStream, bool fTruncationOk = false) { return _Create(*pStream, fTruncationOk); }

protected:
    bool _Create(sci::istream &byteStream, bool fTruncationOk = false);
    bool _IsDirty() { return _fDirty; }
    virtual std::string _GetMissingName(uint16_t wName) const { return ""; }
    std::vector<std::string> _names;
    bool _fDirty;
};

std::unordered_set<std::string> GetDefaultSelectorNames(SCIVersion version);

//
// Selector names
//
class SelectorTable : public ILookupNames
{
public:
    SelectorTable() : _firstInvalidSelector(0), _fDirty(false) {}
    std::string Lookup(uint16_t wName) const override;
    std::vector<std::string> GetNamesForDisplay() const;
    bool ReverseLookup(std::string name, uint16_t &wIndex) const;
    bool IsSelectorName(const std::string &name) const;
    const std::vector<std::string> &GetNames() const { return _names; }

    bool Load(const GameFolderHelper &helpern);
    uint16_t Add(const std::string &str);
    void Save();
    bool IsDefaultSelector(uint16_t value);

protected:
    bool _Create(sci::istream &byteStream);
    std::string _GetMissingName(uint16_t wName) const;

private:
    std::vector<int> _indices;          // Selector value indices into _names.
    std::vector<std::string> _names;
    std::unordered_map<std::string, uint16_t> _nameToValueCache;
    bool _fDirty;
    size_t _firstInvalidSelector;
    SCIVersion _version;
    std::unordered_set<uint16_t> _defaultSelectors;
};

//
// Kernel names
//
class KernelTable : public CVocabWithNames
{
public:
    bool Load(const GameFolderHelper &helper);
    virtual bool ReverseLookup(std::string name, uint16_t &wIndex) const;

protected:
    std::string _GetMissingName(uint16_t wName) const;
};

// fwd decl
class CompiledScript;
class SpeciesTable;

//
// Given a "class #", returns the script that it is in.
// The global class table is used for disassembling scripts.
// It is based off the same resource as the species table,
// except that it also groks the script resources and pulls
// in the class names.
//
class GlobalClassTable : public ILookupNames
{
public:
    bool Load(const GameFolderHelper &helper);
    const std::vector<uint16_t> &GetScriptNums() { return _scriptNums; } // REVIEW: remove this

    bool LookupSpeciesCompiledName(const std::string &className, uint16_t &species);
    std::vector<uint16_t> GetSubclassesOf(uint16_t species);

    // ILookupNames
    std::string Lookup(uint16_t wIndex) const;
    bool GetSpeciesPropertySelector(uint16_t wSpeciesIndex, std::vector<uint16_t> &props, std::vector<CompiledVarValue> &values);

    std::vector<CompiledScript*> GetAllScripts();

    bool GetSpeciesScriptNumber(uint16_t species, uint16_t &scriptNumber);

private:
    bool _Create(const SpeciesTable &speciesTable);

    std::unordered_map<std::string, uint16_t> _nameToSpecies;
    std::unordered_map<uint16_t, uint16_t> _speciesToScriptNumber;
    std::unordered_map<uint16_t, CompiledObject*> _speciesToCompiledObjectWeak;
    std::vector<std::unique_ptr<CompiledScript>> _scripts;
    std::vector<uint16_t> _scriptNums; // ClassBrowser uses this, but I'm not sure what it's doing with it.
};

//
// Maps species index to a particular script (and index within that script)
//
class SpeciesTable
{
public:
    SpeciesTable() { _wNewSpeciesIndex = 0; _fDirty = false; }
    bool Load(const GameFolderHelper &helper);
    void Save();
    bool GetSpeciesIndex(uint16_t wScript, uint16_t wClassIndexInScript, SpeciesIndex &wSpeciesIndex) const;
    bool GetSpeciesLocation(SpeciesIndex wSpeciesIndex, uint16_t &wScript, uint16_t &wClassIndexInScript) const;
    SpeciesIndex MaybeAddSpeciesIndex(uint16_t wScript, uint16_t wClassIndexInScript);
    std::vector<std::string> GetNames() const;

    void PurgeOldClasses(const GameFolderHelper &helper);

private:
    bool _Create(sci::istream &byteStream);

	typedef std::unordered_map<uint16_t, std::vector<uint16_t> > species_map;

    // Fast lookup of wScript + wScriptIndex -> wSpeciesIndex
    species_map _map;
    // Data in its basic form, as stored (e.g. a list of scripts)
    std::vector<uint16_t> _direct;
    uint16_t _wNewSpeciesIndex;
    bool _fDirty;
};
