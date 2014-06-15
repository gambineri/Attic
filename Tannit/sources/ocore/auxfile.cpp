/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: auxfile.cpp,v $
 * $Revision: 1.47 $  
 * $Author: massimo $
 * $Date: 2002-06-25 18:25:14+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */



#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "defines.h"
#include "itxtypes.h"
#include "auxfile.h"



DebugFile       g_DebugFile;



/*****************************************************************************
 ------------------------ Global DebugTrace  Method --------------------------
 *****************************************************************************/

#ifdef MAINT_1   //check if must use dbg

//----------- DebugTrace
  void DebugTrace(char* strarg, ...)
  {
    if (g_DebugFile.m_UseDebug)
    {
      va_list args;
      
      va_start(args, strarg);
      g_DebugFile.Trace(DEFAULT, strarg, args);
	    va_end(args);
    }
  }

//----------- DebugTrace2
  void DebugTrace2(int type, char* strarg, ...)
  {
    try
    {
      if (g_DebugFile.m_UseDebug)
      {
        va_list args;
      
        va_start(args, strarg);
        g_DebugFile.Trace(type, strarg, args);
	      va_end(args);
      }
    }
    catch(...)
    {
      throw;
    }
  }

//----------- TimeTrace
  void TimeTrace(char* strarg, ...)
  {
#ifdef TIMETRACE

    if (g_DebugFile.m_UseDebug)
    {
      va_list       args;

      va_start(args, strarg);
      g_DebugFile._TimeTrace(strarg, args);
	    va_end(args);
    }

#endif // TIMETRACE
  }

//----------- StartMilliCount
void StartMilliCount()
  {
#ifdef TIMETRACE

    if (g_DebugFile.m_UseDebug)
      g_DebugFile._StartMilliCount();

#endif // TIMETRACE
  }

//----------- StopMilliCount
void StopMilliCount()
  {
#ifdef TIMETRACE

    if (g_DebugFile.m_UseDebug)
      g_DebugFile._StopMilliCount();

#endif // TIMETRACE
  }

//----------- TraceMilliDiff
void TraceMilliDiff(char* msg)
  {
#ifdef TIMETRACE

    if (g_DebugFile.m_UseDebug)
      g_DebugFile._TraceMilliDiff(msg);

#endif // TIMETRACE
  }

//----------- CurrentDateTime
struct tm* CurrentDateTime()
  {
#ifdef TIMETRACE

    if (g_DebugFile.m_UseDebug)
      return g_DebugFile._CurrentDateTime();
    else
      return NULL;

#endif // TIMETRACE
    return NULL;
  }

//----------- CurrentDateTimeStr
char* CurrentDateTimeStr()
  {
#ifdef TIMETRACE

    if (g_DebugFile.m_UseDebug)
    {
      struct tm* ptm = g_DebugFile._CurrentDateTime();
      return (ptm != NULL ? asctime(ptm) : "");
    }
    else
      return "";

#endif // TIMETRACE
    return "-TIMETRACE DISABLED-";
  }

//----------- SQLTrace
void SQLTrace(itxSQLConnection* conn)
{
  int error_condition = DEFAULT;
  itxString errormsg;

  memset(conn->m_sqlstate, '\0', ITX_SQL_STATUS_LEN);
  memset(conn->m_message, '\0', ITX_SQL_MAX_ERRMSG);
  errormsg = "<br>";

  //Diagnostic environment errors
  SQLGetDiagRec(SQL_HANDLE_ENV, conn->m_henv, 1, conn->m_sqlstate, &(conn->m_nativerr), conn->m_message, 
                ITX_SQL_MAX_ERRMSG - 1, &(conn->m_msglength));

  if (!ISNULL((const char*) conn->m_sqlstate))
  {
    error_condition = IN_WARNING;
    errormsg += "   SQL_HANDLE_ENV : ";
    errormsg += (char*) conn->m_sqlstate;
    errormsg += "<br>";
    errormsg += (char*) conn->m_message;
    errormsg += "<br>";
  }

  //Diagnostic db connection errors
  SQLGetDiagRec(SQL_HANDLE_DBC, conn->m_hdbc, 1, conn->m_sqlstate, &(conn->m_nativerr), conn->m_message, 
                ITX_SQL_MAX_ERRMSG - 1, &(conn->m_msglength));

  if (!ISNULL((const char*) conn->m_sqlstate))
  {
    error_condition = IN_WARNING;
    errormsg += "   SQL_HANDLE_DBC : ";
    errormsg += (char*) conn->m_sqlstate;
    errormsg += "<br>";
    errormsg += (char*) conn->m_message;
    errormsg += "<br>";
  }

  //Diagnostic sql statement errors
  SQLGetDiagRec(SQL_HANDLE_STMT, conn->m_hstmt, 1, conn->m_sqlstate, &(conn->m_nativerr), conn->m_message, 
                ITX_SQL_MAX_ERRMSG - 1, &(conn->m_msglength));

  if (!ISNULL((const char*) conn->m_sqlstate))
  {
    error_condition = IN_WARNING;
    errormsg += "   SQL_HANDLE_STMT : ";
    errormsg += (char*) conn->m_sqlstate;
    errormsg += "<br>";
    errormsg += (char*) conn->m_message;
    errormsg += "<br>";
  }

  try
  {
    errormsg += "Last Executed Query: <br>";
    errormsg += conn->m_statement;
    errormsg += "<br>";
    if (error_condition == DEFAULT)
    {
      errormsg += "ODBC statement completed successfully<br>";
    }
  }
  catch(...)
  {
    DebugTrace("CATCH INSIDE itxSQLConnection::DebugTrace\n");
  }

  DebugTrace2(error_condition, "%s", errormsg.GetBuffer());  
}

