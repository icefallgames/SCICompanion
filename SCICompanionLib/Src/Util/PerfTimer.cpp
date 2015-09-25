#include "stdafx.h"
#include "PerfTimer.h"
#include "format.h"

PerfTimer::PerfTimer(const std::string &name, bool spew) : _spew(spew), _name(name)
{
    QueryPerformanceCounter(&_lStart);
}

PerfTimer::~PerfTimer()
{
    LARGE_INTEGER lEnd;
    QueryPerformanceCounter(&lEnd);
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    double timeTaken = (double)(lEnd.LowPart - _lStart.LowPart) * 1000.0 / (double)freq.LowPart;
    OutputDebugString(fmt::format("PREF - {0}: {1}ms\n", _name, timeTaken).c_str());
}

