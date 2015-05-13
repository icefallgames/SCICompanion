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

class IDecompilerConfig
{
public:
    virtual std::vector<std::string> GetParameterNamesFor(sci::ClassDefinition *classDef, const std::string &methodName) const = 0;
    virtual void ResolveMethodCallParameterTypes(sci::SendParam &sendParam) const = 0;
    virtual void ResolveProcedureCallParameterTypes(sci::ProcedureCall &procCall) const = 0;
    virtual bool IsBitfieldProperty(const std::string &propertyName) const = 0;
    virtual ~IDecompilerConfig() {}

    std::string error;
};

std::unique_ptr<IDecompilerConfig> CreateDecompilerConfig(const GameFolderHelper &helper, const SelectorTable &selectorTable);
