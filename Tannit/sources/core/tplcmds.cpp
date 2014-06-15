/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#include "tannit.h"
#include "itannitc.h"
#include "extVars.h"

int	Ciccio=1;

//
  
/*************************************************************************************
NOME			:isAlphaNum
Categoria	:servizio
attivita'	:controlla se il carattere in input e' un alfanumerico
return		:l'indice del carattere trovato in un ipotetica lista che comprende i dieci
					 simboli numerici, quindi l'alfabeto minuscolo e quindi l'alfabeto maiuscolo;
					 zero se il carattere non viene trovato.
*************************************************************************************/
int isAlphaNum(char testCh)
{
	int charKind = 0;
	if			((testCh > 47) && (testCh < 58))	charKind = testCh - 47;
	else if ((testCh > 64) && (testCh < 91))	charKind = testCh - 64 + 10;
	else if ((testCh > 96) && (testCh < 123))	charKind = testCh - 96 + 36;

	return charKind;
}

/*************************************************************************************
NOME			:reAlphaNum
Categoria	:servizio
attivita'	:inversa della funzione isAlphaNum;
					 cerca il carattere corrispondente all'intero di input considerando questo
					 come un indice in un ipotetica lista comprendente i dieci simboli numerici,
					 quindi l'alfabeto minuscolo e quindi l'alfabeto maiuscolo;
return		:il carattere in chiaro; non ho capito cosa fa quando non trova l'indice
					 si alluppa? boh
*************************************************************************************/
int reAlphaNum(char codedCh)
{
	int resCh = 0;
	while(1)
	{
		if			((codedCh > 0) && (codedCh < 11))		{resCh = codedCh + 47;			break;}
		else if ((codedCh > 10) && (codedCh < 37))	{resCh = codedCh + 64 - 10;	break;}
		else if ((codedCh > 36) && (codedCh < 63))	{resCh = codedCh + 96 - 36;	break;}
		else if (codedCh > 62)	codedCh = codedCh - 62;
		else break;
	}
	return resCh;
}


/*************************************************************************************
NOME			:getHide
stato			:NON MODIFICATA (incluso commento)
Categoria	:servizio
attivita'	:esegue la formattazione per la spedizione via get (cambia i caratteri
					 speciali con i caratteri flag);
					 attualmente e' attiva solo la sostituzione del carattere 'spazio';
					 si prevede che diventi una funzione associata ad un comando.
chiamante	:getValue
*************************************************************************************/
void getHide(char * stringa) {
	unsigned int i;
	for (i=0;i<(strlen(stringa));i++)
	{
		if			(*(stringa+i)==' ')  *(stringa+i)='+';
/*	else if (*(stringa+i)=='&')  *(stringa+i)='@';
		else if (*(stringa+i)=='%')  *(stringa+i)='^';
		else if (*(stringa+i)=='=')  *(stringa+i)='�';
*/
	}
}


/*************************************************************************************
NOME			:remBlindingChar
Categoria	:servizio
attivita'	:elimina i caratteri di delimitazione di stringa definiti nella define
					 BLINDER sostituendovi degli spazi; e' necessario alzare la guardia sullo
					 errore.
chiamante	:execQuery, execInsQuery;
*************************************************************************************/
void remBlindingChar(char * queryString)
{
	int found = 0;
	char * holdQStr;

	holdQStr = queryString;
	while (*(queryString) != 0)
	{
		if (*queryString == BLINDER)
		{
			*queryString = ' ';
			found = 1;
			break;
		}
		queryString++;
	}
	while (*(++queryString)!=0);
	while (queryString != holdQStr)
	{
		if (*queryString == BLINDER)
		{
			*queryString = ' ';
			found = found + 10;
			break;
		}
		queryString--;
	}
	if (found != 11)
	{
		fprintf(cgiOut, "%s<br>\n", ERR_REM_BLINDCH);
		EXIT(-11);
	}
}

/*************************************************************************************
NOME			:pickPar
Categoria	:servizio
attivita'	:invocata dalle funzioni corrispondenti ai comandi per estrarre il parametro
					 alla 'position'; legge la inputStr, ALLOCA returnStr adeguatamente e vi
					 scrive il parametro richiesto
return		:0 se parametro non trovato, 1 se trovato
*************************************************************************************/
int pickPar(char* inputStr, int position, char ** returnStr)
{
	char	*strCursor;
	int		parLen=0, parNum = 0;
	int		parsOffstArr[PARAM_NUMBER];
	int		i, searchPars = 1;

  // nel caso che non vi sia nessun parametro
  if ( inputStr == NULL )
    return(PARAM_NOT_FOUND);

  if ( strlen(inputStr) == 0 )
  	  return(PARAM_NOT_FOUND);

	position = position - 1;

	for (i = 0; i < PARAM_NUMBER; i++) parsOffstArr[i] = 0;

	// inizializzazione del cursore al primo carattere della stringa di input
	strCursor = inputStr;
	while (*strCursor)
	{
		if (*strCursor == BLINDER)
		{
			if(searchPars) searchPars = 0; else searchPars = 1;
		}
		if ( (searchPars) && (*strCursor == ',') )
		{
			parsOffstArr[++parNum] = strCursor - inputStr + 1;
		}
		strCursor++;
	}

  if (parNum > position)
	{
		parLen = parsOffstArr[position + 1] - parsOffstArr[position];
		if (NULL==(*returnStr = (char *) malloc( (parLen) * sizeof(char) ))) EXIT(MEMERR);
/**/if(alloctrc){fprintf(debug, "- - *returnStr:%d - - len:%d\n", *returnStr, (parLen) * sizeof(char) );fflush(debug);}

		//si copia il parametro sulla stringa di uscita
		strncpy(*returnStr, inputStr + parsOffstArr[position], parLen -1);
		(*returnStr)[parLen-1]=0;
	}
	else if (parNum == position)
	{
		parLen = strlen(inputStr) - parsOffstArr[position];
		if (NULL==(*returnStr = (char *) malloc( (parLen + 1) * sizeof(char) ))) EXIT(MEMERR);
/**/if(alloctrc){fprintf(debug, "- - *returnStr:%d - - len:%d\n", *returnStr, (parLen + 1) * sizeof(char) );fflush(debug);}
		strcpy(*returnStr, &inputStr[parsOffstArr[position]]);
	}
	else	//parametro non trovato
	{
		if (NULL==(*returnStr = (char *) malloc( sizeof(char) ))) EXIT(MEMERR);
/**/if(alloctrc){fprintf(debug, "- - *returnStr:%d - - len:%d\n", *returnStr, sizeof(char) );fflush(debug);}
		*returnStr[0]=0;
		return(PARAM_NOT_FOUND);
	}
  //**/if(usedbg){fprintf(debug, "PICKPAR RETURNING\n");fflush(debug);}
  //**/if(usedbg){fprintf(debug, "*returnStr: %s\n", *returnStr);fflush(debug);}
	return(PARAM_FOUND);
}

/*************************************************************************************
NOME			:writeWebUrl
Categoria	:Gasp Command
attivita'	:ritorna l'indirizzo di partenza del sito cosi' come specificato nel file di
					 inizializzazione (esempio "http://www.aitecsa.com")
*************************************************************************************/
void * writeWebUrl(int vuoto1,char *vuoto2, char * vuoto3)
{
	static char buffer[HOME_DIR_LEN];
	sprintf(buffer,"%s",WebUrl);
	return buffer;
}

/*************************************************************************************
NOME			:writeHomeDir
Categoria	:Gasp Command
attivita'	:ritorna la directory del sito relativa alla home page cosi' come specificato
					 nel file di inizializzazione (esempio "aspub")
*************************************************************************************/
void * writeHomeDir(int vuoto1,char *vuoto2, char * vuoto3)
{
	static char buffer[HOME_DIR_LEN];
	sprintf(buffer,"%s",WebHome);
	return buffer;
}

/*************************************************************************************
NOME			:writeImgDir
Categoria	:Gasp Command
attivita'	:ritorna la directory delle immagini relativa alla home page cosi' come
					 specificato nel file di inizializzazione (esempio "/img")
*************************************************************************************/
void * writeImgDir(int vuoto1,char *vuoto2, char * vuoto3)
{
	static char buffer[IMG_DIR_LEN];
	sprintf(buffer,"%s",ImgDir);
	return buffer;
}

/*************************************************************************************
NOME			:writeSSDir
Categoria	:Gasp Command - *SSDIR()
attivita'	:ritorna la directory degli style sheet relativa alla home page cosi' come
					 specificato nel file di inizializzazione (esempio "/style")
*************************************************************************************/
void * writeSSDir(int vuoto1,char *vuoto2, char * vuoto3)
{
	static char buffer[SS_DIR_LEN];
	sprintf(buffer,"%s",SSDir);
	return buffer;
}

/*************************************************************************************
NOME			:writeCgiPath
Categoria	:Gasp Command
attivita'	:ritorna l'url completo della applicazione cgi cosi' come specificato nel
					 file di inizializzazione
					 (es: "http://www.aitecsa.it/cgi-bin/tannit.exe")
*************************************************************************************/
void * writeCgiPath(int vuoto1,char *vuoto2, char * vuoto3)
{
	static char buffer[IMG_DIR_LEN];
	sprintf(buffer,"%s/%s/%s", WebUrl, CgiDir, CgiName);
	return buffer;
}

/*************************************************************************************
NOME			:writeCorrAppDir
Categoria	:Gasp Command
attivita'	:ritorna il parametro CrAppDir specificato nel file di inizializzazione
*************************************************************************************/
void * writeCorrAppDir(int vuoto1,char *vuoto2, char * vuoto3)
{
	static char buffer[PAR_VALUE_LEN];
	sprintf(buffer,"%s",CorrAppDir);
	return buffer;
}

/*************************************************************************************
NOME			:writeCorrFileName
Categoria	:Gasp Command
attivita'	:ritorna il parametro CrFileName specificato nel file di inizializzazione
*************************************************************************************/
void * writeCorrFileName(int vuoto1,char *vuoto2, char * vuoto3)
{
	static char buffer[PAR_VALUE_LEN];
	sprintf(buffer,"%s",CorrFileName);
	return buffer;
}

