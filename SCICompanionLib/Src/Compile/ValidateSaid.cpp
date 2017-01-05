#include "stdafx.h"
#include "ValidateSaid.h"
#include "GameFolderHelper.h"
#include "CompiledScript.h"
#include "CompileInterfaces.h"
#include "ResourceContainer.h"
#include "Vocab000.h"
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

void FindAllSaidTokens(CompiledScript &script, std::set<uint16_t> &tokens)
{
    for (const auto &sequence : script.GetSaids())
    {
        std::copy_if(sequence.begin(), sequence.end(), std::inserter(tokens, tokens.begin()),
            [](uint16_t token) { return ((token < 0x00f0) || (token > 0x00f9)); } // Copy in words, not specific tokens
            );
    }
}

CompileResult ProduceError(const Vocab000 &vocab000, int scriptNumber, uint16_t wordGroup, uint16_t mainWordGroup, const std::string &message)
{
    return CompileResult(fmt::format("Script {0}: '{1}' is a synonym {2} and will be converted to '{3}' when evaluated.", scriptNumber, vocab000.GetGroupWordString(wordGroup), message, vocab000.GetGroupWordString(mainWordGroup)), CompileResult::CompileResultType::CRT_Error);
}

class ExtractSaids : public IExploreNode, public ILookupSaids
{
public:
    ExtractSaids(ScriptId scriptId, CompileLog &log, const Vocab000 &vocab000) : _scriptId(scriptId), _log(log), _vocab000(vocab000) {}

    void ExploreNode(SyntaxNode &node, ExploreNodeState state) override
    {
        if (state == ExploreNodeState::Pre)
        {
            switch (node.GetNodeType())
            {
            case NodeTypeValue:
            {
                _Extract(static_cast<PropertyValue&>(node));
            }
            break;

            case NodeTypeComplexValue:
            {
                _Extract(static_cast<ComplexPropertyValue&>(node));
            }
            break;

            case NodeTypeSynonym:
            {
                Synonym &synClause = static_cast<Synonym&>(node);
                for (const auto &syn : synClause.Synonyms)
                {
                    _allWordGroups.insert((uint16_t)_vocab000.GroupFromString(syn.c_str()));
                }
            }
            break;
            }
        }
    }

    bool LookupWord(const std::string &word, uint16_t &wordGroup) override
    {
        DWORD dwWordGroup;
        bool result = _vocab000.LookupWord(word, dwWordGroup);
        wordGroup = (uint16_t)dwWordGroup;
        return result;
    }

    void ValidateAgainst(ScriptId synonymScript, const SynonymVector &synonymsSource, bool validateSynonymClause)
    {
        // Convert to word groups first, exactly matching synonymsSource
        // Basically we'll compile a map of all synonyms, and which word they convert to.
        unordered_map<uint16_t, Synonym*> synonymToSource;
        for (const auto &synonymsSource : synonymsSource)
        {
            uint16_t mainWordGroup;
            if (LookupWord(synonymsSource->MainWord, mainWordGroup))
            {
                vector<uint16_t> synonynms;
                for (const string &synonymSource : synonymsSource->Synonyms)
                {
                    uint16_t wordGroup = 0;
                    if (LookupWord(synonymSource, wordGroup))
                    {
                        if (wordGroup == mainWordGroup)
                        {
                            // It's a synonym of itself. Don't add it though, because it can't cause any issues.
                            // But maybe notify the user
                            if (validateSynonymClause)
                            {
                                std::string errorMessage = fmt::format(
                                    "{0}: '{1}' is already a synonym of '{2}': '{3}'",
                                    synonymScript.GetFileName(),
                                    synonymSource,
                                    synonymsSource->MainWord,
                                    _vocab000.GetGroupWordString(wordGroup)
                                );
                                _log.ReportResult(CompileResult(
                                    errorMessage,
                                    _scriptId,
                                    synonymsSource->GetLineNumber() + 1,
                                    synonymsSource->GetColumnNumber(),
                                    CompileResult::CompileResultType::CRT_Warning
                                ));

                            }
                        }
                        else
                        {
                            synonymToSource[wordGroup] = synonymsSource.get();
                        }
                    }
                }
            }
        }

        for (const auto &pair : _allSaids)
        {
            // Go through each token
            for (auto saidTokenPair : pair.second)
            {
                uint16_t saidToken = saidTokenPair.first;
                auto itFind = synonymToSource.find(saidToken);
                if (itFind != synonymToSource.end())
                {
                    // This is a synonym, so it's likely that this will produce unintended effects, since it will never match the said involved.
                    std::string errorMessage = fmt::format(
                        "{0}: '{1}' will be replaced with '{2}' in the player's input when synonyms from {3} are applied. This may cause '{4}' not to match.",
                        _scriptId.GetFileName(),
                        saidTokenPair.second,
                        itFind->second->MainWord,
                        synonymScript.GetFileName(),
                        pair.first->GetStringValue()
                    );

                    _log.ReportResult(CompileResult(
                        errorMessage,
                        _scriptId,
                        pair.first->GetLineNumber() + 1,
                        pair.first->GetColumnNumber(),
                        CompileResult::CompileResultType::CRT_Error
                    ));
                }
            }
        }
    }

