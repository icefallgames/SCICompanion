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
#pragma once

// We allow 4 channels of simultaneous ParseAutoCompleteContext, the results of which are combined.
enum class ParseAutoCompleteContext : uint8_t
{
    None = 0,
    Selector,
    ClassSelector,      // Selectors for current class
    StudioValue,        // Any value, code keyword, procedure name, etc...
    TopLevelKeyword,
    ClassLevelKeyword,
    SuperClass,
    ScriptName,
    DefineValue,        // Only define constants allowed (not variables, methods, etc...)
    Export,
    LValue,

    // We can get more specific for SCI1.1:
    Temp,               // &tmp keyword!
    PureValue,          // defines, variables, class names, instance names, etc. Not selectors or keywords or procedure names.
    StartStatementExtras, // If, cond, repeat, while, etc.... Also procedure and kernel names.
    Rest,               // Special: &rest is ok.
    Else,

    Block               // Use this in parsing in order to block searching up the context stack.
};

enum class ParseAutoCompleteChannel
{
    One = 0,
    Two = 1,
    Three = 2,
    Four = 3,
    All = 7,
};

typedef uint32_t ParseACChannels;

extern const ParseACChannels BlockAllChannels;
extern const ParseACChannels NoChannels;

ParseAutoCompleteContext ExtractChannel(ParseACChannels data, ParseAutoCompleteChannel channel);
ParseACChannels SetChannel(ParseACChannels existing, ParseAutoCompleteChannel channel, ParseAutoCompleteContext ac);
