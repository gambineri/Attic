/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#ifndef __ITX_ITANNIT_CPP__
#define __ITX_ITANNIT_CPP__
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>

#ifdef _ITX_APPC
#include "iappcdim.h"
#endif      

#include "itxtypes.h"
#include "itannitdef.h"
#include "iregtp.h"
#include "itannit.h"



#ifdef _ITX_APPC
#define ROLLBACK_TRANSACTION    { IConversation_CloseTransaction(log); fclose(fd); return FALSE; }
#endif      


//extern itxTP itxtp;

void ZLEADPAD(char* a, int b, char* c)
{  
  char zcode[256];
  memset(zcode, '0', b);
  zcode[b+1] = 0;
  int  l = strlen(a);
  memcpy(zcode + b - l, a, l);
  sprintf(c, "%s", zcode);
}


// Trova, se esiste, la struttura QueryResult di Tannit identificata con name
// Aggiorna m_current
TannitQueryResult* ITannit::FindEntry(char* name)
{
	TannitQueryResult* qres = NULL;
	int icount = 0, i = 0;
	int qcount = *m_qcount;

	if (name != NULL && qcount > 0)
	{
		while (icount < qcount)
		{
      if (m_qres[i] != NULL)
      {
			  if (strcmp(m_qres[i]->id, name) == 0)
				  break;
			  icount++;
      }
      i++;
		}

		if (icount < qcount)
			qres = m_qres[i];
	}
	m_current = qres;
	return qres;
}

int ITannit::Store(TannitQueryResult* qres)
{
  int i = 0;

	while (m_qres[i] != NULL && i < QUERY_NUMBER)
    i++;

  if (m_qres[i] == NULL)
  {
    m_qres[i] = qres;
    (*m_qcount)++; 
    m_current = qres;
    return i;
  }
  else
    return ITXFAILED;
}

int ITannit::GetIndex(char* name)
{
	int icount = 0, i = 0;
	int qcount = *m_qcount;

	if (name != NULL && qcount > 0)
	{
		while (icount < qcount)
		{
      if (m_qres[i] != NULL)
      {
			  if (strcmp(m_qres[i]->id, name) == 0)
				  break;
			  icount++;
      }
      i++;
		}

		if (icount < qcount)
			return i;
	}

  return -1;
}

// Crea una nuova Entry (QueryResult)
// Aggiorna m_current
// Transaction Program Based
TannitQueryResult* ITannit::AllocateNewEntry(itxTP* tpset, char* name)
{
	unsigned short tpid;
	TannitQueryResult* qres = NULL;
  int i;

	if ((tpid = tpset->GetTPID(name)) != NUM_TPAS400)
	{	
		qres = (TannitQueryResult*) calloc(1, sizeof(TannitQueryResult));
		if (qres != NULL)
		{
      if ((qres->queryHeader = (TannitColumnHeader*) calloc(tpset->m_tpas400[tpid].n_fields, sizeof(TannitColumnHeader))) != NULL)
      {
			  strcpy(qres->id, tpset->m_tpas400[tpid].name);
			  qres->colsNumber = tpset->m_tpas400[tpid].n_fields;
			  qres->actualRow = STARTING_ROW;

        for(i = 0; i < qres->colsNumber; i++)
          sprintf(qres->queryHeader[i].name, "f%d\0", i + 1);

			  Store(qres);
      }
      else
        FREE(qres);
		}
	}

	return qres;
}

// Crea una nuova Entry (QueryResult)
// Aggiorna m_current
// General version
TannitQueryResult* ITannit::AllocateNewEntry(char* queryname, int numfields, ...)
{
	TannitQueryResult* qres = NULL;
  va_list fieldlist;
  char* fieldname;
  int ifields;

  va_start(fieldlist, numfields); 

	qres = (TannitQueryResult*) calloc(1, sizeof(TannitQueryResult));
	if (qres != NULL)
	{
    if ((qres->queryHeader = (TannitColumnHeader*) calloc(numfields, sizeof(TannitColumnHeader))) != NULL)
    {
  	  strcpy(qres->id, queryname);
			qres->colsNumber = numfields;
			qres->actualRow = STARTING_ROW;

      ifields = 0;
      while (ifields < numfields)
      {
        if ((fieldname = va_arg(fieldlist, char*)) != NULL)
          sprintf(qres->queryHeader[ifields].name, "%s\0", fieldname);
        else
        {
          va_start(fieldlist, numfields); 
          sprintf(qres->queryHeader[ifields].name, "f%d\0", ifields + 1);
        }
        ifields++;
      }

			Store(qres);
    }
    else
      FREE(qres);
	}

	return qres;
}