/*************************************************************************************
NOME			:execQuery
Categoria	:Gasp Command: exeQ
attivita'	:esegue una chiamata sql all'odbc di default specificato nel file di
					 inizializazione; gli eventuali risultati vengono messi in una struttura
					 rintracciabile per il nome della query
par 1			:nome della query (costituira' l'etichetta di identificazione dei risultati)
par 2			:query sql compresa tra doppi apici
par 3	opz	:primo record del resultset da memorizzare nella struttura (opzionale,
					 valore di default = STARTING_ROW, tipicamente 1)
par 4	opz :numero di record del resultset da memorizzare nella struttura (opzionale,
					 valore di default = a ROWS_TO_STORE, tipicamente 512)
*************************************************************************************/
void * execQuery(int vuoto1,char *vuoto2, char * inputStr)
{
	itxString refinedQString;
	char *queryName, *queryString, *firstRecord, *recsToStore;
	int firstRecInt = STARTING_ROW, recsToStoreInt = ROWS_TO_STORE;

/**/if(usedbg){fprintf(debug, "execQuery-inputStr:%s\n", inputStr);fflush(debug);}

	if(!pickPar(inputStr, 1, &queryName)   )	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 2, &queryString) )	return(PARAM_NOT_FOUND_MSG);
	if( pickPar(inputStr, 3, &firstRecord) )	firstRecInt = atoi(firstRecord);
	if( pickPar(inputStr, 4, &recsToStore) )	recsToStoreInt = atoi(recsToStore);

	remBlindingChar(queryString);
	refinedQString.Space(strlen(queryString));
	refinedQString = queryString;
	refinedQString.SubstituteSubString("\"\"", "\"");

/**/if(usedbg){fprintf(debug, "execQuery: sending query;%s\n", refinedQString.GetBuffer());fflush(debug);}
	dbInterface(queryName, refinedQString.GetBuffer(), firstRecInt, recsToStoreInt);

/**/if(usedbg){fprintf(debug, "execQuery: dati archiviati;\n");fflush(debug);}
	return 	0;
}


/*************************************************************************************
NOME			:execInsQuery
Categoria	:Gasp Command: exeIQ
attivita'	:simile ad execQuery aggiunge un recordset di nome 'C' + nome query che
					 contiene un record con un campo ("conta") che riporta il max(id) di un campo
					 della tabella in input
par 1			:nome della query (costituira' l'etichetta di identificazione dei risultati)
par 2			:query sql compresa tra doppi apici
par 3			:nome della tabella da investigare alla ricerca del valore massimo di un campo
par 4	opz :nome del campo da investigare (default = 'id')
note			:la funzione viene intesa di utilita' quando in seguito ad una query (ad 
					 esempio di inserimento) si vuole avere a disposizione un contatore per
					 monitorarne lo stato;
*************************************************************************************/
void * execInsQuery(int vuoto1,char *vuoto2, char * inputStr)
{
	char *queryName, *queryString, *targetTable, *idField;
	char tableId[ID_FIELD_LEN];
	char countName[64];
	char countQuery[256];

	strcpy(tableId, "id");

	if(!pickPar(inputStr, 1, &queryName)		)	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 2, &queryString)	)	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 3, &targetTable)	)	return(PARAM_NOT_FOUND_MSG);
	if( pickPar(inputStr, 4, &idField		)		)	strcpy(tableId, idField);

	remBlindingChar(queryString);
	dbInterface(queryName, queryString, 1, 1);

	sprintf(countName, "C%s", queryName);
	sprintf(countQuery, "SELECT max(%s) conta FROM %s", tableId, targetTable);

	dbInterface(countName, countQuery, 1, 1);
	return 	0;
}


/*************************************************************************************
NOME			:remQuery
Categoria	:Gasp Command: remQ
attivita'	:non operativa, in futuro deve rimuovere una query dalla lista dei resultset
					 o liberando la memoria o rinominando la query
par 1			:nome della query da fare fuori
*************************************************************************************/
void * remQuery(int vuoto1,char *vuoto2, char * inputStr)
{
	int		qIdx;
	char *queryName;

//**/if(usedbg){fprintf(debug, "remQuery: Starting;\n");fflush(debug);}
	if(!pickPar(inputStr, 1, &queryName)   )	return(PARAM_NOT_FOUND_MSG);

	qIdx = queryIndex( queryName, 0);
	strcpy( (QueryResultSet[qIdx]->id), "ertyu1234565" );

//	ITannit_Create(QueryResultSet, &QueryCounter);
///**/if(usedbg){ITannit_Dump(debug);fflush(debug);}
////	ITannit_RemoveQueries(queryName, NULL);
//  ITannit_Destroy();
///**/if(usedbg){fprintf(debug, "remQuery: 4;\n");fflush(debug);}
	return 	0;
}


/*************************************************************************************
NOME			:writeTotalRecords
Categoria	:Gasp Command: *totRecs(queryName)
attivita'	:restituisce numero di record della query;
par 1			:nome del resultset da identificare
*************************************************************************************/
void * writeTotalRecords(int vuoto1,char *vuoto2, char * inputStr)
{
	char *queryName;
	static char retVal[GET_PAR_VAL_LEN];
	int qIdx = 0;

	if(!pickPar(inputStr, 1, &queryName)	)	return(PARAM_NOT_FOUND_MSG);

//**/if(usedbg){fprintf(debug, "writeTotalRecords -----------------------------  queryName: %s\n", queryName);fflush(debug);}
	qIdx = queryIndex( queryName, 1 );
	if ( qIdx == -1)
	{
		return("0");
	}
//if(QQres !=NULL)/**/if(usedbg){fprintf(debug, "writeTotalRecords -----------------------------  totalRows: %d\n", QQres->totalRows);fflush(debug);}
//**/if(usedbg){fprintf(debug, "writeTotalRecords -----------------------------  qIdx: %d\n", qIdx);fflush(debug);}
	listTQRnames();
	sprintf(retVal, "%d",QueryResultSet[qIdx]->totalRows);

	return retVal;
}


/*************************************************************************************
NOME			:writeActualRec
Categoria	:Gasp Command: *actRec(queryName)
attivita'	:restituisce numero di record della query;
par 1			:nome del resultset da identificare
*************************************************************************************/
void * writeActualRec(int vuoto1,char *vuoto2, char * inputStr)
{
	char *queryName;
	static char retVal[GET_PAR_VAL_LEN];
	int qIdx = 0;

	if(!pickPar(inputStr, 1, &queryName)	)	return(PARAM_NOT_FOUND_MSG);

	qIdx = queryIndex( queryName, 0);

	sprintf(retVal, "%d",QueryResultSet[qIdx]->actualRow);

	return retVal;
}


/*************************************************************************************
NOME			:writeMaxRecords
Categoria	:Gasp Command: *maxRecs(queryName)
attivita'	:restituisce numero di record della query;
par 1			:nome del resultset da identificare
*************************************************************************************/
void * writeMaxRecords(int vuoto1,char *vuoto2, char * inputStr)
{
	char *queryName;
	static char retVal[GET_PAR_VAL_LEN];
	int qIdx = 0;

	if(!pickPar(inputStr, 1, &queryName)	)	return(PARAM_NOT_FOUND_MSG);

	qIdx = queryIndex( queryName, 0);

	sprintf(retVal, "%d",QueryResultSet[qIdx]->rowsToStore);

	return retVal;
}


/*************************************************************************************
NOME			:writeFirstRecord
Categoria	:Gasp Command: *firstRec(queryName)
attivita'	:restituisce il record di partenza che verra' memorizzato;
par 1			:nome del resultset da identificare
*************************************************************************************/
void * writeFirstRecord(int vuoto1,char *vuoto2, char * inputStr)
{
	char *queryName;
	static char retVal[GET_PAR_VAL_LEN];
	int qIdx = 0;

	if(!pickPar(inputStr, 1, &queryName)	)	return(PARAM_NOT_FOUND_MSG);

	qIdx = queryIndex( queryName, 0 );

	sprintf(retVal, "%d",QueryResultSet[qIdx]->startingRow);

	return retVal;
}


/*************************************************************************************
NOME			:moreRecsMsg
Categoria	:Gasp Command: *moreRecs(queryName, msg)
attivita'	:se il resulset ha piu' record di quelli memorizzati visualizza il messaggio
par 1			:nome del resultset da identificare
par 2			:messaggio
*************************************************************************************/
void * moreRecsMsg(int vuoto1,char *vuoto2, char * inputStr)
{
	char *queryName, *imfoMsg;
	int qIdx;

	if(!pickPar(inputStr, 1, &queryName)	)	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 2, &imfoMsg)		)	return(PARAM_NOT_FOUND_MSG);
	qIdx = queryIndex( queryName, 0 );

	if ( QueryResultSet[qIdx]->rowsToStore +  QueryResultSet[qIdx]->startingRow	< QueryResultSet[qIdx]->totalRows )
	{
		return (imfoMsg);
	}

	return 0;
}


/*************************************************************************************
NOME			:recVal
Categoria	:Gasp Command: recVal
attivita'	:restituisce il valore del campo specificato per il record corrente del
					 resultset specificato;
par 1			:nome del resultset da identificare
par 2			:nome del campo
par 3	opz	:offset rispetto al record corrente (es: offset = -1 restituisce il record
					 precedente); default = 0
*************************************************************************************/
void * recVal(int vuoto1,char *vuoto2, char * inputStr)
{
	char *queryName, *queryField, *recOffsetSt;
	int recOffset=0, qIdx;
	void * tempZ;
	int errCode = 0;

//**/if(usedbg){fprintf(debug, "recVal-STARTING\n");fflush(debug);}
	if(!pickPar(inputStr, 1, &queryName)	)	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 2, &queryField)	)	return(PARAM_NOT_FOUND_MSG);
	if( pickPar(inputStr, 3, &recOffsetSt))	recOffset = atoi(recOffsetSt);
	qIdx = queryIndex( queryName, 0 );

	tempZ = (void*) storedData(QueryResultSet[qIdx]->actualRow + recOffset, queryField, queryName, &errCode);//record corrente, nome del campo, nome della query

	return tempZ;
}


/*************************************************************************************
NOME			:recValCnd
Categoria	:Gasp Command: *recCnd(queryName, queryField, concatStr)
attivita'	:restituisce il valore del campo specificato (per il record corrente e del
					 resultset specificato) solo se il dato e' diverso da null;
					 se e' presente la stringa opzionale questa viene aggiunta in testa al campo
					 (sempre solo se il campo non e' nullo)
par 1			:nome del resultset da identificare
par 2			:nome del campo
par 3	opz	:stringa opzionale da concatenare
*************************************************************************************/
void * recValCnd(int vuoto1,char *vuoto2, char * inputStr)
{
	char *queryName, *queryField, *concatStr, *onTailSt;
	static char stringaVuota[1] = {'\0'};
	int recOffset=0, qIdx;
	char * retVal;
	char * tempZ;
	int errCode = 0, onTail = 0;

	if(!pickPar(inputStr, 1, &queryName	)	)	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 2, &queryField)	)	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 3, &concatStr	)	)	concatStr = stringaVuota;
	if( pickPar(inputStr, 4, &onTailSt	)	)	onTail = atoi(onTailSt);

