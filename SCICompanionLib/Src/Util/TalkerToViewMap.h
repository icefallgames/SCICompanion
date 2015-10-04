#pragma once

class AppState;

class TalkerToViewMap
{
public:
    TalkerToViewMap() {}
    TalkerToViewMap(const std::string &talker);
    bool TalkerToViewLoop(uint16_t talker, uint16_t &view, uint16_t &loop) const;
    void SetTalkerToViewLoop(uint16_t talker, uint16_t view, uint16_t loop);

private:
    std::string _filename;
    std::string _folder;
    std::unordered_map<uint16_t, std::pair<uint16_t, uint16_t>> _talkerToViewLoop;
};