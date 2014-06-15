/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#ifndef __TNTSQL_H__
#define __TNTSQL_H__

#include <stdio.h>

#ifndef WIN32
typedef void* HWND;
#else
#include <windows.h>
#endif

#include <sqltypes.h>

// SQL INTERFACE CONSTANTS
#define  ITX_QRS_MAX_NAME_LEN     63
#define  ITX_QRS_MAX_FIELD_LEN    8191
#define  ITX_QRS_MAX_QUERY_LEN    32767
//

#define ITX_SQL_DSN_LENGTH      32
#define ITX_SQL_UID_LENGTH      16
#define ITX_SQL_PWD_LENGTH      16
#define ITX_SQL_MAX_ERRMSG      512
#define ITX_SQL_STATUS_LEN      6

class itxSQLConnection
{
public:
  HENV		  m_henv;
  HDBC	  	m_hdbc;
  SQLHANDLE m_hstmt;
  char      m_dsn[ITX_SQL_DSN_LENGTH];
  char      m_uid[ITX_SQL_UID_LENGTH];
  char*     m_statement;

  SQLINTEGER m_ind;

  short int m_cols;
  short int m_rows;

private:  
  char      m_pwd[ITX_SQL_PWD_LENGTH];

public:
  // Error parameters
  SQLCHAR     m_sqlstate[ITX_SQL_STATUS_LEN];
  SQLINTEGER  m_nativerr;
  SQLCHAR     m_message[ITX_SQL_MAX_ERRMSG];
  SQLSMALLINT m_msglength;

public:
  itxSQLConnection(); 
  ~itxSQLConnection() { };

  bool IsConnected() { return m_henv == NULL ? FALSE : TRUE; };  
  bool Create(char* dsn, char* uid, char* pwd);
  bool SetAttributes(int attr, void* value, int valuelen);
  bool Destroy();
  bool Execute(char* statement);
  bool GetColInfo(short int col, char* name, short* type, long int* size);
  bool SkipRecords(int start, int end);
  bool BindCol(int col, char* value, int size);
  bool Fetch();
  bool FreeStmt();
  bool MoreResults();
  void Error(FILE* log);
  bool GetColsNumber();
  bool ManualCommit();
  bool AutoCommit();
  bool Commit();
  bool Rollback();
};

#endif //__TNTSQL_H__
