/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#ifndef __ITX_TNTSQL_CPP__
#define __ITX_TNTSQL_CPP__
#endif

#include <time.h>

#include "tntsql.h"

#include <sql.h>
#include <sqlext.h>

#define SQBOOL(a)   (SQL_SUCCEEDED(a)? TRUE : FALSE)

itxSQLConnection::itxSQLConnection()
{
  memset(m_dsn, 0, ITX_SQL_DSN_LENGTH);
  memset(m_uid, 0, ITX_SQL_UID_LENGTH);
  memset(m_pwd, 0, ITX_SQL_PWD_LENGTH);

  m_henv  = 0;
  m_hdbc  = 0;
  m_hstmt = 0;

  m_statement = 0;
  m_cols = 0;
  m_rows = 0;
}

bool itxSQLConnection::Create(char* dsn, char* uid, char* pwd)
{  
  if (IsConnected())
    return TRUE;

  SQLRETURN retcode;

	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);	/*Allocate environment handle */

	if (SQL_SUCCEEDED(retcode)) 
  {
		retcode = SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0); /* Set the ODBC version environment attribute */
		
    if (SQL_SUCCEEDED(retcode)) 
    {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc); /* Allocate connection handle */
		
    	if (SQL_SUCCEEDED(retcode)) 
      {
				retcode = SQLConnect(m_hdbc, (SQLCHAR*) dsn, SQL_NTS,	/* Connect to data source */
														(SQLCHAR*) uid, SQL_NTS,(SQLCHAR*) pwd, SQL_NTS);
			
      	if (SQL_SUCCEEDED(retcode))
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &m_hstmt); /* Allocate statement handle */
      }
    }
  }

  return SQBOOL(retcode);
}

bool itxSQLConnection::SetAttributes(int attr, void* value, int valuelen)
{
  SQLRETURN retcode;

	retcode = SQLSetConnectAttr(m_hdbc, (SQLINTEGER) attr, (SQLPOINTER) value, (SQLINTEGER) valuelen);
  return SQBOOL(retcode);
}

bool itxSQLConnection::ManualCommit()
{
  return SetAttributes(SQL_ATTR_AUTOCOMMIT, (void*) SQL_AUTOCOMMIT_OFF, 0);
}

bool itxSQLConnection::AutoCommit()
{
  return SetAttributes(SQL_ATTR_AUTOCOMMIT, (void*) SQL_AUTOCOMMIT_ON, 0);
}

bool itxSQLConnection::Commit()
{
  SQLRETURN retcode;

	retcode = SQLEndTran(SQL_HANDLE_DBC, m_hdbc, SQL_COMMIT);
  return SQBOOL(retcode);
}

bool itxSQLConnection::Rollback()
{
  SQLRETURN retcode;

	retcode = SQLEndTran(SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK);
  return SQBOOL(retcode);
}

#if 0
// Versione per i DSN su file: bisogna valorizzare il parametro ODBCDSN nel prm 
// con la stringa 'FILEDSN=nome_file_dsn;'
bool itxSQLConnection::Create(char* dsn, char* uid = "", char* pwd = "")
{  
	char        OutStr[1024];
  SQLRETURN   retcode;
  SQLSMALLINT outstrlen;

	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);	/*Allocate environment handle */

	if (SQL_SUCCEEDED(retcode)) 
  {
		retcode = SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0); /* Set the ODBC version environment attribute */
		
    if (SQL_SUCCEEDED(retcode)) 
    {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc); /* Allocate connection handle */
		
    	if (SQL_SUCCEEDED(retcode)) 
      {
				retcode = SQLDriverConnect(m_hdbc, NULL, (SQLCHAR*)dsn, SQL_NTS, (SQLCHAR*)OutStr, 1024, &outstrlen, SQL_DRIVER_NOPROMPT);
			
      	if (SQL_SUCCEEDED(retcode))
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &m_hstmt); /* Allocate statement handle */
        else
        {
          char        sqlstate[10];
          SQLINTEGER  nativerr;
          char        msg[1024];
          SQLSMALLINT msglength;

          SQLGetDiagRec(SQL_HANDLE_DBC, m_hdbc, 1, (SQLCHAR*)sqlstate, &nativerr, (SQLCHAR*)msg, 1024, &msglength);
        }
      }
    }
  }

  return SQBOOL(retcode);
}
#endif

bool itxSQLConnection::BindCol(int col, char* value, int size)
{
  SQLRETURN retcode;

  retcode = SQLBindCol(m_hstmt,	(unsigned short)(col+1), SQL_C_CHAR, (SQLPOINTER) value, size, &m_ind);

  return SQBOOL(retcode);
}

