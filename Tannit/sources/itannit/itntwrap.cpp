/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#ifndef __ITX_ITNTWRAP_CPP__
#define __ITX_ITNTWRAP_CPP__
#endif

#include "tntsql.h"
#include <sql.h>
#include "itannitc.h"
#include "itannit.h"
#include "iregtp.h"

ITannit itannit;
itxTP itxtp;
itxSQLConnection isqlconn;

////////////////////////////////////////////////
//BEGIN Interfaccia C alla classe ITannit

// Attiva l'oggetto ITAnnit in modo che interfacci la struttura dati portante di Tannit
bool ITannit_Create(PTannitQueryResult* qres, int* count)
{
//  IConversation_Set();

	if (itannit.Get() == NULL)
	{
		itannit.Init(qres, count);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

#ifdef _ITX_APPC
// Attiva l'oggetto itxTP che contiene le informazioni relative ai TP da usare
bool ITannit_RegisterTP(char* filename)
{
  return itxtp.Load(filename);
}

// Attiva l'oggetto ITAnnit in modo che interfacci la struttura dati portante di Tannit
bool ITannit_CreateEnv(PTannitQueryResult* qres, int* count, char* llu, char* rtp)
{
  IConversation_SetEnv(llu, rtp);
	if (itannit.Get() == NULL)
	{
		itannit.Init(qres, count);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
#endif

// Disattiva l'oggetto ITAnnit
void ITannit_Destroy()
{
#ifdef _ITX_APPC
  IConversation_Reset();
#endif

	itannit.Init(NULL, NULL);
}

int ITannit_StoreToken(PTannitQueryResult qres, char* token, int field_num)
{
	return itannit.StoreField(qres, token, field_num);
}

int ITannit_StoreValue(PTannitQueryResult qres, char* field, char* value)
{
  int field_num = 0;

  if (qres->current_record == NULL)
  {
    if (!itannit.AllocateNewRecord(qres))
      return 0;
    
	  qres->totalRows++;
	  qres->rowsToStore++;
    qres->startingRow = STARTING_ROW;
  }

  if ((field_num = itannit.GetColPos(qres, field)) > 0)
	  return itannit.StoreField(qres, value, field_num - 1);
  return 0;
}

int ITannit_StoreRecord(char* name, ...)
{
  PTannitQueryResult qres;
  va_list fieldlist;
  char* token;
  int field_count = 0;

  va_start(fieldlist, name); 

  if (  (qres = ITannit_DBStructure(name)) == NULL )
    return ITXFAILED;
  
  while ((token = va_arg(fieldlist, char*)) != NULL)
  {
		if (field_count < qres->colsNumber)
		{
			if ((field_count = ITannit_StoreToken(qres, token, field_count)) == ITXFAILED)
			{
				ITannit_RemoveCurrentRec(qres);
				return ITXFAILED;
			}
		}
  }

	while (field_count < qres->colsNumber)
	{
		if ((field_count = ITannit_StoreToken(qres, "", field_count)) == ITXFAILED)
		{
			ITannit_RemoveCurrentRec(qres);
			return ITXFAILED;
		}
	}

	if (!ITannit_CheckCurrentRec(field_count))
	{
		ITannit_RemoveCurrentRec(qres);
		return ITXFAILED;
	}
	
	qres->totalRows++;
	qres->rowsToStore++;
  qres->startingRow = STARTING_ROW;

	return field_count;
}

// row counter starts from 1
char* ITannit_AllocateSQLInsertStringWith(char* target, char* table, int row, va_list morefields)
{
	TannitQueryResult* qres = NULL;
  char* sqlstring = NULL;
  char  buffer[ITX_QRS_MAX_QUERY_LEN];
  char  fields[ITX_QRS_MAX_QUERY_LEN];
  char  values[ITX_QRS_MAX_QUERY_LEN];
  char  qvalz[ITX_QRS_MAX_FIELD_LEN];
  TannitRecord* record;
  int i;
  char* fieldname;
  char* fieldvalue;
  int type;

	if ((qres = itannit.FindEntry(table)) != NULL)
	{
    if ((record = itannit.GetRecordNumber(qres, row)) != NULL)
    {
      memset(buffer, 0, ITX_QRS_MAX_QUERY_LEN);
      memset(fields, 0, ITX_QRS_MAX_QUERY_LEN);
      memset(values, 0, ITX_QRS_MAX_QUERY_LEN);

      for(i = 0; i < qres->colsNumber; i++)
      {
        type = qres->queryHeader[i].sqlDataType;

        if (!ISNULL(record->row[i]))
        {    
          strcat(fields, qres->queryHeader[i].name);
          strcat(fields, ",");
          
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
              strcat(values, "'");
              memset(qvalz, '\0', ITX_QRS_MAX_FIELD_LEN);
              ITannit_SubStr(record->row[i], qvalz, ITX_QRS_MAX_FIELD_LEN, "'", "''");
              strcat(values, qvalz);
              strcat(values, "',");              
              break;

            default:
              strcat(values, record->row[i]);
              strcat(values, ",");
              break;
          }
        }
      }

      if (morefields != NULL)
      {
        while ((fieldname = va_arg(morefields, char*)) != NULL)
        {
          strcat(values, "'");
          strcat(fields, fieldname);

          if ((fieldvalue = va_arg(morefields, char*)) == NULL)
            return NULL;

          strcat(values, fieldvalue);
          strcat(fields, ",");
          strcat(values, "',");
        }
      }

      fields[strlen(fields)-1] = 0;
      values[strlen(values)-1] = 0;

      sprintf(buffer, "INSERT INTO %s (%s) VALUES (%s)", target, fields, values);
      sqlstring = (char*) malloc(strlen(buffer));
      strcpy(sqlstring, buffer);
    }
  }

  return sqlstring;
}

char* ITannit_AllocateSQLUpdateStringWith(char* target, char* table, int row, ...)
{
	TannitQueryResult* qres = NULL;
  char* sqlstring = NULL;
  char  buffer[ITX_QRS_MAX_QUERY_LEN];
  char  set[ITX_QRS_MAX_QUERY_LEN];
  char  filter[ITX_QRS_MAX_QUERY_LEN];
  TannitRecord* record;
  int i;
  va_list prmfilters;
  int flt;

  va_start(prmfilters, row);

	if ((qres = itannit.FindEntry(table)) != NULL)
	{
    if ((record = itannit.GetRecordNumber(qres, row)) != NULL)
    {
      memset(buffer, 0, ITX_QRS_MAX_QUERY_LEN);
      memset(set, 0, ITX_QRS_MAX_QUERY_LEN);
      memset(filter, 0, ITX_QRS_MAX_QUERY_LEN);

      for(i = 0; i < qres->colsNumber; i++)
      {
        if (!ISNULL(record->row[i]))
        {    
          strcat(set, qres->queryHeader[i].name);
          strcat(set, "=");
          strcat(set, "'");
          strcat(set, record->row[i]);
          strcat(set, "',");
        }
      }

      set[strlen(set)-1] = 0;

      while ((flt = va_arg(prmfilters, int)) != NULL)
      {
        strcat(filter, "(");
        strcat(filter, qres->queryHeader[flt].name);  
        strcat(filter, "=");
        strcat(filter, "'");
        strcat(filter, record->row[flt]);
        strcat(filter, "') AND ");
      }
      
      if (!ISNULL(filter))
      {
        memset(filter + strlen(filter)-5, 0, 5);
        sprintf(buffer, "UPDATE %s SET (%s) WHERE (%s)", target, set, filter);
      }
      else
        sprintf(buffer, "UPDATE %s SET (%s)", target, set);

      sqlstring = (char*) malloc(strlen(buffer));
      strcpy(sqlstring, buffer);
    }
  }

  return sqlstring;
}

// attiva la connessione con SQL Server
bool ITannit_ConnectSQL(char* dsn, char* uid, char* pwd)
{
  if (isqlconn.IsConnected()) 
    isqlconn.Destroy();
  return isqlconn.Create(dsn, uid, pwd);  
}


// la connessione con SQL Server deve essere attiva
bool ITannit_DisconnectSQL()
{
  return isqlconn.Destroy();  
}

bool ITannit_ManualCommitSQL()
{
  return isqlconn.ManualCommit();  
}

bool ITannit_AutoCommitSQL()
{
  return isqlconn.AutoCommit();  
}

bool ITannit_CommitSQL()
{
  return isqlconn.Commit();  
}

bool ITannit_RollbackSQL()
{
  return isqlconn.Rollback();  
}

// la connessione con SQL Server deve essere attiva
bool ITannit_ExecuteSQL(char* stm)
{
  return itannit.ExecuteSQL(&isqlconn, stm);  
}

int ITannit_ExecuteSQLQuery(char* dsn, char* uid, char* pwd, char* queryname, char* query, int firstRec, int recsToStore)
{
  isqlconn.Create(dsn, uid, pwd);
  return itannit.ExecuteQuery(&isqlconn, queryname, query, firstRec, recsToStore);
}

// la connessione con SQL Server deve essere attiva
short int ITannit_GetSQLCols()
{
  return isqlconn.m_cols;
}

// la connessione con SQL Server deve essere attiva
bool ITannit_GetSQLColInfo(short int col, char* name, short* type, long int* size)
{
  return isqlconn.GetColInfo(col, name, type, size);
}

// la connessione con SQL Server deve essere attiva
bool ITannit_SkipSQLRecords(int start, int end)
{
  if (end < start) return TRUE;
  return isqlconn.SkipRecords(start, end);
}

bool ITannit_FillTQRColsInfoFromSQL(char* dsn, char* uid, char* pwd, char* tqrname, char* table)
{
	TannitQueryResult* qres = NULL;
	int actualCol;
  bool result = TRUE;
  char query[256];

  if (!ITannit_ConnectSQL(dsn, uid, pwd))  
    return FALSE;

  // allocazione ed inizializzazione di una nuova TannitQueryResult con le informazioni 
  // del corrispondente RecordSet su SQL Server
	if ((qres = ITannit_FindQuery(tqrname)) != NULL)
  {
    sprintf(query, "SELECT * FROM %s", table);  
    if (ITannit_ExecuteSQL(query))
    {
		  for (actualCol = 0; actualCol < qres->colsNumber; actualCol++)
        ITannit_GetSQLColInfo(actualCol, &(qres->queryHeader[actualCol].name[0]),
                                         &(qres->queryHeader[actualCol].sqlDataType),
                                         &(qres->queryHeader[actualCol].colummnSize));
    }
    else
      return FALSE;
  }
  else
    result = FALSE;

  ITannit_DisconnectSQL();

  return result;
}
/* DEPRECATED 
// Questa funzione � atomica rispetto alla connessione.
// se recsToStore = 0 si prendono tutti i record fino al massimo ammissibile. 
bool ITannit_SelectFromSQL(char* dsn, char* uid, char* pwd, char* queryname, char* query, int firstRec, int recsToStore)
{
	TannitQueryResult* qres = NULL;
	long int counter, actualCol;
  int recordsNumber = 0;
  bool result = TRUE;
  SQLRETURN retcode;

  if (!ITannit_ConnectSQL(dsn, uid, pwd))  
    return FALSE;

  // allocazione ed inizializzazione di una nuova TannitQueryResult con le informazioni 
  // del corrispondente RecordSet su SQL Server
	if ((qres = itannit.AllocateNewEntry(queryname, query, firstRec, recsToStore)) != NULL)
  {
    if (ITannit_SkipSQLRecords(1, qres->startingRow - 1))
    {
      recordsNumber = qres->startingRow + qres->rowsToStore - 1; 
      for (counter = qres->startingRow; counter <= recordsNumber; counter++)
      {
        if (itannit.AllocateNewRecord(qres))
        {
          for (actualCol = 0; actualCol < qres->colsNumber; actualCol++)
          {
            itannit.AllocateFieldSpace(qres, qres->queryHeader[actualCol].colummnSize, actualCol);
            if (!SQL_SUCCEEDED(isqlconn.BindCol(actualCol, (qres->current_record)->row[actualCol], 
                               qres->queryHeader[actualCol].colummnSize + 1)))  
              break;
          }
          retcode = isqlconn.Fetch();
          if (!SQL_SUCCEEDED(retcode))
          {
            break;
          }
          else
          {
            (qres->totalRows)++;
          }
        }
        else
        {
          result = FALSE;
          break;
        }
      }
    }
    else
      result = FALSE;
  }
  else
    result = FALSE;

  ITannit_DisconnectSQL();

  return result;
}
*/

bool ITannit_SelectDirectFromSQL(char* dsn, char* uid, char* pwd, char* queryname, char* query, int firstRec, int recsToStore)
{
  isqlconn.Create(dsn, uid, pwd);
  return itannit.SelectFromSQL(&isqlconn, queryname, query, firstRec, recsToStore);
}

// just Insert. it's up to the user previously delete the record from Database.
bool ITannit_InsertSQL(char* target, char* table, int row, ...)
{
  va_list morefields;
  char* query = NULL;

  va_start(morefields, row);

  if ((query = ITannit_AllocateSQLInsertStringWith(target, table, row, morefields)) == NULL)
    return FALSE;

  if (!ITannit_ExecuteSQL(query))
    return FALSE;
  
  free(query);

  return TRUE;
}

// just Insert. it's up to the user previously delete the record from Database.
bool ITannit_BulkInsertSQL(FILE* log, char* target, char* table, ...)
{
  va_list morefields;
  TannitQueryResult* qres = NULL;
  char* query = NULL;

  va_start(morefields, table);   

  if ((qres = ITannit_FindQuery(table)) == NULL)
    return FALSE;

  for (int row = 1; row <= qres->totalRows; row++)
  {
    if ((query = ITannit_AllocateSQLInsertStringWith(target, table, row, morefields)) == NULL)
      return FALSE;

    if (!ITannit_ExecuteSQL(query))
      ITannit_ErrorSQL(log);
  
    free(query);
  }

  return TRUE;
}

void ITannit_DeleteSQL(char* target, ...)
{
  va_list prmfilters;
  char* flt;
  char* fieldvalue;
  char* sqlstring = NULL;
  char  buffer[ITX_QRS_MAX_QUERY_LEN];
  char  filter[ITX_QRS_MAX_QUERY_LEN];

  va_start(prmfilters, target);

  memset(buffer, 0, ITX_QRS_MAX_QUERY_LEN);
  memset(filter, 0, ITX_QRS_MAX_QUERY_LEN);

  while ((flt = va_arg(prmfilters, char*)) != NULL)
  {
    strcat(filter, "(");
    strcat(filter, flt);  
    strcat(filter, "=");
    strcat(filter, "'");
    if ((fieldvalue = va_arg(prmfilters, char*)) == NULL)
      return;
    strcat(filter, fieldvalue);
    strcat(filter, "') AND ");
  }
  
  if (!ISNULL(filter))
  {
    memset(filter + strlen(filter)-5, 0, 5);
    sprintf(buffer, "DELETE FROM %s WHERE (%s)", target, filter);
  }
  else
    sprintf(buffer, "DELETE FROM %s", target);

  sqlstring = (char*) malloc(strlen(buffer));
  strcpy(sqlstring, buffer);

  ITannit_ExecuteSQL(sqlstring);
  free(sqlstring);
}

int ITannit_ErrorSQL(FILE* log)
{
  isqlconn.Error(log);
  return isqlconn.m_nativerr;
}


TannitQueryResult* ITannit_FindQuery(char* token)
{
	return itannit.FindEntry(token);
}

TannitQueryResult* ITannit_FindRegTP(int tpid)
{
	return itannit.FindEntry(itxtp.GetTPName(tpid));
}

int ITannit_GetTPFields(char* tpname)
{
  return itxtp.GetTPNFields(tpname);  
}

// Ritorna il puntatore alla struttura QueryResult di Tannit da utilizzare.
// Eventualmente ne forza la creazione.
// Prepara lo spazio per un nuovo record
TannitQueryResult* ITannit_DBStructure(char* token)
{
	TannitQueryResult* qres = NULL;
	
	if ((qres = itannit.FindEntry(token)) == NULL)
	{
		//devo creare una nuova QueryResult
		qres = itannit.AllocateNewEntry(&itxtp, token);
	}
	
	if (qres != NULL)
	{
		if (!itannit.AllocateNewRecord(qres))
			qres = NULL;
	}

	return qres;
}

TannitQueryResult* ITannit_NewEntry(char* token, int numfields)
{
	TannitQueryResult* qres = NULL;
	
	if ((qres = itannit.FindEntry(token)) == NULL)
	{
		//devo creare una nuova QueryResult
		qres = itannit.AllocateNewEntry(token, numfields, NULL);
	}
	
	return qres;
}

TannitQueryResult* ITannit_NewTQRFrom(char* source, char* field, char* value, char* destination)
{
  TannitQueryResult* qres = NULL;
  TannitQueryResult* qsrc = NULL;
  TannitRecord* record;
  char* filter;
  int irow = 0;

	if ((qsrc = itannit.FindEntry(source)) == NULL)
    return qres;

	if ((qres = itannit.Duplicate(qsrc, destination)) != NULL)
	{
    qres->totalRows = 0;
    record = qsrc->recPtr;
//    while (record != NULL)
    while (irow < qsrc->totalRows)
    {
      irow++;
      filter = ITannit_GetValue(qsrc, irow, field);

      if (ISEQUAL(filter, value))
      {
        if (!itannit.AllocateNewRecord(qres))
          return qres;
        itannit.CopyRecord(qsrc, record, qres);
        qres->totalRows++;
      }
      record = record->next;
    }
  }
  return qres;
}


void ITannit_RemoveCurrentRec(TannitQueryResult* qres)
{
	itannit.RemoveCurrentRec(qres);
}

void ITannit_RemoveQueries(char* firstname, ...)
{
  va_list tables;
	TannitQueryResult* qres;
  char* name;

  if (firstname == NULL)
    return;
  else
  {
		if ((qres = itannit.FindEntry(firstname)) != NULL)
	    itannit.RemoveEntry(qres);

    va_start(tables, firstname); 
    while((name = va_arg(tables, char*)) != NULL)
    {
			if ((qres = itannit.FindEntry(name)) != NULL)
				itannit.RemoveEntry(qres);
	  }
  }  
}

void ITannit_RemoveExcluding(char* firstname, ...)
{
  va_list tables;
	int i = 0;
	TannitQueryResult* qres;
  char* name;
  bool exclude;

  if (firstname == NULL)
    itannit.DoEmpty();
  else
  {
    int qcount = itannit.GetCount();
    for (i = 0; i < qcount; i++)
    {
      exclude = FALSE;
		  qres = itannit.Get(i);
      if (qres != NULL)
      {
        if (ISEQUAL(firstname, qres->id))
          exclude = TRUE;
        else
        {
          va_start(tables, firstname); 
          while((name = va_arg(tables, char*)) != NULL)
          {
            if (ISEQUAL(name, qres->id))
            {
              exclude = TRUE;
              break;
            }
          }
        }

		    if (!exclude)
          itannit.RemoveEntry(qres);
      }
	  }
  }  
}

bool ITannit_CheckCurrentRec(int numfield)
{
	return itannit.CheckCurrentRec(numfield);
}

void ITannit_SetUser(char* user)
{
  itannit.SetUser(user);
}

bool ITannit_Dump(FILE* fp)
{
	return itannit.Dump(fp);
}

#ifdef _ITX_APPC
char* ITannit_TPExecute(PACKET_TYPE msgtype, int maxretray, FILE* log,  ...)
{
	char  packet[ITX_APPC_MAX_INFO_LENGTH + 2];
	char* returned_buffer = NULL;
  va_list tables;

  va_start(tables, log); 
   
  memset(packet, '\0', ITX_APPC_MAX_INFO_LENGTH + 2);

  itannit.BuildPacketToSend(packet, ITX_APPC_LAST_PACKET, msgtype, tables);
  returned_buffer = TPSendAndReceive(packet, log, msgtype, maxretray);
  
  if (returned_buffer != NULL)
    ITannit_ParsTP(NULL, returned_buffer, 1, 1, log, ITX_APPC_RECORD_SEP, ITX_APPC_FIELD_SEP);
  
	return returned_buffer;
}

char* ITannit_TPExecuteCnv(PACKET_TYPE msgtype, int maxretray, FILE* log,  ...)
{
	char  packet[ITX_APPC_MAX_INFO_LENGTH + 2];
	char* returned_buffer = NULL;
  long rblen = 0;
  va_list tables;

  va_start(tables, log); 
   
  memset(packet, '\0', ITX_APPC_MAX_INFO_LENGTH + 2);

  itannit.BuildPacketToSend(packet, ITX_APPC_LAST_PACKET, msgtype, tables);
  returned_buffer = TPSendAndReceive(packet, log, msgtype, maxretray);
  
  rblen = strlen(returned_buffer);

  IConversation_EBCDtoASCII(returned_buffer, rblen);
  if (returned_buffer != NULL)
    ITannit_ParsTP(NULL, returned_buffer, 1, 1, log, ITX_ASCII_RECORD_SEP, ITX_ASCII_FIELD_SEP);
  
	return returned_buffer;
}


char* ITannit_TPExecuteFromFile(FILE* fp, FILE* log, PACKET_TYPE msgwaited, int maxretray)
{
	char* returned_buffer = NULL;
	char* appc_packet;
  
	if((appc_packet = TPReceiveFromFile(fp)) == NULL)
			return NULL;
  
//  int i;
//  appc_packet = (char*) calloc(1,95);
//  for(i = 161; i<256; i++)
//    appc_packet[i - 161] = i;    

  printf("%s\n", appc_packet);
//  appc_packet[strlen(appc_packet) - 1] = 0;
  
  returned_buffer = TPSendAndReceive(appc_packet, log, msgwaited, maxretray);
  
	return returned_buffer;
}

bool ITannit_ReceiveFileFromAS400(char* filepath, FILE* log)
{
	char   packet[ITX_APPC_MAX_INFO_LENGTH + 2];
  FILE*  fd;
	char*  returned_buffer = NULL;
  char   wrc[4];
  char*  dati = NULL;
	bool   file_completed = FALSE;
  char   as400_end_info[ITX_APPC_AS400_TX_END_LEN + 1];

  if (!IConversation_OpenTransaction(log))
    return FALSE;

  if ((fd = fopen(filepath, "wb")) == NULL)
  {
    IConversation_CloseTransaction(log);
    return FALSE;
  }
  
	memset(as400_end_info, '\0', ITX_APPC_AS400_TX_END_LEN + 1);
  memset(packet, '\0', ITX_APPC_MAX_INFO_LENGTH + 2);
  memset(wrc, '\0', 4);
  
  if (!itannit.BuildSinglePacketToSend(packet, ITX_APPC_PC_FTX, "C00"))
    ROLLBACK_TRANSACTION;
    
  returned_buffer = TPSendAndReceive(packet, log, ITX_APPC_ANY_MSG, 1);


  if (returned_buffer == NULL)
    ROLLBACK_TRANSACTION;
  
  ITannit_ParsTP(NULL, returned_buffer, 1, 1, log, ITX_APPC_RECORD_SEP, ITX_APPC_FIELD_SEP);
  memcpy(wrc, returned_buffer, 3);

  if (ISEQUAL(wrc, "E00"))
    ROLLBACK_TRANSACTION;

  if ((dati = itannit.GetValue("C01", 1, 4)) == NULL)
    ROLLBACK_TRANSACTION;

  if (ISEQUAL(dati, "Y"))
  {
    if (!itannit.BuildSinglePacketToSend(packet, ITX_APPC_PC_FTX, "C02"))
      ROLLBACK_TRANSACTION;

    if (!IConversation_OpenTransaction(log))
      return FALSE;

    if (!IConversation_SendData(packet, log))
        ROLLBACK_TRANSACTION;

    IConversation_PrepareReceive(log);

    // AS400 a questo punto invia un C05 in testa al file di ritorno contente
    // informazioni sul fatto che vi siano ulteriori informazioni disponibili.
    // Per il momento questo pacchetto non viene processato.
    memset(packet, '\0', ITX_APPC_MAX_INFO_LENGTH + 2);
    if (!IConversation_ReceiveFileChunk(packet, ITX_APPC_MAX_INFO_LENGTH, log))
      ROLLBACK_TRANSACTION;

    while (!file_completed)
    {
      memset(packet, '\0', ITX_APPC_MAX_INFO_LENGTH + 2);

      if (!IConversation_ReceiveFileChunk(packet, ITX_APPC_MAX_INFO_LENGTH, log))
        ROLLBACK_TRANSACTION;
      strncpy(as400_end_info, packet, 4);

      if (ISEQUAL(as400_end_info, ITX_APPC_AS400_TX_END))
        file_completed = TRUE;
      else if (!ISNULL(packet))
      {
        packet[strlen(packet)] = '\n';  
        packet[strlen(packet) + 1] = '\0';
        itxCharToOem(packet, packet);
        fputs(packet, fd);
      }
    }

    IConversation_CloseTransaction(log);
  }
  fclose(fd);

  return TRUE;
}

bool ITannit_SendHOK(FILE* log, PACKET_TYPE ptype)
{
	char   packet[ITX_APPC_MAX_INFO_LENGTH + 2];

  ITannit_StoreRecord("HOK", "HOK", NULL);

  memset(packet, '\0', ITX_APPC_MAX_INFO_LENGTH + 1);
  if (!itannit.BuildSinglePacketToSend(packet, ptype, "HOK"))
  {
    IConversation_CloseTransaction(log);
    return FALSE;
  }

  if (!IConversation_OpenTransaction(log))
    return FALSE;

  if (!IConversation_SendData(packet, log))
  {
    IConversation_CloseTransaction(log);
    return FALSE;
  }

  IConversation_PrepareReceive(log);
  IConversation_ReceiveFileChunk(packet, ITX_APPC_MAX_INFO_LENGTH, log);
  IConversation_CloseTransaction(log);

  return TRUE;
}

bool ITannit_SendFileToAS400(char* filepath, FILE* log)
{
	char  packet[ITX_APPC_MAX_INFO_LENGTH + 2];
  char  fdata[ITX_APPC_MAX_INFO_LENGTH + 2];
  char  trash[ITX_APPC_MAX_INFO_LENGTH + 1];
  FILE* fd;
	char* returned_buffer = NULL;
  char  wrc[4];
  char* dati = NULL;
	bool file_completed = FALSE;
  char as400_end_info[ITX_APPC_AS400_TX_END_LEN + 1];
	int size;

  if (!IConversation_OpenTransaction(log))
    return FALSE;

  if ((fd = fopen(filepath, "rb")) == NULL)
  {
    IConversation_CloseTransaction(log);
    return FALSE;
  }
  
	memset(as400_end_info, '\0', ITX_APPC_AS400_TX_END_LEN + 1);
  memset(packet, '\0', ITX_APPC_MAX_INFO_LENGTH + 2);
  memset(wrc, '\0', 4);
  
  if (!itannit.BuildSinglePacketToSend(packet, ITX_APPC_PC_FTX, "C00"))
    ROLLBACK_TRANSACTION;
    
  returned_buffer = TPSendAndReceive(packet, log, ITX_APPC_ANY_MSG, 1);

  if (returned_buffer == NULL)
    ROLLBACK_TRANSACTION;
  
  ITannit_ParsTP(NULL, returned_buffer, 1, 1, log, ITX_APPC_RECORD_SEP, ITX_APPC_FIELD_SEP);
  memcpy(wrc, returned_buffer, 3);

  if (ISEQUAL(wrc, "E00"))
    ROLLBACK_TRANSACTION;

  memset(packet, '\0', ITX_APPC_MAX_INFO_LENGTH + 2);
  if (!itannit.BuildSinglePacketToSend(packet, ITX_APPC_PC_FTX, "C03"))
    ROLLBACK_TRANSACTION;
    
  if (!IConversation_OpenTransaction(log))
    return FALSE;

  if (!IConversation_SendData(packet, log))
    ROLLBACK_TRANSACTION;

  while (!feof(fd))
  {
    memset(fdata, '\0', ITX_APPC_MAX_INFO_LENGTH + 2);
		fgets(fdata, ITX_APPC_MAX_INFO_LENGTH, fd);
    itxOemToChar(fdata, fdata);
		size = strlen(fdata);
		if(size == ITX_APPC_MAX_INFO_LENGTH)
			fgets(trash, ITX_APPC_MAX_INFO_LENGTH, fd);
//    if(size != 0)
    if(size > ITX_APPC_HEADER_LENGTH)
    {
      padnstr(fdata, ITX_APPC_MAX_INFO_LENGTH, ' ');  
      if (!IConversation_SendData(fdata, log))
        ROLLBACK_TRANSACTION;
    }
  }

  memset(packet, '\0', ITX_APPC_MAX_INFO_LENGTH + 2);
  memcpy(packet, ITX_APPC_AS400_TX_END, 5);
  padnstr(packet, ITX_APPC_MAX_INFO_LENGTH, ' ');
  packet[ITX_APPC_MAX_INFO_LENGTH] = '\0';
    if (!IConversation_SendData(packet, log))
      ROLLBACK_TRANSACTION;
  
  IConversation_PrepareReceive(log);
  IConversation_ReceiveFileChunk(packet, ITX_APPC_MAX_INFO_LENGTH, log);
  IConversation_CloseTransaction(log);

  fclose(fd);

  return TRUE;
}
#endif // _ITX_APPC

// row � espresso in base 1.
char* ITannit_GetValue(TannitQueryResult* qres, int row, char* field)
{
  char* value = NULL;
  TannitRecord* record;

  if (row > 0 && row <= qres->totalRows)
  {
    record = qres->recPtr;
    for (int i = 1; i < row; i++)
      record = record->next;
    
    int columnpos = 0;
    if ((columnpos = itannit.GetColPos(qres, field)) > 0)
      value = record->row[columnpos - 1];
  }

  return value;
}

// row deve essere in base 1
void ITannit_SetCurrentRecord(TannitQueryResult* qres, int row)
{
  TannitRecord* record;

  if (row > 0 && row <= qres->totalRows)
  {
    record = qres->recPtr;
    for (int i = 1; i < row; i++)
      record = record->next;
  
    qres->current_record = record;
  }

  return;
}

//END Interfaccia C alla classe ITannit
////////////////////////////////////////////////
