/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxsystem.h,v $
 * $Revision: 1.4 $  
 * $Author: administrator $
 * $Date: 2002-07-23 16:05:31+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */


#ifndef __ITXSYSTEM_H__
#define __ITXSYSTEM_H__

#include <time.h>

#include "itxwin32.h" //main header file for win32 platforms
#include "itxlinux.h" //main header file for linux platforms

#include "itxstring.h"


//-------------------------------------------------
//----------------    itxSystem   -----------------
//-------------------------------------------------

class itxSystem
{
public:
  // *** Base Services        BS
  int           BSSetenv(char* varname, char* varval);
  int           BSGetLastError();

  // *** Processes            PR
  int           PRGetProcessId();
  void          PRGetModuleName(void* pmodule, char* pfilename, int bufsize);

  // *** Dynamic Linking      DL
  void*         DLLoadLibrary(char* fmodule);
  bool          DLFreeLibrary(void* pmodule);
  void*         DLGetFunction(void* pmodule, char* pfuncname);
  int           DLGetLastError();


  // *** File System          FS
  void          FSDirToTokenString(char* dir, char* filter, char token, itxString* retlist);
  bool          FSFileIsReadOnly(char* pfilename);
  void          FSSetMode(FILE* fp, int mode);
  void          FSGetCurrentDirectory(char* pfilename, int bufsize);

  // *** Time                 TM
  void          TMGetMilliTime(time_t* now, int* milli);

  // *** Threads              TH
  void*         THCreateThread(void* entrypointfunc, void* entrypointarg, unsigned int* pretid);
  void          THPauseThread(void* thread);
  void          THResumeThread(void* thread);
  void          THTerminateThread(void* thread);

  // *** Sockets              SO
  int           SOGetLastError(int socket);
  bool          SOInitLibrary();
  bool          SOCloseLibrary();
  void          SOInitDescriptors(void* fdstruct, int socket);
  void          SOClose(int socket);


  //CONSTRUCTION-DESTRUCTION
  itxSystem(){}
  ~itxSystem(){}
};


#endif // __ITXSYSTEM_H__

