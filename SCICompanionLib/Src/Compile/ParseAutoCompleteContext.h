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

enum class ParseAutoCompleteContext
{
    None,
    Selector,
    ClassSelector,
    Value,
    TopLevelKeyword,
    ClassLevelKeyword,
    SuperClass,
    ScriptName,
    DefineValue,        // Only define constants allowed (not variables, methods, etc...)
    Export,
    LValue,
    Block               // Use this in parsing in order to block searching up the context stack.
};