// Crea una nuova Entry (QueryResult)
// Aggiorna m_current
// SQL Server Based
TannitQueryResult* ITannit::AllocateNewEntry(itxSQLConnection* psqlconn, char* tqrname, char* query, int firstRec, int recsToStore)
{
	TannitQueryResult* qres = NULL;
	short int actualCol, colsNumber;
	TannitColumnHeader* colummnHead;


  if (ExecuteSQL(psqlconn, query))
  {
	  qres = (TannitQueryResult*) calloc(1, sizeof(TannitQueryResult));
	  if (qres != NULL)
	  {
      colsNumber = GetSQLCols(psqlconn);
		  colummnHead = (TannitColumnHeader*) calloc(colsNumber, sizeof(TannitColumnHeader));
      if (colummnHead != NULL)
      {
		    qres->queryHeader = colummnHead;
		    qres->colsNumber = colsNumber;
    	  strcpy(qres->id, tqrname);
        if (firstRec < STARTING_ROW) firstRec = STARTING_ROW;
        if (recsToStore < 1)	recsToStore = ROWS_TO_STORE;
        qres->startingRow = firstRec;
        qres->rowsToStore = recsToStore;
        qres->actualRow = STARTING_ROW;
        qres->totalRows = 0;

		    for (actualCol = 0; actualCol < colsNumber; actualCol++)
          GetSQLColInfo(psqlconn, actualCol, &(qres->queryHeader[actualCol].name[0]),
                                           &(qres->queryHeader[actualCol].sqlDataType),
                                           &(qres->queryHeader[actualCol].colummnSize));

        Store(qres);
      }
      else
        FREE(qres);
	  }
  }

  return qres;
}

TannitQueryResult* ITannit::Duplicate(TannitQueryResult* qsrc, char* destination)
{
	TannitQueryResult* qres = NULL;
  int ifields, numfields;

  if (qsrc == NULL) return NULL;
  numfields = qsrc->colsNumber;

	qres = (TannitQueryResult*) calloc(1, sizeof(TannitQueryResult));

	if (qres != NULL)
	{
    if ((qres->queryHeader = (TannitColumnHeader*) calloc(numfields, sizeof(TannitColumnHeader))) != NULL)
    {
  	  strcpy(qres->id, destination);
			qres->colsNumber = numfields;
			qres->startingRow = STARTING_ROW;
			qres->actualRow = STARTING_ROW;
      qres->rowsToStore = ROWS_TO_STORE;

      ifields = 0;
      while (ifields < numfields)
      {
        sprintf(qres->queryHeader[ifields].name, "%s\0", qsrc->queryHeader[ifields].name);
        ifields++;
      }

			Store(qres);
    }
    else
      FREE(qres);
	}

	return qres;
}


// Riserva lo spazio necessario per la memorizzazione di un nuovo record
// Aggiorna current_record
bool ITannit::AllocateNewRecord(TannitQueryResult* qres)
{
	TannitRecord* last_record;
	TannitRecord* new_record;

	new_record = (TannitRecord*) calloc(1, sizeof(TannitRecord));
	if (new_record != NULL)
	{
		if (qres->colsNumber > 0)
		{
			new_record->row = (char**) calloc(qres->colsNumber, sizeof(char*));
			if (new_record->row == NULL)
			{
				free(new_record);
				return FALSE;
			}
		}
	}
	else
		return FALSE;

	last_record = qres->recPtr;
	if (last_record == NULL)
		qres->recPtr = new_record;
	else
	{
		while (last_record->next != NULL)
			last_record = last_record->next;

		last_record->next = new_record;
	}

	qres->current_record = new_record;

	return TRUE;
}

