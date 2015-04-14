# Microsoft Developer Studio Project File - Name="ProfUISDLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ProfUISDLL - Win32 Unicode Debug RDE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ProfUISDLL_600.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ProfUISDLL_600.mak" CFG="ProfUISDLL - Win32 Unicode Debug RDE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ProfUISDLL - Win32 ANSI Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ProfUISDLL - Win32 ANSI Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ProfUISDLL - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ProfUISDLL - Win32 Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ProfUISDLL - Win32 MBCS Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ProfUISDLL - Win32 MBCS Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ProfUISDLL - Win32 ANSI Debug RDE" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ProfUISDLL - Win32 ANSI Release RDE" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ProfUISDLL - Win32 MBCS Debug RDE" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ProfUISDLL - Win32 MBCS Release RDE" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ProfUISDLL - Win32 Unicode Debug RDE" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ProfUISDLL - Win32 Unicode Release RDE" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ProfUISDLL - Win32 ANSI Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\AnsiRelease\ProfUISDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_WINDLL" /D "_AFXEXT" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\Include" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\Bin_600/ProfUIS292n.dll"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "ProfUISDLL - Win32 ANSI Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\AnsiDebug\ProfUISDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /I "..\Include" /D "_DEBUG" /D "_WINDLL" /D "_AFXEXT" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\Include" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Bin_600/ProfUIS292nd.dll" /pdbtype:sept

!ELSEIF  "$(CFG)" == "ProfUISDLL - Win32 Unicode Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ProfUISDLL___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "ProfUISDLL___Win32_Unicode_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\UnicodeDebug\ProfUISDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /I "..\Include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "__PROF_UIS_IMPL__" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /I "..\Include" /D "_DEBUG" /D "_WINDLL" /D "_AFXEXT" /D "_UNICODE" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\Include" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Bin_600/ProfUIS21d.dll" /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Bin_600/ProfUIS292ud.dll" /pdbtype:sept

!ELSEIF  "$(CFG)" == "ProfUISDLL - Win32 Unicode Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ProfUISDLL___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "ProfUISDLL___Win32_Unicode_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\UnicodeRelease\ProfUISDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "__PROF_UIS_IMPL__" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_WINDLL" /D "_AFXEXT" /D "_UNICODE" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\Include" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\Bin_600/ProfUIS21.dll"
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\Bin_600/ProfUIS292u.dll"

!ELSEIF  "$(CFG)" == "ProfUISDLL - Win32 MBCS Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ProfUISDLL___Win32_MBCS_Debug"
# PROP BASE Intermediate_Dir "ProfUISDLL___Win32_MBCS_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\MbcsDebug\ProfUISDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /I "..\Include" /D "_DEBUG" /D "_WINDLL" /D "_AFXEXT" /D "__PROF_UIS_IMPL__" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /I "..\Include" /D "_DEBUG" /D "_WINDLL" /D "_AFXEXT" /D "_AFXDLL" /D "_MBCS" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\Include" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Bin_600/ProfUIS292d.dll" /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Bin_600/ProfUIS292md.dll" /pdbtype:sept

!ELSEIF  "$(CFG)" == "ProfUISDLL - Win32 MBCS Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ProfUISDLL___Win32_MBCS_Release"
# PROP BASE Intermediate_Dir "ProfUISDLL___Win32_MBCS_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\MbcsRelease\ProfUISDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_WINDLL" /D "_AFXEXT" /D "__PROF_UIS_IMPL__" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_WINDLL" /D "_AFXEXT" /D "_AFXDLL" /D "_MBCS" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\Include" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\Bin_600/ProfUIS292.dll"
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\Bin_600/ProfUIS292m.dll"

!ELSEIF  "$(CFG)" == "ProfUISDLL - Win32 ANSI Debug RDE"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ProfUISDLL___Win32_ANSI_Debug_RDE"
# PROP BASE Intermediate_Dir "ProfUISDLL___Win32_ANSI_Debug_RDE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\AnsiDebugRDE\ProfUISDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /I "..\Include" /D "_DEBUG" /D "_WINDLL" /D "_AFXEXT" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /I "..\Include" /D "_DEBUG" /D "_WINDLL" /D "_AFXEXT" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_FOR_REGULAR_DLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\Include" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Bin_600/ProfUIS292nd.dll" /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Bin_600/ProfUIS292nd-RDE.dll" /pdbtype:sept

