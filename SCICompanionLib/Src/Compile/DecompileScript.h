#pragma once

namespace sci
{
    class Script;
}
class CompiledScript;
class DecompileLookups;
class ILookupNames;

sci::Script *Decompile(const CompiledScript &compiledScript, DecompileLookups &lookups, const ILookupNames *pWords);