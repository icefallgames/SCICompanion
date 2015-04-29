#pragma once

namespace sci
{
    class Script;
}
class CompiledScript;
class DecompileLookups;
class ILookupNames;
class GameFolderHelper;
sci::Script *Decompile(const GameFolderHelper &helper, const CompiledScript &compiledScript, DecompileLookups &lookups, const ILookupNames *pWords);