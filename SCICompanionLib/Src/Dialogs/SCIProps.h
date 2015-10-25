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

//
// Properties used in the room editor
//

#define SCIPROP_NAME                TEXT("Name")
#define SCIPROP_SPECIES             TEXT("Species")
#define SCIPROP_VIEW                TEXT("view")
#define SCIPROP_X                   TEXT("x")
#define SCIPROP_Y                   TEXT("y")
#define SCIPROP_LOOP                TEXT("loop")
#define SCIPROP_CEL                 TEXT("cel")
#define SCIPROP_Z                   TEXT("z")
#define SCIPROP_CYCLESPEED          TEXT("cycleSpeed")
#define SCIPROP_DOORBLOCK           TEXT("doorBlock")
#define SCIPROP_DOORCTRL            TEXT("doorCtrl")
#define SCIPROP_ENTRANCETO          TEXT("entranceTo")
#define SCIPROP_ROOMCTRL            TEXT("roomCtrl")
#define SCIPROP_MOVESPEED           TEXT("moveSpeed")
#define SCIPROP_XSTEP               TEXT("xStep")
#define SCIPROP_YSTEP               TEXT("yStep")
#define SCIPROP_PRIORITY            TEXT("Priority")
#define SCIPROP_IGNOREACTORS        TEXT("Ignore Actors")
#define SCIPROP_ADDTOPIC            TEXT("Add To Pic")
#define SCIPROP_OBSERVECONTROL      TEXT("Observe Control")

#define SCIMETHOD_SETPRI            TEXT("setPri")
#define SCIMETHOD_IGNOREACTORS      TEXT("ignoreActors")
#define SCIMETHOD_ADDTOPIC          TEXT("addToPic")
#define SCIMETHOD_OBSERVECONTROL    TEXT("observeControl")
#define SCIMETHOD_IGNORECONTROL     TEXT("ignoreControl")

#define SCISPECIES_ACT              TEXT("Act")
#define SCISPECIES_PROP             TEXT("Prop")
#define SCISPECIES_VIEW             TEXT("View")
#define SCISPECIES_DOOR             TEXT("Door")

namespace sci
{
	class ClassDefinition;
	class PropertyValue;
}

// Pseudo prop conversion
typedef bool(*PFNCLASSTOPSEUDOPROP)(const sci::ClassDefinition*, PCTSTR, sci::PropertyValue&);
typedef bool(*PFNPSEUDOPROPTOCLASS)(sci::ClassDefinition*, PCTSTR, const sci::PropertyValue&);

BOOL IsPseudoProp(PCTSTR pszProp, PFNCLASSTOPSEUDOPROP *ppfn);





