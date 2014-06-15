/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: tqrodbc.cpp,v $
 * $Revision: 1.32 $  
 * $Author: administrator $
 * $Date: 2002-07-23 17:45:20+02 $
 *
 * Management of TQRs from ODBC source 
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               aitecsa s.r.l. via baglivi 3 00161 roma italy
 *               aitecsa@aitecsa.it
 */
 
#ifndef __TQRODBC_CPP__
#define __TQRODBC_CPP__
#endif

#include "auxfile.h"
#include "tqrodbc.h"
#include "itxtypes.h"


// si connette al dsn specificato
// da utilizzare:
//   a livello command solo per le connessioni temporanee 
//   a livello strutturale per le connessioni persistenti
void TQRODBCManager::Connect(char* dsn, char* uid, char* pwd)
{ 
  try
  {
    if (!m_connected)
      m_connected = m_iconn->Create(dsn, uid, pwd);
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRODBCManager::Connect\n");
    throw;
  }
}

// utilizza una connessione attiva
void TQRODBCManager::SetConnect(itxSQLConnection* conn)
{ 
  m_iconn = conn;
  m_connected = true;
  if (m_iconn == NULL)
    m_connected = false;
}

// da utilizzare:
//   a livello command solo per le connessioni temporanee 
//   a livello strutturale per le connessioni persistenti
void TQRODBCManager::Disconnect()
{ 
  try
  {
    if (m_connected)
      m_iconn->Destroy();
    m_connected = false;
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRODBCManager::Disconnect\n");
    throw;
  }
}


// chiama l'inquiry per gli attributi dei column header.
void TQRODBCManager::SetTQRColumnsAttributesFromTable(TQR* qres, char* tablename)
{
  itxString query;

  query = "SELECT * FROM ";
  query += tablename;
  
  try
  {
    if (m_iconn->Prepare(query.GetBuffer()))
    {
      this->SetTQRColumnsAttributes((void*) qres);
    }
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRODBCManager::SetTQRColumnsAttributes\n");
    SQLTrace(m_iconn);
  }
}

// chiama l'inquiry per gli attributi dei column header.
// deve essere chiamata dopo che sia stata inviata una Execute su ODBC
void TQRODBCManager::SetTQRColumnsAttributes(TQRHANDLE hqres)
{
  TQR* qres = (TQR*) hqres;
  short int actualCol; 
  
  char colname[COL_NAME_LEN];
  short coltype;
  long colsize;

  try
  {
	  for (actualCol = 0; actualCol < qres->GetColsNum(); actualCol++)
    {
      memset(colname, '\0', COL_NAME_LEN);
      m_iconn->GetColInfo(actualCol, &(colname[0]), &(coltype), &(colsize));
      this->SetColumnAttributes(qres, actualCol, colname, coltype, colsize + 1);
    }
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRODBCManager::SetTQRColumnsAttributes\n");
    SQLTrace(m_iconn);
  }
}

