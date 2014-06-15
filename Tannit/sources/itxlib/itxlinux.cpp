/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxlinux.cpp,v $
 * $Revision: 1.5 $  
 * $Author: administrator $
 * $Date: 2002-07-23 16:05:31+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */


#ifdef LINUX

//This file implements the itxSystem class for Linux systems
#pragma message("=== itxSystem object is implemented for Linux platforms ===\n")

#include "itxsystem.h"
#include "itxstring.h"


//**************************************************************
int itxSystem::BSGetLastError()
{
  return errno;
}

//**************************************************************
int itxSystem::PRGetProcessId()
{
  return getpid();
}

//**************************************************************
int itxSystem::DLGetLastError()
{
  return atoi(dlerror());
}

//**************************************************************
//* Returns current time_t and millisecs fraction.
void itxSystem::TMGetMilliTime(time_t* now, int* milli)
{
  struct timeb timebuffer;   
  ftime(&timebuffer);

  if (now != NULL)
    *now = timebuffer.time;
  if (milli != NULL)
    *milli = timebuffer.millitm;
}

//**************************************************************
void itxSystem::FSSetMode(FILE* fp, int mode)
{
  //TBD maybe nothing to do :)
}

//**************************************************************
void* itxSystem::DLLoadLibrary(char* fmodule)
{
  return dlopen(fmodule, RTLD_LAZY);
}

//**************************************************************
bool itxSystem::DLFreeLibrary(void* pmodule)
{
  //TBD: verificare parametro di ritorno
  dlclose(pmodule);
  return true;
}

//**************************************************************
void* itxSystem::DLGetFunction(void* pmodule, char* pfuncname)
{
  return dlsym(pmodule, pfuncname);
}

//**************************************************************
void itxSystem::PRGetModuleName(void* pmodule, char* pfilename, int bufsize)
{
  //TBD
}

//**************************************************************
void itxSystem::FSGetCurrentDirectory(char* pfilename, int bufsize)
{
  getcwd(pfilename, (size_t)bufsize);
}

//**************************************************************
bool itxSystem::FSFileIsReadOnly(char* pfilename)
{
  //TBD
  return 0;
}

//**************************************************************
int itxSystem::BSSetenv(char* varname, char* varval)
{
  return setenv(varname, varval, 1);
}

//**************************************************************
void itxSystem::FSDirToTokenString(char* dir, char* filter, char token, itxString* retlist)
{
  if (dir == NULL || filter == NULL || retlist == NULL)
    return;
  if (*dir == 0 || *filter == 0)
    return;
  
  //TBD
}

//**************************************************************
void* itxSystem::THCreateThread(void* threadproc, void* threadarg, unsigned int* pretid)
{
  //TBD
  return 0;
}

//**************************************************************
void itxSystem::THPauseThread(void* thread)
{
  //TBD
}

//**************************************************************
void itxSystem::THResumeThread(void* thread)
{
  //TBD
}

//**************************************************************
void itxSystem::THTerminateThread(void* thread)
{
  //TBD
}

//**************************************************************
int itxSystem::SOGetLastError(int socket)
{
  int   res;
  char  lasterror[20]; 

  memset(lasterror, '\0', 0);

  socklen_t len;
  if ((res = getsockopt(socket, SOL_SOCKET, SO_ERROR, lasterror, &len)) == 0)
    return -1;
  
  sprintf(lasterror, "%d", res);
  return atoi(lasterror);
}

//**************************************************************
bool itxSystem::SOInitLibrary()
{
  return true;  //nothing to do here!
}

//**************************************************************
bool itxSystem::SOCloseLibrary()
{
  return true;  //nothing to do here!
}

//**************************************************************
void itxSystem::SOInitDescriptors(void* fdstruct, int socket)
{
  if (fdstruct != NULL)
  {
    FD_ZERO((fd_set*)fdstruct); //initialization
    FD_SET(socket, (fd_set*)fdstruct);
  }
}

//**************************************************************
void itxSystem::SOClose(int socket)
{
  close(socket); 
}




#endif  //LINUX