/**/if(usedbg){fprintf(debug, "FUNCTION recValCnd; query:%s; - field:%s; \n", queryName, queryField);fflush(debug);}
	qIdx = queryIndex( queryName, 0 );

	tempZ = storedData(QueryResultSet[qIdx]->actualRow + recOffset, queryField, queryName, &errCode);

	if( tempZ == 0 )
	{
		retVal = (char*) malloc( 3 * sizeof(char)  );if (!retVal) EXIT(MEMERR);
		sprintf(retVal, " ");
		return (retVal);
	}
	if( ( strcmp(tempZ, DATA_VALUE_ON_ERROR) == 0 ) || ( strcmp(tempZ, "") == 0 )  )
	{
		retVal = (char*) malloc( 3 * sizeof(char)  );if (!retVal) EXIT(MEMERR);
		sprintf(retVal, " ");
		return (retVal);
	}

	retVal = (char*) malloc( (strlen(concatStr) + strlen(tempZ) + 2 )* sizeof(char)  );if (!retVal) EXIT(MEMERR);
/**/if(alloctrc){fprintf(debug, "- - retVal:%d - - len:%d\n", retVal, (strlen(concatStr) + strlen(tempZ) + 2 )* sizeof(char) );fflush(debug);}

	if(onTail == 0)
		sprintf(retVal, "%s%s", concatStr, tempZ);
	else if(onTail == 1)
		sprintf(retVal, "%s%s", tempZ, concatStr);
	else
		sprintf(retVal, "%s%s", concatStr, tempZ);

	return (void*) retVal;
}


/*************************************************************************************
NOME			:recValConf
Categoria	:Gasp Command: recSEL
attivita'	:analogo a recVal restituisce il valore del campo specificato per il record
					 corrente del resultset specificato; aggiunge la stringa " SELECTED" se il
					 dato e' uguale al dato determinato dalla seconda coppia di parametri
par 1			:nome del resultset da identificare
par 2			:nome del campo
par 3			:nome del resultset da identificare per il confronto; se il nome e' "get"
					 il dato di confronto viene cercato nella stringa di get
par 4			:nome del campo da utilizzare per il confronto
*************************************************************************************/
void * recValConf(int vuoto1,char *vuoto2, char * inputStr)
{
	char *queryName, *queryField, *cnfField, *cnfSrc;
	int qIdx, qIdx2;
	char *firstVal, *secVal, *retVal;
	int errCode = 0;
//**/if(usedbg){fprintf(debug, "recValConf-STARTED;\n");fflush(debug);}

	if(!pickPar(inputStr, 1, &queryName)	)	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 2, &queryField)	)	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 3, &cnfSrc)	)			return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 4, &cnfField)	)		return(PARAM_NOT_FOUND_MSG);

	qIdx = queryIndex( queryName, 0 );
	firstVal = storedData(QueryResultSet[qIdx]->actualRow, queryField, queryName, &errCode);
	if(!firstVal) 
    return " ";

	if (strcmp(cnfSrc,"get")==0)
	{
		secVal = (char*) malloc( GET_PAR_VAL_LEN * sizeof(char)  );
    if (!secVal) 
      EXIT(23);
/**/if(alloctrc){fprintf(debug, "- - secVal:%d - - len:%d\n", secVal, GET_PAR_VAL_LEN * sizeof(char) );fflush(debug);}
		if (cgiFormString( cnfField, secVal, 0) != cgiFormSuccess)
			sprintf(secVal, "%s", GET_PAR_VAL);		/*valore di default generico*/
//**/if(usedbg){fprintf(debug, "recValConf-secVal: %s;\n", secVal);fflush(debug);}
	}
	else
	{
		qIdx2 = queryIndex( cnfSrc, 0 );
		secVal  = storedData(QueryResultSet[qIdx2]->actualRow, cnfField, cnfSrc, &errCode);//record corrente, nome del campo, nome della query
	}

	if(!secVal) 
    return " ";

	if (!strcmp(firstVal, secVal))
	{
		retVal = (char*) malloc( ( strlen(firstVal) + SELECTED_ADD_ON ) * sizeof(char)  );
    if (!retVal) 
      EXIT(23);
/**/if(alloctrc){fprintf(debug, "- - retVal:%d - - len:%d\n", retVal, ( strlen(firstVal) + SELECTED_ADD_ON ) * sizeof(char) );fflush(debug);}
		sprintf(retVal, "%s SELECTED", firstVal);
	}
	else
	{
		retVal = (char*) malloc( ( strlen(firstVal) + 1) * sizeof(char)  );
    if (!retVal) 
      EXIT(23);
/**/if(alloctrc){fprintf(debug, "- - retVal:%d - - len:%d\n", retVal, ( strlen(firstVal) + 1 ) * sizeof(char) );fflush(debug);}
		strcpy(retVal, firstVal);	
	}

//**/if(usedbg){fprintf(debug, "recValConf-RETURNING;\n");fflush(debug);}
	return retVal;
}


/*************************************************************************************
NOME			:recValLookAsStr
Categoria	:Gasp Command: *recLookS(TQR, TQR_field, table_2, t2_id, dscr_field)
attivita'	:seleziona da TQR il valore del record corrente per il campo TQR_field
					 quindi esegue una query sulla table_2 con condizione t2_id = TQR_field
					 ed estrae dscr_field

NOTE: TQR_field e' supposto una STRINGA
*************************************************************************************/
void * recValLookAsStr(int vuoto1,char *vuoto2, char * inputStr)
{
	char *queryName, *queryField, *tableName, *tableField, *dscrField;
	int qIdx, qIdx2;
	char *tqrVal, *retVal;
	char secondQuery[1024];
	int errCode = 0;

	if(!pickPar(inputStr, 1, &queryName)	)	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 2, &queryField)	)	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 3, &tableName)	)	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 4, &tableField)	)	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 5, &dscrField)	)	return(PARAM_NOT_FOUND_MSG);

	qIdx = queryIndex( queryName, 0 );
	tqrVal = storedData(QueryResultSet[qIdx]->actualRow, queryField, queryName, &errCode);
	if(!tqrVal)	return "";

	sprintf(secondQuery, "SELECT %s FROM %s WHERE %s = '%s'", dscrField, tableName, tableField, tqrVal);
	newAutoName();
	dbInterface(QueryLabel, secondQuery, 1, 1);

	qIdx2 = queryIndex( QueryLabel, 0 );
	retVal = storedData(QueryResultSet[qIdx2]->actualRow, dscrField, QueryLabel, &errCode);
	if(!retVal) return " ";

	return retVal;
}


/*************************************************************************************
NOME			:cycleQuery
Categoria	:Gasp Command: *cycleQ(TQRName, OPZonEmptyListMessage, OPZonEmptyListFile)
attivita'	:definisce il punto di partenza di un ciclo che viene effettuato sui record
					 del TQR specificato. Il comando di fine ciclo, necessario pena un
					 comportamento imprevedibile del template, � *endCQ(TQRName).
					 TQRName							: nome del tqr;
					 OPZonEmptyListMessage: opzionale, stringa da ritornare in caso di nessun
																	record trovato;
					 OPZonEmptyListFile		: file da includere in caso di nessun record trovato;
*************************************************************************************/
void * cycleQuery (int vuoto1,		char* vuoto2,		char*	inputStr)
{
	char *queryName, *onEmptyMsg, *onEmptyFileName, *tplStr, *outputStr;
	int	qIdx;

/**/if(usedbg){fprintf(debug, "cycleQuery - STARTING\n" );fflush(debug);}
	if ( ReadCycle[CycLevel-1] == 0)
	{
		ReadCycle[CycLevel] = 0;
/**/if(usedbg){fprintf(debug, "cycleQuery - abortING\n" );fflush(debug);}
		return (0);
	}

	if(!pickPar(inputStr, 1, &queryName				)		)	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 2, &onEmptyMsg			)   )	onEmptyMsg = 0;
	if(!pickPar(inputStr, 3, &onEmptyFileName	)   )	onEmptyFileName = 0;	

	qIdx = queryIndex(queryName, 0);

	if ( QueryResultSet[qIdx]->totalRows	== 0 )
	{
		ReadCycle[CycLevel] = 0;

		if (onEmptyFileName)
		{
			bufferTpl(&tplStr, onEmptyFileName, TplDir);
			procTplData(tplStr, cgiOut, &outputStr);
			if (tplStr)
			{
				/**/if(alloctrc){fprintf(debug, "* * tplStr:%d\n", tplStr);fflush(debug);}
				free(tplStr);
			}
			//fflush(cgiOut);
			EXIT(1);
		}
		else if (onEmptyMsg)
		{
			fprintf(cgiOut, "%s\n", onEmptyMsg);//fflush(cgiOut);
		}
	}
	else if (
		( QueryResultSet[qIdx]->actualRow	<  (QueryResultSet[qIdx]->rowsToStore + 1) ) &&
		( (QueryResultSet[qIdx]->actualRow	+  QueryResultSet[qIdx]->startingRow)	<  (QueryResultSet[qIdx]->totalRows + 2) )
					)
	{
/**/if(usedbg){fprintf(debug, "cycleQuery - actualRow - %d\n", QueryResultSet[qIdx]->actualRow );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cycleQuery - rowsToStore- %d\n", QueryResultSet[qIdx]->rowsToStore);fflush(debug);}
/**/if(usedbg){fprintf(debug, "cycleQuery - startingRow- %d\n", QueryResultSet[qIdx]->startingRow);fflush(debug);}
/**/if(usedbg){fprintf(debug, "cycleQuery - totalRows- %d\n", QueryResultSet[qIdx]->totalRows);fflush(debug);}
		ReadCycle[CycLevel] = 1;
	}
	else
	{
		ReadCycle[CycLevel] = 0;
	}

/**/if(usedbg){fprintf(debug, "cycleQuery - endING\n" );fflush(debug);}
	return (0);
}


void * endCycleQuery (int vuoto1,		char* vuoto2,		char*	inputStr)
{
	char *queryName;
	int qIdx;
/**/if(usedbg){fprintf(debug, "endCycleQuery - STARTING\n" );fflush(debug);}
	if(!pickPar(inputStr, 1, &queryName)   )	return(PARAM_NOT_FOUND_MSG);

	qIdx = queryIndex(queryName, 0);

	QueryResultSet[qIdx]->actualRow++;

/**/if(usedbg){fprintf(debug, "endCycleQuery - endING\n" );fflush(debug);}
	return 0;
}