!ELSEIF  "$(CFG)" == "ProfUISDLL - Win32 ANSI Release RDE"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ProfUISDLL___Win32_ANSI_Release_RDE"
# PROP BASE Intermediate_Dir "ProfUISDLL___Win32_ANSI_Release_RDE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\AnsiReleaseRDE\ProfUISDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_WINDLL" /D "_AFXEXT" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_WINDLL" /D "_AFXEXT" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_FOR_REGULAR_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\Include" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\Bin_600/ProfUIS292n.dll"
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\Bin_600/ProfUIS292n-RDE.dll"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "ProfUISDLL - Win32 MBCS Debug RDE"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ProfUISDLL___Win32_MBCS_Debug_RDE"
# PROP BASE Intermediate_Dir "ProfUISDLL___Win32_MBCS_Debug_RDE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\MbcsDebugRDE\ProfUISDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /I "..\Include" /D "_DEBUG" /D "_WINDLL" /D "_AFXEXT" /D "_AFXDLL" /D "_MBCS" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /I "..\Include" /D "_DEBUG" /D "_MBCS" /D "_WINDLL" /D "_AFXEXT" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_FOR_REGULAR_DLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\Include" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Bin_600/ProfUIS292md.dll" /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Bin_600/ProfUIS292md-RDE.dll" /pdbtype:sept

!ELSEIF  "$(CFG)" == "ProfUISDLL - Win32 MBCS Release RDE"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ProfUISDLL___Win32_MBCS_Release_RDE"
# PROP BASE Intermediate_Dir "ProfUISDLL___Win32_MBCS_Release_RDE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\MbcsReleaseRDE\ProfUISDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_WINDLL" /D "_AFXEXT" /D "_AFXDLL" /D "_MBCS" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_MBCS" /D "_WINDLL" /D "_AFXEXT" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_FOR_REGULAR_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\Include" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\Bin_600/ProfUIS292m.dll"
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\Bin_600/ProfUIS292m-RDE.dll"

!ELSEIF  "$(CFG)" == "ProfUISDLL - Win32 Unicode Debug RDE"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ProfUISDLL___Win32_Unicode_Debug_RDE"
# PROP BASE Intermediate_Dir "ProfUISDLL___Win32_Unicode_Debug_RDE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\UnicodeDebugRDE\ProfUISDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /I "..\Include" /D "_DEBUG" /D "_WINDLL" /D "_AFXEXT" /D "_UNICODE" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /I "..\Include" /D "_DEBUG" /D "_UNICODE" /D "_WINDLL" /D "_AFXEXT" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_FOR_REGULAR_DLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\Include" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Bin_600/ProfUIS292ud.dll" /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Bin_600/ProfUIS292ud-RDE.dll" /pdbtype:sept

!ELSEIF  "$(CFG)" == "ProfUISDLL - Win32 Unicode Release RDE"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ProfUISDLL___Win32_Unicode_Release_RDE"
# PROP BASE Intermediate_Dir "ProfUISDLL___Win32_Unicode_Release_RDE"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Bin_600"
# PROP Intermediate_Dir "..\Bin_600\UnicodeReleaseRDE\ProfUISDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_WINDLL" /D "_AFXEXT" /D "_UNICODE" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GR /GX /O2 /I "..\Include" /D "NDEBUG" /D "_UNICODE" /D "_WINDLL" /D "_AFXEXT" /D "_AFXDLL" /D "__PROF_UIS_IMPL__" /D _WIN32_WINNT=0x0400 /D _WIN32_IE=0x0400 /D "WIN32" /D "_WINDOWS" /D "__PROF_UIS_FOR_REGULAR_DLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\Include" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\Bin_600/ProfUIS292u.dll"
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\Bin_600/ProfUIS292u-RDE.dll"

!ENDIF 

# Begin Target

# Name "ProfUISDLL - Win32 ANSI Release"
# Name "ProfUISDLL - Win32 ANSI Debug"
# Name "ProfUISDLL - Win32 Unicode Debug"
# Name "ProfUISDLL - Win32 Unicode Release"
# Name "ProfUISDLL - Win32 MBCS Debug"
# Name "ProfUISDLL - Win32 MBCS Release"
# Name "ProfUISDLL - Win32 ANSI Debug RDE"
# Name "ProfUISDLL - Win32 ANSI Release RDE"
# Name "ProfUISDLL - Win32 MBCS Debug RDE"
# Name "ProfUISDLL - Win32 MBCS Release RDE"
# Name "ProfUISDLL - Win32 Unicode Debug RDE"
# Name "ProfUISDLL - Win32 Unicode Release RDE"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
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

SOURCE=..\Src\ExtControls.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtProgressWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ExtControlsCommon.cpp
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
# End Source Fil
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
# ADD CPP /I "..\ProfUISDLL"
# End Source File
# Begin Source File

SOURCE=..\Src\ExtToolControlBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ProfUISDLL.cpp
# End Source File
# Begin Source File

SOURCE=.\ProfUISDLL.def
# End Source File
# Begin Source File

