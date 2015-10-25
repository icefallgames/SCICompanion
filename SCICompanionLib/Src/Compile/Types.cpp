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
#include "AppState.h"
#include "Types.h"
#include "ScriptOMAll.h"
#include "CompileContext.h"

// Some predefined data types
const std::string TypeStringString = "string";
const std::string TypeStringSaidString = "said_string";
const std::string TypeStringInt = "int";
const std::string TypeStringUInt = "uint";
const std::string TypeStringPointer = "pointer";
const std::string TypeStringBool = "bool";
const std::string TypeStringChar = "char";
const std::string TypeStringSelector = "selector";
const std::string TypeStringAny = "var";
const std::string TypeStringVoid = "void";
// These are some other data types used in kernel functions.
const std::string TypeStringKWindow = "k_window";
const std::string TypeStringKList = "k_list";
const std::string TypeStringKNode = "k_node";

bool IsZero(const sci::SingleStatement *pStatement)
{
    const sci::PropertyValue *pValue = pStatement->CastSyntaxNode<sci::PropertyValue>();
    if (pValue)
    {
        return pValue->IsZero();
    }
    else
    {
        const sci::ComplexPropertyValue *pValue = pStatement->CastSyntaxNode<sci::ComplexPropertyValue>();
        if (pValue)
        {
            return pValue->IsZero();
        }
    }
    return false;
}

bool IsNumberType(SpeciesIndex type)
{
    switch (type)
    {
    case DataTypeChar:
    case DataTypeInt:
    case DataTypeUInt:
        return true;
    }
    return false;
}

bool IsPointerType(SpeciesIndex type)
{
    switch (type)
    {
    case DataTypeString:
        return true;
    case DataTypePointer:
        return true;
    }
    return false;
}

bool IsOperatorAddSubtract(const std::string &op)
{
    return (op == "+") || (op == "-") || (op == "+=") || (op == "-=");
}

bool DoesTypeMatch(CompileContext &context, SpeciesIndex destType, SpeciesIndex sourceType, const std::string *pOperator, const sci::SingleStatement *pStatement)
{
    // Guard against uninitialized memory
    ASSERT(destType != 0xcccc);
    ASSERT(sourceType != 0xcccc);

    if (sourceType == destType)
    {
        if (IsPointerType(sourceType) && pOperator && IsOperatorAddSubtract(*pOperator))
        {
            // You shouldn't be able to add two pointers together, etc...
            return false;
        }
        // REVIEW: there are other things we could consider here, and possibly
        // other error messages (e.g. '%' is not defined for pointers)
        return true; // They are the same
    }

    if (destType == DataTypeAny)
    {
        // Can assign anything to var, except for void.
        return (sourceType != DataTypeVoid);
    }
     
    if (sourceType == DataTypeAny)
    {
        return true; // Always ok
    }

    if (IsNumberType(destType) && IsNumberType(sourceType))
    {
        return true;
    }
    if (IsPointerType(destType) && IsPointerType(sourceType))
    {
        return true;
    }

    // We do some implicit casting here.
    if (destType == DataTypeBool)
    {
        return (sourceType != DataTypeVoid); // Anything can be cast to bool except void
    }

    if (pOperator)
    {
        // e.g. adding a number to a pointer.
        if (IsPointerType(destType) && IsNumberType(sourceType))
        {
            return IsOperatorAddSubtract(*pOperator);
        }
    }

    // Check if destType is a superclass of sourceType.
    if (!IsPODType(destType) && !IsPODType(sourceType))
    {
        // These are both classes.  Is destType a super class or sourceType?  If so, that's a match.
        // (The reverse will require an explicit cast).
        SpeciesIndex superClass = sourceType;
        while (superClass != DataTypeNone)
        {
            if (superClass == destType)
            {
                return true; // Match.
            }
            superClass = context.GetSpeciesSuperClass(superClass);
        }
    }
    
    // Special cases:
    if (pStatement)
    {
        if (IsZero(pStatement))
        {
            // Zero has implicit casts to many types.  First of all, to any object type.
            if (destType < UserDefinedDataTypeBoundary)
            {
                return true;
            }

            // And to these built in type:
            switch (destType)
            {
            case DataTypeString:
            case DataTypeSaidString:
            case DataTypePointer:
            case DataTypeKWindow:
            case DataTypeKList:
            case DataTypeKNode:
                return true;
            }
        }
    }

    return false;
}