/*************************************************************************************
NOME			:setActualRow
Categoria	:Gasp Command: *setRec(queryName,recNum)
attivita'	:imposta il record corrente della query;
par 1			:nome del resultset da identificare
par 2			:(opz, se omesso si sottointende il primo record) 
					 valore del record numerico,
					 oppure stringa "last" per forzare all'ultimo record,
					 oppure stringa ++ per incrementare il record attuale;
note			:se si eccede il massimo numero accettabile per il TQR, il record viene
					 imostato all'ultimo record; viceversa se si va sotto a STARTING_ROW si
					 forza a STARTING_ROW.
*************************************************************************************/
void * setActualRow(int vuoto1,		char* vuoto2,		char*	inputStr)
{
	char *queryName, *recordNumbSt;
	int	qIdx, recordNumber = STARTING_ROW;
	int lastRow; 

	if(!pickPar(inputStr, 1, &queryName				)		)	return(PARAM_NOT_FOUND_MSG);
	qIdx = queryIndex(queryName, 0);
	lastRow = QueryResultSet[qIdx]->rowsToStore;

	if( pickPar(inputStr, 2, &recordNumbSt		)   )	
	{
		if ( strcmp(recordNumbSt, "last") == 0 )
			recordNumber = lastRow;
		else if(strcmp(recordNumbSt, "++") == 0)
			recordNumber = QueryResultSet[qIdx]->actualRow + 1;
		else
			recordNumber = atoi(recordNumbSt);
	}

	if (recordNumber > lastRow)
		recordNumber = lastRow;
	else if (recordNumber < STARTING_ROW)
		recordNumber = STARTING_ROW;

	QueryResultSet[qIdx]->actualRow	= recordNumber;

	return (0);
}



/*************************************************************************************
NOME			:startCndBlk
Categoria	:Gasp Command: *if(condizione)/*ifblk(condizione)
attivita'	:inizio del blocco condizionato; necessita di un comando *endif()/*endblk()
					 per la terminazione del blocco:
					 la condizione utilizza gli operatori
					 ==	uguale
					 != diverso
					 <	minore
					 >	maggiore
					 <=	minore-uguale
					 =>	uguale-maggiore
					 per il confronto tra i due termini.
					 Nel caso entrambi i termini del confronto siano convertibili a numeri
					 interi il confronto avviene tra interi. altrimenti tra stringhe.
					 Se la condizione � una stringa nulla l'if non ha successo.
					 Se la condizione non contiene nessun opetratore e la stringa non � nulla
					 l'if ha successo.
					 Se il confronto ha successo il blocco viene elaborato dal parser ed i
					 comandi al suo interno non vengono eseguiti.
*************************************************************************************/
void * startCndBlk (int vuoto1,		char* vuoto2,		char*	inputStr)
{
	char *operators[] = {"==","!=","<","<=","=>",">"};
	char *opPosition[CND_OP_NUM];
	const int opNum = CND_OP_NUM;
	int i, opOrd, lOpLen, rOpLen, result;
	char *leftOp, *rightOp;

//**/if(usedbg){fprintf(debug, "START CND\n");fflush(debug);}
//**/if(usedbg){fprintf(debug, "inputStr:%s\n", inputStr);fflush(debug);}

// si ereditano le colpe dei padri
//	if (ValidBlock[CndLevel-1] == 0)
//	{
//		ValidBlock[CndLevel] = 0;
//		return 0;
//	}

	opOrd = -1;
	for (i = 0; i < opNum; i++)
	{
		opPosition[i] = strstr(inputStr, operators[i]);
		if (opPosition[i])
		{
			opOrd = i;
			break;
		}
	}


	if (opOrd == -1)
	{
		// se non vi � operatore il blocco si considera TRUE se vi e' una qualsiasi stringa
		if(strlen(inputStr) > 0)
			ValidBlock[CndLevel] = 1;
		else
			ValidBlock[CndLevel] = 0;

		return 0;
	}
	lOpLen = opPosition[opOrd] - inputStr;

	leftOp = (char*) malloc((lOpLen + 1) * sizeof(char));if (!leftOp) EXIT(23);
/**/if(alloctrc){fprintf(debug, "- - leftOp:%d - - len:%d\n", leftOp, (lOpLen + 1) * sizeof(char) );fflush(debug);}
	strncpy(leftOp, inputStr, lOpLen);
	leftOp[lOpLen] = 0;

	rOpLen = strlen(inputStr) - strlen(operators[opOrd]) - lOpLen;
	rightOp = (char*) malloc((rOpLen + 1)* sizeof(char));if (!rightOp) EXIT(23);
/**/if(alloctrc){fprintf(debug, "- - rightOp:%d - - len:%d\n", rightOp, (rOpLen + 1) * sizeof(char) );fflush(debug);}
	strcpy( rightOp, &inputStr[ lOpLen + strlen(operators[opOrd]) ] );
	rightOp[rOpLen]=0;

//trimIt(leftOp);
//trimIt(rightOp);

	result = strcmp(leftOp,rightOp);
	// nel caso di operatori convertibili ad interi vendono confrontati i valori interi
	if ( atoi(leftOp) && atoi(rightOp) )	result= atoi(leftOp) - atoi(rightOp);

	switch (opOrd) {
	case 0:
		if (result)			ValidBlock[CndLevel] = 0;	else	ValidBlock[CndLevel] = 1;		break;
	case 1:
		if (result)			ValidBlock[CndLevel] = 1;	else	ValidBlock[CndLevel] = 0;		break;
	case 2:
		if (result<0)		ValidBlock[CndLevel] = 1;	else	ValidBlock[CndLevel] = 0;		break;
	case 3:
		if (result<=0)	ValidBlock[CndLevel] = 1;	else	ValidBlock[CndLevel] = 0;		break;
	case 4:
		if (result>=0)	ValidBlock[CndLevel] = 1;	else	ValidBlock[CndLevel] = 0;		break;
	case 5:
		if (result>0)		ValidBlock[CndLevel] = 1;	else	ValidBlock[CndLevel] = 0;		break;
	}

//**/if(usedbg){fprintf(debug, "END CND\n");fflush(debug);}
	return 0;
}

/*************************************************************************************
NOME			:elseCndBlk
Categoria	:Gasp Command: *else(condizione)/*elseblk(condizione)
attivita'	:else del blocco condizionato
*************************************************************************************/
void * elseCndBlk (int vuoto1,		char* vuoto2,		char*	inputStr)
{
	// si ereditano le colpe dei padri
//	if (ValidBlock[CndLevel-1] == 0)
//	{
//		ValidBlock[CndLevel] = 0;
//		return 0;
//	}

	return 0;
}

/*************************************************************************************
NOME			:endCndBlk
Categoria	:Gasp Command: *endif(condizione)/*endblk(condizione)
attivita'	:fine del blocco condizionato.
*************************************************************************************/
void * endCndBlk (int vuoto1,		char* vuoto2,		char*	inputStr)
{
//**/if(usedbg){fprintf(debug, "ENDIF\n");fflush(debug);}
	return 0;
}

/*************************************************************************************
NOME			:getValue
Categoria	:Gasp Command: *get(parametro,OPZdefault)
attivita'	:restituisce il valore del parametro di get indicato
					 - parametro: nome del paremetro di get di cui restituire il valore
					 - OPZdefault: valore di default ritornato nel caso non si trovi il
												 parametro.
						 N.B. se OPZdefault � il carattere '+' il valore
						 ricavato per il parametro viene modificato sostituendo tutti
						 i caratteri non alfanumerici con la stringa %xx dove xx �
						 il valore ascii esadecimale del carattere sostituito.
*************************************************************************************/
void * getValue(int vuoto1, char *vuoto2, char * inputStr)
{
	static char	returnValue[GET_PAR_VAL_LEN];
	char	*defParVal, *getParName;
	int		useDefault = 0;
  itxString istr;

	if(!pickPar(inputStr, 1, &getParName)	)	return(PARAM_NOT_FOUND_MSG);
	if( pickPar(inputStr, 2, &defParVal))	useDefault = 1;

//**/if(usedbg){fprintf(debug, "getParName:%s\n",getParName);fflush(debug);}
	if (cgiFormString( getParName, returnValue, 0) != cgiFormSuccess)
	{
		if (useDefault)
		{
			// valore opzionale di default definito dall'utente
			strcpy(returnValue,defParVal);
		}
		else
		{
			// valore di default generico
			sprintf(returnValue, "%s", GET_PAR_VAL);
		}
	}

	if (strcmp(defParVal,"+")==0)
	{
		char * modRetVal;
		if(strcmp(returnValue,"+")==0)
			returnValue[0] = '\0';

		istr.EscapeChars(returnValue, &modRetVal);
		return((void *)modRetVal);
	}

	return((void *)returnValue);
}

/*************************************************************************************
NOME			:getmad
Categoria	:Gasp Command: *getmad(parametro,OPZdefault)
attivita'	:come la getValue ma raddopia gli apici se presenti ed il valore opzionale +
					 si limita ad indicare che si devono sostituire gli spazi con caratteri +
*************************************************************************************/
void * getmad(int vuoto1, char *vuoto2, char * inputStr)
{
	itxString buffer;
	char	*retVal;
	int		retValLen=0;
	char	*defParVal, *getParName;
	int		useDefault = 0;

/**/if(usedbg){fprintf(debug, "getmad START with:%s\n", inputStr);fflush(debug);}

	if(!pickPar(inputStr, 1, &getParName)	)	return(PARAM_NOT_FOUND_MSG);
	if( pickPar(inputStr, 2, &defParVal))	useDefault = 1;

	
	if (cgiFormStringSpaceNeeded( getParName, &retValLen) != cgiFormSuccess)
		retValLen = 0;

	buffer.Space(retValLen+1);

	if (cgiFormString( getParName, buffer.GetBuffer(), 0) != cgiFormSuccess)
	{
		if (useDefault)
			buffer = defParVal;
		else
			buffer = GET_PAR_VAL;

		if (strcmp(defParVal,"+")==0)
			getHide(buffer.GetBuffer());
	}

  buffer.AdjustStr();
	buffer.SubstituteSubString("\"", "\"\"");

	retVal = (char*) calloc(buffer.Len() + 1, sizeof(char));
	strcpy(retVal, buffer.GetBuffer());

	return((void *)retVal);
}

void * quotestr(int vuoto1, char *vuoto2, char * inputStr)
{
	itxString buffer;
  char* pstring;
	static char	string[256];

  memset(string, '\0', 256);

/**/if(usedbg){fprintf(debug, "quotestr START with:%s\n", inputStr);fflush(debug);}

	if(!pickPar(inputStr, 1, &pstring))	return(PARAM_NOT_FOUND_MSG);
	
  buffer = pstring;
  buffer.SubstituteSubString("'", "\\'");

	strcpy(string, buffer.GetBuffer());

	return ((void*)string);
}

/*************************************************************************************
NOME			:putGetHide
Categoria	:Gasp Command: *gethide(parametro)
attivita'	:nasconde i caratteri non alfanumerici del parametro trasformandoli in %xx
					 dove xx � il valore ascii esadecimale del carattere sostituito.
*************************************************************************************/
void * putGetHide(int vuoto1, char *vuoto2, char * inputStr)
{
	char	*string = NULL;
	char	*retVal = NULL;
  itxString istr;

	if(!pickPar(inputStr, 1, &string)	)	return "";

	//getHide(string);
	istr.EscapeChars(string,&retVal);

	return retVal;
}

