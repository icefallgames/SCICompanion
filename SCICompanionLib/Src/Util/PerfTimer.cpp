/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
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

