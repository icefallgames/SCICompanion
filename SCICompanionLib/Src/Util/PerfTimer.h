#pragma once

class PerfTimer
{
public:
    PerfTimer(const std::string &name, bool spew = true);
    ~PerfTimer();
private:
    bool _spew;
    std::string _name;
    LARGE_INTEGER _lStart;
};