/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

// AITECSA s.r.l.   2000
//    filename:     tqrext.cpp
//    description:  tannitqueryresult manipolation functions from
//					        client inputs and various tannit template command extensions
//    project:      tannit

#ifndef __ITX_TQREXT_CPP__
#define __ITX_TQRTEXT_CPP__
#endif

#include "tannit.h"
#include "itannitc.h"
#include "extVars.h"
#include "itxtypes.h"

#define ECHO_KEYWORD    "echo"
#define TRAK_KEYWORD    "trak"
#define TQR_TABLE_SEP   "__"
#define TQR_FIELD_SEP   "_"

extern cgiFormEntry *cgiFormEntryFirst;

void redirectExit(char* filename)
{
	char * tplStr, *outputStr;

	if (filename != NULL)
	{
		bufferTpl(&tplStr, filename, TplDir);
		procTplData(tplStr, cgiOut, &outputStr);
		//fflush(cgiOut);
	}
	EXIT(1);
}

/*************************************************************************************
NOME			:	StoreTQR
Categoria	: Gasp Command: StoreTQR(nomeTQR,numCols,separatori,valBuffer)
attivita'	: permette di creare (se necessario) una nuova TQR ed inserirvi i valori
            contenuti nel buffer.
						- nomeTQR		: nome del TQR da inserire
						- numCols		: numero delle colonne
						- separatori: separatori di campo e di record
						- valBuffer	: buffer dei valori con separatori
*************************************************************************************/
void* StoreTQR(int vuoto1, char *vuoto2, char * inputStr)
{
	char* tqrname;
  char* sep;
  char* buffer;
  char* nfields;
  char fieldsep, recsep;
  itxString istr;

  if(!pickPar(inputStr, 1, &tqrname	))	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 2, &nfields	))	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 3, &sep			))	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 4, &buffer	))	return(PARAM_NOT_FOUND_MSG);

  /**/if(usedbg){fprintf(debug, "StoreTQR buffer: %s\n", buffer);fflush(debug);}  
  istr.InBlinder(&buffer, '\x27');
  /**/if(usedbg){fprintf(debug, "StoreTQR buffer: %s\n", buffer);fflush(debug);}  

  fieldsep = *(sep);
  recsep = *((char*)(sep+1));
  ITannit_Create(QueryResultSet, &QueryCounter);
  ITannit_NewEntry(tqrname, atoi(nfields));
  ITannit_ParsTP(tqrname, buffer, 0, 0, debug, recsep, fieldsep);
  ITannit_Destroy();

  return 0;
}

/*************************************************************************************
Categoria	: Gasp Command: TQRInsert
attivita'	: trasferisce il TQR indicato nel database.
par 1			: nome del TQR da inserire
par 2			: nome della tabella target
*************************************************************************************/
void* TQRInsert(int vuoto1, char *vuoto2, char * inputStr)
{
	char* tqrname;
  char* tablename;

  if(!pickPar(inputStr, 1, &tqrname))	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 2, &tablename))	return(PARAM_NOT_FOUND_MSG);

  ITannit_Create(QueryResultSet, &QueryCounter);

  ITannit_FillTQRColsInfoFromSQL(Odbcdsn, Odbcuid, Odbcpwd, tqrname, tablename);
  
  ITannit_ConnectSQL(Odbcdsn, Odbcuid, Odbcpwd);
  ITannit_BulkInsertSQL(debug, tablename, tqrname, NULL);
  ITannit_DisconnectSQL();

  ITannit_Destroy();

  return 0;
}

/*************************************************************************************
Categoria	: Gasp Command: getP00 (ASWEB Specific)
attivita'	: recupera i dati di connessione utente dal database.
par 1			: identificativo dell'utente
*************************************************************************************/
void *getP00(int vuoto1, char *vuoto2, char * inputStr)
{
	char* username;

  if(!pickPar(inputStr, 1, &username))	return(PARAM_NOT_FOUND_MSG);

  ITannit_Create(QueryResultSet, &QueryCounter);

//  ITannit_GetP00FromSQL(Odbcdsn, Odbcuid, Odbcpwd, username, debug);
  
  ITannit_Destroy();

  return 0;
}

