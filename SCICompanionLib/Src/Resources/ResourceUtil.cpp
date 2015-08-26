// ResourceBase.cpp : implementation file
//

#include "stdafx.h"
#include "ResourceUtil.h"
#include "format.h"
#include <regex>

using namespace fmt;

SCI_RESOURCE_INFO g_resourceInfo[18] =
{
    { "view", "View", "view.{}", "{}.v56", "view\\.{}", "{}\\.v56" },
    { "pic", "Pic", "pic.{}", "{}.p56", "pic\\.{}", "{}\\.p56" },
    { "script", "Script", "script.{}", "{}.scr", "script\\.{}", "{}\\.scr" },
    { "text", "Text", "text.{}", "{}.tex", "text\\.{}", "{}\\.tex" },
    { "sound", "Sound", "sound.{}", "{}.snd", "sound\\.{}", "{}\\.snd" },
    { "memory", "Memory", "memory.{}", "{}.mem", "memory\\.{}", "{}\\.mem" },
    { "vocab", "Vocab", "vocab.{}", "{}.voc", "vocab\\.{}", "{}\\.voc" },
    { "font", "Font", "font.{}", "{}.fon", "font\\.{}", "{}\\.fon" },
    { "cursor", "Cursor", "cursor.{}", "{}.cur", "curosr\\.{}", "{}\\.cur" },
    { "patch", "Patch", "patch.{}", "{}.pat", "patch\\.{}", "{}\\.pat" },
    { "bitmap", "Bitmap", "bitmap.{}", "{}.bit", "bitmap\\.{}", "{}\\.bit" },        // Purpose unknown, file extension unknown
    { "palette", "Palette", "palette.{}", "{}.pal", "palette\\.{}", "{}\\.pal" },
    { "cda", "CD Audio", "cda.{}", "{}.cda", "cda\\.{}", "{}\\.cda" },
    { "audio", "Audio", "audio.{}", "{}.aud", "audio\\.{}", "{}\\.aud" },
    { "syn", "Sync", "syn.{}", "{}.syn", "syn\\.{}", "{}\\.syn" },
    { "msg", "Message", "message.{}", "{}.msg", "message\\.{}", "{}\\.msg" },
    { "map", "Map", "map.{}", "{}.map", "map\\.{}", "{}\\.map" },
    { "heap", "Heap", "heap.{}", "{}.hep", "heap\\.{}", "{}\\.hep" },
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

bool MatchesResourceFilenameFormat(const std::string &filename, ResourceType type, SCIVersion version, int *numberOut, std::string &nameOut)
{
    SCI_RESOURCE_INFO &resInfo = GetResourceInfo(type);
    std::smatch cm;
    std::string regexSearchBase = (version.MapFormat > ResourceMapFormat::SCI0) ? resInfo.pszNameMatch_SCI1 : resInfo.pszNameMatch_SCI0;
    std::string regexSearchNumber = fmt::format(regexSearchBase, "(\\d+)");
    if (std::regex_search(filename, cm, std::regex(regexSearchNumber)) && (cm.size() > 1))
    {
        std::string number = cm[1];
        *numberOut = StrToInt(number.c_str());
        nameOut = "";
        return true;
    }
    else
    {
        // Perhaps we can get one without a number (e.g. "foobar.v56")
        // (?: <blahbalbhalbh> ?)  -> Non-capturing group
        // \\ -> turns into a single backslash (c++)
        // \. -> .
        // \- -> -
        // \w -> number, letter, underscore
        regexSearchNumber = fmt::format(regexSearchBase, "((?:\\.|\\-|\\w?)+)");
        if (std::regex_search(filename, cm, std::regex(regexSearchNumber)) && (cm.size() > 1))
        {
            *numberOut = -1;
            nameOut = cm[1];
            return true;
        }
    }
    return false;
}

bool MatchesResourceFilenameFormat(const std::string &filename, SCIVersion version, int *numberOut, std::string &nameOut)
{
    for (int i = 0; i < ARRAYSIZE(g_resourceInfo); i++)
    {
        if (MatchesResourceFilenameFormat(filename, (ResourceType)i, version, numberOut, nameOut))
        {
            return true;
        }
    }
    return false;
}