/*************************************************************************************
NOME			:encrIt
Categoria	:servizio
attivita'	:Criptaggio distruttivo di sicurezza molto blando accoppiato alla vecrIt.
					 Usato dalla Authorize.
*************************************************************************************/
void * encrIt(int vuoto1, char *vuoto2, char * inputStr)
{
	static char	returnValue[GET_PAR_VAL_LEN];
	char *seedStr;
	struct _timeb timebuffer;
	char appo[4];
	char milsec[4];
	int crs, i;
	int keyIdx = 0;

	if(!pickPar(inputStr, 1, &seedStr)	)	return(PARAM_NOT_FOUND_MSG);

	_ftime( &timebuffer );
	sprintf( milsec, "%d", timebuffer.millitm );

	if			(strlen(milsec) == 1)	sprintf( appo, "00%s", milsec );
	else if	(strlen(milsec) == 2)	sprintf( appo, "0%s", milsec );
	else													sprintf( appo, "%s", milsec );

	for(i=0;seedStr[i];i++)
	{
		if (crs = isAlphaNum(seedStr[i]))
		{
			keyIdx = i % 3;
			seedStr[i] = crs + appo[keyIdx];
			if (seedStr[i] = reAlphaNum(seedStr[i])); else EXIT(__LINE__);
		}
		else EXIT(__LINE__);
	}

	sprintf( returnValue, "%s%s", appo, seedStr);

	return((void *)returnValue);
}


/*************************************************************************************
NOME			:encrIt
Categoria	:servizio
attivita'	:verifica del crittaggio usato dalla encrIt. Usato dalla Authorize
*************************************************************************************/
int vecrIt(char *encrStr, char * candidate)
{
	static char	returnValue[GET_PAR_VAL_LEN];
	int crs, i;
	int keyIdx = 0;

	if (candidate)
	{
		for(i=0;candidate[i];i++)
		{
			if (crs = isAlphaNum(candidate[i]))
			{
				keyIdx = i % 3;
				candidate[i] = crs + encrStr[keyIdx];
				if (candidate[i] = reAlphaNum(candidate[i])); else return 0;
			}
			else return 0;
		}
		if (strcmp(candidate, &(encrStr[3]) ) == 0) return 1;
		else return 0;
	}
	else return 0;

}

/*************************************************************************************
NOME			:authorize
Categoria	:Gasp Command: *auth(login,cpwd,OPZextraField,OPZextraVal) - Uso Deprecato
attivita'	:autorizzazione utenti, fa uso della tabella del database specificata nel
					 file dei parametri al valore LoginTable.
					- login		: identificatore dell'utente candidato alla autenticazione;
											viene usato per selezionare il record uguagliandolo al campo
											specificato nel file di inizializzazione nel parametro LoginField
					- pwd			: password dell utente candidato alla autenticazione;
											viene usata per convalidare il record selezionato dal campo login
											uguagliandola al valore del campo specificato nel file di
											inizializzazione nel parametro PwdField
					- OPZextraField: campo addizionale della tabella;
					- OPZextraVal  :valore del campo addizionale da verificare se specificato
Note			:usa le encrIt e vecrIt -> PROTEZIONE BLANDA.
*************************************************************************************/
void * authorize(int vuoto1, char *vuoto2, char * inputStr)
{
	static char	returnValue[GET_PAR_VAL_LEN];
	static char	queryString[GET_PAR_VAL_LEN];
	char *encrStr, *candidate, *loginSt;
	char *extraField, *extraVal;
	char * errStr, *outputStr;
	int	errCode = 0;
	char defExtra[]={'1',0};

	if(!pickPar(inputStr, 1, &loginSt		)	)	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 2, &encrStr		)	)	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 3, &extraField)	)	extraField = defExtra;
	if(!pickPar(inputStr, 4, &extraVal	)	)	extraVal = defExtra;

	sprintf(queryString, "select %s from %s where %s = '%s' AND %s=%s ", 
    PwdField, LoginTable, LoginField, loginSt, extraField, extraVal);
	dbInterface("qtst", queryString, 1, 1);
	candidate = storedData(1, PwdField, "qtst", &errCode);

	// Se si verifica un errore di sintassi si manda su schermo un messaggio di informazione
	if ( errCode == ERR_COL_NOT_FOUND || errCode == ERR_QUERY_NOT_FOUND|| errCode == ERR_VOID_QUERY_NAME	)
	{
		fprintf(cgiOut, "%s\n", candidate);//fflush(cgiOut);
	}

	if ( vecrIt(encrStr, candidate) != 0)
	{
		sprintf( returnValue, "OK");
	}
	else
	{
		if( bufferTpl(&errStr, INVALID_LOGIN_FILE, TplDir) != 0)
    {
  		procTplData(errStr, cgiOut, &outputStr);
  		if (errStr)
			{
/**/if(alloctrc){fprintf(debug, "* * errStr:%d\n", errStr);fflush(debug);}
				free(errStr);
			}
    }
    else
    {
      fprintf(cgiOut, "%s\n", retMsg(LOGIN_ERROR));
    }
		//fflush(cgiOut);
		EXIT(1);
	}
	return (0);
}


/*************************************************************************************
NOME			:validate
Categoria	:Gasp Command: *valid(login,cpwd,OPZextraField,OPZextraVal)
attivita'	:autorizzazione utenti, fa uso della tabella del database specificata nel
					 file dei parametri al valore LoginTable.
					- login		: identificatore dell'utente candidato alla autenticazione;
											viene usato per selezionare il record uguagliandolo al campo
											specificato nel file di inizializzazione nel parametro LoginField
					- pwd			: password dell utente candidato alla autenticazione;
											viene usata per convalidare il record selezionato dal campo login
											uguagliandola al valore del campo specificato nel file di
											inizializzazione nel parametro PwdField
					- OPZextraField: campo addizionale della tabella;
					- OPZextraVal  :valore del campo addizionale da verificare se specificato
Note			:funzioni di criptaggio per una -> PROTEZIONE FORTE.
*************************************************************************************/
void * validate(int vuoto1, char *vuoto2, char * inputStr)
{
	static char	returnValue[GET_PAR_VAL_LEN];
	static char	queryString[GET_PAR_VAL_LEN];
	char *passwd, *loginSt;
	char *extraField, *extraVal;
	char * errStr, *outputStr;
	int	errCode = 0;
	char defExtra[]={'1',0};
  int validated =1;

	if(!pickPar(inputStr, 1, &loginSt		)	)	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 2, &passwd		)	)	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 3, &extraField)	)	extraField = defExtra;
	if(!pickPar(inputStr, 4, &extraVal	)	)	extraVal = defExtra;

	//
	// Controllo che la password nel DB sia la stessa di input
	//
	validated = checkDbPwd(loginSt, passwd, extraField, extraVal );

/**/if(usedbg){fprintf(debug, "----------VALIDATED? - %d\n",retMsg(validated));fflush(debug);}

  if ( validated == ERR_COL_NOT_FOUND   ||
       validated == ERR_QUERY_NOT_FOUND ||
       validated == ERR_VOID_QUERY_NAME     )
	{
		fprintf(cgiOut, "%s\n", retMsg(validated));//fflush(cgiOut);
	}

	if ( validated == NOT_VALIDATED)
	{
    //apre il template, alloca tplString, e vi copia i dati
		if( bufferTpl(&errStr, INVALID_LOGIN_FILE, TplDir) != 0)
    {
      //interpretazione dei dati e scrittura su outputStream
  		procTplData(errStr, cgiOut, &outputStr);
  		if (errStr)
			{
/**/if(alloctrc){fprintf(debug, "* * errStr:%d\n", errStr);fflush(debug);}
				free(errStr);
			}
    }
    else
    {
      fprintf(cgiOut, "%s\n", retMsg(LOGIN_ERROR));
    }
		//fflush(cgiOut);
		exit(1);
	}
	return (0);
}


/*************************************************************************************
NOME			:validAleph
Categoria	:ALEPH servizio
attivita'	:autorizzazione utenti per il client Aleph a partire dai dati loginSt e
					 passwd e dalle variabili globali del file di inizializzazione che
					 specificano tabella, campo di login e campo di pwd.
Note			:Usa funzioni di criptaggio per una -> PROTEZIONE FORTE.
*************************************************************************************/
void * validAleph(int vuoto1, char *vuoto2, char * inputStr)
{
	static char	returnValue[GET_PAR_VAL_LEN];
	static char	queryString[GET_PAR_VAL_LEN];
	char *passwd, *loginSt;
	int	errCode = 0;
  int validated =1;

	if(!pickPar(inputStr, 1, &loginSt		)	)	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 2, &passwd		)	)	return(PARAM_NOT_FOUND_MSG);

/**/if(usedbg){fprintf(debug, "sto entrando nella validateAleph\n");fflush(debug);}
	//
	// Controllo che la password nel DB sia la stessa di input
	//
	validated = checkDbPwd(loginSt, passwd, "1", "1" );

  if ( validated == ERR_COL_NOT_FOUND   ||
       validated == ERR_QUERY_NOT_FOUND ||
       validated == ERR_VOID_QUERY_NAME     )
	{
	  fprintf(cgiOut, "%s:0;\n", ALEPH_START_COM);
	  fprintf(cgiOut, "%s:%d;\r\n", ALEPH_END_COM, FATAL_ERROR);
    EXIT(0);
	}

	if ( validated == NOT_VALIDATED)
	{
	  fprintf(cgiOut, "%s:0;\n", ALEPH_START_COM);
	  fprintf(cgiOut, "%s:%d;\r\n", ALEPH_END_COM, LOGIN_ERROR);
    EXIT(0);
	}


  sprintf(returnValue, "%s:0;%s:%d;", ALEPH_START_COM, ALEPH_END_COM, LOGIN_SUCCESS);
/**/if(usedbg){fprintf(debug, "validateAleph return value = %s\n", returnValue);fflush(debug);}
	return (returnValue);
}