typedef std::unordered_map<std::string, WORD> BuiltInSpeciesMap;
typedef std::unordered_map<WORD, std::string> BuiltInSpeciesMapR;
BuiltInSpeciesMap g_builtInSpeciesMap;
BuiltInSpeciesMapR g_builtInSpeciesMapR;

//
// Sets up the maps that convert SpeciesIndex to string, and vice-verse.
//
void _EnsureTypeMaps()
{
    if (g_builtInSpeciesMapR.empty())
    {
        // Populate it.
        g_builtInSpeciesMapR[DataTypeBool] = TypeStringBool;
        g_builtInSpeciesMapR[DataTypeAny] = TypeStringAny;
        g_builtInSpeciesMapR[DataTypeVoid] = TypeStringVoid;
        g_builtInSpeciesMapR[DataTypeInt] = TypeStringInt;
        g_builtInSpeciesMapR[DataTypeUInt] = TypeStringUInt;
        g_builtInSpeciesMapR[DataTypeChar] = TypeStringChar;
        g_builtInSpeciesMapR[DataTypeString] =  TypeStringString;
        g_builtInSpeciesMapR[DataTypeSaidString] = TypeStringSaidString;
        g_builtInSpeciesMapR[DataTypePointer] = TypeStringPointer;
        g_builtInSpeciesMapR[DataTypeSelector] = TypeStringSelector;
        g_builtInSpeciesMapR[DataTypeKWindow] = TypeStringKWindow;
        g_builtInSpeciesMapR[DataTypeKList] = TypeStringKList;
        g_builtInSpeciesMapR[DataTypeKNode] = TypeStringKNode;

        // Populate the reverse map.
        ASSERT(g_builtInSpeciesMap.empty());
        for(BuiltInSpeciesMapR::value_type &typeToString : g_builtInSpeciesMapR)
        {
            g_builtInSpeciesMap[typeToString.second] = typeToString.first;
        }
    }
}

bool IsPODType(const std::string &type)
{
    SpeciesIndex wDummy;
    return GetBuiltInDataTypeSpecies(type, wDummy);
}

bool IsPODType(SpeciesIndex wSpecies)
{
    return !GetBuiltInDataTypeString(wSpecies).empty();
}

bool IsUnsignedType(SpeciesIndex si)
{
    return !IsPODType(si) ||            // All "pointers" are unsigned.
           (si == DataTypeUInt) ||
           (si == DataTypeSelector) ||
           IsPointerType(si);
}

//
// If type is a built-in data type, returns the species number we use for it.
//
bool GetBuiltInDataTypeSpecies(const std::string &type, SpeciesIndex &wSpecies)
{
    bool fRet = false;
    if (type.empty())
    {
        fRet = true;
        wSpecies = DataTypeAny; // Special case - no type means "any".  This is for SCIStudio syntax.
    }
    else
    {
        _EnsureTypeMaps();
        BuiltInSpeciesMap::iterator it = g_builtInSpeciesMap.find(type);
        fRet = (it != g_builtInSpeciesMap.end());
        if (fRet)
        {
            wSpecies = it->second;
        }
    }
    return fRet;
}

//
// Given a species index, come up with the string for it.
//
std::string GetBuiltInDataTypeString(SpeciesIndex wSpecies)
{
    std::string ret;
    _EnsureTypeMaps();
    BuiltInSpeciesMapR::iterator iter = g_builtInSpeciesMapR.find(wSpecies.Type());
    if (iter != g_builtInSpeciesMapR.end())
    {
        ret = iter->second;
    }
    return ret;
}