// Si suppone che qres sia gi� posizionata sul record corrente.
int ITannit::StoreField(TannitQueryResult* qres, char* token, int position)
{
	char* newfield = NULL;
	int result = ITXFAILED;

	newfield = (char*) calloc(strlen(token) + 1, sizeof(char));
	if (newfield != NULL)
	{
		strcpy(newfield, token);
// BEGIN LMM 27/06/2000 special char translation support
//    itxEBCDToASCII(newfield);
//   END LMM 27/06/2000 special char translation support
		qres->current_record->row[position] = newfield;
		result = position + 1;
	}

	return result;
}

int ITannit::AllocateFieldSpace(TannitQueryResult* qres, int space, int position)
{
	char* newfield = NULL;
	int result = ITXFAILED;

	newfield = (char*) calloc(space + 1, sizeof(char));
	if (newfield != NULL)
	{
		qres->current_record->row[position] = newfield;
		result = position + 1;
	}
	return result;
}

bool ITannit::CopyRecord(TannitQueryResult* qsrc, TannitRecord* source, TannitQueryResult* qres)
{
  int i;

  for (i = 0; i < qsrc->colsNumber; i++)
  {
    AllocateFieldSpace(qres, strlen(source->row[i]), i);
    strcpy(qres->current_record->row[i], source->row[i]);
  }  

  return TRUE;
}


void ITannit::DoEmpty()
{
	int i;
	TannitQueryResult* qres;
  int count = (*m_qcount);

  i = 0;
	while(i < count)
	{
		qres = m_qres[i];
    if (qres != NULL)
    {
		  m_current = qres;
		  RemoveEntry(qres);
      i++;
    }
	}
}

void ITannit::RemoveEntry(TannitQueryResult* qres)
{
  int index = GetIndex(qres->id);

/*
  while (qres->recPtr != NULL)
  {
    qres->current_record = qres->recPtr;
    while (qres->current_record->next != NULL)
      qres->current_record = qres->current_record->next;

		RemoveCurrentRec(qres);
  }
*/
  FREE(qres->queryHeader);
  FREE(qres);
  (*m_qcount) -= 1;
  m_qres[index] = NULL;
}

void ITannit::RemoveCurrentRec(TannitQueryResult* qres)
{
	TannitRecord* previous;
	TannitRecord* current;
	char* field;
	int i = 0;

	// Vengono rimossi i campi allocati
	for (i = 0; i < qres->colsNumber; i++)
	{
		if((field = qres->current_record->row[i]) != NULL)
			free(field);
	}

	// Viene rimosso l'array di puntatori per i campi
	FREE(qres->current_record->row);
//	qres->current_record->row = NULL;
	
	previous = qres->recPtr;
	current = previous;
	while (current != qres->current_record)
	{
		previous = current;
		current = current->next;
	}
  previous->next = qres->current_record->next;
  if (qres->current_record == qres->recPtr)
    qres->recPtr = NULL;
	FREE(qres->current_record);

  qres->totalRows--;
}

bool ITannit::CheckCurrentRec(int numfield)
{
	TannitQueryResult* qres = m_current;

	if (qres->colsNumber != numfield)
		return FALSE;
	return TRUE;
}

// versione posteriore al 11/01/2000. supporta la lista di record.
int ITannit::BuildDataToSend(char* packet, va_list tables)
{
	TannitQueryResult* qres;
  char record[ITX_APPC_MAX_RECORD_LENGTH];
  char* tablename;
  int bytes_in_packet = 0;

  while ((tablename = va_arg(tables, char*)) != NULL)
  {
  	if ((qres = FindEntry(tablename)) != NULL)
    {
      for (int i = 0; i < qres->totalRows; i++)
      {
        memset(record, '\0', ITX_APPC_MAX_RECORD_LENGTH);
	      SerializeRecord(record, i);
        bytes_in_packet += strlen(record);
        strcat(packet, record);
      }
    }
  }

	return bytes_in_packet;
}