/*************************************************************************************
NOME			:exitOnFile
Categoria	:Gasp Command: *exof(OPZexitType,OPZexitMessage,OPZexitFile)
attivita'	:interrompe il processing del template ed esce dopo aver, a seconda del
					 parametro exitType, mostrato un messaggio o effettuato il parsing di un file;
					 - exitType		:pu� assumere i seguenti valori validi m (message) f (file)
												 b (both)
					- exitMessage	:messaggio da visualizzare prima di uscire
					- exitFile		:file da processare prima di uscire
*************************************************************************************/
void * exitOnFile(int vuoto1, char *vuoto2, char * inputStr)
{
	char *exType, *exMsg, *addFile;
	char * tplStr, *outputStr;

	if(!pickPar(inputStr, 1, &exType	)	)		exType	= 0;
	if(!pickPar(inputStr, 2, &exMsg		)	)		exMsg		= 0;
	if(!pickPar(inputStr, 3, &addFile	)	)		addFile	=	0;

	if (exType)
	{
		// scrive il messaggio (par 2) prima di uscire
		if (*exType=='m')
		{
			if (exMsg)
			{
				fprintf(cgiOut, "%s\n", exMsg);//fflush(cgiOut);
			}
			EXIT(1);
		}
		// fa il pars del file (par 3) prima di uscire
		else if (*exType=='f')
		{
			if (exMsg)
			{
				if (strlen(exMsg))
				{
					bufferTpl(&tplStr, exMsg, TplDir);
					procTplData(tplStr, cgiOut, &outputStr);
//					fflush(cgiOut);
				}
			}
			EXIT(1);
		}
		// scrive il messaggio (par 2) prima di uscire e
		// fa il pars del file (par 3) prima di uscire
		else if (*exType=='b')
		{
			if (exMsg && addFile)
			{
				fprintf(cgiOut, "%s\n", exMsg);//fflush(cgiOut);
				if (strlen(addFile))
				{
					bufferTpl(&tplStr, addFile, TplDir);
					procTplData(tplStr, cgiOut, &outputStr);
//					fflush(cgiOut);
				}
			}
			EXIT(1);
		}
	}
	return (0);
}

/*************************************************************************************
NOME			:syntChk
Categoria	:Gasp Command: *chkIt(stringToTest,flag,............
											DA DOCUMENTARE
											DA DOCUMENTARE
											DA DOCUMENTARE
											DA DOCUMENTARE
*************************************************************************************/
void * syntChk(int vuoto1, char *vuoto2, char * inputStr)
{
	static char	returnValue[GET_PAR_VAL_LEN];
	char *strToTest, *typeFlag, *minLenS, *maxLenS;
	int i, minLen, maxLen, numStr;

	if(	!pickPar(	inputStr, 1, &strToTest	)	)		return(PARAM_NOT_FOUND_MSG);
	if(	!pickPar(	inputStr, 2, &typeFlag	)	)		return(PARAM_NOT_FOUND_MSG);
	if(	 pickPar(	inputStr, 3, &minLenS		)	)		minLen = atoi(minLenS);
	if(	 pickPar(	inputStr, 4, &maxLenS		)	)		maxLen = atoi(maxLenS);

	numStr = (int)strlen( strToTest );

	if ( !strcmp(typeFlag, "a") )
	{
		for ( i = 0; strToTest[i]; i++)
		{
			if ( !isAlphaNum( strToTest[i] ) ) return ("FALSE");
		}
	}
	else if ( !strcmp(typeFlag, "n") )
	{
		numStr = atoi( strToTest );
	}

	if ( (  numStr < minLen ) || ( numStr  > maxLen ) )	return ("FALSE");

	return ("TRUE");
}


/*************************************************************************************
NOME			:setVar
Categoria	:Gasp Command: *setVar(varName,varValue)
attivita'	:assegna alla variabile varName il valore varValue.
*************************************************************************************/
void * setVar(int vuoto1, char *vuoto2, char * inputStr)
{
	char *varName, *varValue;
	int i;

	if(	!pickPar(	inputStr, 1, &varName	)	)		return(PARAM_NOT_FOUND_MSG);
	if(	!pickPar(	inputStr, 2, &varValue)	)		return(PARAM_NOT_FOUND_MSG);

	for ( i = 0; i < TplVars.idx; i++)
	{
		if ( ( TplVars.names[i] != 0 ) && ( varName != 0 ) )
		{
			if ( strcmp(TplVars.names[i] , varName ) == 0 )
				break;
		}
	}

	if (i == TplVars.idx)
	{
		if ( ( TplVars.idx < TPL_VARS_NUM ) && (varValue != 0) )
		{
			TplVars.names[TplVars.idx] = varName ;
			TplVars.values[TplVars.idx] = varValue ;
			TplVars.idx++;
		}
		else
			return(UNABLE_TO_WRITE_VAR);
	}
	else
		TplVars.values[i] = varValue ;

	return (0);
}


/*************************************************************************************
NOME			:getVar
Categoria	:Gasp Command: *getVar(varName)
attivita'	:restituisce il valore della variabile varName.
*************************************************************************************/
void * getVar(int vuoto1, char *vuoto2, char * inputStr)
{
	char *varName, *varDefValue;
	int		i;

	if(	!pickPar(	inputStr, 1, &varName			)	)				return(PARAM_NOT_FOUND_MSG);
	if(	!pickPar(	inputStr, 2, &varDefValue	)	)				varDefValue = 0;

	for ( i = 0; i < TplVars.idx; i++)
	{
//**/if(usedbg){fprintf(debug, "i:%d\t\t%s\n", i,	TplVars.names[i]);fflush(debug);}
//**/if(usedbg){fprintf(debug, "name\t\t%s\n", varName);fflush(debug);}
		if ( ( TplVars.names[i] != 0 ) && ( varName != 0 ) )
		{
			if ( strcmp(TplVars.names[i] , varName ) == 0 )	return (TplVars.values[i]);
		}
	}

	if ( varDefValue != 0 )	return(varDefValue);
	else										return("");
}

/*************************************************************************************
NOME			:extrIdG
Categoria	:Gasp Command: *getId(getVarName,OPZtableIdField,OPZtableName)
attivita'	:restituisce il valore del campo indice il cui valore � la variabile di get
					 getVarName.
					 getVarName				-nome della variabile di get di cui trovare l'indice
					 OPZtableIdField	-nome, opzionale, del campo di esplorazione il default �
														 ExtrField
					 OPZtableName			-nome, opzionale, della tabella di lavoro il default �
														 ExtrTable
*************************************************************************************/
void * extrIdG(int vuoto1, char *vuoto2, char * inputStr)
{
	char	*tableName, *tableField,  *idValue, *fieldValSource;
	char	fieldValue[EXTRID_FIELD_LEN];
	char	queryString[GET_PAR_VAL_LEN];
	int		errCode;

	if(	!pickPar(	inputStr, 1, &fieldValSource	)	)		return(PARAM_NOT_FOUND_MSG);
	if(	!pickPar(	inputStr, 2, &tableField			)	)		tableField = ExtrField;
	if(	!pickPar(	inputStr, 3, &tableName				)	)		tableName = ExtrTable;

//**/fprintf(cgiOut, "<br>flagGet:%d<br>\n", flagGet);fflush(cgiOut);
	if ( cgiFormString( fieldValSource, fieldValue, 0) != cgiFormSuccess )
	{
		return (FIELD_NAME_NOT_FOUND);
	}
	sprintf(queryString, "select %s from %s where %s = '%s'"
		, IdField, tableName, tableField, fieldValue);
//**/fprintf(cgiOut, "<br>queryString:%s<br>\n", queryString);fflush(cgiOut);
	dbInterface("idExtr", queryString, 1, 1);
	idValue = storedData(1, IdField, "idExtr", &errCode);
	return (idValue);
}

/*************************************************************************************
NOME			:extrId
Categoria	:Gasp Command: *extId(fieldVal,OPZtableIdField,OPZtableName)
attivita'	:restituisce il valore del campo indice il cui valore fieldVal.
					 fieldVal					-valore di cui trovare l'indice
					 OPZtableIdField	-nome, opzionale, del campo di esplorazione il default �
														 ExtrField
					 OPZtableName			-nome, opzionale, della tabella di lavoro il default �
														 ExtrTable
*************************************************************************************/
void * extrId(int vuoto1, char *vuoto2, char * inputStr)
{
	char	*tableName, *tableField,  *idValue, *fieldValSource;
	char	*fieldValue;
	char	queryString[GET_PAR_VAL_LEN];
	int		errCode;

	if(	!pickPar(	inputStr, 1, &fieldValSource	)	)		return(PARAM_NOT_FOUND_MSG);
	if(	!pickPar(	inputStr, 2, &tableField			)	)		tableField = ExtrField;
	if(	!pickPar(	inputStr, 3, &tableName				)	)		tableName = ExtrTable;

	fieldValue = fieldValSource;
	sprintf(queryString, "select %s from %s where %s = '%s'", IdField, tableName, tableField, fieldValue);
//**/fprintf(cgiOut, "<br>queryString:%s<br>\n", queryString);fflush(cgiOut);
	dbInterface("idExtr", queryString, 1, 1);
	idValue = storedData(1, IdField, "idExtr", &errCode);
	return (idValue);
}


/*************************************************************************************
NOME			:translateIt
Categoria	:Gasp Command: *trans(phraseTag,OPZforceLang)
attivita'	:traduce la phraseTag nella lingua corrente. Fa uso dei parametri
					 LangTable, LangNameField, LangCodeField del file di inizializzazione per
					 definire tabella dei linguaggi, campo del nome della lingua, campo del
					 codice della lingua. Ricava il valore del campo LangNameField per il record
					 in cui il campo LangCodeField corrisponde al valore del parametro di get
					 identificato dal valore del parametro LangTagGet del file di inizializzazione.
					 Il nome del campo della lingua � quello da estrarre dalla tabella delle
					 traduzioni (definita dal parametro TransTable nell'file di inizializzazione)
					 in corrispondenza del valore del campo TransTagField (sempre nel file dei
					 parametri) che uguaglia la stringa di input del comando phraseTag.
					 OPZforceLang, � un parametro che forza la lingua al valore specificato.
*************************************************************************************/
void * translateIt(int vuoto1, char *vuoto2, char * inputStr)
{
	char	*translatedPhrase, *phraseTag, *langName, *forceLang;
	char	getLangCode[LANG_ID_LEN];
	char	queryString[GET_PAR_VAL_LEN];
	char	appo[GET_PAR_VAL_LEN];
	char	getContext[GET_PAR_VAL_LEN];
	int		errCode = 0;

	if(	!pickPar(	inputStr, 1, &phraseTag		)	)		return(PARAM_NOT_FOUND_MSG);
	if(	!pickPar(	inputStr, 2, &forceLang		)	)		forceLang = 0;

	if (cgiFormString( LangTagGet, getLangCode, 0) != cgiFormSuccess)
		sprintf(getLangCode, "%s", DefaultLanguageId);	/*identificativo della lingua di default*/

	if ( forceLang != 0 )
		sprintf(getLangCode, "%s", forceLang);	/*valore dell'id della lingua forzato a forceLang*/

	sprintf(queryString,"select %s from %s	where %s = %s",
		LangNameField, LangTable, LangCodeField, getLangCode);

	newAutoName();
	dbInterface(QueryLabel, queryString, 1, 1);
	langName = storedData(1, LangNameField, QueryLabel, &errCode);

	sprintf(queryString, "select %s from %s where %s = '%s'", 
		langName, TransTable, TransTagField, phraseTag);

	if ((TargetTplField != 0) && (TplTable != 0) && 
      (TplTableId != 0)     && (TplTableName != 0))
  {
    if ((strcmp(TargetTplField,"")!=0) &&
        (strcmp(TplTable,"")!=0) &&
        (strcmp(TplTableId,"")!=0) &&
        (strcmp(TplTableName ,"")!=0))
    {
			if (strcmp(CurrentTpl[TplNest],"")!=0)
			{
				sprintf(queryString,
					"SELECT f.%s "
					"FROM %s f, %s t "
					"WHERE	f.%s = '%s' "
					"AND		f.%s = t.%s "
					"AND		t.%s = '%s' ",
					 langName,
					 TransTable, TplTable,
					 TransTagField, phraseTag,
					 TargetTplField, TplTableId,
					 TplTableName, CurrentTpl[TplNest]);
				if (cgiFormString( ContextTag, getContext, 0) == cgiFormSuccess)
				{
					sprintf(appo," AND		t.%s = %s ", ContextField, getContext);
					strcat(queryString, appo);
				}
			}
    }
  }

	newAutoName();
	dbInterface(QueryLabel, queryString, 1, 1);

	translatedPhrase = storedData(1, langName, QueryLabel, &errCode);

/**/if(usedbg){fprintf(debug, "translateIt-translatedPhrase:%s\n", translatedPhrase);fflush(debug);}
	return (translatedPhrase);
}