SOURCE=.\ProfUISDLL.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
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

SOURCE=..\Include\ExtControls.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtProgressWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtControlsCommon.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtGroupBox.h
# End Source File
# Begin Source File

SOURCE=..\Src\ExtHook.h
# End Source File
# Begin Source File

SOURCE=..\Include\ExtIconEditDlg.h
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

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\IDB_EXT_CUSTOM_TB_IMAGES_HI.bmp
# End Source File
# Begin Source File

SOURCE=.\res\IDB_EXT_RESIZABLE_DOCK_MARKER_2005_BOTTOM.bmp
# End Source File
# Begin Source File

SOURCE=.\res\IDB_EXT_RESIZABLE_DOCK_MARKER_2005_CENTER.bmp
# End Source File
# Begin Source File

SOURCE=.\res\IDB_EXT_RESIZABLE_DOCK_MARKER_2005_LEFT.bmp
# End Source File
# Begin Source File

SOURCE=.\res\IDB_EXT_RESIZABLE_DOCK_MARKER_2005_RIGHT.bmp
# End Source File
# Begin Source File

SOURCE=.\res\IDB_EXT_RESIZABLE_DOCK_MARKER_2005_TAB.bmp
# End Source File
# Begin Source File

SOURCE=.\res\IDB_EXT_RESIZABLE_DOCK_MARKER_2005_TOP.bmp
# End Source File
# Begin Source File

SOURCE=.\res\IDB_RESIZABLE_DOCK_MARKER_2005_BOTTOM.bmp
# End Source File
# Begin Source File

SOURCE=.\res\IDB_RESIZABLE_DOCK_MARKER_2005_LEFT.bmp
# End Source File
# Begin Source File

SOURCE=.\res\IDB_RESIZABLE_DOCK_MARKER_2005_RIGHT.bmp
# End Source File
# Begin Source File

SOURCE=.\res\IDB_RESIZABLE_DOCK_MARKER_2005_TAB.bmp
# End Source File
# Begin Source File

SOURCE=.\res\IDB_RESIZABLE_DOCK_MARKER_2005_TOP.bmp
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_ARROW_INVERTED_H.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_BIG_X_CROSS.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_BLACK_ARROW_BOTTOM.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_BLACK_ARROW_LEFT.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_BLACK_ARROW_RIGHT.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_BLACK_ARROW_TOP.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_BLACK_FINGER.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_BLACK_HAND.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_CMD_COPY.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_CMD_DELETE.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_CMD_MOVE.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_HAND.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_HOLLOW_CROSS_NORMAL.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_HOLLOW_CROSS_SMALL.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_PAN_ALL.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_PAN_BOTTOM.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_PAN_BOTTOM_LEFT.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_PAN_BOTTOM_RIGHT.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_PAN_HORZ.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_PAN_LEFT.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_PAN_RIGHT.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_PAN_TOP.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_PAN_TOP_LEFT.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_PAN_TOP_RIGHT.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_PAN_VERT.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_RESIZE_H1.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_RESIZE_H2.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_RESIZE_V1.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_RESIZE_V2.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_SUPPRESS.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_SUPPRESS_ARROW.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_ZOOM_HOLLOW.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_ZOOM_MINUS.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_EXT_ZOOM_PLUS.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_IED_TOOL_AIRBRUSH.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_IED_TOOL_BRUSH.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_IED_TOOL_CP.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_IED_TOOL_ELLIPSE.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_IED_TOOL_FILL.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_IED_TOOL_LINE.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_IED_TOOL_MAGNIFY.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_IED_TOOL_PENCIL.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_IED_TOOL_RECT.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_IED_TOOL_RR.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDC_IED_TOOL_RS.cur
# End Source File
# Begin Source File

SOURCE=.\res\IDD_EXT_COLOR_DIALOG.ico
# End Source File
# Begin Source File

SOURCE=.\res\IDD_EXT_ICON_EDIT_DIALOG.ico
# End Source File
# Begin Source File

SOURCE=.\res\IDD_EXT_ICON_MOUSE_POS.ico
# End Source File
# Begin Source File

SOURCE=.\res\IDD_EXT_ICON_SELECTION_SIZE.ico
# End Source File
# Begin Source File

SOURCE=.\res\IDR_EXT_TOOLBAR_IED_HELPER.bmp
# End Source File
# Begin Source File

SOURCE=.\res\IDR_EXT_TOOLBAR_IED_TOOLS.bmp
# End Source File
# Begin Source File

SOURCE=.\res\IDR_EXT_TOOLBAR_PROPERTY_GRID.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ProfUISDLL.rc2
# End Source File
# Begin Source File

SOURCE=.\res\toolbar_.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
