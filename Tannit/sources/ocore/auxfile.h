/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: auxfile.h,v $
 * $Revision: 1.24 $  
 * $Author: massimo $
 * $Date: 2002-06-25 18:25:14+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */


#ifndef __AUXFILE_H__
#define __AUXFILE_H__


#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "defines.h"
#include "itxlib.h"


#define   INI_REL_PATH0     "dbgname.par"
#define   INI_REL_PATH1     "." PATH_SEPARATOR "cgi-bin" PATH_SEPARATOR INI_REL_PATH0
#define   INI_REL_PATH2     "." PATH_SEPARATOR "cgi-itx" PATH_SEPARATOR INI_REL_PATH0

#define   DBGPATH           "DbgPath"

#define   CLOCKS_PER_MILLI  CLOCKS_PER_SEC/1000


//Text/Graphical aspect
#define   NOTHTML_HSEP        "-------------------------------------------------------------------------------\n"
#define   FIRST_ERROR_ANCHOR  "\"#FIRST_ERROR_ANCHOR\""

//Trace types
#define   DEFAULT             0
#define   IN_COMMAND          1   //Messages (not errors at all) incoming from commands.
#define   IN_WARNING          2   //Errors caused by the user
#define   IN_TNTAPI           4   //Errors incoming from a call made by the user to a TNT-api function.
#define   IN_ERROR            8   //INSIDE-CORE ERRORS: top severity. Use with care (best is: do not use).
#define   TEMPLATE            16  //Template separator for the debug page
                            



/*****************************************************************************
 ----------------------------  DebugFile object  -----------------------------
 *****************************************************************************/

class DebugFile
{
public:
//MEMBERS
  itxString     m_DebugPath;
  itxString     m_CurrentTemplate;
  bool          m_UseDebug;
  bool          m_Errors;
  bool          m_FirstErrorAnchor;
  FILE*         m_Debug;
  int           m_HTML;
  int           m_ReportLevel;
  unsigned int  m_StartMilliCount;
  unsigned int  m_StopMilliCount;
  itxSystem     m_Sys;

//METHODS
  void          Trace(int type, char* strarg, va_list args);
  void          _TimeTrace(char* strarg, va_list args);
  void          _StartMilliCount();
  void          _StopMilliCount();
  void          _TraceMilliDiff(char* msg);
  struct tm*    _CurrentDateTime();

  bool          Bufferize(itxString* outDebugBuf);
  void          Open();
  void          Close();

private:
  bool          ReadDbgname(FILE*, itxString*);
  void          ManageTypeOpen(int type);
  void          ManageTypeClose(int type);

public:
//CONSTRUCTION-DESTRUCTION
  DebugFile();
  ~DebugFile();
};



//////////////////////////////////////
// EXTERN Section

extern DebugFile   g_DebugFile;

#ifdef MAINT_1
  extern void       DebugTrace(char* strarg, ...);
  extern void       DebugTrace2(int type, char* strarg, ...);
  extern void       TimeTrace(char* strarg, ...);
  extern void       StartMilliCount();
  extern void       StopMilliCount();
  extern void       TraceMilliDiff(char* msg);
  extern struct tm* CurrentDateTime();
  extern char*      CurrentDateTimeStr();
  extern void       SQLTrace(itxSQLConnection* conn);
#else
  //toggles off some tedious warnings: 
  #pragma warning(disable : 4002) //too many actual parameters for macro '....'
  #pragma warning(disable : 4390) //';' : empty controlled statement found; is this the intent?

  #define DebugTrace()
  #define DebugTrace2()
  #define TimeTrace()
  #define StartMilliCount()
  #define StopMilliCount()
  #define TraceMilliDiff()
  #define CurrentDateTime()
  #define CurrentDateTimeStr()
#endif /* MAINT_1 */



#endif /* __AUXFILE_H__ */