int ITannit::BuildSingleData(char* packet, char* tablename)
{
	TannitQueryResult* qres;
  char record[ITX_APPC_MAX_RECORD_LENGTH];
  int bytes_in_packet = 0;

  if ((qres = FindEntry(tablename)) != NULL)
  {
    memset(record, '\0', ITX_APPC_MAX_RECORD_LENGTH);
	  SerializeRecord(record, 0);
    bytes_in_packet += strlen(record);
    strcat(packet, record);
  }

	return bytes_in_packet;
}

bool ITannit::BuildPacketToSend(char* packet, int progr, PACKET_TYPE ptype, va_list tables)
{
  int bytes_in_packet = 0;

  switch (ptype)
  {
  case ITX_APPC_PC_REQ:
    if (!BuidHeaderToSend(packet, progr, ptype))
      return FALSE;
    break;
  case ITX_APPC_HS_FTX:
    if (!BuidCorrHeaderToSend(packet, progr, ptype))
      return FALSE;
    break;
  default:
    if (!BuidHeaderToSend(packet, progr, ptype))
      return FALSE;
    break;
  }
  
  if ((bytes_in_packet = BuildDataToSend(packet, tables)) == 0)
    return FALSE;

//BEGIN: modifica del 22/5/2000 per il supporto del carattere di fine informazione
  packet[bytes_in_packet + ITX_APPC_HEADER_LENGTH] = ITX_APPC_ENDINFO_SEP;
  packet[bytes_in_packet + ITX_APPC_HEADER_LENGTH + 1] = '\0';
//END  : modifica del 22/5/2000 per il supporto del carattere di fine informazione

  if (bytes_in_packet > (ITX_APPC_MAX_INFO_LENGTH - ITX_APPC_HEADER_LENGTH))
    return FALSE;

#ifdef _ITX_APPC
  padnstr(packet, ITX_APPC_MAX_INFO_LENGTH, ' ');  
#endif

  return TRUE;
}

bool ITannit::BuildSinglePacketToSend(char* packet, PACKET_TYPE ptype, char* name)
{
  int bytes_in_packet = 0;

  switch (ptype)
  {
  case ITX_APPC_PC_REQ:
    if (!BuidHeaderToSend(packet, ITX_APPC_LAST_PACKET, ptype))
      return FALSE;
    break;
  case ITX_APPC_PC_FTX:
    if (!BuidCorrHeaderToSend(packet, ITX_APPC_LAST_PACKET, ptype))
      return FALSE;
    break;
  default:
    if (!BuidHeaderToSend(packet, ITX_APPC_LAST_PACKET, ptype))
      return FALSE;
    break;
  }
  
  if ((bytes_in_packet = BuildSingleData(packet, name)) == 0)
    return FALSE;

  packet[bytes_in_packet + ITX_APPC_HEADER_LENGTH] = ITX_APPC_ENDINFO_SEP;
  packet[bytes_in_packet + ITX_APPC_HEADER_LENGTH + 1] = '\0';

#ifdef _ITX_APPC 
  padnstr(packet, ITX_APPC_MAX_INFO_LENGTH, ' ');  
#endif

  return TRUE;
}

bool ITannit::BuidCorrHeaderToSend(char* packet, int progr, PACKET_TYPE ptype)
{
	char appc_header[ITX_APPC_HEADER_LENGTH + 1];
  char prg[5];
  char tm[20];
  time_t ltime;
  struct tm *today;

	memset(appc_header, ITX_APPC_PC_FILLER, ITX_APPC_HEADER_LENGTH);
  appc_header[ITX_APPC_HEADER_LENGTH] = 0;
	memset(prg, '0', 5);

  appc_header[1] = 'P';
  if (ISNULL(m_user))
    memset(&appc_header[2], '0', 6);
  else
    memcpy(&appc_header[2], m_user, strlen(m_user));

  memset(&appc_header[8], '0', 2);
  sprintf(&appc_header[19], "%05d", 99999);

  time( &ltime );
  today = localtime( &ltime );
  strftime(tm, 20, "%Y%m%d", today);
  memcpy(&appc_header[24], tm, 8);
  
  strcpy(packet, appc_header);
  return TRUE;
}

