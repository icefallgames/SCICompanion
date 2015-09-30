#include "stdafx.h"
#include "PhonemeMap.h"
#include "cpptoml.h"

using namespace std;
using namespace cpptoml;

PhonemeMap::PhonemeMap(const std::string &filename) : _filename(filename)
{
    try
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
    }
    catch (parse_exception)
    {
    }
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