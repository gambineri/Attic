/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxwin32.cpp,v $
 * $Revision: 1.4 $  
 * $Author: administrator $
 * $Date: 2002-07-23 16:05:31+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */

#ifdef WIN32

//This file implements the itxSystem class for (maybe) all WIN32 subsystems
#pragma message("=== itxSystem object is implemented for WIN32 platforms ===\n")


#include "itxsystem.h"
#include "itxstring.h"


//**************************************************************
int itxSystem::BSGetLastError()
{
  return ::GetLastError();
}

//**************************************************************
int itxSystem::PRGetProcessId()
{
  return ::GetCurrentProcessId();
}

//**************************************************************
int itxSystem::DLGetLastError()
{
  return ::GetLastError();
}

//**************************************************************
//* Returns current time_t and millisecs fraction.
void itxSystem::TMGetMilliTime(time_t* now, int* milli)
{
  struct _timeb timebuffer;

  _ftime(&timebuffer);

  if (now != NULL)
    *now = timebuffer.time;
  if (milli != NULL)
    *milli = timebuffer.millitm;
}

//**************************************************************
void itxSystem::FSSetMode(FILE* fp, int mode)
{
  _setmode(_fileno(fp), mode);
}

//**************************************************************
void* itxSystem::DLLoadLibrary(char* fmodule)
{
  return (fmodule!= NULL ? ::LoadLibrary(fmodule) : NULL);
}

//**************************************************************
bool itxSystem::DLFreeLibrary(void* pmodule)
{
  return (::FreeLibrary((HINSTANCE)pmodule) ? true : false);
}

//**************************************************************
void* itxSystem::DLGetFunction(void* pmodule, char* pfuncname)
{
  return ::GetProcAddress((HINSTANCE)pmodule, (LPCSTR)pfuncname);
}

//**************************************************************
void itxSystem::PRGetModuleName(void* pmodule, char* pfilename, int bufsize)
{
  ::GetModuleFileName((HINSTANCE)pmodule, pfilename, bufsize);
}

//**************************************************************
void itxSystem::FSGetCurrentDirectory(char* pfilename, int bufsize)
{
  ::GetCurrentDirectory(bufsize, pfilename);
}

//**************************************************************
bool itxSystem::FSFileIsReadOnly(char* pfilename)
{
  return ((::GetFileAttributes(pfilename) & FILE_ATTRIBUTE_READONLY) ? true : false);
}

//**************************************************************
int itxSystem::BSSetenv(char* varname, char* varval)
{
  itxString appo;
  appo  = varname;
  appo += "=";
  appo += varval;
  return _putenv(appo.GetBuffer());
}

//**************************************************************
void itxSystem::FSDirToTokenString(char* dir, char* filter, char token, itxString* retlist)
{
  if (dir == NULL || filter == NULL || retlist == NULL)
    return;
  if (*dir == 0 || *filter == 0)
    return;
  
  itxString       auxpath;
  itxString       pattern;
  WIN32_FIND_DATA ffdata;
  HANDLE          hff;
  bool            goon;

  auxpath  = dir;
  if (dir[strlen(dir) - 1] != PATH_SEPARATOR_C)
    auxpath += PATH_SEPARATOR_C;

  pattern = auxpath;
  pattern += filter;
  
  hff = ::FindFirstFile(pattern.GetBuffer(), &ffdata);
  if (hff != INVALID_HANDLE_VALUE)
  {
    goon = true;
    while (goon)
    {
      if (strcmp(ffdata.cFileName, ".") != 0 && strcmp(ffdata.cFileName, "..") != 0)
      {
        *retlist += auxpath;
        *retlist += ffdata.cFileName;
        *retlist += token;
      }
      goon = (::FindNextFile(hff, &ffdata) != 0);
    }
  }
}

//**************************************************************
void* itxSystem::THCreateThread(void* threadproc, void* threadarg, unsigned int* pretid)
{
  DWORD retid = 0;
  SECURITY_ATTRIBUTES sa;

  sa.nLength              = sizeof(SECURITY_ATTRIBUTES); 
  sa.lpSecurityDescriptor = NULL;
  sa.bInheritHandle       = TRUE; 

	void* rethandle = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadproc, threadarg, 0, &retid);
  
  if (rethandle != NULL && pretid != NULL)
    *pretid = retid;
  else
    pretid = NULL;
  
  return rethandle;
}

//**************************************************************
void itxSystem::THPauseThread(void* thread)
{
  ::SuspendThread(thread);
}

//**************************************************************
void itxSystem::THResumeThread(void* thread)
{
  ::ResumeThread(thread);
}

//**************************************************************
void itxSystem::THTerminateThread(void* thread)
{
  ::TerminateThread(thread, -1);
}

//**************************************************************
int itxSystem::SOGetLastError(int socket)
{
  //socket param unused under win32
  return ::WSAGetLastError();
}

//**************************************************************
bool itxSystem::SOInitLibrary()
{
	WORD     wVersionRequested = MAKEWORD(1,1);
  WSADATA  wsaData; 

  if (::WSAStartup(wVersionRequested, &wsaData) == SOCKET_ERROR) 
  { 
    ::WSACleanup();
    return false;
  }

  return true;
}

//**************************************************************
bool itxSystem::SOCloseLibrary()
{
  ::WSACleanup();
  return true;
}

//**************************************************************
void itxSystem::SOInitDescriptors(void* fdstruct, int socket)
{
  if (fdstruct != NULL)
  {
    ((fd_set*)fdstruct)->fd_array[0] = socket;
    ((fd_set*)fdstruct)->fd_count = 1;
  }
}

//**************************************************************
void itxSystem::SOClose(int socket)
{
  closesocket(socket); 
}




#endif  //WIN32

