#pragma once

#include "ScriptOM.h"

static const WORD KernelScriptNumber = 0xfffe;

class CompileContext;
class CSCOPublicExport;

sci::Script &GetKernelSignaturesScript(ICompileLog &log);

CSCOPublicExport GetKernelSCO(SCIVersion version, CompileContext &context, const std::string &name);



