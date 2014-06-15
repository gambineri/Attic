/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: tqr.h,v $
 * $Revision: 1.37 $  
 * $Author: fabio $
 * $Date: 2001-10-09 12:23:43+02 $
 *
 * Tannit Query Resultset Object and Manager Definition
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */
 
#ifndef __TQR_H__
#define __TQR_H__


#include <stdarg.h>

#include "itxlib.h"

#define	COL_NAME_LEN			    128
#define	QUERY_NAME_LEN		    128
#define	QUERY_NUMBER			    1000

#define	STARTING_ROW					1
#define	ROWS_TO_STORE					512

#define TQR_NOT_EXIST         -1

class TQRRecord
{
friend class TQRColumnHeader;
friend class TQR;

private:
	itxString* m_row;
	TQRRecord* m_next;
	TQRRecord* m_previous;

public:
	int colsNumber;

  TQRRecord() { m_row = NULL; m_next = NULL; m_previous = NULL; }
  TQRRecord(int colnum);
  TQRRecord(TQRRecord* previous, int colnum, TQRRecord* next);
  ~TQRRecord();

  void operator=(TQRRecord& src);

  void  ExpandCol(int colindex, int size);
  char* FieldValue(int colindex) {return (colindex < 0 ? NULL : m_row[colindex].GetBuffer());}
  void  SetField(int colindex, char* colvalue) { if (colindex >= 0) m_row[colindex] = colvalue; }
  itxString* GetColValue(int colindex) { return &m_row[colindex]; };
};

class TQRColumnHeader
{
friend class TQR;

private:
	itxString name;
	short     sqlDataType;
	long int  colummnSize;

public:
  void SetAttributes(char* colname, short coltype, long colsize);
};

class TQR
{
friend class TQRManager;
friend class TQRCollection;

private:
	itxString id;
	int actualRow;
	int colsNumber;
	int startingRow;
  int rowsToStore;          // user request
  int totalRows;            // total rows in current TQR (may be less than user request)

  bool m_MoreDBRows;        // DB contains more records
  int  m_SourceRecordCount; // DB source Record count

	TQRRecord* current_record;
	TQRRecord* recordshead;
	TQRRecord* recordstail;
	TQRColumnHeader* queryHeader;

public:
  TQR() {};
	TQR(char* queryname);
	TQR(char* queryname, int numfields);
  ~TQR();

  void operator=(TQR* src);

  TQR*       Clone(char* clonename);

  itxString& GetName() { return id; };
  void       SetName(char* name) { id = name; };

  void       SetColumnAttributes(int colindex, char* colname, short coltype, long colsize);
  int        GetColsNum() { return colsNumber; };
  long int   GetColSize(int colindex) { return queryHeader[colindex].colummnSize; };
  int        GetColPos(char* colname);
  short      GetColType(int colindex) { return queryHeader[colindex].sqlDataType; };
  char*      GetColName(int colindex) { return queryHeader[colindex].name.GetBuffer(); };

  bool       AddTail();
  void       RemoveTail();

  int        GetStartingRow() { return startingRow; }
  int        GetRowsToStore() { return rowsToStore; }
  int        GetActualRow() { return actualRow; }
  int        GetActualPage() { return (((startingRow - 1) / rowsToStore) + 1); }
	int        GetTotalRows() { return totalRows; }
  int        GetTotalPages() { return (((m_SourceRecordCount - 1) / rowsToStore) + 1); }
  int        GetPrevPageRow() { return ((startingRow - rowsToStore) > 0 ? (startingRow - rowsToStore) : 1 ); }
  int        GetNextPageRow() { return ((startingRow + rowsToStore) > m_SourceRecordCount ? startingRow : (startingRow + rowsToStore)); }
  int        GetLastPageRow() { return ((GetTotalPages() - 1) * rowsToStore + 1); }

  bool       GetMoreDBRows() { return m_MoreDBRows; }
  int        GetSourceRecordCount() { return m_SourceRecordCount; }
  TQRRecord* GetCurrentRecord() { return current_record; }
  char*      GetCurrentRecordField(int colindex) { return (totalRows ? current_record->FieldValue(colindex) : NULL); }
  char*      GetCurrentRecordField(char* colname) { return (totalRows ? current_record->FieldValue(GetColPos(colname)) : NULL); }
  void       SetCurrentRecordField(char* colname, char* colvalue) { current_record->SetField(GetColPos(colname), colvalue); }
  void       SetCurrentRecordField(int colpos, char* colvalue) { current_record->SetField(colpos, colvalue); }
  void       SetMoreDBRows(bool newval) { m_MoreDBRows = newval; }
  void       SetSourceRecordCount(int newval) { m_SourceRecordCount = newval; }

