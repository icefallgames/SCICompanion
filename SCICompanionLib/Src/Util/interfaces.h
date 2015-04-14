#pragma once

//
// Generic interface used for looking up names based on an index
//
class ILookupNames
{
public:
    virtual std::string Lookup(WORD wName) const = 0;
};