/*************************************************************************************
Categoria	: Gasp Command: fillmsk
attivita'	: scandisce i parametri di Form alla ricerca della maschera del tipo
              TQR_TABLE_SEPtable_nameTQR_FIELD_SEPfield_name
            ha tre modalit� di lavoro a seconda dei parametri passati:
              NessunParametro : inserisce nel TQR omonimo il valore associato al field
              trak : come sopra ma ritorna la lista separata da virgole dei TQR modificati
              echo : riinvia sul cgiOut gli stessi parametri ripristinando i caratteri escape
par 1			: null, trak o echo
*************************************************************************************/
void *fillmsk(int vuoto1, char *vuoto2, char * inputStr)
{
	cgiFormEntry *e;
  char table[32];
  char field[32];
  static char trak[1024];
  char* value;
  char* ptable;
  char* pfield;
  TannitQueryResult* qres = NULL;
  int fieldnum = 0;
  char* echoflag = NULL;
  int retval = 0;
  itxString istr;

  /**/if(usedbg){fprintf(debug, "fillmsk---START\n");fflush(debug);}
  retval = pickPar(inputStr, 1, &echoflag);
  /**/if(usedbg){if (echoflag != NULL){fprintf(debug, "esamining echoflag: %s\n", echoflag);fflush(debug);}}

  if(retval == PARAM_NOT_FOUND)
  {
    ITannit_Create(QueryResultSet, &QueryCounter);

    e = cgiFormEntryFirst;
	  while (e != NULL) 
    {
      memset(table, '\0', 32);
      memset(field, '\0', 32);

  /**/if(usedbg){fprintf(debug, "esamining cgiFormEntry: %s\n", e->attr);fflush(debug);}
      if ((ptable = strstr(e->attr, TQR_TABLE_SEP)) == e->attr)
      {
  /**/if(usedbg){fprintf(debug, "processing cgiFormEntry: %s\n", e->attr);fflush(debug);}
        value = e->value;
        if ((pfield = (char*)((strstr((char*)(ptable + 2), TQR_FIELD_SEP)) + 1)) != 0)
        {
          strcpy(field, pfield);
          memcpy(table, (char*)(ptable + 2), (size_t)((size_t)pfield - (size_t)ptable - 3));

    /**/if(usedbg){fprintf(debug, "         table: %s\n", table);fflush(debug);}
    /**/if(usedbg){fprintf(debug, "         field: %s\n", field);fflush(debug);}
          fieldnum = 0;
          if ((fieldnum = ITannit_GetTPFields(table)) != 0)
          {
            if ((qres = ITannit_NewEntry(table, fieldnum)) != NULL)
            {
              _strupr(value);
              ITannit_StoreValue(qres, field, value);
            }
          }
        }
      }
		  e = e->next;
	  }
  
//**/if(usedbg){ ITannit_Dump(debug);}
    ITannit_Destroy();
    return 0;
  }
  
/**/if(usedbg){if (echoflag != NULL){fprintf(debug, "esamining echoflag: %s\n", echoflag);fflush(debug);}}
  if (ISEQUAL(ECHO_KEYWORD, echoflag))
  {
    e = cgiFormEntryFirst;
	  while (e != NULL) 
    {
      memset(table, '\0', 32);
      memset(field, '\0', 32);

  /**/if(usedbg){fprintf(debug, "esamining cgiFormEntry: %s\n", e->attr);fflush(debug);}
      if ((ptable = strstr(e->attr, TQR_TABLE_SEP)) == e->attr)
      {
  /**/if(usedbg){fprintf(debug, "processing cgiFormEntry: %s\n", e->attr);fflush(debug);}
        value = e->value;
        if ((pfield = (char*)((strstr((char*)(ptable + 2), TQR_FIELD_SEP)) + 1)) != 0)
        {
          strcpy(field, pfield);
          memcpy(table, (char*)(ptable + 2), (size_t)((size_t)pfield - (size_t)ptable - 3));

    /**/if(usedbg){fprintf(debug, "         table: %s\n", table);fflush(debug);}
    /**/if(usedbg){fprintf(debug, "         field: %s\n", field);fflush(debug);}

          char* rvalue;
          istr.EscapeChars(value, &rvalue);
          fprintf(cgiOut, "&%s%s%s%s=%s", TQR_TABLE_SEP, table, TQR_FIELD_SEP, field, rvalue);
        }
      }
		  e = e->next;
	  }
    return 0;
  }

  if (ISEQUAL(TRAK_KEYWORD, echoflag))
  {
    memset(trak, '\0', 1024);
    ITannit_Create(QueryResultSet, &QueryCounter);

    e = cgiFormEntryFirst;
	  while (e != NULL) 
    {
      memset(table, '\0', 32);
      memset(field, '\0', 32);

  /**/if(usedbg){fprintf(debug, "esamining cgiFormEntry: %s\n", e->attr);fflush(debug);}
      if ((ptable = strstr(e->attr, TQR_TABLE_SEP)) == e->attr)
      {
  /**/if(usedbg){fprintf(debug, "processing cgiFormEntry: %s\n", e->attr);fflush(debug);}
        value = e->value;
        if ((pfield = (char*)((strstr((char*)(ptable + 2), TQR_FIELD_SEP)) + 1)) != 0)
        {
          strcpy(field, pfield);
          memcpy(table, (char*)(ptable + 2), (size_t)((size_t)pfield - (size_t)ptable - 3));

    /**/if(usedbg){fprintf(debug, "         table: %s\n", table);fflush(debug);}
    /**/if(usedbg){fprintf(debug, "         field: %s\n", field);fflush(debug);}
          
          if (strstr(trak, table) == 0)
          {
            strcat(trak, table);
            strcat(trak, ",");
          }

          fieldnum = 0;
          if ((fieldnum = ITannit_GetTPFields(table)) != 0)
          {
            if ((qres = ITannit_NewEntry(table, fieldnum)) != NULL)
            {
              _strupr(value);
              ITannit_StoreValue(qres, field, value);
            }
          }
        }
      }
		  e = e->next;
	  }
    trak[strlen(trak) - 1] = 0;
//**/if(usedbg){ ITannit_Dump(debug);}
    ITannit_Destroy();
    return trak;
  }

  return UNABLE_TO_WRITE_VAR;
}

/*************************************************************************************
Categoria	: Gasp Command: *tunnel
attivita'	: scandisce i parametri di Form alla ricerca della maschera del tipo                
                  PRM_TUNNEL_TAGparameter_name
						e rinvia sul cgiOut gli stessi parametri in formato di url
						&prm1_name=prm_value&prm2_name=prm_value&prm3_name=prm_value&...
*************************************************************************************/
void *prmTunnel(int vuoto1, char *vuoto2, char * inputStr)
{
	char *separator;
	cgiFormEntry *e;
	static char appo[256];
	char * retval;
	char * maskedPar[512];
  itxString istr;

/**/if(usedbg){fprintf(debug, "TUNNEL: START\n");fflush(debug);}

  if ((retval = (char*)calloc(4096, sizeof(char))) == NULL)
    return NULL;
  
	if(pickPar(inputStr, 1, &separator))
  {
		istr.InBlinder(&separator, BLINDER);

		e = cgiFormEntryFirst;
		while (e != NULL) 
		{
			if ( strstr(e->attr, PRM_TUNNEL_TAG) != 0)
			{
				sprintf(appo, "%s%s", e->attr, separator);
				strcat(retval, appo);
			}
			e = e->next;
		}
	}
  else
  {
		int k = 0;
		e = cgiFormEntryFirst;
		while (e != NULL) 
		{
			if ( strstr(e->attr, PRM_TUNNEL_TAG) != 0)
			{
				istr.EscapeChars(e->value,&(maskedPar[k]));
				sprintf(appo, "&%s=%s", e->attr, maskedPar[k]);
				strcat(retval, appo);
			}
			k++;
			e = e->next;
		}
	}

/**/if(usedbg){fprintf(debug, "TUNNEL: END\n");fflush(debug);}
  return retval;
}

