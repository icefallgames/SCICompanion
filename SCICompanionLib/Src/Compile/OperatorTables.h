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

#include "Operators.h"

extern std::vector<std::pair<std::string, BinaryOperator>> studioNameToBinaryOp;
extern std::vector<std::pair<std::string, BinaryOperator>> sciNameToBinaryOp;
extern std::vector<std::pair<std::string, BinaryOperator>> cppNameToBinaryOp;

extern std::vector<std::pair<std::string, AssignmentOperator>> studioNameToAssignmentOp;
extern std::vector<std::pair<std::string, AssignmentOperator>> sciNameToAssignmentOp;
extern std::vector<std::pair<std::string, AssignmentOperator>> cppNameToAssignmentOp;

extern std::vector<std::pair<std::string, UnaryOperator>> studioNameToUnaryOp;
extern std::vector<std::pair<std::string, UnaryOperator>> sciNameToUnaryOp;
extern std::vector<std::pair<std::string, UnaryOperator>> cppNameToUnaryOp;