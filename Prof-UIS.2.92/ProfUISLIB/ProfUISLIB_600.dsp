# Microsoft Developer Studio Project File - Name="ProfUISLIB" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ProfUISLIB - Win32 Static Unicode Debug RDE with MFC DLL
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ProfUISLIB_600.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ProfUISLIB_600.mak" CFG="ProfUISLIB - Win32 Static Unicode Debug RDE with MFC DLL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ProfUISLIB - Win32 Static ANSI Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static ANSI Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static MBCS Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static MBCS Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static ANSI Debug with MFC DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static ANSI Release with MFC DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static Unicode Debug with MFC DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static Unicode Release with MFC DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static MBCS Debug with MFC DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static MBCS Release with MFC DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static ANSI Debug RDE" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static ANSI Debug RDE with MFC DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static ANSI Release RDE" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static ANSI Release RDE with MFC DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static MBCS Debug RDE" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static MBCS Debug RDE with MFC DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static MBCS Release RDE" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static MBCS Release RDE with MFC DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static Unicode Debug RDE" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static Unicode Debug RDE with MFC DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static Unicode Release RDE" (based on "Win32 (x86) Static Library")
!MESSAGE "ProfUISLIB - Win32 Static Unicode Release RDE with MFC DLL" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ProfUISLIB - Win32 Static ANSI Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticAnsiRelease\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /i "..\Include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292sn.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static ANSI Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticAnsiDebug\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GR /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /i "..\Include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292snd.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static Unicode Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Unicode Debug"
# PROP BASE Intermediate_Dir "Unicode Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticUnicodeDebug\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__PROF_UIS_IMPL__" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GR /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "_UNICODE" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\Include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292sud.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static Unicode Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Unicode Release"
# PROP BASE Intermediate_Dir "Unicode Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticUnicodeRelease\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\Include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__PROF_UIS_IMPL__" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_UNICODE" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\Include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292su.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static MBCS Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MBCS Debug"
# PROP BASE Intermediate_Dir "MBCS Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticMbcsDebug\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__PROF_UIS_IMPL__" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GR /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "_MBCS" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\Include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292smd.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static MBCS Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MBCS Release"
# PROP BASE Intermediate_Dir "MBCS Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticMbcsRelease\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\Include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__PROF_UIS_IMPL__" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_MBCS" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\Include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292sm.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static ANSI Debug with MFC DLL"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ProfUISLIB___Win32_Debug_with_MFC_DLL"
# PROP BASE Intermediate_Dir "ProfUISLIB___Win32_Debug_with_MFC_DLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticAnsiDebugWithMfcDll\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_IMPL__" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "__STATPROFUIS_WITH_DLLMFC__" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\Include" /d "_DEBUG" /d "_AFXDLL" /d "__STATPROFUIS_WITH_DLLMFC__"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Bin_600\ProfUIS292snd.lib"
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292ynd.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static ANSI Release with MFC DLL"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ProfUISLIB___Win32_Release_with_MFC_DLL"
# PROP BASE Intermediate_Dir "ProfUISLIB___Win32_Release_with_MFC_DLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticAnsiReleaseWithMfcDll\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\Include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_IMPL__" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "__STATPROFUIS_WITH_DLLMFC__" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\Include" /d "NDEBUG" /d "_AFXDLL" /d "__STATPROFUIS_WITH_DLLMFC__"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Bin_600\ProfUIS292sn.lib"
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292yn.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static Unicode Debug with MFC DLL"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ProfUISLIB___Win32_Unicode_Debug_with_MFC_DLL"
# PROP BASE Intermediate_Dir "ProfUISLIB___Win32_Unicode_Debug_with_MFC_DLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticUnicodeDebugWithMfcDll\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_IMPL__" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "_UNICODE" /D "__STATPROFUIS_WITH_DLLMFC__" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\Include" /d "_DEBUG" /d "_AFXDLL" /d "__STATPROFUIS_WITH_DLLMFC__"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Bin_600\ProfUIS292sud.lib"
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292yud.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static Unicode Release with MFC DLL"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ProfUISLIB___Win32_Unicode_Release_with_MFC_DLL"
# PROP BASE Intermediate_Dir "ProfUISLIB___Win32_Unicode_Release_with_MFC_DLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticUnicodeReleaseWithMfcDll\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\Include" /D "NDEBUG" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_IMPL__" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_UNICODE" /D "__STATPROFUIS_WITH_DLLMFC__" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\Include" /d "NDEBUG" /d "_AFXDLL" /d "__STATPROFUIS_WITH_DLLMFC__"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Bin_600\ProfUIS292su.lib"
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292yu.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static MBCS Debug with MFC DLL"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ProfUISLIB___Win32_MBCS_Debug_with_MFC_DLL"
# PROP BASE Intermediate_Dir "ProfUISLIB___Win32_MBCS_Debug_with_MFC_DLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticMbcsDebugWithMfcDll\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__PROF_UIS_IMPL__" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "_MBCS" /D "__STATPROFUIS_WITH_DLLMFC__" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\Include" /d "_DEBUG" /d "_AFXDLL" /d "__STATPROFUIS_WITH_DLLMFC__"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Bin_600\ProfUIS292smd.lib"
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292ymd.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static MBCS Release with MFC DLL"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ProfUISLIB___Win32_MBCS_Release_with_MFC_DLL"
# PROP BASE Intermediate_Dir "ProfUISLIB___Win32_MBCS_Release_with_MFC_DLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticMbcsReleaseWithMfcDll\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\Include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__PROF_UIS_IMPL__" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_MBCS" /D "__STATPROFUIS_WITH_DLLMFC__" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\Include" /d "NDEBUG" /d "_AFXDLL" /d "__STATPROFUIS_WITH_DLLMFC__"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Bin_600\ProfUIS292sm.lib"
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292ym.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static ANSI Debug RDE"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ProfUISLIB___Win32_Static_ANSI_Debug_RDE"
# PROP BASE Intermediate_Dir "ProfUISLIB___Win32_Static_ANSI_Debug_RDE"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticAnsiDebugRde\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GR /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_FOR_REGULAR_DLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /i "..\Include" /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\Include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Bin_600\ProfUIS292snd.lib"
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292snd-RDE.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static ANSI Debug RDE with MFC DLL"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ProfUISLIB___Win32_Static_ANSI_Debug_RDE_with_MFC_DLL"
# PROP BASE Intermediate_Dir "ProfUISLIB___Win32_Static_ANSI_Debug_RDE_with_MFC_DLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticAnsiDebugRdeWithMfcDll\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "__STATPROFUIS_WITH_DLLMFC__" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "__STATPROFUIS_WITH_DLLMFC__" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_FOR_REGULAR_DLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /i "..\Include" /d "_DEBUG" /d "_AFXDLL" /d "__STATPROFUIS_WITH_DLLMFC__"
# ADD RSC /l 0x419 /i "..\Include" /d "_DEBUG" /d "_AFXDLL" /d "__STATPROFUIS_WITH_DLLMFC__"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Bin_600\ProfUIS292ynd.lib"
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292ynd-RDE.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static ANSI Release RDE"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ProfUISLIB___Win32_Static_ANSI_Release_RDE"
# PROP BASE Intermediate_Dir "ProfUISLIB___Win32_Static_ANSI_Release_RDE"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticAnsiReleaseRde\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_FOR_REGULAR_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /i "..\Include" /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\Include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Bin_600\ProfUIS292sn.lib"
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292sn-RDE.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static ANSI Release RDE with MFC DLL"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ProfUISLIB___Win32_Static_ANSI_Release_RDE_with_MFC_DLL"
# PROP BASE Intermediate_Dir "ProfUISLIB___Win32_Static_ANSI_Release_RDE_with_MFC_DLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticAnsiReleaseRdeWithMfcDll\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "__STATPROFUIS_WITH_DLLMFC__" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "__STATPROFUIS_WITH_DLLMFC__" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_FOR_REGULAR_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /i "..\Include" /d "NDEBUG" /d "_AFXDLL" /d "__STATPROFUIS_WITH_DLLMFC__"
# ADD RSC /l 0x419 /i "..\Include" /d "NDEBUG" /d "_AFXDLL" /d "__STATPROFUIS_WITH_DLLMFC__"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Bin_600\ProfUIS292yn.lib"
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292yn-RDE.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static MBCS Debug RDE"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ProfUISLIB___Win32_Static_MBCS_Debug_RDE"
# PROP BASE Intermediate_Dir "ProfUISLIB___Win32_Static_MBCS_Debug_RDE"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticMbcsDebugRde\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "_MBCS" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GR /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "_MBCS" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_FOR_REGULAR_DLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /i "..\Include" /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\Include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Bin_600\ProfUIS292smd.lib"
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292smd-RDE.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static MBCS Debug RDE with MFC DLL"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ProfUISLIB___Win32_Static_MBCS_Debug_RDE_with_MFC_DLL"
# PROP BASE Intermediate_Dir "ProfUISLIB___Win32_Static_MBCS_Debug_RDE_with_MFC_DLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticMbcsDebugRdeWithMfcDll\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "_MBCS" /D "__STATPROFUIS_WITH_DLLMFC__" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "_MBCS" /D "__STATPROFUIS_WITH_DLLMFC__" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_FOR_REGULAR_DLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /i "..\Include" /d "_DEBUG" /d "_AFXDLL" /d "__STATPROFUIS_WITH_DLLMFC__"
# ADD RSC /l 0x419 /i "..\Include" /d "_DEBUG" /d "_AFXDLL" /d "__STATPROFUIS_WITH_DLLMFC__"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Bin_600\ProfUIS292ymd.lib"
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292ymd-RDE.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static MBCS Release RDE"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ProfUISLIB___Win32_Static_MBCS_Release_RDE"
# PROP BASE Intermediate_Dir "ProfUISLIB___Win32_Static_MBCS_Release_RDE"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticMbcsReleaseRde\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_MBCS" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_MBCS" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_FOR_REGULAR_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /i "..\Include" /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\Include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Bin_600\ProfUIS292sm.lib"
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292sm-RDE.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static MBCS Release RDE with MFC DLL"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ProfUISLIB___Win32_Static_MBCS_Release_RDE_with_MFC_DLL"
# PROP BASE Intermediate_Dir "ProfUISLIB___Win32_Static_MBCS_Release_RDE_with_MFC_DLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticMbcsReleaseRdeWithMfcDll\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_MBCS" /D "__STATPROFUIS_WITH_DLLMFC__" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_MBCS" /D "__STATPROFUIS_WITH_DLLMFC__" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_FOR_REGULAR_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /i "..\Include" /d "NDEBUG" /d "_AFXDLL" /d "__STATPROFUIS_WITH_DLLMFC__"
# ADD RSC /l 0x419 /i "..\Include" /d "NDEBUG" /d "_AFXDLL" /d "__STATPROFUIS_WITH_DLLMFC__"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Bin_600\ProfUIS292ym.lib"
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292ym-RDE.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static Unicode Debug RDE"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ProfUISLIB___Win32_Static_Unicode_Debug_RDE"
# PROP BASE Intermediate_Dir "ProfUISLIB___Win32_Static_Unicode_Debug_RDE"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticUnicodeDebugRde\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "_UNICODE" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GR /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "_UNICODE" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_FOR_REGULAR_DLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /i "..\Include" /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\Include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Bin_600\ProfUIS292sud.lib"
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292sud-RDE.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static Unicode Debug RDE with MFC DLL"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ProfUISLIB___Win32_Static_Unicode_Debug_RDE_with_MFC_DLL"
# PROP BASE Intermediate_Dir "ProfUISLIB___Win32_Static_Unicode_Debug_RDE_with_MFC_DLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticUnicodeDebugRdeWithMfcDll\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "_UNICODE" /D "__STATPROFUIS_WITH_DLLMFC__" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /GX /ZI /Od /I "..\Include" /D "_DEBUG" /D "_UNICODE" /D "__STATPROFUIS_WITH_DLLMFC__" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_FOR_REGULAR_DLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /i "..\Include" /d "_DEBUG" /d "_AFXDLL" /d "__STATPROFUIS_WITH_DLLMFC__"
# ADD RSC /l 0x419 /i "..\Include" /d "_DEBUG" /d "_AFXDLL" /d "__STATPROFUIS_WITH_DLLMFC__"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Bin_600\ProfUIS292yud.lib"
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292yud-RDE.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static Unicode Release RDE"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ProfUISLIB___Win32_Static_Unicode_Release_RDE"
# PROP BASE Intermediate_Dir "ProfUISLIB___Win32_Static_Unicode_Release_RDE"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticUnicodeReleaseRde\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_UNICODE" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_UNICODE" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_FOR_REGULAR_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /i "..\Include" /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\Include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Bin_600\ProfUIS292su.lib"
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292su-RDE.lib"