bool ITannit::BuidHeaderToSend(char* packet, int progr, PACKET_TYPE ptype)
{
	TannitQueryResult* qres;
	char appc_header[ITX_APPC_HEADER_LENGTH + 1];
  char prg[5];
  char tm[20];
  time_t ltime;
  struct tm *today;

  if ((qres = FindEntry("P00")) == NULL)
    return FALSE;

	memset(appc_header, ITX_APPC_PC_FILLER, ITX_APPC_HEADER_LENGTH);
  appc_header[ITX_APPC_HEADER_LENGTH] = 0;
	memset(prg, '0', 5);

  appc_header[1] = ptype;
  if (ISNULL(qres->current_record->row[3]))
    memset(&appc_header[2], '0', 6);
  else
  {
    char ccode[7];
    memset(ccode, '\0', 7);
    ZLEADPAD(qres->current_record->row[3], 6, ccode);
    memcpy(&appc_header[2], ccode, 6);
  }
  appc_header[8] = '0';
  appc_header[9] = '0';
  
  sprintf(&appc_header[19], "%05d", progr);
  
  time( &ltime );
  today = localtime( &ltime );
  strftime(tm, 20, "%Y%m%d", today);
  memcpy(&appc_header[24], tm, 8);

  if (ISNULL(qres->current_record->row[2]))
    memset(&appc_header[41], 'I', 1);
  else
    memcpy(&appc_header[41], qres->current_record->row[2], 1);

  strcpy(packet, appc_header);
  return TRUE;
}

bool ITannit::DumpTable(FILE* fp, char* name)
{
	int i;
	TannitQueryResult* qres;
  char record[ITX_APPC_MAX_RECORD_LENGTH];

	if ((qres = FindEntry(name)) == NULL)
		return FALSE;

	fprintf(fp, "\n");fflush(fp);
	fprintf(fp, "TABLE %s\n", name);fflush(fp);
	fprintf(fp, "\t startingRow: %d\n", qres->startingRow);fflush(fp);
	fprintf(fp, "\t rowsToStore: %d\n", qres->rowsToStore);fflush(fp);
	fprintf(fp, "\t   totalRows: %d\n", qres->totalRows);fflush(fp);
	fprintf(fp, "\t  colsNumber: %d\n", qres->colsNumber);fflush(fp);
	fprintf(fp, "\n");fflush(fp);

	for (i = 1;	i <= qres->totalRows; i++)
	{
		if (!SerializeRecord(record, i - 1))
			return FALSE;
//    itxEBCDToASCII(record);
		fprintf(fp, "%4d\t\t%s\n", i, record);
		fflush(fp);
		memset(record, '\0', ITX_APPC_MAX_RECORD_LENGTH);
	}

	return TRUE;
}

bool ITannit::Dump(FILE* fp)
{
	int icount, i;
	TannitQueryResult* qres;
	time_t tm;
	
	time(&tm);

	fprintf(fp, "------------------------------------------------------------\n");
	fprintf(fp, " �AITECSA s.r.l.\n");
	fprintf(fp, " COMPLETE DUMP OF THE TANNIT QUERYRESULT DATA STRUCTURE SET\n");
	fprintf(fp, "  %s", ctime(&tm));
	fprintf(fp, "------------------------------------------------------------\n");
	fprintf(fp, "\n");fflush(fp);

	icount = 0;
  i = 0;
  while (icount < (*m_qcount))
  {
    if ((qres = m_qres[i]) != NULL)
    {
  		if (!DumpTable(fp, qres->id))
	  		return FALSE;
      else
        icount++;
    }
    i++;
	}

	return TRUE;
}

