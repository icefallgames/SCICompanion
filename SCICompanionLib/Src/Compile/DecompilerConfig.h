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

class GameFolderHelper;
class SelectorTable;

namespace cpptoml
{
    class table;
}
namespace sci
{
    class ClassDefinition;
    class SendParam;
}
class SelectorTable;

class IDecompilerConfig
{
public:
    virtual std::vector<std::string> GetParameterNamesFor(sci::ClassDefinition *classDef, const std::string &methodName) const = 0;
    virtual void ResolveMethodCallParameterTypes(sci::SendParam &sendParam) const = 0;
    virtual void ResolveProcedureCallParameterTypes(sci::ProcedureCall &procCall) const = 0;
    virtual void ResolveSwitchStatementValues(sci::SwitchStatement &switchStatement) const = 0;
    virtual void ResolveBinaryOpValues(sci::BinaryOp &binaryOp) const = 0;
    virtual bool IsBitfieldProperty(const std::string &propertyName) const = 0;
    virtual const SelectorTable &GetSelectorTable() const = 0;
    virtual ~IDecompilerConfig() {}

    std::string error;
};

std::unique_ptr<IDecompilerConfig> CreateDecompilerConfig(const GameFolderHelper &helper, const SelectorTable &selectorTable);
