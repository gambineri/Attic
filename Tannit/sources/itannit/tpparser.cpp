/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

// AITECSA s.r.l.   1999
//    filename:     tpparser.cpp
//    description:  itannit AS400 interface library
//                  AS400 transaction program packet parsing
//    project:      ASWEB

#ifndef __ITX_TP_PARSER_CPP__
#define __ITX_TP_PARSER_CPP__
#endif

#include <strstrea.h>
#include <string.h>
#include <time.h>

#include "itxtypes.h"
#include "itannitc.h"


void ITannit_SubStr(char* source, char* destination, int maxlength, char* tag, char* newtag)
{
  char* src = source;
  char* src_cursor = source;
  char* dst = destination;
  
  unsigned long chunk = 0;
  unsigned long taglen = strlen(tag);
  unsigned long ntglen = strlen(newtag);

  memset(destination, '\0', maxlength);

  while ((src_cursor = strstr(src, tag)) != NULL)
  {
    chunk = (unsigned long) src_cursor - (unsigned long) src;
    strncpy(dst, src, chunk);
    dst += chunk;
    src += chunk;
    strncpy(dst, newtag, ntglen);
    dst += ntglen;
    src += taglen;
  }

  strcpy(dst, src);
}

char* ITannit_LTrim(char* source)
{
  int i = 0;
  char* p = source;

  while (p[i] == ' ') i++;
  return p+i;
}

char* ITannit_RTrim(char* source)
{
  int i = strlen(source);
  char* p = source;

  while (p[i-1] == ' ')
  {
    p[i-1] = '\0';
    i--;
  }
  return source;
}

char* ITannit_Trim(char* source)
{
  ITannit_RTrim(source);
  return ITannit_LTrim(source); 
}

void ITannit_EBCDtoASCII(PTannitQueryResult qres)
{
  int i;
	TannitRecord* rec = qres->recPtr;

	while (rec != NULL)
  {
	  for (i = 0;	i < qres->colsNumber; i++);
//      itxEBCDToASCII(rec->row[i]);

  	rec = rec->next;
	}	
}

// Si intende che la stringa puntata da tp sia gi� stata assemblata e 
// ripulita della parte intestazione e del ITX_APPC_ENDINFO_SEP.
// Gli argomenti fromrecord e numrecord non vengono utilizzati per il momento.
void ITannit_ParsTP(char* tablename, char* tp, int fromrecord, int numrecord, FILE* fp,
                    char recsep, char fieldsep)
{
  istrstream str_TP(tp);
  char record[ITX_APPC_MAX_RECORD_LENGTH];

  while(!str_TP.eof())
  {
    str_TP.getline(record, ITX_APPC_MAX_RECORD_LENGTH, recsep);
		if (!ISNULL(record))
			if (ITannit_ParsRecord(tablename, record, fp, fieldsep) == ITXFAILED)
				ITannit_ParserLog(fp, "RECORD", record);
  }
}

int ITannit_ParsRecord(char* tablename, char* record, FILE* fp, char fieldsep)
{
  istrstream str_REC(record);
  
  char stoken[ITX_APPC_MAX_FIELD_LENGTH];
  char* token;

  bool record_head = TRUE;
	TannitQueryResult* qres = NULL;
	int field_count = 0;


  while(!str_REC.eof())
  {
    str_REC.getline(stoken, ITX_APPC_MAX_FIELD_LENGTH, fieldsep);
    token = ITannit_Trim(stoken);

		if (record_head)
		{
      if (tablename == NULL)
        tablename = token;

      if ((qres = ITannit_DBStructure(tablename)) == NULL)
				return ITXFAILED;
							
			record_head = FALSE;
		}
		if (field_count < qres->colsNumber)
		{
			if ((field_count = ITannit_StoreToken(qres, token, field_count)) == ITXFAILED)
			{
				ITannit_RemoveCurrentRec(qres);
				ITannit_ParserLog(fp, "FIELD", token);
				return ITXFAILED;
			}
		}
	}

	while (field_count < qres->colsNumber)
	{
		if ((field_count = ITannit_StoreToken(qres, "", field_count)) == ITXFAILED)
		{
			ITannit_RemoveCurrentRec(qres);
			ITannit_ParserLog(fp, "FIELD", token);
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

void ITannit_ParserLog(FILE* fp, char* des, char* token)
{
	time_t tm;

  if (fp == NULL) return;

	time(&tm);   
  fprintf(fp, "\n");
	fprintf(fp, " %s", ctime(&tm));
	fprintf(fp, " ERROR IN PARSING %s :\n", des);
	fprintf(fp, " ----> %s\n", token);
}