/*************************************************************************************
Categoria	: Gasp Command: exeTP (ASWEB Specific)
attivita'	: scandisce la inputStr per recuperare tutti i TQR con cui costruire il buffer
            da inviare ad AS400. Supporta un massimo di 20 TP.
par vars  : a parametri variabili. Nomi di altrettanti TQR.
*************************************************************************************/
void *exeTP(int vuoto1, char *vuoto2, char * inputStr)
{
  int retval = 0;
  int param = 1;
  char* tp[20];
	char*  returned_buffer = NULL;
  TannitQueryResult* qres = NULL;
  TannitQueryResult* p00 = NULL;
  TannitQueryResult* h00 = NULL;
	char * sempreY;

	sempreY = (char*)calloc(2, sizeof(char));
	sempreY[0]='Y';
	sempreY[1]='\0';

  memset(tp, 0, 20 * sizeof(char*));

  /**/if(usedbg){fprintf(debug, "exeTP---START\n");fflush(debug);}

#ifdef _ITX_APPC
  ITannit_CreateEnv(QueryResultSet, &QueryCounter, Llu, Rtp);

  while ((retval = pickPar(inputStr, param, &(tp[param-1]))) != PARAM_NOT_FOUND)
  {
  /**/if(usedbg){fprintf(debug, "%s\n", tp[param-1]);fflush(debug);}
  // original patch (it works only if H00 has benn loaded in memory)
    if (ISEQUAL(tp[param-1], "HBB") || ISEQUAL(tp[param-1], "HBA"))
    {
	    p00 = ITannit_FindQuery("P00");
      h00 = ITannit_FindQuery("H00");
      p00->recPtr->row[17] = sempreY;
//    p00->recPtr->row[17] = h00->recPtr->row[36];
		}
    param++;
  }

  if ((returned_buffer = ITannit_TPExecute(ITX_APPC_PC_REQ, 1, debug,
                              tp[0], tp[1], tp[2], tp[3], tp[4], tp[5], 
                              tp[6], tp[7], tp[8], tp[9], tp[10], tp[11],
                              tp[12], tp[13], tp[14], tp[15], tp[16], tp[17], 
                              tp[18], tp[19], NULL)) == NULL)
  {
    if ((qres = ITannit_NewEntry("ConnectErr", 3)) != NULL)
      ITannit_StoreValue(qres, "f1", "AS400");
  }
  ITannit_Destroy();
#endif  

  return 0;
}

/*************************************************************************************
Nome			:	setTQRValues
Categoria	: Gasp Command: tqrval (TQRname,recordToMod,field1Name,field1Val,...)
attivita'	: scandisce la inputStr per recuperare tutti i valori da inserire nel TQR 
						- TQRname			: nome del TQR
						- recordToMod :	posizione in base 1 del record da modificare
						- field1Name,field1Val,...:
														numero variabile di coppie di valori 'nome campo', 'valore'
*************************************************************************************/
void *setTQRValues(int vuoto1, char *vuoto2, char * inputStr)
{
  char* tqrname = NULL;
  char* trownumber = NULL;
  char* fieldname = NULL;
  char* fieldvalue = NULL;
  itxString istr;

  TannitQueryResult* qres = NULL;
  int   rownumber;
  int   numpar;

  /**/if(usedbg){fprintf(debug, "tqrval Started\n%s\n", inputStr);fflush(debug);}

  if(!pickPar(inputStr, 1, &tqrname	)) return(PARAM_NOT_FOUND_MSG);
  //**/if(usedbg){fprintf(debug, "FOUND %s\n", tqrname);fflush(debug);}
  if(!pickPar(inputStr, 2, &trownumber))	return(PARAM_NOT_FOUND_MSG);
  //**/if(usedbg){fprintf(debug, "%s\n", trownumber);fflush(debug);}
  if(!pickPar(inputStr, 3, &fieldname))	return(PARAM_NOT_FOUND_MSG);
  //**/if(usedbg){fprintf(debug, "%s\n", fieldname);fflush(debug);}
  if(!pickPar(inputStr, 4, &fieldvalue))	return(PARAM_NOT_FOUND_MSG);
  
	istr.InBlinder(&fieldvalue, BLINDER);

  /**/if(usedbg){fprintf(debug, "%s\n", fieldvalue);fflush(debug);}

  rownumber = atoi(trownumber);
  /**/if(usedbg){fprintf(debug, "%d\n", rownumber);fflush(debug);}

  ITannit_Create(QueryResultSet, &QueryCounter);

  if ((qres = ITannit_FindQuery(tqrname)) == NULL)
    return 0;

  ITannit_SetCurrentRecord(qres, rownumber); 
  ITannit_StoreValue(qres, fieldname, fieldvalue);
  
  numpar = 5;
  while ((pickPar(inputStr, numpar, &fieldname) == PARAM_FOUND) &&
         (pickPar(inputStr, numpar + 1, &fieldvalue) == PARAM_FOUND))
  {

		istr.InBlinder(&fieldvalue, BLINDER);
  //**/if(usedbg){fprintf(debug, "%s\n", fieldname);fflush(debug);}
  //**/if(usedbg){fprintf(debug, "%s\n", fieldvalue);fflush(debug);}
    ITannit_StoreValue(qres, fieldname, fieldvalue);
    numpar += 2;
  }
  ITannit_Destroy();

  return 0;
}

