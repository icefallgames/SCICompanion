#pragma once

class GameFolderHelper;

namespace cpptoml
{
    class table;
}
namespace sci
{
    class ClassDefinition;
}

class IDecompilerConfig
{
public:
    virtual std::vector<std::string> GetParametersFor(sci::ClassDefinition *classDef, const std::string &methodName) const = 0;
    virtual ~IDecompilerConfig() {}

    std::string error;
};

std::unique_ptr<IDecompilerConfig> CreateDecompilerConfig(const GameFolderHelper &helper);