/*************************************************************************************
NOME			:crypt
Categoria	:Gasp Command: *crypt(toBeCrypted)
attivita'	:cripta il parametro
*************************************************************************************/
void *crypt(int vuoto1, char *vuoto2, char * inputStr)
{
	char	*toBeCrypted;
	char * crypted;

	if(	!pickPar(	inputStr, 1, &toBeCrypted		)	)		return(PARAM_NOT_FOUND_MSG);

  crypted = (char*) malloc (10 * strlen(toBeCrypted) * sizeof(char) );
/**/if(alloctrc){fprintf(debug, "- - crypted:%d - - len:%d\n", crypted, 10 * strlen(toBeCrypted) * sizeof(char) );fflush(debug);}

  tannitEncrypt(toBeCrypted, crypted);

  return (crypted);
}

/*************************************************************************************
NOME			:decrypt
Categoria	:Gasp Command: *decr(toBeDecrypted)
attivita'	:decripta il parametro
*************************************************************************************/
void *decrypt(int vuoto1, char *vuoto2, char * inputStr)
{
	char	*toBeDecr;
	char * clear;

	if(	!pickPar(	inputStr, 1, &toBeDecr		)	)		return(PARAM_NOT_FOUND_MSG);
/**/if(usedbg){fprintf(debug, "toBeDecr:::::::::%s\n", toBeDecr);fflush(debug);}

  clear = (char*) malloc (strlen(toBeDecr) * sizeof(char) );
/**/if(alloctrc){fprintf(debug, "- - clear:%d - - len:%d\n", clear, strlen(toBeDecr) * sizeof(char) );fflush(debug);}

  tannitDecrypt(toBeDecr, clear);
/**/if(usedbg){fprintf(debug, "toBeDecr:::::::::%s\n", toBeDecr);fflush(debug);}
/**/if(usedbg){fprintf(debug, "clear::::::::::::%s\n", clear);fflush(debug);}

  return (clear);
}


/*************************************************************************************
NOME			:crypt2
Categoria	:Funzione inutile che non funziona (chissa', un giorno...)
attivita'	:nessuna
*************************************************************************************/
void *crypt2(int vuoto1, char *vuoto2, char * inputStr)
{
	char	*toBeCrypted;
	unsigned char* crypted;

	if(	!pickPar(	inputStr, 1, &toBeCrypted		)	)		return(PARAM_NOT_FOUND_MSG);

	crypted = (unsigned char*) calloc(sizeof(char), MAX_CRYPTING_BUFFER);

//  Sta roba non ha mai funzionato va sostituito ma non si sa
//	if ((result = itxCodeString(toBeCrypted, crypted, MAX_CRYPTING_BUFFER) == 0))
//		return (CRYPTING_ERROR);
//**/if(usedbg){fprintf(debug, "result:	%d;\n", result);fflush(debug);}
//	crypted[result] = '\0';

//**/if(usedbg){fprintf(debug, "crypted:	%s;\n", crypted);fflush(debug);}
//**/if(usedbg){fprintf(debug, "toBeCrypted:	%s;\n", toBeCrypted);fflush(debug);}
  return (crypted);
}

/*************************************************************************************
NOME			:processExternFile
Categoria	:Gasp Command: *prex(fileToInclude)
attivita'	:prosegue il parsing nel file specificato. Una volta concluso quest'ultimo
					 prosegue per il file di partenza.
*************************************************************************************/
void * processExternFile(int vuoto1, char *vuoto2, char * inputStr)
{
	char * fileName;
	char * externData;
	char * outputString = 0;

	if(!pickPar(inputStr, 1, &fileName		)	)	return(PARAM_NOT_FOUND_MSG);

	// apre il template, alloca tplString, e vi copia i dati
	TplNest++;
  if( bufferTpl(&externData, fileName, TplDir) == 0)
	{
		fprintf(cgiOut,"%s\n", ERR_OPEN_TPL_FILE);//fflush(cgiOut);
		EXIT(__LINE__);
	}
	// interpretazione dei dati e scrittura su cgiOut
	procTplData(externData, cgiOut, &outputString);
	TplNest--;

	return (0);
}


/*************************************************************************************
NOME			:setCookie
Categoria	:Gasp Command: *setCoo(name,value)
attivita'	:imposta all'utente il cookie "nome=valore".
*************************************************************************************/
void * setCookie(int vuoto1, char *vuoto2, char * inputStr)
{
	char * name, *value, *expires;
  int retvalue = 0;
	
	if(	!pickPar(	inputStr, 1, &name		)	)		return(PARAM_NOT_FOUND_MSG);
	if(	!pickPar(	inputStr, 2, &value		)	)		return(PARAM_NOT_FOUND_MSG);
	retvalue = pickPar(	inputStr, 3, &expires	);
	
	/* in later version, do checks for valid variables */
  printf("Set-Cookie: %s=%s;",name,value);
  if (retvalue != PARAM_NOT_FOUND)
    printf(" EXPIRES=%s;",expires);
  printf(" PATH='/';");
  printf("\n");
	return 0;
}


/*************************************************************************************
NOME			:isInTheDomain
Categoria	:Gasp Command: *inDomain()
attivita'	:sgama se l'utente remoto � nel dominio tale...
						DA DOCUMENTARE
						DA DOCUMENTARE
						DA DOCUMENTARE
						DA DOCUMENTARE
						DA DOCUMENTARE
*************************************************************************************/
void*	isInTheDomain(int vuoto1, char *vuoto2, char * inputStr)
{
	static char localHost[HOME_DIR_LEN];
	static char remotHost[HOME_DIR_LEN];
  char * cursor = NULL;
  char * cLevel = NULL;
  int ipLen = 0;

 	if(	!pickPar(	inputStr, 1, &cLevel		)	)		return("false");

  // Acquisizione della stringa del dominio del client e dell'host
	sprintf(localHost,"%s",cgiRemoteHost);
	sprintf(remotHost,"%s",cgiHttpHost);

  // Troncamento dei livelli  A e B dell'ip dell'Host
  if ( (ipLen = strlen(localHost) ) > 15 )
    return ("false");
  cursor = strpbrk(localHost, ".");
  if ( (cursor == NULL) || (cursor - localHost > 4) )
    return ("false");
  cursor++;
  if ( (cursor - localHost  > ipLen ) || (cursor - localHost < 0) )
    return ("false");
  cursor = strpbrk(cursor, ".");
  if ( (cursor == NULL) )
    return ("false");
  ipLen = cursor - localHost;
  if ( ( ipLen > ipLen ) || ( ipLen < 0) )
    return ("false");
  cursor[0] = 0;
  cursor++;

  if (  strncmp( remotHost, localHost, ipLen) != 0)
    return ("false");

  char * appo  = NULL;
  if ( (appo  = strstr(cursor, cLevel) ) == NULL )
    return ("false");
  if (  (cursor - appo ) != 0 )
    return ("false");

  if( cursor[strlen(cLevel)+1] != '.')
    return ("false");

	return ("true");
}

