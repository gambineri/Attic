/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: tqrodbc.h,v $
 * $Revision: 1.16 $  
 * $Author: massimo $
 * $Date: 2002-05-28 12:55:01+02 $
 *
 * Management of TQRs from ODBC source 
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               aitecsa s.r.l. via baglivi 3 00161 roma italy
 *               aitecsa@aitecsa.it
 */
 
#ifndef __TQRODBC_H__
#define __TQRODBC_H__

#include "itxlib.h"

#include "tqr.h"

#define MAX_QUERYHEADER_NUM       256

class TQRODBCManager : public TQRManager
{
  itxSQLConnection*  m_iconn;
  bool               m_connected;

public:
  TQRODBCManager() { m_connected = false; m_iconn = NULL;};
  TQRODBCManager(TQRCollection* tqrs) : TQRManager(tqrs) { m_connected = false; m_iconn = NULL;} 
  ~TQRODBCManager() {};

// TQRODBCManager Interface
  void Connect(char* dsn, char* uid, char* pwd);
  void SetConnect(itxSQLConnection*  conn);
  void Disconnect();
  int  Execute(char* tqrname, char* query, int firstRec = STARTING_ROW, int recsToStore = ROWS_TO_STORE);
  void Inquiry(char* tqrname, char* query);
  bool BulkInsert(char* target, char* tqrname, ...);
  bool PrepareSQLInsertStringWith(itxString* query, char* target, TQR* qres, TQRRecord* record, va_list morefields);
  void SetTQRColumnsAttributesFromTable(TQR* qres, char* tablename);
  
  itxSQLConnection* GetConnection() { return m_iconn; };

// Internals
  void SetTQRColumnsAttributes(TQRHANDLE qres);

  bool IsNumeric(TQR* qres, char* colname);
  bool IsReal(TQR* qres, int colindex);
  bool IsAlpha(TQR* qres, char* colname);
  bool IsDate(TQR* qres, char* colname);
};

#endif