#endif /* MAINT_1 */


/*****************************************************************************
 ----------------------------  DebugFile METHODS  ----------------------------
 *****************************************************************************/

DebugFile::DebugFile()
{
  m_Debug       = NULL;
  m_UseDebug    = false;
  m_Errors      = false;
  m_ReportLevel = 0;
  m_FirstErrorAnchor = false;
}

DebugFile::~DebugFile()
{
  if (m_Debug != NULL) 
    fclose(m_Debug);
}

//----------------------------------------------------------------------
void DebugFile::Open()
{
#ifdef MAINT_1

	FILE* dbgnamefp;
	if((dbgnamefp = fopen(INI_REL_PATH1, "r")) == NULL)
	{
		if((dbgnamefp = fopen(INI_REL_PATH2, "r")) == NULL)
			dbgnamefp = fopen(INI_REL_PATH0, "r");
	}

	if(dbgnamefp != NULL)
	{
	  itxString parValue;
	  itxString pid;

    if (ReadDbgname(dbgnamefp, &parValue))
    {
#ifdef FCGITANNIT
		  m_DebugPath  = ".";
      m_DebugPath += PATH_SEPARATOR;
      m_DebugPath += "Tannit_";

    //The last part of the name of the dbg file is the pid of the Tannit current instance.
      pid.SetInt(m_Sys.PRGetProcessId());

		  m_DebugPath += pid;
		  m_DebugPath += TPL_EXT;
#else
//		  m_DebugPath  = ".";
//      m_DebugPath += PATH_SEPARATOR;
//      m_DebugPath += "Tannit.htm";

		  m_DebugPath  = parValue;
#endif  //FCGITANNIT

      m_HTML = 1; //TBD: currently force to use html ... maybe forever?
    }

		fclose(dbgnamefp);
    
    struct tm *newtime; // needed for Start Time
    time_t aclock;

		if ((m_Debug = fopen(m_DebugPath.GetBuffer(), "w")) != NULL)
    {
			m_UseDebug = true;
      
      // Start Time
      time(&aclock);                 
      newtime = localtime(&aclock);  

      if (m_HTML)
      {
        fprintf(m_Debug, "<HTML><BODY bgcolor=\"#FFFFFF\">\n");

        fprintf(m_Debug, "<FONT FACE=Verdana SIZE=2><center><table border=1 cellpadding=5>\n");
        fprintf(m_Debug, "<tr>\n");
        fprintf(m_Debug, "<td>Regular text</td>\n");
        fprintf(m_Debug, "<td>Normal internal tracing</td>\n");
        fprintf(m_Debug, "</tr>\n");

        fprintf(m_Debug, "<tr>\n");
        fprintf(m_Debug, "<td><b>Bold text<b></td>\n");
        fprintf(m_Debug, "<td>Inside command internal tracing</td>\n");
        fprintf(m_Debug, "</tr>\n");

        fprintf(m_Debug, "<tr>\n");
        fprintf(m_Debug, "<td><FONT COLOR=#0033FF>Blu text</FONT></td>\n");
        fprintf(m_Debug, "<td>warning reports: usually bad command usage</td>\n");
        fprintf(m_Debug, "</tr>\n");

        fprintf(m_Debug, "<tr>\n");
        fprintf(m_Debug, "<td><FONT COLOR=#00AA00>Green text</FONT></td>\n");
        fprintf(m_Debug, "<td>Tannit API internal errors</td>\n");
        fprintf(m_Debug, "</tr>\n");

        fprintf(m_Debug, "<tr>\n");
        fprintf(m_Debug, "<td><FONT COLOR=#FF0000>Red text</FONT></td>\n");
        fprintf(m_Debug, "<td>Tannit internal errors</td>\n");
        fprintf(m_Debug, "</tr>\n");
        fprintf(m_Debug, "</table></center><hr>\n");

        fprintf(m_Debug, "<A HREF=%s>First Error</A>\n", FIRST_ERROR_ANCHOR);

        fprintf(m_Debug, "<center>At %s Tannit begins ...</center><hr><br>\n", asctime(newtime));
      }
      else
      {
        fprintf(m_Debug, "At %s Tannit begins ...\n", asctime(newtime));
        fprintf(m_Debug, NOTHTML_HSEP);
      }
    }
	}
#endif /* MAINT_1 */
}