  void       LoadRecordBuffer(char* record, char fieldsep);

  void MoveFirst() 
  { 
    current_record = recordshead; 
    current_record != NULL ? actualRow = 1 : actualRow = 0; //BOF
  }
  void MoveNext() 
  { 
    current_record = (current_record == NULL ? NULL : current_record->m_next); 
    current_record != NULL ? actualRow++ : actualRow = -1;  //EOF
  }
  void MoveLast() { current_record = recordstail; actualRow = totalRows; }
  void MoveTo(int rowindex);
};

typedef class TQR* PTQR;
typedef void* TQRHANDLE;

typedef class TQRManager* PTQRMng;

class TQRCollection
{
friend TQRManager;

private:
	PTQR	  m_qres[QUERY_NUMBER];
  PTQRMng m_mng[QUERY_NUMBER];
	int 	  m_qcount;

public:
  TQRCollection() { memset(m_qres, 0, QUERY_NUMBER * sizeof(PTQR)); m_qcount = 0; }
  ~TQRCollection() {};

  TQR*  CreateTQR(char* queryname, int numfields);
  int   Store(PTQRMng, PTQR qres);
  void  Remove(char* name); 
  TQR*  Retrieve(char* name);  
  int   Index(char* name);
  int   Exist(char* name);

  char* GetCurrentRecordField(char* name, char* colname);
  void  SetCurrentRecordField(char* name, char* colname, char* colvalue);

  void MoveFirst(PTQR qres) { qres->MoveFirst(); }
  void MoveNext(PTQR qres) { qres->MoveNext(); }
  void MoveLast(PTQR qres) { qres->MoveLast(); }
  void MoveTo(PTQR qres, int rowindex) { qres->MoveTo(rowindex); };
};

class TQRManager
{
private:
	TQRCollection*   m_tqrs;

public:
  TQRManager() {};
  TQRManager(TQRCollection* tqrs) { m_tqrs = tqrs; }
  ~TQRManager() {};

  void SetTQRCollection(TQRCollection* tqrs) { m_tqrs = tqrs; };

  TQR* Get(char* name) { return m_tqrs->Retrieve(name); };
  int  Store(TQR* qres) { return m_tqrs->Store(this, qres); };
  int  Exist(char* name) { return m_tqrs->Exist(name); };
  void Remove(char* name){ m_tqrs->Remove(name); }; 

// TQRManager Interface
  TQR* CreateTQR(char* queryname);
  TQR* CreateTQR(char* queryname, int numfields);
  void SetColumnAttributes(char* queryname, int colindex, char* colname, short coltype, long colsize);
  void SetName(TQR* qres, char* name);
  void LoadDataBuffer(char* tqrname, int tqrcols, char recsep, char fieldsep, char* buffer);
  TQR* Filter(char* source, char* field, char* value, char* destination);
  TQR* Sample(char* source, char* destination, int destMaxRecs, int seed);
  int  GetRecordNumber(char* tqrname) { return Get(tqrname)->totalRows; };

  void MoveFirst(TQR* qres) { qres->MoveFirst(); }
  void MoveNext(TQR* qres) { qres->MoveNext(); }
  void MoveLast(TQR* qres) { qres->MoveLast(); }
  void MoveTo(TQR* qres, int rowindex ) { qres->MoveTo(rowindex); }

  TQRRecord* GetCurrentRecord(PTQR qres) { return qres->GetCurrentRecord(); }

  bool AddTail(TQR* qres) { return qres->AddTail(); };

  char* GetCurrentRecordField(TQR* qres, char* colname) { return qres->GetCurrentRecordField(colname); }
  void  SetCurrentRecordField(TQR* qres, char* colname, char* value) { qres->SetCurrentRecordField(colname, value); }

// Internals
  void SetColumnAttributes(TQR* qres, int colindex, char* colname, short coltype, long colsize);
  void SetRowsParam(TQR* qres, int startingrow, int rowstostore, int actualrow, int totalrows)
  {
    qres->startingRow = startingrow;
    qres->rowsToStore = rowstostore;
    qres->actualRow = actualrow;
    qres->totalRows = totalrows;
  }
};

#endif

