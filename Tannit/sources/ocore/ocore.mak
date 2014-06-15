# Microsoft Developer Studio Generated NMAKE File, Based on ocore.dsp
!IF "$(CFG)" == ""
CFG=ocore - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ocore - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ocore - Win32 Release" && "$(CFG)" != "ocore - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ocore - Win32 Release"

OUTDIR=.\..\..\bin
INTDIR=.\Release

ALL : ".\otannit.exe"


CLEAN :
	-@erase "$(INTDIR)\auxfile.obj"
	-@erase "$(INTDIR)\cgiresolver.obj"
	-@erase "$(INTDIR)\commands.obj"
	-@erase "$(INTDIR)\desx.obj"
	-@erase "$(INTDIR)\parser.obj"
	-@erase "$(INTDIR)\tannitobj.obj"
	-@erase "$(INTDIR)\templatefile.obj"
	-@erase "$(INTDIR)\tntapiimpl.obj"
	-@erase "$(INTDIR)\tntsql.obj"
	-@erase "$(INTDIR)\tqr.obj"
	-@erase "$(INTDIR)\tqrodbc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\otannit.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "MAINT_1" /D "FCGITANNIT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ocore.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib advapi32.lib odbc32.lib winmm.lib libci.lib libc.lib oldnames.lib itxlib.lib libfcgi.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\otannit.pdb" /machine:I386 /nodefaultlib /out:"otannit.exe" /libpath:"..\..\lib" 
LINK32_OBJS= \
	"$(INTDIR)\tannitobj.obj" \
	"$(INTDIR)\auxfile.obj" \
	"$(INTDIR)\cgiresolver.obj" \
	"$(INTDIR)\commands.obj" \
	"$(INTDIR)\desx.obj" \
	"$(INTDIR)\parser.obj" \
	"$(INTDIR)\templatefile.obj" \
	"$(INTDIR)\tntapiimpl.obj" \
	"$(INTDIR)\tntsql.obj" \
	"$(INTDIR)\tqr.obj" \
	"$(INTDIR)\tqrodbc.obj"

".\otannit.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : ".\otannit.exe"
   copy otannit.exe "E:\Programmi\Apache Group\Apache\cgi-bin\otannit.exe"
	copy otannit.exe "W:\wwwroot\cometest\cgi-bin\temp_tannit.exe"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "ocore - Win32 Debug"

OUTDIR=.\..\..\bin
INTDIR=.\Debug

ALL : ".\otannit.exe"


CLEAN :
	-@erase "$(INTDIR)\auxfile.obj"
	-@erase "$(INTDIR)\cgiresolver.obj"
	-@erase "$(INTDIR)\commands.obj"
	-@erase "$(INTDIR)\desx.obj"
	-@erase "$(INTDIR)\parser.obj"
	-@erase "$(INTDIR)\tannitobj.obj"
	-@erase "$(INTDIR)\templatefile.obj"
	-@erase "$(INTDIR)\tntapiimpl.obj"
	-@erase "$(INTDIR)\tntsql.obj"
	-@erase "$(INTDIR)\tqr.obj"
	-@erase "$(INTDIR)\tqrodbc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\otannit.pdb"
	-@erase ".\otannit.exe"
	-@erase ".\otannit.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "MAINT_1" /D "FCGITANNIT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ocore.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib advapi32.lib odbc32.lib winmm.lib libc.lib libci.lib oldnames.lib itxlib.lib libfcgi.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\otannit.pdb" /debug /machine:I386 /out:"otannit.exe" /pdbtype:sept /libpath:"..\..\lib" 
LINK32_OBJS= \
	"$(INTDIR)\tannitobj.obj" \
	"$(INTDIR)\auxfile.obj" \
	"$(INTDIR)\cgiresolver.obj" \
	"$(INTDIR)\commands.obj" \
	"$(INTDIR)\desx.obj" \
	"$(INTDIR)\parser.obj" \
	"$(INTDIR)\templatefile.obj" \
	"$(INTDIR)\tntapiimpl.obj" \
	"$(INTDIR)\tntsql.obj" \
	"$(INTDIR)\tqr.obj" \
	"$(INTDIR)\tqrodbc.obj"

".\otannit.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : ".\otannit.exe"
   copy otannit.exe "E:\Programmi\Apache Group\Apache\cgi-bin\otannit.exe"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ocore.dep")
!INCLUDE "ocore.dep"
!ELSE 
!MESSAGE Warning: cannot find "ocore.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ocore - Win32 Release" || "$(CFG)" == "ocore - Win32 Debug"
SOURCE=.\auxfile.cpp

"$(INTDIR)\auxfile.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cgiresolver.cpp

"$(INTDIR)\cgiresolver.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\commands.cpp

"$(INTDIR)\commands.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\desx.cpp

"$(INTDIR)\desx.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\parser.cpp

"$(INTDIR)\parser.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\tannitobj.cpp

"$(INTDIR)\tannitobj.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\templatefile.cpp

"$(INTDIR)\templatefile.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\tntapiimpl.cpp

"$(INTDIR)\tntapiimpl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\tntsql.cpp

"$(INTDIR)\tntsql.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\tqr.cpp

"$(INTDIR)\tqr.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\tqrodbc.cpp

"$(INTDIR)\tqrodbc.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

