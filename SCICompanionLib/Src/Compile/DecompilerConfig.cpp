#include "stdafx.h"
#include "cpptoml.h"
#include "GameFolderHelper.h"
#include "DecompilerCore.h"
#include "DecompilerConfig.h"

using namespace std;
using namespace cpptoml;

class DecompilerConfig : public IDecompilerConfig
{
public:
    DecompilerConfig(const GameFolderHelper &helper)
    {
        string decompilerIniPath = helper.GetSrcFolder() + "\\Decompiler.ini";
        try
        {
            _table = make_unique<table>(parse_file(decompilerIniPath));
            if (_table->contains("methodParameterNames"))
            {
                _methodParameterNames = _table->get_table("methodParameterNames");
            }
        }
        catch (parse_exception &e)
        {
            error = e.what();
        }
    }

    std::vector<std::string> GetParametersFor(sci::ClassDefinition *classDef, const std::string &methodName) const
    {
        vector<string> paramsOut;
        if (_methodParameterNames && _methodParameterNames->contains(methodName))
        {
            auto params = _methodParameterNames->get(methodName);
            if (params->is_array())
            {
                for (const auto &paramName : params->as_array()->get())
                {
                    auto name = paramName->as<std::string>();
                    if (name)
                    {
                        paramsOut.push_back(name.get()->get());
                    }
                }
            }
            else if (params->is_value()) // Just a single guy
            {
                auto name = params->as<string>();
                if (name)
                {
                    paramsOut.push_back(name.get()->get());
                }
            }
        }

        return paramsOut;
    }

private:
    std::unique_ptr<cpptoml::table> _table;
    std::shared_ptr<cpptoml::table> _methodParameterNames;
};

std::unique_ptr<IDecompilerConfig> CreateDecompilerConfig(const GameFolderHelper &helper)
{
    return make_unique<DecompilerConfig>(helper);
}