// esegue la query e ritorna il numero di TQR creati ovvero ITXFAILED in caso di insuccesso.
// in caso di pi� TQR di ritorno questi vengono chiamati:
//                  primo tqr : tqrname,
//                secondo tqr : tqrname1,
//                  terzo tqr : tqrname2
//                        etc..
int TQRODBCManager::Execute(char* tqrname, char* query, int firstRec, int recsToStore)
{
	TQR* qres = NULL;
  bool moreresult = true;
  int  resultsetnum = 0;
	long int counter;
  int recordsNumber = 0;
  bool result;
  char tqr_name[QUERY_NAME_LEN];
  short int actualCol;
  int  furtherows = 1;
  float jolly[MAX_QUERYHEADER_NUM];
  bool isreal[MAX_QUERYHEADER_NUM];
  bool somereal = false;

  memset(jolly, 0, MAX_QUERYHEADER_NUM * sizeof(float));

  try
  {
    if (m_iconn->Execute(query))
    {
      while (moreresult)
      {
        if (m_iconn->m_cols != 0)
        {
          if (resultsetnum == 0)
            sprintf(tqr_name, "%s", tqrname);
          else
            sprintf(tqr_name, "%s%d", tqrname, resultsetnum);

          if ((qres = Get(tqr_name)) != NULL)
            Remove(tqr_name);

	        qres = CreateTQR(tqr_name, m_iconn->m_cols);
          resultsetnum++;

          if (firstRec < STARTING_ROW) firstRec = STARTING_ROW;
          if (recsToStore < 1)	recsToStore = ROWS_TO_STORE;

          SetRowsParam(qres, firstRec, recsToStore, STARTING_ROW, 0);
          SetTQRColumnsAttributes((void*)qres);

          if (m_iconn->SkipRecords(1, qres->GetStartingRow() - 1))
          {
            recordsNumber = firstRec + recsToStore - 1; 
            for (counter = firstRec; counter <= recordsNumber; counter++)
            {
              if (qres->AddTail())
              {
                for (actualCol = 0; actualCol < qres->GetColsNum(); actualCol++)
                {
                  (qres->GetCurrentRecord())->ExpandCol(actualCol, qres->GetColSize(actualCol));
                  
                  bool bindresult = false;
                  if (isreal[actualCol] = IsReal(qres, actualCol))
                  {
                    somereal = true;
                    bindresult = m_iconn->BindCol(actualCol, &jolly[actualCol], qres->GetColSize(actualCol));  
                  }
                  else
                    bindresult = m_iconn->BindCol(actualCol, qres->GetCurrentRecordField(actualCol), qres->GetColSize(actualCol));
                  if (!bindresult)  
                  {
                    DebugTrace2(IN_ERROR, "itxSQLConnection::BindCol\n");
                    SQLTrace(m_iconn);
                    break;
                  }
                }
                result = m_iconn->Fetch();

                if (!result) 
                {
                  // should be an SQL_NO_DATA
                  SQLTrace(m_iconn);
                  qres->RemoveTail();
                  break;
                }
                else if (somereal)
                {
                  for (actualCol = 0; actualCol < qres->GetColsNum(); actualCol++)
                  {
                    TQRRecord* currentrecord = (qres->GetCurrentRecord());
                    if (isreal[actualCol])
                    {
                      itxString* Value = currentrecord->GetColValue(actualCol);
                      Value->CVFromDouble(jolly[actualCol]);
                    }
                  }
                }
              }
              else
              {
                resultsetnum = ITXFAILED;
                break;
              }
            }

            // unbind tqr record
            m_iconn->FreeStmt(SQL_UNBIND);

            // Set cursor to the beginning of tqr and check if
            // the statement contained other rows. Eventually counts these rows.
            MoveFirst(qres);
            qres->SetMoreDBRows(m_iconn->Fetch());
            if (qres->GetMoreDBRows())
            {
              while (m_iconn->Fetch())
                furtherows++;
            }
            else
              furtherows--;
            qres->SetSourceRecordCount(furtherows + qres->GetStartingRow() - 1 +  qres->GetTotalRows());
          }
          else
            resultsetnum = ITXFAILED;
	      } // if (m_iconn->m_cols != 0)

        moreresult = m_iconn->MoreResults();
      } // while (moreresult)
    }
    else
    {
      SQLTrace(m_iconn);
      return ITXFAILED;
    }
  }
  catch(...)
  {
    if (m_iconn == NULL)
      DebugTrace2(IN_WARNING, "Cannot find a valid ODBC connection.\n");
    else
    {
      DebugTrace2(IN_ERROR, "TQRODBCManager::Execute\n");
      SQLTrace(m_iconn);
      throw;
    }
  }
  return resultsetnum;
}


bool TQRODBCManager::IsAlpha(TQR* qres, char* colname)
{
  int type;
  
  try
  {
    type = qres->GetColType(qres->GetColPos(colname));
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRODBCManager::IsAlpha\n");
    throw;
  }

  if (
       (type == SQL_CHAR) | (type == SQL_VARCHAR) | (type == SQL_LONGVARCHAR) 
#ifndef USE_ODBC_10
       | (type == SQL_WCHAR) | (type == SQL_WVARCHAR) | (type == SQL_WLONGVARCHAR)
#endif
     )
    return true;
  return false;
}

bool TQRODBCManager::IsNumeric(TQR* qres, char* colname)
{
  int type;
  
  try
  {
    type = qres->GetColType(qres->GetColPos(colname));
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRODBCManager::IsNumeric\n");
    throw;
  }

  if ((type == SQL_DECIMAL) | (type == SQL_NUMERIC) |
      (type == SQL_SMALLINT) | (type == SQL_INTEGER) |
      (type == SQL_DOUBLE) | (type == SQL_BIT) |
      (type == SQL_TINYINT) | (type == SQL_BIGINT) |
      (type == SQL_BINARY) | (type == SQL_VARBINARY) | (type == SQL_LONGVARBINARY) |
      (type == SQL_REAL) | (type == SQL_FLOAT))
    return true;
  return false;
}

bool TQRODBCManager::IsReal(TQR* qres, int colindex)
{
  int type;
  
  try
  {
    type = qres->GetColType(colindex);
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRODBCManager::IsReal\n");
    throw;
  }

  if ((type == SQL_DOUBLE) | (type == SQL_REAL) | (type == SQL_FLOAT))
    return true;
  return false;
}

