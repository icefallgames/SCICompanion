#include "stdafx.h"
#include "ValidateSelectors.h"
#include "GameFolderHelper.h"
#include "CompiledScript.h"
#include "CompileInterfaces.h"
#include "ResourceContainer.h"
#include "Vocab99x.h"
#include "CompileContext.h"
#include "AppState.h"
#include "ScriptOM.h"
#include <format.h>

// This contains code to warn about situations where a synonym statement will replace word A with B in the player's input,
// but word A is used in a Said string. This will cause word A not to match, which is a possible said/parsing bug.
// e.g.
// (synonyms
//     (man boy)
// )
//
// (if (Said 'look/boy')
//     ; this will never match, since 'boy' will be replaced with 'man' in the player's input.
// )


using namespace sci;
using namespace std;

class ExtractSelectors : public IExploreNode
{
public:
    ExtractSelectors(ScriptId scriptId, CompileLog &log, std::unordered_set<std::string> &allSelectors) : _scriptId(scriptId), _log(log), _allSelectors(allSelectors) {}

    void ExploreNode(SyntaxNode &node, ExploreNodeState state) override
    {
        if (state == ExploreNodeState::Pre)
        {
            switch (node.GetNodeType())
            {
            case NodeTypeClassDefinition:
            {
                ClassDefinition &classDef = static_cast<ClassDefinition&>(node);
                for (auto &method : classDef.GetMethods())
                {
                    _allSelectors.insert(method->GetName());
                }
                for (auto &prop : classDef.GetProperties())
                {
                    _allSelectors.insert(prop->GetName());
                }
                break;
            }
            }
        }
    }

private:

    CompileLog &_log;
    std::unordered_set<std::string> &_allSelectors;
    ScriptId _scriptId;
};

void ValidateSelectors(CompileLog &log, SelectorTable &selectors)
{
    std::map<uint16_t, int> saidsUsedInScripts;
    std::map<string, int> rootsUsedInScripts;

    std::vector<ScriptId> scripts;
    appState->GetResourceMap().GetAllScripts(scripts);
    std::unordered_set<std::string> allSelectors;
    for (ScriptId scriptId : scripts)
    {
        if (scriptId.GetResourceNumber() != 0)
        {
            std::unique_ptr<Script> script = SimpleCompile(log, scriptId);
            ExtractSelectors scriptSelectors(scriptId, log, allSelectors);
            script->Traverse(scriptSelectors);

        }
    }

    std::vector<std::string> toRemove;
    for (auto &selector : selectors.GetNames())
    {
        if (allSelectors.find(selector) == allSelectors.end())
        {
            toRemove.push_back(selector);
            //OutputDebugString(selector.c_str());
            //OutputDebugString("\n");
        }
    }

    //selectors.Add
}