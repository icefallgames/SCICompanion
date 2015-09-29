#pragma once

class PhonemeMap
{
public:
    PhonemeMap(const std::string &filename);

    int PhonemeToCel(const std::string &phoneme);

private:
    std::string _filename;
    std::unordered_map<std::string, int> _phonemeToCel;
};