/*************************************************************************************
Categoria	: Gasp Command: *verCoo(OPZdeltaT)
attivita'	: verifica che il Cookie di tipo ITXCOO del client sia valido: ovvero non sia stato
            creato o aggiornato pi� di deltat secondi prima del momento del check.
OPZdeltaT : opzionale: se presente � il deltat da utilizzare, se non presente si prende il
            deltat del file di parametri e se non si trova si utilizza il deltat definito
            come ITXCOO_TIME_DELAY (10.000 secondi).
*************************************************************************************/
void *verCookie(int vuoto1, char *vuoto2, char * inputStr)
{
  int retval = 0;
  char* login = NULL;
  char* efile = NULL;
  char* strdeltat = NULL;
  char* cooName = NULL;
  char* ptime = NULL;
  char* plogin = NULL;
  char* pcoo = NULL;
	char* pcomma = NULL;
  char decoo[256];

  time_t deltat = 0;
	time_t now;
	time_t before;
	
	time(&now);

  memset(decoo, '\0', 256);

/**/if(usedbg){fprintf(debug, "verCookie---START\n");fflush(debug);}

	if(	!pickPar(	inputStr, 1, &cooName		)	)		return(PARAM_NOT_FOUND_MSG);
	if(	!pickPar(	inputStr, 2, &login			)	)		return(PARAM_NOT_FOUND_MSG);
	if(	!pickPar(	inputStr, 3, &efile     )	)		return(PARAM_NOT_FOUND_MSG);
  retval = pickPar(inputStr, 4, &strdeltat);

  deltat = ITXCOO_TIME_DELAY;
  if(retval == PARAM_NOT_FOUND)
  {
    if (!ISNULL(CooTimeDelay))
      deltat = atoi(CooTimeDelay);
  }
  else
    deltat = atoi(strdeltat);

  plogin = decoo;

/**/if(usedbg){fprintf(debug, "verCookie---cgiCookie:%s\n",cgiCookie);fflush(debug);}
  if ((pcoo = strstr(cgiCookie,cooName)) != 0)
	{
		pcoo = pcoo + strlen(cooName);
		pcomma = strstr(pcoo, ";");
		if (pcomma != 0)
		{
			*pcomma = '\0';
		}
	}
	
	
/**/if(usedbg){fprintf(debug, "verCookie---pcoo:%s\n",pcoo);fflush(debug);}
	tannitDecrypt(pcoo, decoo);
/**/if(usedbg){fprintf(debug, "verCookie---decoo:%s\n",decoo);fflush(debug);}
  if ((ptime = strstr(decoo,ITXCOO_SEP)) == 0)
	{
    redirectExit(efile);
  }
/**/if(usedbg){fprintf(debug, "verCookie---decoo:%s\n",decoo);fflush(debug);}
  *ptime = '\0';
/**/if(usedbg){fprintf(debug, "verCookie---decoo:%s\n",decoo);fflush(debug);}
  ptime++; 
/**/if(usedbg){fprintf(debug, "verCookie---decoo:%s\n",decoo);fflush(debug);}

  if (!ISEQUAL(login, plogin))
	{
    redirectExit(efile);
  }

  before = atoi(ptime);
  if (deltat < (now - before))
	{
		redirectExit(efile);
	}

  return 0;
}

/*************************************************************************************
Categoria	: Gasp Command: *now(OPZflag)
attivita'	: restituisce il numero di secondi passati dalla mezzanote del 1.1.1970
						(universal time); se � presente il parametro opzionale restituisce, a
						seconda del valore di quest'ultimo,
						d - il giorno corrente
						m - il mese corrente
						y - l'anno corrente
*************************************************************************************/
void *now(int vuoto1, char *vuoto2, char * inputStr)
{
	static char rsec[256];
	time_t tm;
	struct tm *today; 
	char* part;

	time(&tm);
	today = localtime(&tm);

	memset(rsec, '\0', 256);
	sprintf(rsec, "%ld", tm); 

	if(pickPar(inputStr, 1, &part))
	{
/**/if(usedbg){fprintf(debug, "NOW-part %s\n",part);fflush(debug);}
		if (ISEQUAL(part, "d"))
			strftime(rsec, 256,"%d", today); 
		else if (ISEQUAL(part, "m"))
			strftime(rsec, 256,"%m", today); 
		else if (ISEQUAL(part, "y"))
			strftime(rsec, 256,"%Y", today); 
	}

	return rsec;
}

/*************************************************************************************
Categoria	: Gasp Command: *pkdate(sdate)
attivita'	: torna la data senza caratteri '/' es:
						11/12/1999 diventa 11121999
*************************************************************************************/
void *pkdate(int vuoto1, char *vuoto2, char * inputStr)
{
  static char rsec[256];
	char dd[3];
	char mm[3];
	char yy[5];
	
	char* sdate;
	char* pos;
	int l;

  if(!pickPar(inputStr, 1, &sdate))	return(PARAM_NOT_FOUND_MSG);

	memset(rsec, '\0', 256);
	memset(dd, '\0', 3);
	memset(mm, '\0', 3);
	memset(yy, '\0', 5);
	memset(dd, '0', 2);
	memset(mm, '0', 2);
	
	//locate day
  if ((pos = strtok(sdate, "/")) == NULL) return rsec;
	if ((l = strlen(pos)) > 2) return rsec;
	memcpy(dd, pos + 2 - l, l);

	//locate month
  if ((pos = strtok(NULL, "/")) == NULL) return rsec;
	if ((l = strlen(pos)) > 2) return rsec;
	memcpy(mm, pos + 2 - l, l);

	//locate year
  if ((pos = strtok(NULL, "/")) == NULL) return rsec;
	if ((l = strlen(pos)) != 4) return rsec;
	memcpy(yy, pos, l);

	sprintf(rsec, "%s%s%s", dd, mm, yy);

	return rsec;
}

/*************************************************************************************
Categoria	: Gasp Command: exist
attivita'	: ritorna la true o false a seconda se esiste il TQR richiesto
*************************************************************************************/
void *exist(int vuoto1, char *vuoto2, char * inputStr)
{
	char* tqrname;
  TannitQueryResult* qres;

  if(!pickPar(inputStr, 1, &tqrname))	return(PARAM_NOT_FOUND_MSG);

  ITannit_Create(QueryResultSet, &QueryCounter);

  qres = ITannit_FindQuery(tqrname);

  ITannit_Destroy();

  if (qres == NULL) 
    return TANNIT_FALSE;
  else
    return TANNIT_TRUE;
}



/*************************************************************************************
Categoria	: Gasp Command: *fconn(escapeFile,login,lid)
attivita'	: prima connessione all'AS400 e caricamento dell'H00 su SQLServer;
						i TQR H00, P00 e listini vengono anche caricati in memoria.
						Effettua inoltre il controllo dell'accesso dell'utente: in caso di accesso
						non autorizzato si esce su escapeFile
*************************************************************************************/
void *firstConn(int vuoto1, char *vuoto2, char * inputStr)
{
	char *login, *lid, *escapeFile;	 
	TannitQueryResult *qerr =NULL;
	if(!pickPar(inputStr, 1, &escapeFile		)	)		redirectExit(ITX_MISINTER);
	if(!pickPar(inputStr, 2, &login					)	)		redirectExit(ITX_MISINTER);
	if(!pickPar(inputStr, 3, &lid						)	)		redirectExit(ITX_MISINTER);


#ifdef _ITX_APPC
  ITannit_CreateEnv(QueryResultSet, &QueryCounter, Llu, Rtp);
  if (!ITannit_FirstConn(Odbcdsn, Odbcuid, Odbcpwd, login, lid, debug))
  {
/**/if(usedbg){fprintf(debug, "firstConn; ITannit_FirstConn could not establish a valid connection\n");fflush(debug);}
		redirectExit(escapeFile);
    return 0;
  }
#endif

/**/if(usedbg){ITannit_Dump(debug);fflush(debug);}
  // controllo se FirstConn abbia generato un errore lato server
  qerr = ITannit_FindQuery("E00");
  if (qerr != 0)
  {
/**/if(usedbg){fprintf(debug, "firstConn; malicious error: exiting\n");fflush(debug);}
		redirectExit(escapeFile);
  }
  ITannit_Destroy();
	return 0;
}

