/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxsql.h,v $
 * $Revision: 1.4 $  
 * $Author: massimo $
 * $Date: 2002-06-24 13:38:58+02 $
 *
 * Definition of the itxSQLConnection object (ODBC connection wrapper)
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */
 
#ifndef __TNTSQL_H__
#define __TNTSQL_H__


#include "itxsystem.h"

#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>

#include "itxbuffer.h"
#include "itxlib.h"

// SQL INTERFACE CONSTANTS
#define  ITX_QRS_MAX_NAME_LEN     63
#define  ITX_QRS_MAX_FIELD_LEN    32767
#define  ITX_QRS_MAX_QUERY_LEN    32767
//

#define ITX_SQL_DSN_LENGTH      32
#define ITX_SQL_UID_LENGTH      16
#define ITX_SQL_PWD_LENGTH      16
#define ITX_SQL_MAX_ERRMSG      512
#define ITX_SQL_STATUS_LEN      6

#define ITX_SQL_MAX_CONN        10
#define ITX_SQL_NAME_CONN_LEN   32

class itxSQLConnection
{
public:
  itxString m_name;

  itxString m_dsn;
  itxString m_usr;
  itxString m_pwd;

  HENV		  m_henv;
  HDBC	  	m_hdbc;
  SQLHANDLE m_hstmt;

  char m_statement[ITX_QRS_MAX_QUERY_LEN];
  
  SQLINTEGER m_ind;

  short int m_cols;
  short int m_rows;

public:
  // Error parameters
  SQLCHAR     m_sqlstate[ITX_SQL_STATUS_LEN];
  SQLINTEGER  m_nativerr;
  SQLCHAR     m_message[ITX_SQL_MAX_ERRMSG];
  SQLSMALLINT m_msglength;

public:
  itxSQLConnection(); 
  ~itxSQLConnection() { Destroy(); };

  bool IsConnected() { return (m_henv ? true : false ); };  
  bool Create(char* dsn, char* uid, char* pwd);
  bool Create(char* name, char* dsn, char* uid, char* pwd);
  bool Create();
  bool SetAttributes(int attr, void* value, int valuelen);
  bool Destroy();
  bool Execute(char* statement);
  bool Prepare(char* statement);
  bool GetColInfo(short int col, char* name, short* type, long int* size);
  bool SkipRecords(int start, int end);
  bool BindCol(int col, char* value, int size, SQLSMALLINT target_type = SQL_C_CHAR);
  bool BindCol(int col, float* value, int size);

  bool Fetch();
  bool GetBinData(int col, itxBuffer& outbuf);
  bool FreeStmt(unsigned short flag);
  bool MoreResults();
  void Trace();
  bool GetColsNumber();
  bool ManualCommit();
  bool AutoCommit();
  bool Commit();
  bool Rollback();
};

class itxSQLConnCollection
{
private:
  itxSQLConnection* m_connections[ITX_SQL_MAX_CONN];

public:
  itxSQLConnCollection();
  ~itxSQLConnCollection() {};

  itxSQLConnection* Get(char* name);
  void              Put(itxSQLConnection* sqlconn);

  bool              CloseAll();
};

#endif //__TNTSQL_H__
