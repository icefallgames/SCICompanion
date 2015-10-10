#pragma once

class AppState;

class PhonemeMap
{
public:
    PhonemeMap(const std::string &filename);
    
    uint16_t PhonemeToCel(const std::string &phoneme) const;
    void SetCel(const std::string &phoneme, uint16_t cel);
    const std::string &GetErrors() const { return _errors; }
    const std::string &GetFilespec() const { return _filespec; }
    const std::string &GetFileContents() const { return _fileContents; }
    bool IsEmpty() const;
    
    bool HasErrors() const { return !_errors.empty(); }

    const std::map<std::string, int> &Entries() { return _phonemeToCel; }

private:
    std::map<std::string, int> _phonemeToCel;

    std::string _errors;
    std::string _filespec;
    std::string _fileContents;
};

bool SaveForViewLoop(const std::string &text, AppState *appState, int view, int loop, std::string &errors);
std::unique_ptr<PhonemeMap> LoadPhonemeMapForViewLoop(AppState *appState, int view, int loop);
std::string GetPhonemeMapPath(AppState *appState, int view, int loop);
std::string GetPhonemeMapFilespec(AppState *appState, int view, int loop);
