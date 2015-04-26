#pragma once

class DecompileLookups;
namespace sci
{
    class FunctionBase;
}

void DisassembleFallback(sci::FunctionBase &func, code_pos start, code_pos end, DecompileLookups &lookups);