/*************************************************************************************
Categoria	: Gasp Command: *ckcli(escapeFile,codice_cliente,codice_contratto)
attivita'	: convalida l'utente, in caso di errore ridireziona su escapeFile
*************************************************************************************/
void *ckcli(int vuoto1, char *vuoto2, char * inputStr)
{
	char *codCliente, *codContratto, *escapeFile;
	int errCode;
	char *ccli, *ccon;

	if(!pickPar(inputStr, 1, &escapeFile		)	)		redirectExit(ITX_MISINTER);
	if(!pickPar(inputStr, 2, &codCliente		)	)		redirectExit(escapeFile);
	if(!pickPar(inputStr, 3, &codContratto	)	)		redirectExit(escapeFile);

	ccli = storedData(1, "f4", "P00", &errCode);
	if ( errCode == ERR_COL_NOT_FOUND || errCode == ERR_QUERY_NOT_FOUND || errCode == ERR_VOID_QUERY_NAME	)
	{
/**/if(usedbg){fprintf(debug, "firstConn; login error 1: exiting\n");fflush(debug);}
		redirectExit(escapeFile);
	}

	ccon = storedData(1, "f5", "H00", &errCode);

	if ( errCode == ERR_COL_NOT_FOUND || errCode == ERR_QUERY_NOT_FOUND || errCode == ERR_VOID_QUERY_NAME	)
	{
/**/if(usedbg){fprintf(debug, "firstConn; login error 2: exiting\n");fflush(debug);}
		redirectExit(escapeFile);
	}

	if (!(ISEQUAL(ccli, codCliente) && ISEQUAL(ccon, codContratto)))
	{
/**/if(usedbg){fprintf(debug, "firstConn; login error 3: exiting\n");fflush(debug);}
		redirectExit(escapeFile);
	}

	// se tutti i controlli sono andati a buon fine si va avanti felici
	return 0;
}

/*************************************************************************************
Categoria	: Gasp Command: *fddate(31122009)
attivita'	: riformatta la data di esempio in: 31/12/2009
*************************************************************************************/
void *formatDimDate(int vuoto1, char *vuoto2, char * inputStr)
{
	char *dataGrezza;
	static char dataRaffinata[12];

	memset(dataRaffinata, 0, 12);

	pickPar(inputStr, 1, &dataGrezza);

	if (strlen(dataGrezza) == 8)
		sprintf(dataRaffinata, "%c%c/%c%c/%c%c%c%c",
			dataGrezza[0],
			dataGrezza[1],
			dataGrezza[2],
			dataGrezza[3],
			dataGrezza[4],
			dataGrezza[5],
			dataGrezza[6],
			dataGrezza[7]
			);
	else if (strlen(dataGrezza) == 7)
		sprintf(dataRaffinata, "%c/%c%c/%c%c%c%c",
			dataGrezza[0],
			dataGrezza[1],
			dataGrezza[2],
			dataGrezza[3],
			dataGrezza[4],
			dataGrezza[5],
			dataGrezza[6]
			);
	else if (strlen(dataGrezza) == 6)
		sprintf(dataRaffinata, "%c%c/%c%c%c%c",
			dataGrezza[0],
			dataGrezza[1],
			dataGrezza[2],
			dataGrezza[3],
			dataGrezza[4],
			dataGrezza[5]
			);
	else if (strlen(dataGrezza) == 5)
		sprintf(dataRaffinata, "%c/%c%c%c%c",
			dataGrezza[0],
			dataGrezza[1],
			dataGrezza[2],
			dataGrezza[3],
			dataGrezza[4]
			);

	return dataRaffinata;
}

/*************************************************************************************
Categoria	: Gasp Command: *fdcap(100)
attivita'	: riformatta il cap di esempio in: 00100
*************************************************************************************/
void *formatDimCap(int vuoto1, char *vuoto2, char * inputStr)
{
	char *capGrezzo;
	static char capRaffinato[6];

	memset(capRaffinato, 0, 6);

	pickPar(inputStr, 1, &capGrezzo);

	if (strlen(capGrezzo) == 0)
		sprintf(capRaffinato, "");
	else if (strlen(capGrezzo) == 1)
		sprintf(capRaffinato, "0000%c",
			capGrezzo[0]
		);
	else if (strlen(capGrezzo) == 2)
		sprintf(capRaffinato, "000%c%c",
			capGrezzo[0],
			capGrezzo[1]
		);
	else if (strlen(capGrezzo) == 3)
		sprintf(capRaffinato, "00%c%c%c",
			capGrezzo[0],
			capGrezzo[1],
			capGrezzo[2]
		);
	else if (strlen(capGrezzo) == 4)
		sprintf(capRaffinato, "0%c%c%c%c",
			capGrezzo[0],
			capGrezzo[1],
			capGrezzo[2],
			capGrezzo[3]
		);
	else if (strlen(capGrezzo) == 5)
		sprintf(capRaffinato, "%s", capGrezzo	);

	return capRaffinato;
}