!ELSEIF  "$(CFG)" == "ProfUISLIB - Win32 Static Unicode Release RDE with MFC DLL"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ProfUISLIB___Win32_Static_Unicode_Release_RDE_with_MFC_DLL"
# PROP BASE Intermediate_Dir "ProfUISLIB___Win32_Static_Unicode_Release_RDE_with_MFC_DLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\StaticUnicodeReleaseRdeWithMfcDll\ProfUISLIB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_UNICODE" /D "__STATPROFUIS_WITH_DLLMFC__" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_UNICODE" /D "__STATPROFUIS_WITH_DLLMFC__" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_FOR_REGULAR_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /i "..\Include" /d "NDEBUG" /d "_AFXDLL" /d "__STATPROFUIS_WITH_DLLMFC__"
# ADD RSC /l 0x419 /i "..\Include" /d "NDEBUG" /d "_AFXDLL" /d "__STATPROFUIS_WITH_DLLMFC__"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Bin_600\ProfUIS292yu.lib"
# ADD LIB32 /nologo /out:"..\Bin_600\ProfUIS292yu-RDE.lib"

!ENDIF 

# Begin Target

# Name "ProfUISLIB - Win32 Static ANSI Release"
# Name "ProfUISLIB - Win32 Static ANSI Debug"
# Name "ProfUISLIB - Win32 Static Unicode Debug"
# Name "ProfUISLIB - Win32 Static Unicode Release"
# Name "ProfUISLIB - Win32 Static MBCS Debug"
# Name "ProfUISLIB - Win32 Static MBCS Release"
# Name "ProfUISLIB - Win32 Static ANSI Debug with MFC DLL"
# Name "ProfUISLIB - Win32 Static ANSI Release with MFC DLL"
# Name "ProfUISLIB - Win32 Static Unicode Debug with MFC DLL"
# Name "ProfUISLIB - Win32 Static Unicode Release with MFC DLL"
# Name "ProfUISLIB - Win32 Static MBCS Debug with MFC DLL"
# Name "ProfUISLIB - Win32 Static MBCS Release with MFC DLL"
# Name "ProfUISLIB - Win32 Static ANSI Debug RDE"
# Name "ProfUISLIB - Win32 Static ANSI Debug RDE with MFC DLL"
# Name "ProfUISLIB - Win32 Static ANSI Release RDE"
# Name "ProfUISLIB - Win32 Static ANSI Release RDE with MFC DLL"
# Name "ProfUISLIB - Win32 Static MBCS Debug RDE"
# Name "ProfUISLIB - Win32 Static MBCS Debug RDE with MFC DLL"
# Name "ProfUISLIB - Win32 Static MBCS Release RDE"
# Name "ProfUISLIB - Win32 Static MBCS Release RDE with MFC DLL"
# Name "ProfUISLIB - Win32 Static Unicode Debug RDE"
# Name "ProfUISLIB - Win32 Static Unicode Debug RDE with MFC DLL"
# Name "ProfUISLIB - Win32 Static Unicode Release RDE"
# Name "ProfUISLIB - Win32 Static Unicode Release RDE with MFC DLL"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Src\ExtControls.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtProgressWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtControlsCommon.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtBtnOnFlat.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtButton.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtCheckListWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtCmdIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtCmdManager.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtColorCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtColorDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtComboBox.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtContentExpandWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtControlBar.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtDockBar.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtGroupBox.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtHook.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtHook.h
# End Source File
# Begin Source File