//----------------------------------------------------------------------
void DebugFile::Close()
{
  if (m_Debug != NULL)
    fclose(m_Debug);
}


/************************************************************************************
NOME			:ReadDbgname
attivita'	:ricerca nel file dbgname.par il valore del parametro 'DbgPath'.
*************************************************************************************/
bool DebugFile::ReadDbgname(FILE* dbgfp, itxString* parValue)
{
	char*  token = NULL;
  char*  retVal = NULL;
	char   fileLine[INIT_FILE_LINE_LEN];

  if(fseek(dbgfp, 0L, SEEK_SET) != 0)
		return false;

	// scansione delle linee del file
	while (fgets(fileLine, INIT_FILE_LINE_LEN, dbgfp) != 0)
	{
		// se il primo carattere e' '#' la linea non va letta: e' un commento
		if (fileLine[0] == '#')
			continue;

		// il segno di uguale determina la fine del token candidato a id del parametro:
		// si confronta il token con il parametro da cercare
		token = strtok(fileLine, "=");
		if (strcmp(token, DBGPATH) == 0)
		{
			retVal = strtok(NULL,"\n");
			if (retVal != NULL)
      {		
        *parValue = retVal;
        return true;  //attualmente leggiamo un solo parametro.
      }
		}
	}

	return false;
}

/************************************************************************************
NOME			: Trace
attivita'	: traccia come una fprintf sul file di debug.
*************************************************************************************/

void DebugFile::Trace(int type, char* strarg, va_list args)
{
#ifdef MAINT_1   //check if must use dbg
  if (m_UseDebug)
  {
    if (type < m_ReportLevel)
      return;

    //Manage type: html 'open' tags
    ManageTypeOpen(type);

    try
    {
      vfprintf(m_Debug, strarg, args);
    }
    catch(...)
    {
      fprintf(m_Debug, "OMISSIS: error while tracing.");
    }

    //Manage type: html 'close' tags 
    ManageTypeClose(type);

    fflush(m_Debug);
  }
#endif /* MAINT_1 */
}

/*************************************************************************************
NOME			: _TimeTrace
attivita'	: traccia come una fprintf sul file di debug il tempo fino ai millisecondi.
*************************************************************************************/
void DebugFile::_TimeTrace(char* strarg, va_list args)
{
#ifdef TIMETRACE
  if (m_UseDebug)
  {
    time_t  now;
    int     millinow;
    char*   timeline;

    m_Sys.TMGetMilliTime(&now, &millinow);
    timeline = ctime(&now);

    try
    {
      fprintf(g_DebugFile.m_Debug, "EVENT: ", args);
      vfprintf(g_DebugFile.m_Debug, strarg, args);
      fprintf(g_DebugFile.m_Debug, " - ", args);
      
      fprintf(g_DebugFile.m_Debug, "TIME IS: %.19s.%hu %s", timeline, millinow, &timeline[20]);
    }
    catch(...)
    {
      fprintf(m_Debug, "OMISSIS: argument is too long to trace");
    }

    if (m_HTML)
      fprintf(m_Debug, "<br>\n");

    fflush(m_Debug);
  }
#endif // TIMETRACE
}

/************************************************************************************
NOME			: _StartMilliCount
attivita'	: Acquisisce i millisecondi dallo startup del processo.
*************************************************************************************/

void DebugFile::_StartMilliCount()
{
#ifdef TIMETRACE
  m_StartMilliCount = clock()*CLOCKS_PER_MILLI;
#endif // TIMETRACE
}

/************************************************************************************
NOME			: _StopMilliCount
attivita'	: Acquisisce i millisecondi dallo startup del processo.
*************************************************************************************/

void DebugFile::_StopMilliCount()
{
#ifdef TIMETRACE
  m_StopMilliCount = clock()*CLOCKS_PER_MILLI;
#endif // TIMETRACE
}

