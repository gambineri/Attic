# Microsoft Developer Studio Project File - Name="itannit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=itannit - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "itannit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "itannit.mak" CFG="itannit - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "itannit - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "itannit - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "itannit"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "itannit - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\itannit.lib"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\itannit.lib"

!ELSEIF  "$(CFG)" == "itannit - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"itannit.lib"

!ENDIF 

# Begin Target

# Name "itannit - Win32 Release"
# Name "itannit - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\iregtp.cpp
# End Source File
# Begin Source File

SOURCE=.\itannit.cpp
# End Source File
# Begin Source File

SOURCE=.\itncnv.cpp
# End Source File
# Begin Source File

SOURCE=.\itntwrap.cpp
# End Source File
# Begin Source File

SOURCE=.\tntsql.cpp
# End Source File
# Begin Source File

SOURCE=.\tpcomm.cpp
# End Source File
# Begin Source File

SOURCE=.\tpparser.cpp
# End Source File
# Begin Source File

SOURCE=.\tptypes.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\iappcdim.h
# End Source File
# Begin Source File

SOURCE=.\iregtp.h
# End Source File
# Begin Source File

SOURCE=.\itannit.h
# End Source File
# Begin Source File

SOURCE=..\..\include\itannitc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\itannitdef.h
# End Source File
# Begin Source File

SOURCE=..\..\include\itxtypes.h
# End Source File
# Begin Source File

SOURCE=..\..\include\tannitds.h
# End Source File
# Begin Source File

SOURCE=.\tntsql.h
# End Source File
# Begin Source File

SOURCE=.\tpcomm.h
# End Source File
# Begin Source File

SOURCE=.\tptypes.h
# End Source File
# End Group
# End Target
# End Project