/*************************************************************************************
Categoria	: Gasp Command: *fdnace(123)		->123
													*fdnace(1234)		->123.4
													*fdnace(12345)	->123.45
													*fdnace(123456)	->123.45.6
													*fdnace(1234567)->123.45.67
attivit�	: riformatta i codici NACE come negli esempi
*************************************************************************************/
void *formatDimNACE(int vuoto1, char *vuoto2, char * inputStr)
{
	char *naceGrezzo;
	static char naceRaffinato[11];

	memset(naceRaffinato, 0, 11);

	pickPar(inputStr, 1, &naceGrezzo);

	if (strlen(naceGrezzo) == 0)
		sprintf(naceRaffinato, "");
	else if (strlen(naceGrezzo) == 1)
		sprintf(naceRaffinato, "%c",
			naceGrezzo[0]
		);
	else if (strlen(naceGrezzo) == 2)
		sprintf(naceRaffinato, "%c%c",
			naceGrezzo[0],
			naceGrezzo[1]
		);
	else if (strlen(naceGrezzo) == 3)
		sprintf(naceRaffinato, "%c%c%c",
			naceGrezzo[0],
			naceGrezzo[1],
			naceGrezzo[2]
		);
	else if (strlen(naceGrezzo) == 4)
		sprintf(naceRaffinato, "%c%c%c.%c",
			naceGrezzo[0],
			naceGrezzo[1],
			naceGrezzo[2],
			naceGrezzo[3]
		);
	else if (strlen(naceGrezzo) == 5)
		sprintf(naceRaffinato, "%c%c%c.%c%c",
			naceGrezzo[0],
			naceGrezzo[1],
			naceGrezzo[2],
			naceGrezzo[3],
			naceGrezzo[4]
		);
	else if (strlen(naceGrezzo) == 6)
		sprintf(naceRaffinato, "%c%c%c.%c%c.%c",
			naceGrezzo[0],
			naceGrezzo[1],
			naceGrezzo[2],
			naceGrezzo[3],
			naceGrezzo[4],
			naceGrezzo[5]
		);
	else if (strlen(naceGrezzo) == 7)
		sprintf(naceRaffinato, "%c%c%c.%c%c.%c%c",
			naceGrezzo[0],
			naceGrezzo[1],
			naceGrezzo[2],
			naceGrezzo[3],
			naceGrezzo[4],
			naceGrezzo[5],
			naceGrezzo[6]
		);
	else if (strlen(naceGrezzo) == 8)
		sprintf(naceRaffinato, "%c%c%c.%c%c.%c%c%c",
			naceGrezzo[0],
			naceGrezzo[1],
			naceGrezzo[2],
			naceGrezzo[3],
			naceGrezzo[4],
			naceGrezzo[5],
			naceGrezzo[6],
			naceGrezzo[7]
		);

	return naceRaffinato;
}

/*************************************************************************************
Categoria	: Gasp Command: *splitREA(RE175873,1) oppure *splitREA(RE175873,2)
attivita'	: ritorna RE oppure  175873
*************************************************************************************/
void *splitREA(int vuoto1, char *vuoto2, char * inputStr)
{
	char *reaGrezzo, *trimTypeSt;
	static char reaRaffinato[16];
	int trimType = 0;

	memset(reaRaffinato, 0, 16);

	if(!pickPar(inputStr, 1, &reaGrezzo		)	)		redirectExit(ITX_MISINTER);
	if( pickPar(inputStr, 2, &trimTypeSt	)	)		trimType = atoi(trimTypeSt);

	if (trimType == 0)
		sprintf(reaRaffinato, "%s",	reaGrezzo);
	else if (trimType == 1)
		sprintf(reaRaffinato, "%c%c",	reaGrezzo[0],	reaGrezzo[1]);
	else if (trimType == 2)
		sprintf(reaRaffinato, "%s", &(reaGrezzo[2]) );
	else
		sprintf(reaRaffinato, "REA_FLAG_ERROR");

	return reaRaffinato;
}


/*************************************************************************************
Categoria	: Gasp Command: *dimDataMagic(dataGrezza,tqrName)
attivita'	: crea un TQR di nome tqrName con due campi: il primo con la data in formato
						grezzo (11121999) ed il secondo in formato classico (11/12/1999)
						se sono presenti pi� date concatenate aggiunge record al TQR
*************************************************************************************/
void *dimDataMagic(int vuoto1, char *vuoto2, char * inputStr)
{
	char *dataGrezza, *tqrName, *p;
	char pezzoDiData[9];
	char cData[11];
	unsigned int i = 0, len = 0;

	if(!pickPar(inputStr, 1, &dataGrezza		)	)		redirectExit(ITX_MISINTER);
	if(!pickPar(inputStr, 2, &tqrName				)	)		redirectExit(ITX_MISINTER);

	trimIt(dataGrezza);
  p = dataGrezza;	
  len = strlen(dataGrezza);

  if (len == 0 || (len % 8) != 0)
    return 0;

  ITannit_Create(QueryResultSet, &QueryCounter);

  QQres = ITannit_NewEntry(tqrName, 2);
  i = 0;
  while (i < len)
  {
		memset(pezzoDiData, 0, 9);
		memset(cData, 0, 11);
		memcpy(pezzoDiData, p, 8);
    memcpy(cData, (char*)formatDimDate(0,0,pezzoDiData), 10);
		ITannit_StoreRecord(tqrName, cData, pezzoDiData, NULL);
    p += 8;
    i += 8;
	}

  ITannit_Destroy();

	return 0;
}


/*************************************************************************************
Categoria	: Gasp Command: *sottra(sottraendo, sottrattore)
*************************************************************************************/
void *subtractIt(int vuoto1, char *vuoto2, char * inputStr)
{
	char *sottraendoSt, *sottrattoreSt;
	long int		risultato, sottraendo = 0, sottrattore = 0;
	static char result[64];

	if(!pickPar(inputStr, 1, &sottraendoSt		)	)		redirectExit(ITX_MISINTER);
	if( pickPar(inputStr, 2, &sottrattoreSt		)	)		sottrattore = atoi(sottrattoreSt);

	sottraendo = atoi(sottraendoSt);

	risultato = sottraendo - sottrattore;
	sprintf(result, "%d", risultato);

	return result;
}

/*************************************************************************************
Categoria	: Gasp Command: *sumf(addendo, addendo, OPZdecimali)
*************************************************************************************/
void *sumFloat(int vuoto1, char *vuoto2, char * inputStr)
{
	char *add1St, *add2St, *decimaliSt;
	int decimali=0;
	double risultato = 0.0, add1 = 0.0, add2 = 0.0;
	static char result[128];
	static char formato[16];

	sprintf(formato,"%%f");

	if(!pickPar(inputStr, 1, &add1St		)	)		redirectExit(ITX_MISINTER);
	if( pickPar(inputStr, 2, &add2St		)	)		add2 = atof(add2St);
	if( pickPar(inputStr, 3, &decimaliSt)	)		sprintf(formato,"%%.%sf",decimaliSt);

	add1 = atof(add1St);
	risultato = add1 + add2;
	sprintf(result, formato, risultato);

	return result;
}