/*************************************************************************************
NOME			:fontStyle
Categoria	:Gasp Command: *fontSt(phraseTag)
attivita'	:ritorna una stringa di definizione di font.
					 La sintassi � CSS, i valori sono ricavati dalla tabella TransTable 
					 (specificata nel file prm) ed il record � individuato dal valore phraseTag
					 nel campo TransTagField.
					 I campi della tabella che definiscono i valori CSS sono specificati nel
					 file dei parametri e sono FFace1, FFace2, FSize, FStyle, FColor, FDecor,
					 Lheight.
*************************************************************************************/
void * fontStyle(int vuoto1, char *vuoto2, char * inputStr)
{
	char	*phraseTag;
	char	*retString;
	char	queryString[GET_PAR_VAL_LEN];
	int		errCode = 0;
	char	*fFace1buf, *fFace2buf, *fSizebuf, *fStylebuf, *fColorbuf, *fDecBuf, *lHeigBuf;
	char	fontFamily[512], fontSize[512], objColor[512], fontStyle[512], fDecoration[512], lHeight[512];
	char	getContext[GET_PAR_VAL_LEN];
	char	appo[GET_PAR_VAL_LEN];


	if(	!pickPar(	inputStr, 1, &phraseTag		)	)		return(PARAM_NOT_FOUND_MSG);

	if(	strlen(FFace1) == 0 || strlen(FFace2) == 0 || strlen(FSize) == 0 || 
			strlen(FStyle) == 0 || strlen(FColor) == 0 || strlen(FDecor) == 0 || 
			strlen(TransTable) == 0 || strlen(TransTagField) == 0
		)
	{
		fprintf(cgiOut, "%s\n", retMsg(ERR_PARAM_NEEDED) );
		EXIT(1);
	}


	sprintf(queryString, "SELECT %s, %s, %s, %s, %s, %s, %s FROM %s WHERE %s = '%s'", 
	FFace1, FFace2, FSize, FStyle, FColor, FDecor, Lheight, TransTable, TransTagField, phraseTag);

	if ((TargetTplField != 0) && (TplTable != 0) && 
      (TplTableId != 0)     && (TplTableName != 0))
  {
    if ((strcmp(TargetTplField,"")!=0) &&
        (strcmp(TplTable,"")!=0) &&
        (strcmp(TplTableId,"")!=0) &&
        (strcmp(TplTableName ,"")!=0))
    {
			if (strcmp(CurrentTpl[TplNest],"")!=0)
			{
				sprintf(queryString,
					"SELECT f.%s, f.%s, f.%s, f.%s, f.%s, f.%s, f.%s "
					"FROM %s f, %s t "
					"WHERE	f.%s = '%s' "
					"AND		f.%s = t.%s "
					"AND		t.%s = '%s' ",
					 FFace1, FFace2, FSize, FStyle, FColor, FDecor, Lheight, 
					 TransTable, TplTable,
					 TransTagField, phraseTag,
					 TargetTplField, TplTableId,
					 TplTableName, CurrentTpl[TplNest]);
				if (cgiFormString( ContextTag, getContext, 0) == cgiFormSuccess)
				{
					sprintf(appo," AND		t.%s = %s ", ContextField, getContext);
					strcat(queryString, appo);
				}
			}
    }
  }

	newAutoName();
/**/if(usedbg){fprintf(debug, "fontStyle - FFace1 at %d: %s\n", FFace1, FFace1);fflush(debug);}
/**/if(usedbg){fprintf(debug, "fontStyle - queryString: %s\n", queryString);fflush(debug);}
	dbInterface(QueryLabel, queryString, 1, 1);

	fFace1buf = storedData(1, FFace1,	QueryLabel, &errCode);
	fFace2buf = storedData(1, FFace2,	QueryLabel, &errCode);
	fSizebuf	= storedData(1, FSize,	QueryLabel, &errCode);
	fStylebuf = storedData(1, FStyle,	QueryLabel, &errCode);
	fColorbuf = storedData(1, FColor,	QueryLabel, &errCode);
	fDecBuf		= storedData(1, FDecor,	QueryLabel, &errCode);
	lHeigBuf	= storedData(1, Lheight,QueryLabel, &errCode);

	sprintf(fontFamily, " ");
	sprintf(fontSize, " ");
	sprintf(fontStyle, " ");
	sprintf(objColor, " ");
	sprintf(fDecoration, " ");
	sprintf(lHeight, " ");

	if(  (strlen(fFace1buf) > 0 ) )
	{
		sprintf(fontFamily, " font-family: '%s'", fFace1buf);
		if(  strlen(fFace2buf) > 0 )
		{
			strcat(fontFamily, ", '");
			strcat(fontFamily, fFace2buf);
			strcat(fontFamily, "'");
		}
		strcat(fontFamily, ";");
	}

	if(  (strlen(fSizebuf) > 0 ) )
	{
		sprintf(fontSize, " font-size: %spt;", fSizebuf);
	}

	if(  (strlen(fStylebuf) > 0 ) )
	{
		sprintf(fontStyle, " font-style: %s;", fStylebuf);
	}

	if ( strlen(fColorbuf) > 0 )
	{
		sprintf(objColor, " color: #%s; ", fColorbuf);
	}

	if ( strlen(fDecBuf) > 0 )
	{
		sprintf(fDecoration, "text-decoration: %s; ", fDecBuf);
	}

	if ( strlen(lHeigBuf) > 0 )
	{
		sprintf(lHeight, "line-height: %spt; ", lHeigBuf);
	}

	retString = (char *)calloc (
					strlen(fontFamily) + strlen(fontSize) + strlen(fontStyle) + 
					strlen(objColor) + strlen(fDecoration) + strlen(lHeight) + 64,
					sizeof(char)
					);

	sprintf(retString, " style=\"%s %s %s %s %s %s\"",
		fontFamily, fontSize, fontStyle, objColor, fDecoration, lHeight);

	return (retString);
}


/*************************************************************************************
NOME			:revealBrowser
Categoria	:Gasp Command: *revBr()
attivita'	:scrive 'explorer' se il browser si identifica come MSIE altrimenti netscape
*************************************************************************************/
void * revealBrowser(int vuoto1, char *vuoto2, char * inputStr)
{
	if (strstr(cgiUserAgent, "MSIE") )
		return ISEXPLORER;
	else
		return ISNETSCAPE;
}

/*************************************************************************************
NOME			:writeSpan
Categoria	:Gasp Command: *span(phraseTag)
attivita'	:� equivalente a
						<SPAN *fontST(phraseTag)>*trans(phraseTag)</SPAN>
*************************************************************************************/
void * writeSpan(int vuoto1, char *vuoto2, char * inputStr)
{
	char	*phraseTag, *fontSt, *transPh, *retString;

//**/if(usedbg){fprintf(debug, "writeSpan:	STARTING\n");fflush(debug);}
	if(	!pickPar(	inputStr, 1, &phraseTag		)	)		return(PARAM_NOT_FOUND_MSG);


	fontSt = (char*) fontStyle(0, 0, phraseTag);
/**/if(usedbg){fprintf(debug, "TargetTplField B- %s\n", TargetTplField);fflush(debug);}
	transPh = (char*) translateIt(0, 0, phraseTag);
/**/if(usedbg){fprintf(debug, "TargetTplField C- %s\n", TargetTplField);fflush(debug);}

	retString = (char*) calloc(strlen(fontSt) + strlen(transPh) + 64, sizeof(char) );

	sprintf(retString, " <SPAN %s>%s</SPAN>", fontSt, transPh);

	return (retString);
}

/*************************************************************************************
NOME			:traceUser
Categoria	:Gasp Command: *traceU(feature)
attivita'	:restituisce informazioni sul client a seconda del parametro feature.
					 feature=brname->nome del browser
					 feature=brver ->versione del browser (solo parte intera)
					 feature=osname->nome del sistema operativo
					 feature=ipnum ->numero ip
					 feature=ipname->nome, quando risolvibile, della macchina remota
*************************************************************************************/
void * traceUser(int vuoto1, char *vuoto2, char * inputStr)
{
	char *feature;
	char *cursor;
	static char brname[128], brver[128], osname[128];

/**/if(usedbg){fprintf(debug, "traceUser:	STARTING\n");fflush(debug);}

	memset(brname, '\0', 128);
	memset(brver, '\0', 128);
	memset(osname, '\0', 128);

/**/if(usedbg){fprintf(debug, "traceUser:	1\n");fflush(debug);}
	if(	!pickPar(	inputStr, 1, &feature		)	)		return(PARAM_NOT_FOUND_MSG);

	cursor = strstr(cgiUserAgent, "Mozilla/");
/**/if(usedbg){fprintf(debug, "traceUser:	2\n");fflush(debug);}

	if (cursor)
	{
/**/if(usedbg){fprintf(debug, "traceUser:	3\n");fflush(debug);}
		if (cursor=strstr(cgiUserAgent, "MSIE"))
		{
/**/if(usedbg){fprintf(debug, "traceUser:	4\n");fflush(debug);}
			strcpy(brname, BROWSER_MSIE);
			brver[0] = cursor[5];
			if(strstr(cgiUserAgent, "Windows 98"))
				strcpy(osname, "MS Windows 98");
			else if(strstr(cgiUserAgent, "Windows 95"))
				strcpy(osname, "MS Windows 95");
			else if(strstr(cgiUserAgent, "Windows NT"))
				strcpy(osname, "MS Windows NT");
			else if(strstr(cgiUserAgent, "Windows 3.1"))
				strcpy(osname, "MS Windows 3.1");
			else if(strstr(cgiUserAgent, "Mac_"))
				strcpy(osname, "Apple Macintosh");
		}
		else if (cursor=strstr(cgiUserAgent, "Opera"))
		{
/**/if(usedbg){fprintf(debug, "traceUser:	5\n");fflush(debug);}
			strcpy(brname, BROWSER_OPERA);
		}
		else
		{
/**/if(usedbg){fprintf(debug, "traceUser:	6\n");fflush(debug);}
			strcpy(brname, BROWSER_NETSCAPE);
/**/if(usedbg){fprintf(debug, "traceUser:	7\n");fflush(debug);}
			brver[0] = cgiUserAgent[8];
/**/if(usedbg){fprintf(debug, "traceUser:	8\n");fflush(debug);}
			if(strstr(cgiUserAgent, "Win98"))
				strcpy(osname, "MS Windows 98");
			else if(strstr(cgiUserAgent, "Win95"))
				strcpy(osname, "MS Windows 95");
			else if(strstr(cgiUserAgent, "WinNT"))
				strcpy(osname, "MS Windows NT");
			else if(strstr(cgiUserAgent, "Win16"))
				strcpy(osname, "MS Windows 3.1");
			else if(strstr(cgiUserAgent, "Macintosh"))
				strcpy(osname, "Macintosh");
			else if(strstr(cgiUserAgent, "Linux"))
				strcpy(osname, "Linux");
			else if(strstr(cgiUserAgent, "SunOS"))
				strcpy(osname, "SunOS");
/**/if(usedbg){fprintf(debug, "traceUser:	9\n");fflush(debug);}
		}
	}

/**/if(usedbg){fprintf(debug, "traceUser:	RETURNING\n");fflush(debug);}
	if (strcmp(feature,"brname")==0)
		return brname;
	else if (strcmp(feature,"brver")==0)
		return brver;
	else if (strcmp(feature,"osname")==0)
		return osname;
	else if (strcmp(feature,"ipnum")==0)
		return cgiRemoteAddr;
	else if (strcmp(feature,"ipname")==0)
		return cgiRemoteHost;

	return 0;
}

/*************************************************************************************
NOME			:exeQnew
*************************************************************************************/
void* exeQnew(int vuoto1,char *vuoto2, char * inputStr)
{
	char *queryName, *queryString, *firstRecord, *recsToStore;
	int firstRecInt = STARTING_ROW, recsToStoreInt = ROWS_TO_STORE;

//**/if(usedbg){fprintf(debug, "execQuery-inputStr:%s\n", inputStr);fflush(debug);}

	if(!pickPar(inputStr, 1, &queryName)   )	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 2, &queryString) )	return(PARAM_NOT_FOUND_MSG);
	if( pickPar(inputStr, 3, &firstRecord) )	firstRecInt = atoi(firstRecord);
	if( pickPar(inputStr, 4, &recsToStore) )	recsToStoreInt = atoi(recsToStore);

	remBlindingChar(queryString);

  ITannit_Create(QueryResultSet, &QueryCounter);
  ITannit_ExecuteSQLQuery(Odbcdsn, Odbcuid, Odbcpwd, queryName, queryString, firstRecInt, recsToStoreInt);
  
  //if (ITannit_MoreResultSQL())
  ITannit_Dump(debug);

  /**/if(usedbg) ITannit_ErrorSQL(debug);
  /**/if(usedbg){fprintf(debug, "execQuery: dati archiviati;\n");fflush(debug);}
	return 	0;
}


void* dbgmsg(int vuoto1,char *vuoto2, char * inputStr)
{
  /**/if(usedbg){fprintf(debug, "Debug Message:%s\n", inputStr);fflush(debug);}
	return 	0;
}


