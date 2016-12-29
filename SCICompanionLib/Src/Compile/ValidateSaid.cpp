#include "stdafx.h"
#include "ValidateSaid.h"
#include "GameFolderHelper.h"
#include "CompiledScript.h"
#include "CompileInterfaces.h"
#include "ResourceContainer.h"
#include "Vocab000.h"
#include <format.h>

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

void ValidateSaids(const GameFolderHelper &helper, const Vocab000 &vocab000, std::vector<CompileResult> &results)
{
	CompiledScript mainScript(0);
	if (mainScript.Load(helper, helper.Version, 0))
	{
		// Find all the tokens used in saids in main
		std::set<uint16_t> mainSaidTokens;
		FindAllSaidTokens(mainScript, mainSaidTokens);

		// Now go through each script and ensure that for any synonyms, they aren't used in that script or in main.
		auto resourceContainer = helper.Resources(ResourceTypeFlags::Script, ResourceEnumFlags::AddInDefaultEnumFlags);
		for (auto &blob : *resourceContainer)
		{
			if (blob->GetNumber() != 0) // skip main
			{
				CompiledScript roomScript(blob->GetNumber()); // Maybe not a room, but we don't care.
				if (roomScript.Load(helper, helper.Version, blob->GetNumber(), blob->GetReadStream())) // Don't need to worry about heap stream, since this is only SCI0
				{
					auto roomSynonyms = roomScript.GetSynonyms();
					if (!roomSynonyms.empty())
					{
						std::set<uint16_t> roomSaidTokens;
						FindAllSaidTokens(roomScript, roomSaidTokens);

						for (const auto &synonyms : roomSynonyms)
						{
							for (uint16_t synonym : synonyms.second)
							{
								if (synonym != synonyms.first)
								{
									// These are the words that will be mapped to others. If someone references them in a script, it's probably a bug since the
									// Said statement will never match. 
									if (contains(mainSaidTokens, synonym))
									{
										results.push_back(ProduceError(vocab000, blob->GetNumber(), synonym, synonyms.first, "referenced in a Said string in Main"));
									}
									if (contains(roomSaidTokens, synonym))
									{
										results.push_back(ProduceError(vocab000, blob->GetNumber(), synonym, synonyms.first, "referenced in a Said string in this script"));
									}
								}
								else
								{
									// In this case, we have something like
									// (apple apple fruit)
									// Where apple is listed as a synonym of apple. This is commonplace in Sierra games, and doesn't cause any issues.
								}
							}
						}
					}
				}
				else
				{
					results.push_back(CompileResult(fmt::format("Couldn't load script {0}", blob->GetNumber()), CompileResult::CompileResultType::CRT_Error));
				}
			}
		}

		results.push_back(CompileResult(fmt::format("{0} potential issues found.", results.size()), CompileResult::CompileResultType::CRT_Message));
	}

}