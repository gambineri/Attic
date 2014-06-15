# Microsoft Developer Studio Project File - Name="ocore" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=ocore - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ocore.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ocore.mak" CFG="ocore - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ocore - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ocore - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "ocore"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ocore - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\include" /I "$(AITECSA_DEV_INCLUDE)" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "MAINT_1" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 odbc32.lib itxlib.lib winmm.lib ws2_32.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"msvcrtd.lib" /out:"otannit.exe" /libpath:"$(AITECSA_DEV_LIB)"
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy otannit.exe "$(TANNIT_TEST_DIR)\tannit.exe"	copy otannit.exe $(TANNITDIR)\bin	copy ..\..\include\tnt.h $(AITECSA_DEV_INCLUDE)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ocore - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\bin"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "$(AITECSA_DEV_INCLUDE)" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "MAINT_1" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 odbc32.lib itxlib.lib winmm.lib ws2_32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /nodefaultlib:"msvcrt.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /out:"otannit.exe" /pdbtype:sept /libpath:"$(AITECSA_DEV_LIB)"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy otannit.exe "$(TANNIT_TEST_DIR)\tannit.exe"	copy otannit.exe $(TANNITDIR)\bin	copy ..\..\include\tnt.h $(AITECSA_DEV_INCLUDE)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "ocore - Win32 Release"
# Name "ocore - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\auxfile.cpp
# End Source File
# Begin Source File

SOURCE=.\cgiresolver.cpp
# End Source File
# Begin Source File

SOURCE=.\commands.cpp
# End Source File
# Begin Source File

SOURCE=.\commands2.cpp
# End Source File
# Begin Source File

SOURCE=.\desx.cpp
# End Source File
# Begin Source File

SOURCE=.\parser.cpp
# End Source File
# Begin Source File

SOURCE=.\tannitobj.cpp
# End Source File
# Begin Source File

SOURCE=.\templatefile.cpp
# End Source File
# Begin Source File

SOURCE=.\tntapiimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\tqr.cpp
# End Source File
# Begin Source File

SOURCE=.\tqrodbc.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\auxfile.h
# End Source File
# Begin Source File

SOURCE=.\cgiresolver.h
# End Source File
# Begin Source File

SOURCE=.\commands.h
# End Source File
# Begin Source File

SOURCE=.\defines.h
# End Source File
# Begin Source File

SOURCE=.\desx.h
# End Source File
# Begin Source File

SOURCE=.\parser.h
# End Source File
# Begin Source File

SOURCE=.\tannitobj.h
# End Source File
# Begin Source File

SOURCE=.\templatefile.h
# End Source File
# Begin Source File

SOURCE=..\..\include\tnt.h
# End Source File
# Begin Source File

SOURCE=.\tntapiimpl.h
# End Source File
# Begin Source File

SOURCE=.\tqr.h
# End Source File
# Begin Source File

SOURCE=.\tqrodbc.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Documents"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Osservazioni.txt
# End Source File
# End Group
# Begin Group "help"

# PROP Default_Filter "*.*"
# Begin Source File

SOURCE=..\..\Help\array.htm
# End Source File
# Begin Source File

SOURCE=..\..\Help\arrayget.htm
# End Source File
# Begin Source File

SOURCE=..\..\Help\arrayset.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\cfc.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\cgipath.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\crypt.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\cycle.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\decrypt.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\dmytime.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\endcycle.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\endfor.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\exeq.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\exist.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\exit.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\fmtcur.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\for.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\foridx.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\get.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\gethide.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\getvar.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\if.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\Index.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\misc.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\now.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\PLACEHOLDER.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\prex.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\prmfile.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\prmval.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\quotes.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\recsel.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\recval.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\return.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\setvar.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\stringcmd.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\strlen.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\styles.css
# End Source File
# Begin Source File

SOURCE=..\..\help\tqrstat.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\tqrstore.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\traceu.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\trans.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\utime.htm
# End Source File
# Begin Source File

SOURCE=..\..\help\valid.htm
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\include\itxtypes.h
# End Source File
# End Target
# End Project
