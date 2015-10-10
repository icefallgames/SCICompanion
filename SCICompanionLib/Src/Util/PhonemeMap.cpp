#include "stdafx.h"
#include "PhonemeMap.h"
#include "AppState.h"
#include "cpptoml.h"
#include "format.h"

using namespace std;
using namespace cpptoml;

std::string GetPhonemeMapFilespec(AppState *appState, int view, int loop)
{
    return fmt::format("phoneme_{0}_{1}.ini", view, loop);
}

std::string GetPhonemeMapPath(AppState *appState, int view, int loop)
{
    std::string folder = appState->GetResourceMap().Helper().GetLipSyncFolder();
    folder += "\\";
    folder += GetPhonemeMapFilespec(appState, view, loop);
    return folder;
}

std::unique_ptr<PhonemeMap> LoadPhonemeMapForViewLoop(AppState *appState, int view, int loop)
{
    std::string fullPath = GetPhonemeMapPath(appState, view, loop);
    return std::make_unique<PhonemeMap>(fullPath);
}

PhonemeMap::PhonemeMap(const std::string &filename) : _filespec(filename.substr(filename.find_last_of("\\/")))
{
    try
    {
        if (PathFileExists(filename.c_str()))
        {
            unique_ptr<cpptoml::table> table = make_unique<cpptoml::table>(parse_file(filename));
            if (table->contains("phoneme_to_cel"))
            {
                shared_ptr<cpptoml::table> phonemes = table->get_table("phoneme_to_cel");
                for (auto entry : *phonemes.get())
                {
                    auto value = entry.second->as<int64_t>();
                    _phonemeToCel[entry.first] = (int)(*value).get();
                }
            }

            ifstream file;
            file.open(filename, std::ios_base::binary | std::ios_base::in);
            if (file.is_open())
            {
                _fileContents = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            }
        }
        else
        {
            _errors = std::string("Can't open ") + filename;
        }
    }
    catch (cpptoml::parse_exception &e)
    {
        _errors = e.what();
    }
}

void PhonemeMap::SetCel(const std::string &phoneme, uint16_t cel)
{
    _phonemeToCel[phoneme] = cel;
}

uint16_t PhonemeMap::PhonemeToCel(const std::string &phoneme) const
{
    uint16_t cel = 0xffff;
    auto it = _phonemeToCel.find(phoneme);
    if (it != _phonemeToCel.end())
    {
        cel = (uint16_t)it->second;
    }
    return cel;
}

// Detects an empty phonememap (all zeroes)
bool PhonemeMap::IsEmpty() const
{
    auto itNotZero = std::find_if(_phonemeToCel.begin(), _phonemeToCel.end(),
        [](const std::pair<std::string, int> &entry) { return entry.second != 0; }
        );
    return itNotZero == _phonemeToCel.end();
}

bool SaveForViewLoop(const std::string &text, AppState *appState, int view, int loop, std::string &errors)
{
    std::string fullPath = GetPhonemeMapPath(appState, view, loop);
    std::string fullPathBak = fullPath + ".bak";

    std::ofstream fileBak;
    fileBak.open(fullPathBak, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    if (fileBak.is_open())
    {
        fileBak << text;
        fileBak.close();
    }

    std::unique_ptr<PhonemeMap> mapTest = std::make_unique<PhonemeMap>(fullPathBak);
    if (!mapTest->HasErrors())
    {
        // Save the real one.
        std::ofstream file;
        file.open(fullPath, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
        if (file.is_open())
        {
            file << text;
            file.close();
        }
    }

    deletefile(fullPathBak);

    errors = mapTest->GetErrors();
    return !mapTest->HasErrors();
}
