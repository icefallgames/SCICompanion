#pragma once

class PhonemeMap
{
public:
    PhonemeMap(const std::string &filename);

    uint16_t PhonemeToCel(const std::string &phoneme) const;

private:
    std::string _filename;
    std::unordered_map<std::string, int> _phonemeToCel;
};