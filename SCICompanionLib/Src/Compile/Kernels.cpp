#include "stdafx.h"
#include "Kernels.h"
#include "ScriptOM.h"
#include "ScriptOMAll.h"
#include "SyntaxParser.h"
#include "CrystalScriptStream.h"
#include "AppState.h"
#include <unordered_map>
#include "CompileContext.h"
#include "SCO.h"
#include "ResourceEntity.h"

sci::Script g_kernelScript;
bool g_fKernelLoaded = false;
std::unordered_map<std::string, CSCOPublicExport> _scoKernels;
bool g_fSCOKernelsLoaded = false;

const sci::Script &GetKernelSignaturesScript(ICompileLog *log)
{
    if (!g_fKernelLoaded)
    {
        ScriptId scriptId(appState->GetResourceMap().GetIncludePath("kernels.scp"));
        g_kernelScript.SetScriptId(scriptId);
        assert(scriptId.Language() == LangSyntaxCpp);

        CCrystalTextBuffer buffer;
        if (buffer.LoadFromFile(scriptId.GetFullPath().c_str()))
        {
            CScriptStreamLimiter limiter(&buffer);
            CCrystalScriptStream stream(&limiter);
            if (g_Parser.Parse(g_kernelScript, stream, PreProcessorDefinesFromSCIVersion(appState->GetVersion()), log))
            {
                g_fKernelLoaded = true;
                g_kernelScript.SetScriptNumber(KernelScriptNumber);
            }
            buffer.FreeAll();
        }
    }
    return g_kernelScript;
}

CSCOPublicExport GetKernelSCO(SCIVersion version, CompileContext &context, const std::string &name)
{
    // Yuck for this const_cast, but GenerateScriptResource *does* need it currently. It can theoretically re-organize
    // case statements and resolve values and such.
    sci::Script &script = const_cast<sci::Script &>(GetKernelSignaturesScript(&context));
    CSCOPublicExport scoSigs;
    if (g_fKernelLoaded)
    {
        if (!g_fSCOKernelsLoaded)
        {
            CompileTables tables;
            tables.Load(version);
            CompileLog log;
            CompileResults results(log);
            PrecompiledHeaders headers(appState->GetResourceMap());
            if (GenerateScriptResource(appState->GetVersion(), script, headers, tables, results))
            {
                for(const CSCOPublicExport &theExport : results.GetSCO().GetExports())
                {
					_scoKernels[theExport.GetName()] = theExport;
                }
                g_fSCOKernelsLoaded = true;
            }
            else
            {
                context.ReportResult(CompileResult("Couldn't load kernels", CompileResult::CRT_Error));
                for(CompileResult &result : log.Results())
                {
                    context.ReportResult(result);    
                }
            }
        }
        if (g_fSCOKernelsLoaded)
        {
            assert(_scoKernels.find(name) != _scoKernels.end());
            scoSigs = _scoKernels[name];
        }
    }
    return scoSigs;
}