/*************************************************************************************
Categoria	: Gasp Command: *divint(dividendo, divisore)
Note			: ritorna un intero approssimato.
*************************************************************************************/
void *divideInt(int vuoto1, char *vuoto2, char * inputStr)
{
	char *dividendoSt, *divisoreSt;
	long int	risultato = 0;
	double		risultatoFl, dividendo = 0.0, divisore = 1.0;
	static char result[128];

	if(!pickPar(inputStr, 1, &dividendoSt		)	)		redirectExit(ITX_MISINTER);
	if(!pickPar(inputStr, 2, &divisoreSt		)	)		redirectExit(ITX_MISINTER);

	if (strcmp(dividendoSt, "") == 0)
		return "";

	divisore = atof(divisoreSt);
	dividendo = atof(dividendoSt);

	if (divisore == 0.0)
		return "";
	
	risultatoFl = dividendo / divisore;
	risultato = (long int)(risultatoFl + 0.5);

	if (risultato == 0)
		return "";

	sprintf(result, "%d", risultato);

	return result;
}

/*************************************************************************************
Categoria	: Gasp Command: *parval(nomeParametro)
attivita'	: restituisce il valore del parametro nel file dei parametri con il
						nomeParametro (n.b.: solo i parametri previsti)
*************************************************************************************/
void *parval(int vuoto1, char *vuoto2, char * inputStr)
{
	char *nomeParametro;

	if(!pickPar(inputStr, 1, &nomeParametro		)	)		redirectExit(ITX_MISINTER);

	if (strcmp(nomeParametro, "PrepKeyTag") == 0)
		return PrepKeyTag;
	else if (strcmp(nomeParametro, "IMGDIR") == 0)
		return ImgDir;

	return "";
}


/*************************************************************************************
Categoria	: Gasp Command: *tgt(shapeTpl,targetTpl)
attivita'	: controlla il livello di elaborazione corrente (visualizzazione,
						pre-visializzazione, pre-processing) e restituisce delle stringe adatte
						ad un inserimento in url.
						nei tre casi:
						visualizzazione			- "tpl=targetTpl"
						pre-visializzazione	- "tpl=shapeTpl&tgt=targetTpl"
						pre-processing			- "tpl=targetTpl"
*************************************************************************************/
void *switchTplTgt(int vuoto1, char *vuoto2, char * inputStr)
{
	char *shapeTpl, *targetTpl;
	char	userPKey[TPL_NAME_LEN];
	static char	retString[512];
  itxString istr;

	memset(retString, 0, 512);

	if(!pickPar(inputStr, 1, &shapeTpl		)	)		redirectExit(ITX_MISINTER);
	if(!pickPar(inputStr, 2, &targetTpl		)	)		redirectExit(ITX_MISINTER);

	istr.InBlinder(&shapeTpl, BLINDER);
	istr.InBlinder(&targetTpl, BLINDER);

	if (cgiFormString(PrepKeyTag, userPKey, TPL_NAME_LEN) == cgiFormSuccess)
	{
		// se e' un preprocessing il template destinazione e' il target
		if (strcmp(userPKey, PrepropKey) == 0 )
		{
			sprintf(retString, "%s=%s", TPL_TAG, targetTpl);
		}
		// se e' un preview il template destinazione e' quello dinamico
		// con associato il target
		else if (strcmp(userPKey, PreviewKey) == 0 )
		{
			sprintf(retString, "%s=%s&%s=%s", TPL_TAG, shapeTpl, TGT_TAG, targetTpl);
		}
		// se e' una visualizzazione utente non dovrei essere qui
		// tuttavia invece di dare errore torno targetTpl
		else if (strcmp(userPKey, NormviewKey) == 0 )
		{
			sprintf(retString, "%s=%s", TPL_TAG, targetTpl);
		}
		else EXIT(0);
	}

	return(retString);
}

/*************************************************************************************
Categoria	: Gasp Command: filtTqr(source,field,value,destination)
attivita'	: filra i record del TQR source e li mette nel TQR destination. Il criterio
						di selezione � quello che rende uguale il campo field al valore value
*************************************************************************************/
void *filterTqr(int vuoto1, char *vuoto2, char * inputStr)
{
	char *source, *field, *value, *destination;

	if(!pickPar(inputStr, 1, &source		)	)		redirectExit(ITX_MISINTER);
	if(!pickPar(inputStr, 2, &field		)	)		redirectExit(ITX_MISINTER);
	if(!pickPar(inputStr, 3, &value		)	)		redirectExit(ITX_MISINTER);
	if(!pickPar(inputStr, 4, &destination		)	)		redirectExit(ITX_MISINTER);

  ITannit_Create(QueryResultSet, &QueryCounter);

  ITannit_NewTQRFrom(source, field, value, destination);

  ITannit_Destroy();

	return(0);
}

/*************************************************************************************
Categoria	: servizio
*************************************************************************************/
char* formatCurrency(char* num, int mant_len, int want_sep)
{
  const char* sep = ".";
  char*       ret = NULL;
  int         numlen = strlen(num);
  int         first_pt = 0;
  
  if (num == NULL || numlen == 0)
    return NULL;

  if ((ret = (char*)calloc(3 * ((numlen >= mant_len ? numlen : mant_len) + 1), 1)) == NULL)
    return NULL;

  if (strstr(num, ",") != NULL)
  {
    strcpy(ret, num);
    return ret;
  }
  
  if (numlen > mant_len)  //Il numero e` > 1 ?
  {
    if (numlen - mant_len > 3 && want_sep)  //Gestione separatore 3 cifre
    {
      first_pt = (numlen - mant_len) % 3;
      if (first_pt > 0)
      {
        memcpy(ret, num, first_pt);
        strcat(ret, sep);
      }

      for (int i = first_pt; i<numlen - mant_len; i+=3)
      {
        memcpy(ret + strlen(ret), num + i, 3);
        strcat(ret, sep);
      }
      if ((i = strlen(ret)) > 0)
        ret[i - 1] = 0;
      
      //Se c'e` parte decimale, metto la virgola e l'attacco
      if (mant_len > 0)
      {
        strcat(ret, ",");
        strcat(ret, num + numlen - mant_len);
      }
    }
    else
    {
      memcpy(ret, num, numlen - mant_len);
      strcat(ret, ",");
      strcat(ret, num + numlen - mant_len);
    }
  }
  else
  {
    strcat(ret, "0,");
    memset(ret + 2, '0', mant_len - numlen);
    strcat(ret, num);
  }

  if (ret[strlen(ret) - 1] == ',')
    ret[strlen(ret) - 1] = 0;

  return ret;
}



