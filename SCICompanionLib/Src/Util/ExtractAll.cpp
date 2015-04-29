#include "stdafx.h"
#include "ExtractAll.h"
#include "AppState.h"
#include "ResourceEntity.h"
#include "Components.h"
#include "PicOperations.h"
#include "View.h"
#include "RasterOperations.h"
#include "PaletteOperations.h"
#include "CompiledScript.h"
#include "Disassembler.h"
#include "Vocab000.h"

void ExtractAllResources(SCIVersion version, const std::string &destinationFolderIn, bool extractPicImages, bool extractViewImages, bool disassembleScripts, IExtractProgress *progress)
{
    std::string destinationFolder = destinationFolderIn;
    if (destinationFolder.back() != '\\')
    {
        destinationFolder += "\\";
    }

    ObjectFileScriptLookups objectFileLookups(appState->GetResourceMap().Helper());
    GlobalCompiledScriptLookups scriptLookups;
    if (disassembleScripts)
    {
        if (!scriptLookups.Load(appState->GetResourceMap().Helper()))
        {
            disassembleScripts = false;
        }
    }

    int totalCount = 0;
    auto resourceContainer = appState->GetResourceMap().Resources(ResourceTypeFlags::All, ResourceEnumFlags::MostRecentOnly | ResourceEnumFlags::ExcludePatchFiles);
    for (auto &blob : *resourceContainer)
    {
        totalCount++;
        if (extractViewImages && (blob->GetType() == ResourceType::View))
        {
            totalCount++;
        }
        if (extractPicImages && (blob->GetType() == ResourceType::Pic))
        {
            totalCount++;
        }
        if (disassembleScripts && (blob->GetType() == ResourceType::Pic))
        {
            totalCount++;
        }
    }

    int count = 0;
    // Get it again, because we don't supprot reset.
    resourceContainer = appState->GetResourceMap().Resources(ResourceTypeFlags::All, ResourceEnumFlags::MostRecentOnly | ResourceEnumFlags::ExcludePatchFiles);
    for (auto &blob : *resourceContainer)
    {
        std::string filename = GetFileNameFor(blob->GetType(), blob->GetNumber(), blob->GetVersion());
        std::string fullPath = destinationFolder + filename;
        bool keepGoing = true;
        try
        {
            count++;
            if (progress)
            {
                keepGoing = progress->SetProgress(fullPath, count, totalCount);
            }
            // Just the resource
            blob->SaveToFile(fullPath);

            if (keepGoing)
            {
                // Then possible pictures
                CBitmap bitmap;
                SCIBitmapInfo bmi;
                BYTE *pBitsDest = nullptr;
                std::string possibleImagePath = fullPath + ".bmp";
                if (extractPicImages && (blob->GetType() == ResourceType::Pic))
                {
                    count++;
                    if (progress)
                    {
                        keepGoing = progress->SetProgress(possibleImagePath, count, totalCount);
                    }

                    std::unique_ptr<ResourceEntity> resource = CreateResourceFromResourceData(*blob);
                    PicComponent &pic = resource->GetComponent<PicComponent>();
                    PaletteComponent *palette = resource->TryGetComponent<PaletteComponent>();
                    bitmap.Attach(GetPicBitmap(PicScreen::Visual, pic, palette, sPIC_WIDTH, sPIC_HEIGHT, &bmi, &pBitsDest));
                }

                if (extractViewImages && (blob->GetType() == ResourceType::View))
                {
                    count++;
                    if (progress)
                    {
                        keepGoing = progress->SetProgress(possibleImagePath, count, totalCount);
                    }

                    std::unique_ptr<ResourceEntity> view = CreateResourceFromResourceData(*blob);
                    std::unique_ptr<PaletteComponent> optionalPalette;
                    if (view->GetComponent<RasterComponent>().Traits.PaletteType == PaletteType::VGA_256)
                    {
                        optionalPalette = appState->GetResourceMap().GetMergedPalette(*view, 999);
                    }
                    bitmap.Attach(CreateBitmapFromResource(*view, optionalPalette.get(), &bmi, &pBitsDest));
                }

                if ((HBITMAP)bitmap)
                {
                    Save8BitBmp(possibleImagePath, bmi, pBitsDest, 0);
                }

                if (disassembleScripts && (blob->GetType() == ResourceType::Script))
                {
                    count++;
                    if (progress)
                    {
                        keepGoing = progress->SetProgress(possibleImagePath, count, totalCount);
                    }

                    CompiledScript compiledScript(blob->GetNumber());
                    compiledScript.Load(appState->GetResourceMap().Helper(), appState->GetVersion(), blob->GetNumber(), blob->GetReadStream());
                    std::stringstream out;
                    DisassembleScript(compiledScript, out, &scriptLookups, &objectFileLookups, appState->GetResourceMap().GetVocab000());
                    std::string scriptPath = fullPath + ".txt";
                    std::string actualPath = MakeTextFile(out.str().c_str(), scriptPath.c_str());
                }
            }
        }
        catch (std::exception)
        {

        }

        if (!keepGoing)
        {
            break;
        }
    }
}