bool TQRODBCManager::IsDate(TQR* qres, char* colname)
{
  int type;
  
  try
  {
    type = qres->GetColType(qres->GetColPos(colname));
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRODBCManager::IsDate\n");
    throw;
  }

  if ((type == SQL_TYPE_DATE) | (type == SQL_TYPE_TIME) |
      (type == SQL_TYPE_TIMESTAMP) | (type == SQL_INTERVAL_MONTH) |
      (type == SQL_INTERVAL_YEAR) | (type == SQL_INTERVAL_YEAR_TO_MONTH) |
      (type == SQL_INTERVAL_DAY) | (type == SQL_INTERVAL_HOUR) |
      (type == SQL_INTERVAL_MINUTE) | (type == SQL_INTERVAL_SECOND) | 
      (type == SQL_INTERVAL_DAY_TO_HOUR) |
      (type == SQL_INTERVAL_DAY_TO_MINUTE) | (type == SQL_INTERVAL_DAY_TO_SECOND) |
      (type == SQL_INTERVAL_HOUR_TO_MINUTE) |
      (type == SQL_INTERVAL_MINUTE_TO_SECOND) | (type == SQL_INTERVAL_HOUR_TO_SECOND))
    return true;
  return false;
}

void TQRODBCManager::Inquiry(char* tqrname, char* query)
{
	TQR* qres = NULL;
  char tqr_name[QUERY_NAME_LEN];

  if (strlen(tqrname) <= QUERY_NAME_LEN)
    strcpy(tqr_name, tqrname);
  else
  {
    DebugTrace2(IN_ERROR, "TQRODBCManager::Inquiry received a tqrname parameter value longer than QUERY_NAME_LEN.\n");
    return;
  }

  try
  {
    if (m_iconn->Prepare(query))
    {
	    qres = this->CreateTQR(tqr_name, m_iconn->m_cols);
      this->SetTQRColumnsAttributes((void*) qres);
      m_iconn->FreeStmt(SQL_CLOSE);
    }
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "TQRODBCManager::Inquiry\n");
    SQLTrace(m_iconn);
    throw;
  }
}

// just Insert. it's up to the user previously delete the record from Database.
bool TQRODBCManager::BulkInsert(char* target, char* tqrname, ...)
{
  va_list morefields;
  TQR* qres = NULL;
  itxString query;

  va_start(morefields, tqrname);   

  if ((qres = Get(tqrname)) == NULL)
    return FALSE;

  SetTQRColumnsAttributesFromTable(qres, target);

  qres->MoveFirst();
  TQRRecord* record;
  while ((record = qres->GetCurrentRecord()) != NULL)
  {
    PrepareSQLInsertStringWith(&query, target, qres, record, morefields);
    Execute(NULL, query.GetBuffer(), 0, 0);
    qres->MoveNext();
  }

  return TRUE;
}

bool TQRODBCManager::PrepareSQLInsertStringWith(itxString* query, char* target, TQR* qres, TQRRecord* record, va_list morefields)
{
  itxString buffer;
  itxString fields;
  itxString values;
  itxString qvalz;
  
  int i;
  char* fieldname;
  itxString* fieldvalue;
  char*      mfieldvalue;
  int   type;

  if (record != NULL)
  {
    buffer = "";
    fields = "";
    values = "";

    for(i = 0; i < record->colsNumber; i++)
    {
      type = qres->GetColType(i);

      fieldvalue = record->GetColValue(i);
      if (!fieldvalue->IsEmpty())
      {    
        fields += qres->GetColName(i);
        fields += ",";
          
        switch (type)
        {
          case 0: 
          case SQL_CHAR:
          case SQL_VARCHAR:
          case SQL_NULL_DATA:
          case SQL_DATETIME:
          case SQL_TYPE_DATE:
          case SQL_TYPE_TIME:
          case SQL_TYPE_TIMESTAMP:
            values += "'";
            qvalz = fieldvalue->GetBuffer();
            qvalz.SubstituteSubString("'", "''");
            values += qvalz;
            values += "',";              
            break;

          default:
            values += fieldvalue->GetBuffer();
            values += ",";
            break;
        }
      }
    } // end for

    if (morefields != NULL)
    {
      while ((fieldname = va_arg(morefields, char*)) != NULL)
      {
        values += "'";
        fields += fieldname;

        if ((mfieldvalue = va_arg(morefields, char*)) == NULL)
          return false;

        values += mfieldvalue;
        fields += ",";
        values += "',";
      }
    }

    fields.Left(fields.Len() - 1);
    values.Left(values.Len() - 1);

    buffer = "INSERT INTO ";
    buffer += target;
    buffer += " (";
    buffer += fields;
    buffer += ") VALUES (";
    buffer += values;
    buffer += ")";

    *query = buffer;
  }
  return true;
}
