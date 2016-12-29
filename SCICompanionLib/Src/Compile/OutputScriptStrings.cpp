#include "stdafx.h"
#include "ScriptOM.h"
#include "CompileContext.h"

using namespace sci;
// For use in a spell-checker, etc...

class ExtractStrings : public IExploreNode
{
public:
    ExtractStrings(std::vector<std::string> &allStrings) : _allStrings(allStrings) {}

    void Extract(PropertyValueBase &value)
    {
        if (!value.GetStringValue().empty())
        {
            if (value.GetType() == ValueType::ResourceString ||
                value.GetType() == ValueType::String)
            {
                _allStrings.push_back(value.GetStringValue());
            }
        }
    }

    void ExploreNode(SyntaxNode &node, ExploreNodeState state) override
    {
        if (state == ExploreNodeState::Pre)
        {
            switch (node.GetNodeType())
            {
                case NodeTypeValue:
                {
                    Extract(static_cast<PropertyValue&>(node));
                }
                break;

                case NodeTypeComplexValue:
                {
                    Extract(static_cast<ComplexPropertyValue&>(node));
                }
                break;
                }
        }
    }

private:
    std::vector<std::string> &_allStrings;
};

void ExtractScriptStrings(CompileLog &log, ScriptId scriptId, std::vector<std::string> &allStrings)
{
    std::unique_ptr<Script> script = SimpleCompile(log, scriptId);

    // Now look for all strings.
    ExtractStrings extractStrings(allStrings);
    script->Traverse(extractStrings);
}