/************************************************************************************
NOME			: _TraceMilliDiff
attivita'	: stampa la differenza tra m_StartMilliCount e m_StopMilliCount.
*************************************************************************************/

void DebugFile::_TraceMilliDiff(char* msg)
{
#ifdef TIMETRACE
  if (m_UseDebug)
  {
    fprintf(m_Debug, "%s - Difference in milliseconds: %d", msg, m_StopMilliCount - m_StartMilliCount);
    if (m_HTML)
      fprintf(m_Debug, "<br>\n");
    fflush(m_Debug);
  }
#endif // TIMETRACE
}

/************************************************************************************
NOME			: _CurrentDateTime
attivita'	: Returns the current time .
*************************************************************************************/
struct tm* DebugFile::_CurrentDateTime()
{
  //Take current time and print it
  time_t aclock;
  time(&aclock);                 
  return localtime(&aclock);  
}

//----------------------------------------------------------------------
void DebugFile::ManageTypeOpen(int type)
{
  if (m_HTML != 0)
  {

    if (!m_FirstErrorAnchor && 
        (type == IN_WARNING || type == IN_ERROR || type == IN_TNTAPI)) 
    { //put the anchor on the first error
      m_FirstErrorAnchor = true;
      fprintf(m_Debug, "<A NAME=%s></a>", FIRST_ERROR_ANCHOR);
    }

    for(int i = 0; i<sizeof(int)*8; i++)
    {
      switch((1<<i) & type)
      {
      case IN_COMMAND:
        fprintf(m_Debug, "<b> template is %s: <br>", m_CurrentTemplate.GetBuffer());
      break;

      case IN_ERROR:
        fprintf(m_Debug, "<FONT COLOR=#FF0000> template is %s: <br>", m_CurrentTemplate.GetBuffer());
        m_Errors = true;
      break;

      case IN_WARNING:
        fprintf(m_Debug, "<FONT COLOR=#0033FF> template is %s: <br>", m_CurrentTemplate.GetBuffer());
        m_Errors = true;
      break;

      case IN_TNTAPI:
        fprintf(m_Debug, "<FONT COLOR=#00AA00> template is %s: <br>", m_CurrentTemplate.GetBuffer());
        m_Errors = true;
      break;

      case TEMPLATE:
        fprintf(m_Debug, "<br><hr><i>");
      break;
      }
    }
  }
  else
  {
    for(int i = 0; i<sizeof(int); i++)
    {
      switch((1<<i) & type)
      {
      case IN_COMMAND:
        fprintf(m_Debug, "    ");
      break;

      case TEMPLATE:
        fprintf(m_Debug, NOTHTML_HSEP);
      break;
      }
    }
  }
}

//----------------------------------------------------------------------
void DebugFile::ManageTypeClose(int type)
{
  if (m_HTML != 0)
  {
    for(int i = 0; i<sizeof(int)*8; i++)
    {
      switch((1<<i) & type)
      {
      case IN_COMMAND:
        fprintf(m_Debug, "</b>");
      break;

      case IN_ERROR:
      case IN_WARNING:
      case IN_TNTAPI:
        fprintf(m_Debug, "</FONT>");
      break;

      case TEMPLATE:
        fprintf(m_Debug, "</i><hr>");
      break;
      }
    }

    fprintf(m_Debug, "<br>");
  }
  else
  {
    for(int i = 0; i<sizeof(int); i++)
    {
      switch((1<<i) & type)
      {
      case TEMPLATE:
        fprintf(m_Debug, NOTHTML_HSEP);
      break;
      }
    }
  }
}

//----------------------------------------------------------------------
bool DebugFile::Bufferize(itxString* outDebugBuf)
{
  if (m_Debug == NULL)
    return false;

  //Valutazione della Content-length
  fseek(m_Debug, 0, SEEK_SET);
  int ContentLen = ftell(m_Debug);
  fseek(m_Debug, 0, SEEK_END);
  ContentLen = ftell(m_Debug) - ContentLen;
  rewind(m_Debug);

  char* dataBuffer;
  if ((dataBuffer = (char*)calloc(ContentLen + 1, sizeof(char))) == NULL) 
    return false;

  fclose(m_Debug);
  m_Debug = fopen(m_DebugPath.GetBuffer(), "rb");

  //lettura 
  int numread;
  if ((numread = fread(dataBuffer, sizeof(char), ContentLen, m_Debug)) < ContentLen)
    return false;
  
	dataBuffer[ContentLen] = 0; // sostituzione del carattere EOF con 0
  *outDebugBuf = dataBuffer;
  free(dataBuffer);

  fclose(m_Debug);
  m_Debug = fopen(m_DebugPath.GetBuffer(), "at");

  return true;
}
