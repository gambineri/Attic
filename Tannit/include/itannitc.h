/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#ifndef __ITANNIT_H__
#define __ITANNIT_H__

#include <stdio.h>

#include "tannitds.h"

#ifdef _ITX_APPC
#pragma message("ITannit.lib AS400 support enabled")
#else
#pragma message("ITannit.lib AS400 support disabled")
#endif

#include "itannitdef.h"

extern "C"
{
  // General functions
	bool ITannit_Create(PTannitQueryResult* qres, int* count);
	void ITannit_Destroy();

  int  ITannit_StoreToken(TannitQueryResult* qres, char* token, int field_num);
  int  ITannit_StoreValue(PTannitQueryResult qres, char* field, char* value);
	TannitQueryResult* ITannit_DBStructure(char* token);
  TannitQueryResult* ITannit_NewEntry(char* token, int numfields);
  TannitQueryResult* ITannit_FindQuery(char* token);
  TannitQueryResult* ITannit_NewTQRFrom(char* source, char* field, char* value, char* destination);

  int  ITannit_StoreRecord(char* name, ...);

  char* ITannit_GetValue(TannitQueryResult* qres, int row, char* field);
  void  ITannit_SetCurrentRecord(TannitQueryResult* qres, int row);

	void ITannit_RemoveCurrentRec(TannitQueryResult* qres);
	bool ITannit_CheckCurrentRec(int numfield);
  void ITannit_RemoveExcluding(char* firstname, ...);
  void ITannit_RemoveQueries(char* firstname, ...);

	bool ITannit_Dump(FILE* fp);
  
  bool ITannit_RegisterTP(char* filename);
  TannitQueryResult* ITannit_FindRegTP(int tpid);
  int ITannit_GetTPFields(char* tpname);

  // APPC INTERFACE FUNCTIONS BEGIN
  bool  ITannit_CreateEnv(PTannitQueryResult* qres, int* count, char* llu, char* rtp);

  char* ITannit_TPReceiveFromFile(FILE* fp, PACKET_TYPE msgwaited);
  char* ITannit_TPExecute(PACKET_TYPE msgwaited, int maxretray, FILE* log, ...);
  char* ITannit_TPExecuteCnv(PACKET_TYPE msgwaited, int maxretray, FILE* log, ...);
  char* ITannit_TPExecuteFromFile(FILE* fp, FILE* log, PACKET_TYPE msgwaited, int maxretray);

  // APPC Parsing functions
  void ITannit_ParsTP(char* tablename, char* tp, int fromrecord, int numrecord, FILE* fp,
                      char recsep, char fieldsep);
	int  ITannit_ParsRecord(char* tablename, char* record, FILE* fp, char fieldsep);
	void ITannit_ParserLog(FILE* fp, char* des, char* record);
  void ITannit_EBCDtoASCII(PTannitQueryResult qres);

  // functions that depend by the particular project for which itannit
  // has benn developed and should be positioned in other library
  bool ITannit_GetP00FromSQL(char* dsn, char* uid, char* pwd, char* connectionID, FILE* log);
  bool ITannit_FirstConn(char* dsn, char* uid, char* pwd,
                         char* user, char* lang, FILE* log);
  bool ITannit_ReceiveFileFromAS400(char* filepath, FILE* log);
  bool ITannit_SendFileToAS400(char* filepath, FILE* log);

  bool ITannit_SendHOK(FILE* log, PACKET_TYPE ptype);

  void ITannit_SetUser(char* user);
  // APPC INTERFACE FUNCTIONS END


  // ODBC INTERFACE FUNCTIONS BEGIN
  char*     ITannit_AllocateSQLInsertStringWith(char* target, char* table, int row, va_list morefields);
  char*     ITannit_AllocateSQLUpdateStringWith(char* table, int row, ...);
  bool      ITannit_ConnectSQL(char* dsn, char* uid, char* pwd);
  bool      ITannit_DisconnectSQL();
  void      ITannit_DeleteSQL(char* target, ...);
  bool      ITannit_ExecuteSQL(char* stm);
  bool      ITannit_InsertSQL(char* target, char* table, int row, ...);
  bool      ITannit_BulkInsertSQL(FILE* log, char* target, char* table, ...);
//  bool      ITannit_SelectFromSQL(char* dsn, char* uid, char* pwd, char* queryname, char* query, int firstRec, int recsToStore);
  bool      ITannit_SelectDirectFromSQL(char* dsn, char* uid, char* pwd, char* queryname, char* query, int firstRec, int recsToStore);
  int       ITannit_ExecuteSQLQuery(char* dsn, char* uid, char* pwd, char* queryname, char* query, int firstRec, int recsToStore);
  short int ITannit_GetSQLCols();
  bool      ITannit_GetSQLColInfo(short int col, char* name, short* type, long int* size);
  bool      ITannit_FillTQRColsInfoFromSQL(char* dsn, char* uid, char* pwd, char* tqrname, char* table);
  int       ITannit_ErrorSQL(FILE* log);
  bool      ITannit_ManualCommitSQL();
  bool      ITannit_AutoCommitSQL();
  bool      ITannit_CommitSQL();
  bool      ITannit_RollbackSQL();
  // ODBC INTERFACE FUNCTIONS END


  // functions that should not be here but in an string
  // manipolation utility library
  void  ITannit_SubStr(char* source, char* destination, int maxlength, char* tag, char* newtag);
  void  itxOemToChar(char* source, char* destination);
  void  itxCharToOem(char* source, char* destination);
  void  itxEBCDToASCII(char* source);
  char* ITannit_Trim(char* source);
  char* ITannit_RTrim(char* source);
}

#endif //__ITANNIT_H__
