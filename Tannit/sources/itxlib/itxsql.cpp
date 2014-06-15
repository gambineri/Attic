/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxsql.cpp,v $
 * $Revision: 1.8 $  
 * $Author: administrator $
 * $Date: 2002-07-23 17:44:45+02 $
 *
 * Implementation of the itxSQLConnection object (ODBC connection wrapper)
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */
 
#ifndef __ITX_TNTSQL_CPP__
#define __ITX_TNTSQL_CPP__
#endif

#include <time.h>
#include <memory.h>

#include "itxsql.h" 
#include "itxsystem.h" 


#define SQBOOL(a)             (SQL_SUCCEEDED(a)? true : false)

itxSQLConnection::itxSQLConnection()
{
  m_henv  = 0;
  m_hdbc  = 0;
  m_hstmt = 0;

  m_cols = 0;
  m_rows = 0;
}

bool itxSQLConnection::Create(char* dsn, char* uid, char* pwd)
{  
  if (IsConnected())
    return true;

  SQLRETURN retcode;

  m_dsn = dsn;
  m_usr = uid;
  m_pwd = pwd;

#ifdef USE_ODBC_10
	retcode = SQLAllocEnv(&m_henv);	/*Allocate environment handle */

	if (SQL_SUCCEEDED(retcode)) 
  {
	  retcode = SQLAllocConnect(m_henv, &m_hdbc);

	  if (SQL_SUCCEEDED(retcode)) 
    {
				retcode = SQLConnect(m_hdbc, (SQLCHAR*) dsn, SQL_NTS,	/* Connect to data source */
														(SQLCHAR*) uid, SQL_NTS,(SQLCHAR*) pwd, SQL_NTS);
			
      	if (SQL_SUCCEEDED(retcode))
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &m_hstmt); /* Allocate statement handle */
    }
  }
#else //ODBC 3.0
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
#endif

  return SQBOOL(retcode);
}

bool itxSQLConnection::Create(char* name, char* dsn, char* uid, char* pwd)
{  
  m_name = name;
  return Create(dsn, uid, pwd);
}

bool itxSQLConnection::Create()
{
  return Create(m_dsn.GetBuffer(), m_usr.GetBuffer(), m_pwd.GetBuffer());
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

#ifdef USE_ODBC_10
  DebugTrace2(DEFAULT, "itxSQLConnection::Create (file DSN version) not yet implemented on this platform.\n");  
#else //ODBC 3.0
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
#endif

  return SQBOOL(retcode);
}
#endif

bool itxSQLConnection::BindCol(int col, char* value, int size, SQLSMALLINT target_type)
{
  SQLRETURN retcode;

  retcode = SQLBindCol(m_hstmt,	(unsigned short)(col+1), target_type, (SQLPOINTER) value, size, &m_ind);

  return SQBOOL(retcode);
}

bool itxSQLConnection::BindCol(int col, float* value, int size)
{
  SQLRETURN retcode;

  retcode = SQLBindCol(m_hstmt,	(unsigned short)(col+1), SQL_C_FLOAT, (SQLPOINTER) value, size, &m_ind);

  return SQBOOL(retcode);
}

// statement must be a null-terminated string
bool itxSQLConnection::Execute(char* statement)
{
  SQLRETURN retcode;

  if (m_hstmt == 0)
    return false;

  memset(m_statement, '\0', ITX_QRS_MAX_QUERY_LEN);
  memcpy(m_statement, statement, strlen(statement));
  m_cols = 0;

	retcode = SQLExecDirect(m_hstmt, (unsigned char *) statement, SQL_NTS);
  if (SQL_SUCCEEDED(retcode)) 
 		retcode = SQLNumResultCols(m_hstmt, &m_cols);
  else
    if (retcode == SQL_NO_DATA)
      retcode = SQL_SUCCESS;

  return SQBOOL(retcode);
}

// statement must be a null-terminated string
bool itxSQLConnection::Prepare(char* statement)
{
  SQLRETURN retcode;

  if (m_hstmt == 0)
    return false;

  memset(m_statement, '\0', ITX_QRS_MAX_QUERY_LEN);
  memcpy(m_statement, statement, strlen(statement));
  m_cols = 0;

	retcode = SQLPrepare(m_hstmt, (unsigned char *) statement, SQL_NTS);
  if (SQL_SUCCEEDED(retcode)) 
 		retcode = SQLNumResultCols(m_hstmt, &m_cols);

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
  
  if (start > end) 
    return true;
  
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

bool itxSQLConnection::FreeStmt(unsigned short flag)
{
  SQLRETURN retcode;

  retcode = SQLFreeStmt(m_hstmt, flag);

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
    return true;

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

// GetBinData assumes the record cursor is at the desired position
bool itxSQLConnection::GetBinData(int col, itxBuffer& outbuf)
{
  SQLRETURN   retcode;
  SQLINTEGER  appo;
  SQLINTEGER  datalen = 0;
  char        aux[1];

  retcode = SQLGetData(m_hstmt, col, SQL_C_BINARY, aux, 0, &datalen);
  
  if (SQL_SUCCEEDED(retcode)) 
  {
    outbuf.Space(datalen + 10); //paranoic overdim

    //ora mi dovrebbe tornare 0 (SQL_SUCCESS): vuol dire che ho finito
    char* p = outbuf.GetBuffer();
    retcode = SQLGetData(m_hstmt, col, SQL_C_BINARY, p, datalen, &appo);

    outbuf.UpdateCursor(datalen);
  }

  return SQBOOL(retcode);
}

//------------------------------------------------------------
// itxSQLConnCollection Implementation

itxSQLConnCollection::itxSQLConnCollection()
{
  for (int i = 0; i < ITX_SQL_MAX_CONN; i++)
    m_connections[i] = NULL;
}

itxSQLConnection* itxSQLConnCollection::Get(char* name)
{
  itxSQLConnection* conn = NULL;

  for (int i = 0; i < ITX_SQL_MAX_CONN; i++)
  {
    if ((m_connections[i]->m_name).Compare(name) == 0)
    {
      conn = m_connections[i];
	    break;
    }
  }

  return conn;
}

void itxSQLConnCollection::Put(itxSQLConnection* conn)
{
  for (int i = 0; i < ITX_SQL_MAX_CONN; i++)
  {
    if (m_connections[i] == 0)
    {
      m_connections[i] = conn;
	    break;
    }
  }
}

bool itxSQLConnCollection::CloseAll()
{
  int i = 0;
  bool result = true;

  while (m_connections[i] != 0)
  {
    result &= m_connections[i]->Destroy();
    i++;
  }

  return result;
}