/*************************************************************************************
Categoria	: Gasp Command: *dfcur(source,decimali)
attivita'	: ritorna la valuta source formattata con le interpunzioni tra le migliaia
						ed il numero di decimali specificato
*************************************************************************************/
void *dimFormatCurrency(int vuoto1, char *vuoto2, char * inputStr)
{
	char *source, *decimaliSt;
	int		decimali = 0;

	if(!pickPar(inputStr, 1, &source		)	)		return "";
	if( pickPar(inputStr, 2, &decimaliSt)	)		decimali = atoi(decimaliSt);

	if(decimali < 0)
		return "";

	return(formatCurrency(source,decimali,1));
}

/*************************************************************************************
Categoria	: Gasp Command: *fdvia(FRZ) -> Frazione
attivit�	: estrae dalla tabella accessoria Vie il valore relativo all'etichetta di
						input	e lo ritorna come nell'esempio
*************************************************************************************/
void * formatDimVia(int vuoto1, char *vuoto2, char * inputStr)
{
	char	*codiceVia, *titoloVia;
	char	queryString[GET_PAR_VAL_LEN];
	int		errCode;

	if(	!pickPar(	inputStr, 1, &codiceVia		)	)		return ("");

	sprintf(queryString,"select DesIT from Vie	where Codice = '%s'",codiceVia);

	newAutoName();
	dbInterface(QueryLabel, queryString, 1, 1);
	titoloVia = storedData(1, "DesIT", QueryLabel, &errCode);

	if (titoloVia == 0)
		return (codiceVia);
	if (strcmp(titoloVia, "") == 0)
		return (codiceVia);

	return (titoloVia);
}

/*************************************************************************************
Categoria	: Gasp Command: *fdnaz(I) -> Nazione
attivit�	: estrae dalla tabella accessoria Nazioni il valore relativo all'etichetta di
						input	e lo ritorna come nell'esempio
*************************************************************************************/
void * formatDimNaz(int vuoto1, char *vuoto2, char * inputStr)
{
	char	*codiceNaz, *titoloNaz;
	char	queryString[GET_PAR_VAL_LEN];
	int		errCode;

	if(	!pickPar(	inputStr, 1, &codiceNaz		)	)		return ("");

	sprintf(queryString,"select DesIT from Nazioni where Codice = '%s'",codiceNaz);

	newAutoName();
	dbInterface(QueryLabel, queryString, 1, 1);
	titoloNaz = storedData(1, "DesIT", QueryLabel, &errCode);

	if (titoloNaz == 0)
		return (codiceNaz);
	if (strcmp(titoloNaz, "") == 0)
		return (codiceNaz);

	return (titoloNaz);
}

/*************************************************************************************
Categoria	: Gasp Command: *ucase(Pallino) -> PALLINO
attivit�	: trasforma in uppercase
*************************************************************************************/
void * uppercaseIt(int vuoto1, char *vuoto2, char * inputStr)
{
	char	*inputSt, *cursor;

	if(	!pickPar(	inputStr, 1, &inputSt)	)		return("");

	cursor = inputSt;

	while (*cursor)
	{
		if ((*cursor)>96 && (*cursor)<123)
			*cursor = (*cursor) - 32; 
		cursor++;
	}

	return (inputSt);
}

/*************************************************************************************
Categoria	: Gasp Command: *tplname(-1) -> CurrentTpl[TplNest-1]
attivit�	:	ritorna il nome del template corrente se il parametro � zero, altrimenti
						usa il parametro come offset per individuare il template relativo nella
						pila dei template
*************************************************************************************/
void *putTplName(int vuoto1, char *vuoto2, char * inputStr)
{
	char	*inputSt;
	int shift = 0;
	int index;

	if(	pickPar(	inputStr, 1, &inputSt)	)		shift = atoi(inputSt);

	index = TplNest + shift;

	if (!(index >= 0 && index < MAX_TPL_NESTS)) index = TplNest;

	return (CurrentTpl[index]);
}


/*************************************************************************************
Categoria	: Gasp Command: *utime(dd,mm,yyyy) -> universaltime
attivit�	:	trasforma data, mese e anno in universal time (secondi passati dalla
						mezzanotte del 1.1.1970)
*************************************************************************************/
void *uTime(int vuoto1, char *vuoto2, char * inputStr)
{
	char	*giorno = 0, *mese = 0, *anno = 0;
	struct tm datar;
	time_t datas;
	char * retVal = 0;

	if(!pickPar(inputStr, 1, &giorno	)	)		redirectExit(ITX_MISINTER);
	if(!pickPar(inputStr, 2, &mese		)	)		redirectExit(ITX_MISINTER);
	if(!pickPar(inputStr, 3, &anno		)	)		redirectExit(ITX_MISINTER);

	datar.tm_mday = atoi(giorno);
	datar.tm_mon	= atoi(mese);
	datar.tm_year = atoi(anno) - 1900;

	datas = mktime(&datar);

  if ((retVal = (char*)calloc(4096, sizeof(char))) == NULL)
    return NULL;

	sprintf(retVal, "%ld", datas);

	return retVal;
}


/*************************************************************************************
Categoria	: Gasp Command: *fdate(utime) = gg/mm/yyyy oppure *fdate(utime,d) = giorno
attivita'	: trasforma una stringa che rappresenta una universal time (secondi
						passati dalla mezzanotte del 1.1.1970) in formato normale oppure, se
						presente un secondo parametro di valore d,m,y, torna il giorno, o il mese, 
						o l'anno; come da esempio
*************************************************************************************/
void *uTimeToDMY(int vuoto1, char *vuoto2, char * inputStr)
{
	static char retVal[256];
	time_t tm;
	struct tm *dt; 
	char* universaltime = 0 , *tag = 0;

	memset(retVal, '\0', 256);

	if(!pickPar(inputStr, 1, &universaltime	)	)		redirectExit(ITX_MISINTER);

  tm = (time_t) atol(universaltime);
	dt = localtime(&tm);

	if( pickPar(inputStr, 2, &tag						)	)
	{
		if (ISEQUAL(tag, "d"))
			strftime(retVal, 256,"%d", dt); 
		else if (ISEQUAL(tag, "m"))
			strftime(retVal, 256,"%m", dt); 
		else if (ISEQUAL(tag, "y"))
			strftime(retVal, 256,"%Y", dt); 
	}
  else
    strftime(retVal, 256, "%d/%m/%Y", dt);

	return retVal;
}
