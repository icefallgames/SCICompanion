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
#include "ExtControlBarNoX.h"

void CExtControlBarNoX::OnNcAreaButtonsReinitialize()
{
    INT nCountOfNcButtons =
    NcButtons_GetCount();
        if( nCountOfNcButtons > 0 )
            return;
    // REMOVED THE 'X' HERE
    #if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
        NcButtons_Add( new
    CExtBarNcAreaButtonAutoHide(this) );
    #endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
        NcButtons_Add( new
    CExtBarNcAreaButtonMenu(this) );
}

