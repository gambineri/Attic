/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

// AITECSA s.r.l.   1999
//    filename:     itannit.h
//    description:  tannit data structure AS400/SQL interface library
//                  internal interface object ITannit definition
//    project:      ASWEB

#ifndef __ITX_ITANNIT_H__
#define __ITX_ITANNIT_H__

#include <stdio.h>
#include <stdarg.h>

#include "tntsql.h"
#include "itxtypes.h"
#include "iregtp.h"
#include "tannitds.h"

class ITannit
{
private:
	PTannitQueryResult*	m_qres;
	int*								m_qcount;
	TannitQueryResult*  m_current;
  char                m_user[7];

public:
	ITannit() { m_qres = NULL; m_qcount = NULL; m_current = NULL; 
              memset(m_user, '\0', 7);};
	ITannit(PTannitQueryResult* qres, int* count)
	{
		m_qres = qres;
		m_qcount = count;
		m_current = NULL;
		if (m_qres != NULL)
			m_current = qres[0];
    memset(m_user, '\0', 7);
	}

	TannitQueryResult* FindEntry(char* name);
  int Store(TannitQueryResult* qres);

  TannitQueryResult* AllocateNewEntry(itxTP* tpset, char* name);
  TannitQueryResult* AllocateNewEntry(char* queryname, int numfields, ...);
  TannitQueryResult* AllocateNewEntry(itxSQLConnection* psqlconn, char* queryname, char* query, int firstRec, int recsToStore);
  TannitQueryResult* Duplicate(TannitQueryResult* qsrc, char* destination);

  bool							 AllocateNewRecord(TannitQueryResult* qres);
	int								 StoreField(TannitQueryResult* qres, char* token, int position);
  int                AllocateFieldSpace(TannitQueryResult* qres, int space, int position);
  bool               CopyRecord(TannitQueryResult* qsrc, TannitRecord* source, TannitQueryResult* qres);

	void  DoEmpty();
	void  RemoveEntry(TannitQueryResult* qres);
	void  RemoveCurrentRec(TannitQueryResult* qres);
  bool  CheckCurrentRec(int numfield);						 
  char* GetValue(char* name, int row, int field);
  void  SetValue(char* name, int row, int field, char* entry);
  int   GetColPos(TannitQueryResult* qres, char* fieldname);
  
  TannitRecord* GetRecordNumber(char* table, int row);
  TannitRecord* GetRecordNumber(TannitQueryResult* qres, int row);

	bool Serialize(char* name);
	bool SerializeRecord(char* record, int pos);
	bool Dump(FILE* fp);
	bool DumpTable(FILE* fp, char* name);

  int  BuildDataToSend(char* packet, va_list tables);
  bool BuidHeaderToSend(char* packet, int progr, PACKET_TYPE ptype);
  bool BuidCorrHeaderToSend(char* packet, int progr, PACKET_TYPE ptype);
  bool BuildPacketToSend(char* packet, int progr, PACKET_TYPE ptype, va_list tables);
  bool BuildSinglePacketToSend(char* packet, PACKET_TYPE ptype, char* name);
  int  BuildSingleData(char* packet, char* tablename);

  // SQL SERVER Interface

  int ITannit::GetIndex(char* name);
	TannitQueryResult* Get() { return m_current; };
	TannitQueryResult* Get(int i)
  {
    return m_qres[i];
  }

	int GetCount() { return *m_qcount; };
	void Init(PTannitQueryResult* qres, int* count)
	{
		m_qres = qres;
		m_qcount = count;
		m_current = NULL;
		if (m_qres != NULL)
			m_current = qres[0];
    memset(m_user, '\0', 7);
	}

  void SetUser(char* user)
  { strcpy(m_user, user); }

	void Set(TannitQueryResult* qres)
	{
		m_current = qres;
	}

  bool SkipSQLRecords(itxSQLConnection* psqlconn, int start, int end);
  bool SelectFromSQL(itxSQLConnection* psqlconn, char* queryname, char* query, int firstRec, int recsToStore);
  bool ExecuteSQL(itxSQLConnection* psqlconn, char* stm);
  int  ExecuteQuery(itxSQLConnection* psqlconn, char* queryname, char* query, int firstRec, int recsToStore);
  short int GetSQLCols(itxSQLConnection* psqlconn);
  bool GetSQLColInfo(itxSQLConnection* psqlconn, short int col, char* name, short* type, long int* size);
  bool MoreResultSQL(itxSQLConnection* psqlconn);
};

#endif //__ITX_ITANNIT_H__