// pos � l'indice in base 0 del record
bool ITannit::SerializeRecord(char* record, int pos)
{
	int i = 0;
	int size;
	TannitQueryResult* qres = m_current;
	TannitRecord* rec = qres->recPtr;

	while (i < pos)
	{
		if (rec != NULL)
			rec = rec->next;
		else
			return FALSE;
		i++;
	}
	
	size = 0;
	for (i = 0;	i < qres->colsNumber; i++)
	{
		if (rec->row[i] == 0)
    {
      sprintf(record + size,"%c", ITX_APPC_FIELD_SEP);
      size += 1;
    }    
    else
    {
      sprintf(record + size,"%s%c", rec->row[i], ITX_APPC_FIELD_SEP);
		  size = size + strlen(rec->row[i]) + 1;
    }
	}

	// rimuovo la coda se necessario
	i = 0;
	while (record[(size - 1) - i - 1] == ITX_APPC_FIELD_SEP)
	{
		record[(size - 1) - i] = 0;
		i++;
	}

  // aggiungo il terminatore di record
  size = strlen(record);
  record[size] = ITX_APPC_RECORD_SEP;
  record[size + 1] = 0;

	return TRUE;
}

// row e field sono espressi in base 1.
char* ITannit::GetValue(char* name, int row, int field)
{
  char* value = NULL;
 	TannitQueryResult* qres = NULL;
  int i;
  TannitRecord* record;

	if ((qres = FindEntry(name)) != NULL)
	{
    if (row <= qres->totalRows)
    {
      record = qres->recPtr;
      for (i = 1; i < row; i++)
        record = record->next;
      
      if (field <= qres->colsNumber)
        value = record->row[field - 1];
    }
  }  

  return value;
}

// determina la posizione di una colonna di nome fieldname in base 1.
// un valore di ritorno di 0 indica che non � stato trovata.
int ITannit::GetColPos(TannitQueryResult* qres, char* fieldname)
{
  int icol = 0;

  while (icol < qres->colsNumber)
  {
    if (strcmp(qres->queryHeader[icol].name, fieldname) == 0)
      break;
    icol++;
  }
  if (icol == qres->colsNumber)
  	icol = 0;
	else
    icol++;

  return icol;
}


TannitRecord* ITannit::GetRecordNumber(char* table, int row)
{
 	TannitQueryResult* qres = NULL;
  int i;
  TannitRecord* record = NULL;

	if ((qres = FindEntry(table)) != NULL)
	{
    if (row <= qres->totalRows)
    {
      record = qres->recPtr;
      for (i = 1; i < row; i++)
        record = record->next;
    }
  }

  return record;
}

TannitRecord* ITannit::GetRecordNumber(TannitQueryResult* qres, int row)
{
  int i;
  TannitRecord* record = NULL;

  if (row <= qres->totalRows)
  {
    record = qres->recPtr;
    for (i = 1; i < row; i++)
      record = record->next;
  }

  return record;
}

// row e field sono espressi in base 1.
void ITannit::SetValue(char* name, int row, int field, char* entry)
{
 	TannitQueryResult* qres = NULL;
  int i;
  TannitRecord* record;

	if ((qres = FindEntry(name)) != NULL)
	{
    if (row <= qres->totalRows)
    {
      record = qres->recPtr;
      for (i = 1; i < row; i++)
        record = record->next;
      
      if (field <= qres->colsNumber)
        record->row[field - 1] = entry;
    }
  }
}

bool ITannit::SkipSQLRecords(itxSQLConnection* psqlconn, int start, int end)
{
  if (end < start) 
    return TRUE;
  return psqlconn->SkipRecords(start, end);
}

bool ITannit::MoreResultSQL(itxSQLConnection* psqlconn)
{
  return psqlconn->MoreResults();  
}

bool ITannit::ExecuteSQL(itxSQLConnection* psqlconn, char* stm)
{
  return psqlconn->Execute(stm);
}

short int ITannit::GetSQLCols(itxSQLConnection* psqlconn)
{
  return psqlconn->m_cols;
}

bool ITannit::GetSQLColInfo(itxSQLConnection* psqlconn, short int col, char* name, short* type, long int* size)
{
  return psqlconn->GetColInfo(col, name, type, size);
}