// statement must be a null-terminated string
bool itxSQLConnection::Execute(char* statement)
{
  SQLRETURN retcode;

  if (m_hstmt == 0)
    return FALSE;

  m_statement = statement;
  m_cols = 0;

	retcode = SQLExecDirect(m_hstmt, (unsigned char *) statement, SQL_NTS);
  if (SQL_SUCCEEDED(retcode)) 
 		retcode = SQLNumResultCols(m_hstmt, &m_cols);
  else
    if (retcode == SQL_NO_DATA)
      retcode = SQL_SUCCESS;

  return SQBOOL(retcode);
}

bool itxSQLConnection::GetColInfo(short int col, char* name, short* type, long int* size)
{
  SQLRETURN retcode;
	short int realColNameLen, decimalDigits;
	unsigned long int colSize;
	SQLSMALLINT nullable;

  retcode = SQLDescribeCol(m_hstmt, (short int)(col + 1), (unsigned char*) name, ITX_QRS_MAX_NAME_LEN, &realColNameLen, type, &colSize, &decimalDigits, &nullable);
  if (SQL_SUCCEEDED(retcode)) 
	{
		colSize = (colSize > ITX_QRS_MAX_FIELD_LEN) ? ITX_QRS_MAX_FIELD_LEN : colSize;
		*size = colSize;
	}

  return SQBOOL(retcode);
}

// start deve essere <= end
bool itxSQLConnection::SkipRecords(int start, int end)
{
  SQLRETURN retcode;
  int counter = start;
  
  retcode = SQLFetch(m_hstmt);

  while (counter < end && SQL_SUCCEEDED(retcode))
  {  
    retcode = SQLFetch(m_hstmt);
    counter++;
  }

  return SQBOOL(retcode);
}

bool itxSQLConnection::Fetch()
{
  SQLRETURN retcode;

  retcode = SQLFetch(m_hstmt);

  return SQBOOL(retcode);
}

bool itxSQLConnection::FreeStmt()
{
  SQLRETURN retcode;

  retcode = SQLFreeStmt(m_hstmt, SQL_CLOSE);

  return SQBOOL(retcode);
}

bool itxSQLConnection::MoreResults()
{
  SQLRETURN retcode;

  retcode = SQLMoreResults(m_hstmt);

  if (SQBOOL(retcode))
    retcode = SQLNumResultCols(m_hstmt, &m_cols);

  return SQBOOL(retcode);
}

bool itxSQLConnection::GetColsNumber()
{
  SQLRETURN retcode;
  retcode = SQLNumResultCols(m_hstmt, &m_cols);
  return SQBOOL(retcode);
}

bool itxSQLConnection::Destroy()
{
  SQLRETURN retcode;

  if (!IsConnected())
  return TRUE;

	retcode = SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);

	if (SQL_SUCCEEDED(retcode)) 
  {
    retcode = SQLDisconnect(m_hdbc);
        
    if (SQL_SUCCEEDED(retcode)) 
    {
      retcode = SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);		
		
    	if (SQL_SUCCEEDED(retcode)) 
    		SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
    }
  }

  m_henv  = 0;
  m_hdbc  = 0;
  m_hstmt = 0;

  return SQBOOL(retcode);
}

void itxSQLConnection::Error(FILE* log)
{
	time_t tm;
	
  if (log == NULL) return;

	time(&tm);

	fprintf(log, "\n------------------------------------------------------------\n");
	fprintf(log, " �AITECSA s.r.l.\n");
  fprintf(log, "SQL INTERFACE MODULE ERROR REPORT\n");
	fprintf(log, "  %s", ctime(&tm));
	fprintf(log, "------------------------------------------------------------\n");
	fprintf(log, "\n");

	fprintf(log, "Last Executed Query:\n");
  fprintf(log, "%s\n", m_statement);
  
  memset(m_sqlstate, '\0', ITX_SQL_STATUS_LEN);
  memset(m_message, '\0', ITX_SQL_MAX_ERRMSG);

  fprintf(log, "\nODBC ERROR REPORT:\n");

  SQLGetDiagRec(SQL_HANDLE_ENV, m_henv, 1, m_sqlstate, &m_nativerr, m_message, 
                ITX_SQL_MAX_ERRMSG - 1, &m_msglength);
  fprintf(log, "   SQL_HANDLE_ENV : %s\n", m_sqlstate);
  fprintf(log, "                    %s\n", m_message);

  SQLGetDiagRec(SQL_HANDLE_DBC, m_hdbc, 1, m_sqlstate, &m_nativerr, m_message, 
                ITX_SQL_MAX_ERRMSG - 1, &m_msglength);
  fprintf(log, "   SQL_HANDLE_DBC : %s\n", m_sqlstate);
  fprintf(log, "                    %s\n", m_message);

  SQLGetDiagRec(SQL_HANDLE_STMT, m_hstmt, 1, m_sqlstate, &m_nativerr, m_message, 
                ITX_SQL_MAX_ERRMSG - 1, &m_msglength);
  fprintf(log, "   SQL_HANDLE_STMT: %s\n", m_sqlstate);
  fprintf(log, "                    %s\n", m_message);

  fprintf(log, "\n\n");
}