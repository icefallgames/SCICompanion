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