bool ITannit::SelectFromSQL(itxSQLConnection* psqlconn, char* queryname, char* query, int firstRec, int recsToStore)
{
	TannitQueryResult* qres = NULL;
	long int counter, actualCol;
  int recordsNumber = 0;
  bool result = TRUE;

  // allocazione ed inizializzazione di una nuova TannitQueryResult con le informazioni 
  // del corrispondente RecordSet su SQL Server
	if ((qres = AllocateNewEntry(psqlconn, queryname, query, firstRec, recsToStore)) != NULL)
  {
    if (SkipSQLRecords(psqlconn, 1, qres->startingRow - 1))
    {
      recordsNumber = qres->startingRow + qres->rowsToStore - 1; 
      for (counter = qres->startingRow; counter <= recordsNumber; counter++)
      {
        if (AllocateNewRecord(qres))
        {
          for (actualCol = 0; actualCol < qres->colsNumber; actualCol++)
          {
            AllocateFieldSpace(qres, qres->queryHeader[actualCol].colummnSize, actualCol);
            if (!psqlconn->BindCol(actualCol, (qres->current_record)->row[actualCol], 
                               qres->queryHeader[actualCol].colummnSize + 1))  
              break;
          }
          result = psqlconn->Fetch();

          if (!result)
            break;
          else
            (qres->totalRows)++;
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

  result = psqlconn->FreeStmt();
  
  return result;
}

int ITannit::ExecuteQuery(itxSQLConnection* psqlconn, char* queryname, char* query, int firstRec, int recsToStore)
{
	TannitQueryResult* qres = NULL;
	short int actualCol, colsNumber;
	TannitColumnHeader* colummnHead;
  bool moreresult = TRUE;
  int  resultsetnum = 0;
	long int counter;
  int recordsNumber = 0;
  bool result;

  if (ExecuteSQL(psqlconn, query))
  {
    while (moreresult)
    {
      colsNumber = GetSQLCols(psqlconn);
      if (colsNumber != 0)
      {
        qres = NULL;
	      qres = (TannitQueryResult*) calloc(1, sizeof(TannitQueryResult));
	      if (qres != NULL)
	      {
		      colummnHead = (TannitColumnHeader*) calloc(colsNumber, sizeof(TannitColumnHeader));
          if (colummnHead != NULL)
          {
		        qres->queryHeader = colummnHead;
		        qres->colsNumber = colsNumber;

            if (resultsetnum == 0)
    	        strcpy(qres->id, queryname);
            else
              sprintf(qres->id, "%s%d", queryname, resultsetnum);

            resultsetnum++;

            if (firstRec < STARTING_ROW) firstRec = STARTING_ROW;
            if (recsToStore < 1)	recsToStore = ROWS_TO_STORE;
            qres->startingRow = firstRec;
            qres->rowsToStore = recsToStore;
            qres->actualRow = STARTING_ROW;
            qres->totalRows = 0;

		        for (actualCol = 0; actualCol < colsNumber; actualCol++)
              GetSQLColInfo(psqlconn, actualCol, &(qres->queryHeader[actualCol].name[0]),
                                               &(qres->queryHeader[actualCol].sqlDataType),
                                               &(qres->queryHeader[actualCol].colummnSize));

            Store(qres);

            if (SkipSQLRecords(psqlconn, 1, qres->startingRow - 1))
            {
              recordsNumber = qres->startingRow + qres->rowsToStore - 1; 
              for (counter = qres->startingRow; counter <= recordsNumber; counter++)
              {
                if (AllocateNewRecord(qres))
                {
                  for (actualCol = 0; actualCol < qres->colsNumber; actualCol++)
                  {
                    AllocateFieldSpace(qres, qres->queryHeader[actualCol].colummnSize, actualCol);
                    if (!psqlconn->BindCol(actualCol, (qres->current_record)->row[actualCol], 
                                       qres->queryHeader[actualCol].colummnSize + 1))  
                      break;
                  }
                  result = psqlconn->Fetch();

                  if (!result)
                    break;
                  else
                    (qres->totalRows)++;
                }
                else
                {
                  resultsetnum = ITXFAILED;
                  break;
                }
              }
            }
            else
              resultsetnum = ITXFAILED;
          }
          else
            FREE(qres);
        }
	    } // if (colsNumber != 0)
      moreresult = MoreResultSQL(psqlconn);
    } // while (moreresult)
  }
  else
    resultsetnum = ITXFAILED;

  return resultsetnum;
}