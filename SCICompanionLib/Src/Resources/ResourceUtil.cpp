// ResourceBase.cpp : implementation file
//

#include "stdafx.h"
#include "ResourceUtil.h"
#include "format.h"
#include <regex>

using namespace fmt;

SCI_RESOURCE_INFO g_resourceInfo[18] =
{
    { "view", "View", "view.{}", "{}.v56", "view\\.(\\d+)", "(\\d+)\\.v56" },
    { "pic", "Pic", "pic.{}", "{}.p56", "pic\\.(\\d+)", "(\\d+)\\.p56" },
    { "script", "Script", "script.{}", "{}.scr", "script\\.(\\d+)", "(\\d+)\\.scr" },
    { "text", "Text", "text.{}", "{}.tex", "text\\.(\\d+)", "(\\d+)\\.tex" },
    { "sound", "Sound", "sound.{}", "{}.snd", "sound\\.(\\d+)", "(\\d+)\\.snd" },
    { "memory", "Memory", "memory.{}", "{}.mem", "memory\\.(\\d+)", "(\\d+)\\.mem" },
    { "vocab", "Vocab", "vocab.{}", "{}.voc", "vocab\\.(\\d+)", "(\\d+)\\.voc" },
    { "font", "Font", "font.{}", "{}.fon", "font\\.(\\d+)", "(\\d+)\\.fon" },
    { "cursor", "Cursor", "cursor.{}", "{}.cur", "curosr\\.(\\d+)", "(\\d+)\\.cur" },
    { "patch", "Patch", "patch.{}", "{}.pat", "patch\\.(\\d+)", "(\\d+)\\.pat" },
    { "bitmap", "Bitmap", "bitmap.{}", "{}.bit", "bitmap\\.(\\d+)", "(\\d+)\\.bit" },        // Purpose unknown, file extension unknown
    { "palette", "Palette", "palette.{}", "{}.pal", "palette\\.(\\d+)", "(\\d+)\\.pal" },
    { "cda", "CD Audio", "cda.{}", "{}.cda", "cda\\.(\\d+)", "(\\d+)\\.cda" },
    { "audio", "Audio", "audio.{}", "{}.aud", "audio\\.(\\d+)", "(\\d+)\\.aud" },
    { "syn", "Sync", "syn.{}", "{}.syn", "syn\\.(\\d+)", "(\\d+)\\.syn" },
    { "msg", "Message", "message.{}", "{}.msg", "message\\.(\\d+)", "(\\d+)\\.msg" },
    { "map", "Map", "map.{}", "{}.map", "map\\.(\\d+)", "(\\d+)\\.map" },
    { "heap", "Heap", "heap.{}", "{}.hep", "heap\\.(\\d+)", "(\\d+)\\.hep" },
};

const char filterString[] = "{0} resources ({1})|{1}|All files (*.*)|*.*|";

std::string GetFileDialogFilterFor(ResourceType type, SCIVersion version)
{
    SCI_RESOURCE_INFO &resInfo = GetResourceInfo(type);
    std::string formatString = (version.MapFormat > ResourceMapFormat::SCI0) ? resInfo.pszFileFilter_SCI1 : resInfo.pszFileFilter_SCI0;
    std::string soloFilter = format(formatString, "*");
    return format(filterString, resInfo.pszTitleDefault, soloFilter);
}

SCI_RESOURCE_INFO &GetResourceInfo(ResourceType type)
{
    return g_resourceInfo[(int)type];
}

std::string GetFileNameFor(ResourceType type, int number, SCIVersion version)
{
    SCI_RESOURCE_INFO &resInfo = GetResourceInfo(type);
    std::string formatString = (version.MapFormat > ResourceMapFormat::SCI0) ? resInfo.pszFileFilter_SCI1 : resInfo.pszFileFilter_SCI0;
    std::string numberFormatString = (version.MapFormat > ResourceMapFormat::SCI0) ? "{:d}" : "{:03d}";
    std::string numberString = format(numberFormatString, number);
    return format(formatString, numberString);
}


ResourceType ValidateResourceType(ResourceType type)
{
    if ((size_t)type >= ARRAYSIZE(g_resourceInfo))
    {
        return ResourceType::None;
    }
    return type;
}

bool MatchesResourceFilenameFormat(const std::string &filename, ResourceType type, SCIVersion version, int *numberOut)
{
    SCI_RESOURCE_INFO &resInfo = GetResourceInfo(type);
    std::smatch cm;
    if (std::regex_search(filename, cm, std::regex((version.MapFormat > ResourceMapFormat::SCI0) ? resInfo.pszNameMatch_SCI1 : resInfo.pszNameMatch_SCI0)) && (cm.size() > 1))
    {
        std::string number = cm[1];
        *numberOut = StrToInt(number.c_str());
        return true;
    }
    return false;
}

bool MatchesResourceFilenameFormat(const std::string &filename, SCIVersion version, int *numberOut)
{
    for (int i = 0; i < ARRAYSIZE(g_resourceInfo); i++)
    {
        if (MatchesResourceFilenameFormat(filename, (ResourceType)i, version, numberOut))
        {
            return true;
        }
    }
    return false;
}