SOURCE=..\Src\ExtLabel.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtMdiWindowsListDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtMenuControlBar.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtMiniDockFrameWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtMouseCaptureSink.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtNcFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtPaintManager.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtPaintManager2.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtPopupMenuWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtProfUISAboutDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtResizableDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtRichContent.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtScrollWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtSliderWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtSpinWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtSplitterWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtStatusControlBar.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtToolControlBar.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Include\ExtControls.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtProgressWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtControlsCommon.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtBtnOnFlat.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtButton.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtCheckListWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtCmdIcon.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtCmdManager.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtColorCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtColorDlg.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtComboBox.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtContentExpandWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtControlBar.h
# End Source File
# Begin Source File

SOURCE=..\Src\ExtDockBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtEdit.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtGroupBox.h
# End Source File
# Begin Source File

SOURCE=..\Src\ExtIntegrityCheckSum.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtLabel.h
# End Source File
# Begin Source File

SOURCE=..\Src\ExtLocalization.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtMdiWindowsListDlg.h
# End Source File
# Begin Source File

SOURCE=..\Src\ExtMemoryDC.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtMenuControlBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtMfcDef.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtMfcSafeNativeTCHAR.h
# End Source File
# Begin Source File

SOURCE=..\Src\ExtMiniDockFrameWnd.h
# End Source File
# Begin Source File

SOURCE=..\Src\ExtMouseCaptureSink.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtNcFrame.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtPaintManager.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtPopupMenuWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtProfUISAboutDialog.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtRegistry.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtResizableDialog.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtRichContent.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtScrollWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtSliderWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtSpinWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtSplitterWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtStatusControlBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtTempl.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtToolControlBar.h
# End Source File
# Begin Source File

SOURCE="..\Include\Prof-UIS.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