    void GrabWordGroups(std::set<uint16_t> &allSaidsInScripts)
    {
        std::copy(_allWordGroups.begin(), _allWordGroups.end(), std::inserter(allSaidsInScripts, allSaidsInScripts.begin()));
    }

private:

    void _Extract(PropertyValueBase &value)
    {
        if (value.GetType() == ValueType::Said)
        {
            string stringCode = value.GetStringValue();
            vector<uint8_t> output;
            vector<string> words;

            ParseSaidString(nullptr, *this, stringCode, &output, &value, &words);
            std::vector<pair<uint16_t, string>> wordGroups;
            int wordIndex = 0;
            for (size_t i = 0; i < output.size(); i++)
            {
                uint8_t token = output[i];
                if (((token < 0xf0) || (token > 0xf9)) && (token != 0xff))
                {
                    // It's a word
                    uint16_t wordGroup = (uint16_t)token << 8;
                    wordGroup |= output[++i];
                    wordGroups.emplace_back(wordGroup, words[wordIndex]);
                    wordIndex++;
                    _allWordGroups.insert(wordGroup);
                }
            }
            _allSaids[&value] = wordGroups;
        }
    }

    CompileLog &_log;
    const Vocab000 &_vocab000;
    ScriptId _scriptId;
    std::unordered_map<PropertyValueBase*, std::vector<pair<uint16_t, std::string>>> _allSaids;
    std::set<uint16_t> _allWordGroups;
};

void ListUnusedOfType(CompileLog &log, const Vocab000 &vocab000, WordClass wordClass, const std::set<uint16_t> &saidsUsedInScripts)
{
    std::string wordClassName = GetWordClassString(wordClass);

    // Now we can list which word groups are never used
    Vocab000::groups_iterator position = vocab000.GroupsBegin();
    while (position != vocab000.GroupsEnd())
    {
        Vocab000::WordGroup dwGroup;
        WordClass dwClass;
        std::string strWords;
        vocab000.EnumGroups(position, dwGroup, dwClass, strWords);
        if (dwClass == wordClass)
        {
            if (saidsUsedInScripts.find((uint16_t)dwGroup) == saidsUsedInScripts.end())
            {
                std::string errorMessage = fmt::format(
                    "Unused {0}: {1}",
                    wordClassName,
                    strWords
                );
                log.ReportResult(CompileResult(
                    errorMessage,
                    ResourceType::Vocab,
                    appState->GetVersion().MainVocabResource,
                    (int)dwGroup
                ));
            }
        }
    }


}

void ValidateSaids(CompileLog &log, const Vocab000 &vocab000)
{
    std::set<uint16_t> saidsUsedInScripts;

    std::vector<ScriptId> scripts;
    appState->GetResourceMap().GetAllScripts(scripts);
    std::unique_ptr<Script> mainScript;
    std::unique_ptr<ExtractSaids> mainSaids;
    for (ScriptId script : scripts)
    {
        if (script.GetResourceNumber() == 0)
        {
            // Main
            mainScript = SimpleCompile(log, script);
            mainSaids = std::make_unique<ExtractSaids>(script, log, vocab000);
            mainScript->Traverse(*mainSaids);
            mainSaids->GrabWordGroups(saidsUsedInScripts);
            break;
        }
    }
    
    if (mainScript)
    {
        for (ScriptId scriptId : scripts)
        {
            if (scriptId.GetResourceNumber() != 0)
            {
                std::unique_ptr<Script> script = SimpleCompile(log, scriptId);
                ExtractSaids scriptSaids(scriptId, log, vocab000);
                script->Traverse(scriptSaids);
                scriptSaids.GrabWordGroups(saidsUsedInScripts);
                if (!script->GetSynonyms().empty())
                {
                    // This script declares synonyms - so we'll validate against itself and main.
                    scriptSaids.ValidateAgainst(scriptId, script->GetSynonyms(), true);

                    // Then against main
                    mainSaids->ValidateAgainst(scriptId, script->GetSynonyms(), false);
                }
            }
        }
    }

    uint16_t wordClass = (uint16_t)WordClass::ImperativeVerb;
    while (wordClass)
    {
        ListUnusedOfType(log, vocab000, (WordClass)wordClass, saidsUsedInScripts);
        wordClass >>= 1;
    }

}