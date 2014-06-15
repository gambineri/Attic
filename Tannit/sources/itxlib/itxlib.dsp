# Microsoft Developer Studio Project File - Name="itxlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=itxlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "itxlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "itxlib.mak" CFG="itxlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "itxlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "itxlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "itxlib"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "itxlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "$(AITECSA_DEV_INCLUDE)" /I "G:\programmi\ssl\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=$(ITXLIBDIR)\Esporta_Release_ITXLIB.cmd	$(TANNITDIR)\sources\ocore\Importa_Release_ITXLIB.cmd
# End Special Build Tool

!ELSEIF  "$(CFG)" == "itxlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(AITECSA_DEV_INCLUDE)" /I "G:\programmi\ssl\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=$(ITXLIBDIR)\Esporta_Debug_ITXLIB.cmd	$(TANNITDIR)\sources\ocore\Importa_Debug_ITXLIB.cmd
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "itxlib - Win32 Release"
# Name "itxlib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\itxbuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\itxcoll.cpp
# End Source File
# Begin Source File

SOURCE=.\itxfileini.cpp
# End Source File
# Begin Source File

SOURCE=.\itxhttp.cpp
# End Source File
# Begin Source File

SOURCE=.\itxlib.cpp
# End Source File
# Begin Source File

SOURCE=.\itxlinux.cpp
# End Source File
# Begin Source File

SOURCE=.\itxsmtp.cpp
# End Source File
# Begin Source File

SOURCE=.\itxsocket.cpp
# End Source File
# Begin Source File

SOURCE=.\itxsql.cpp
# End Source File
# Begin Source File

SOURCE=.\itxstring.cpp
# End Source File
# Begin Source File

SOURCE=.\itxthread.cpp
# End Source File
# Begin Source File

SOURCE=.\itxtime.cpp
# End Source File
# Begin Source File

SOURCE=.\itxwin32.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\itxbuffer.h
# End Source File
# Begin Source File

SOURCE=.\itxcoll.h
# End Source File
# Begin Source File

SOURCE=.\itxdefines.h
# End Source File
# Begin Source File

SOURCE=.\itxexception.h
# End Source File
# Begin Source File

SOURCE=.\itxfileini.h
# End Source File
# Begin Source File

SOURCE=.\itxhttp.h
# End Source File
# Begin Source File

SOURCE=.\itxlib.h
# End Source File
# Begin Source File

SOURCE=.\itxlinux.h
# End Source File
# Begin Source File

SOURCE=.\itxsmtp.h
# End Source File
# Begin Source File

SOURCE=.\itxsocket.h
# End Source File
# Begin Source File

SOURCE=.\itxsql.h
# End Source File
# Begin Source File

SOURCE=.\itxstring.h
# End Source File
# Begin Source File

SOURCE=.\itxsystem.h
# End Source File
# Begin Source File

SOURCE=.\itxthread.h
# End Source File
# Begin Source File

SOURCE=.\itxtime.h
# End Source File
# Begin Source File

SOURCE=.\itxwin32.h
# End Source File
# Begin Source File

SOURCE=.\itxwraplib.h
# End Source File
# End Group
# End Target
# End Project
