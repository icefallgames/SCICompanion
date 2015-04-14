#pragma once

#include "CompiledScript.h"

void DebugOut(const CompiledObjectBase &object, std::ostream &out, bool fRN = false);

void DebugOut(const CompiledScript &script, std::ostream &out, bool fRN = false);
