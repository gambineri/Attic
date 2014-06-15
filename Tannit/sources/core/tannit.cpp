/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/*****************************************************************************
                                 AITECSA S.R.L.
 - PROJECT			: itxWeb - Tannit
 - FILENAME			: tannit.c
 - TAB					: 2, no spaces
 - DESCRIPTION	:	funzioni principali
 *****************************************************************************/

#include	"tannit.h"
#include	"itannitc.h"

char	BgMain[PAR_VALUE_LEN], BgMenu[PAR_VALUE_LEN], BgTop[PAR_VALUE_LEN], BgLeft[PAR_VALUE_LEN];
char	BgRight[PAR_VALUE_LEN], BgBott[PAR_VALUE_LEN], TplDir[PAR_VALUE_LEN], CrudeTplDir[PAR_VALUE_LEN], WebHome[PAR_VALUE_LEN];
char	CgiDir[PAR_VALUE_LEN] ,CgiName[PAR_VALUE_LEN], PrepropKey[PAR_VALUE_LEN], PreviewKey[PAR_VALUE_LEN], NormviewKey[PAR_VALUE_LEN], PrepKeyTag[PAR_VALUE_LEN];
char	ImgDir[PAR_VALUE_LEN], FileDir[PAR_VALUE_LEN], Odbcdsn[PAR_VALUE_LEN], Odbcuid[PAR_VALUE_LEN];
char	Odbcpwd[PAR_VALUE_LEN], WebUrl[PAR_VALUE_LEN], LoginTable[PAR_VALUE_LEN], PwdField[PAR_VALUE_LEN];
char	LoginField[PAR_VALUE_LEN], ExtrTable[PAR_VALUE_LEN], IdField[PAR_VALUE_LEN], ExtrField[PAR_VALUE_LEN];
char	LangTagGet[PAR_VALUE_LEN], LangTable[PAR_VALUE_LEN], LangNameField[PAR_VALUE_LEN], LangCodeField[PAR_VALUE_LEN];
char	TransTable[PAR_VALUE_LEN], TransTagField[PAR_VALUE_LEN], DefaultLanguageId[PAR_VALUE_LEN];
char  UploadDir[PAR_VALUE_LEN],AllowUpload[PAR_VALUE_LEN];
char  CorrTable[PAR_VALUE_LEN], CorrUserField[PAR_VALUE_LEN], CorrCodeField[PAR_VALUE_LEN];
char  CorrAppDir[PAR_VALUE_LEN], CorrFileName[PAR_VALUE_LEN], CorrDndStatus[PAR_VALUE_LEN];
char  CorrDnDir[PAR_VALUE_LEN], Llu[PAR_VALUE_LEN], Rtp[PAR_VALUE_LEN];
char	FFace1[PAR_VALUE_LEN], FFace2[PAR_VALUE_LEN], FSize[PAR_VALUE_LEN], FStyle[PAR_VALUE_LEN], FColor[PAR_VALUE_LEN], FDecor[PAR_VALUE_LEN], Lheight[PAR_VALUE_LEN];
char  DbgPath[PAR_VALUE_LEN], CooTimeDelay[PAR_VALUE_LEN], CooURLEscape[PAR_VALUE_LEN];
char  TargetTplField[PAR_VALUE_LEN],TplTable[PAR_VALUE_LEN],TplTableId[PAR_VALUE_LEN],	TplTableName[PAR_VALUE_LEN];
char  CurrentTpl[MAX_TPL_NESTS][TPL_NAME_LEN], ContextField[PAR_VALUE_LEN],ContextTag[PAR_VALUE_LEN];
char  ForbiddenChars[PAR_VALUE_LEN];
// dopo il porting, da riaggiornare
char  SSDir[PAR_VALUE_LEN];


int		TntRequestType = RT_BROWSE, TplNest;

TannitQueryResult* QQres = NULL;

int		Rumble;
char	QueryLabel[128];

bool	usedbg, alloctrc;

char		UserAgentSpy[256];
struct	QueryResult * QueryResultSet[QUERY_NUMBER];

MessagesSt Message[] = {
	{ERR_COL_NOT_FOUND			, "Error: field not found"										},
	{ERR_QUERY_NOT_FOUND		, "Error: query not found"										},
	{ERR_VOID_QUERY_NAME		, "Error: unable to read query name"					},
	{LOGIN_ERROR						, "Login Error"																},
	{ERR_PARAM_NEEDED				, "Error: parameter needed"										},
	{0		,	""																														}
};

StrStack *   cycleStk;
StrStack *   queryStk;
TplVarsStrct	TplVars;
int		QueryCounter;
int		ReadCycle[CYC_NESTING_DEPTH];
int		ValidBlock[CYC_NESTING_DEPTH];
int		CndLevel, CycLevel;
int		LockLoop;
FILE *debug = NULL;


#ifdef _DEBUG
#include <windows.h> //Serve per la sleep
#endif

void EXIT(int retval)
{
  ITannit_DisconnectSQL();
  exit(retval);
}

/*************************************************************************************
NOME			:cgiMain
attivita'	:presenta l'header al web server; guida la scansione del template HTML
chiamante	:main
*************************************************************************************/
int cgiMain()
{
	char *tplString = 0;			// buffer dove verranno messi i dati grezzi del template
	char *outputString = 0;		// buffer utilizzato da procData per appoggiare i dati interpretati del template
	char	excngFileOk[30];
	FILE * refinedTplFp = 0;

#ifdef _DEBUG
//*
int p=1;
while (p)
  Sleep(1);
//*/
#endif

	/**/if(usedbg){fprintf(debug, "cgiMain - InitGlobals\n");fflush(debug);}
	// inizializzazione dele variabili globali
	initGlobals();

	/**/if(usedbg){fprintf(debug, "cgiMain - readIniPars\n");fflush(debug);}
	// lettura dei parametri definiti nel file di inizialiazione
	if(readIniPars() == 0)
	{
//    cgiHeaderContentType("text/html");
//		fprintf(cgiOut,"%s\n", ERR_OPEN_INI_FILE);//fflush(cgiOut);
		exit(__LINE__);
	}

/*
  // cattura del processo nel caso si debba effettuare una operazione
	// di scambio di file
	if (cgiFormString(EXCNG_FILE_TAG, excngFileOk, GET_VAR_LEN) == cgiFormSuccess)
	{
  	if (strcmp(excngFileOk, SEND_FILE_GRANT) == 0)
	  {
			int retflag;
      if ( (retflag = sendFile()) == 1) EXIT(__LINE__);
	  }
		else if (strcmp(excngFileOk, RECEIVE_FILE_GRANT) == 0)
		{
			receiveFile();
		}
		else if (strcmp(excngFileOk, RECEIVE_APPC_FILE_GRANT) == 0)
		{
			receiveAPPCFile();
		}
		else if (strcmp(excngFileOk, DND_CONFIRM_GRANT) == 0)
		{
			dndConfirm();
		}
	}
*/
	/**/if(usedbg){fprintf(debug, "cgiMain - cgiHeaderContentType\n");fflush(debug);}
	cgiHeaderContentType("text/html");

	/**/if(usedbg){fprintf(debug, "cgiMain - verVersion\n");fflush(debug);}
	// verifica versione
	verVersion();


	/**/if(usedbg){fprintf(debug, "cgiMain - PREPROCESSING\n");fflush(debug);}
	// SWITCH per il controllo del PREPROCESSING:
	TntRequestType = capturePreproc(&tplString, &refinedTplFp);
	if((TntRequestType == RT_PREPROC) || (TntRequestType == RT_PREPROC_ALL))
	{
	/**/if(usedbg){fprintf(debug, "cgiMain - RT_PREPROC\n");fflush(debug);}
		strcpy(TplDir, CrudeTplDir);
		cgiOut = refinedTplFp;
	}
	else if (TntRequestType == RT_PREVIEW)
	{
	/**/if(usedbg){fprintf(debug, "cgiMain - RT_PREVIEW\n");fflush(debug);}
		strcpy(TplDir, CrudeTplDir);
	}

	/**/if(usedbg){fprintf(debug, "cgiMain - TplDir:%s\n", TplDir);fflush(debug);}

	// apertura del template, allocazione e copia dei dati in tplString
  if( bufferTpl(&tplString, 0, TplDir) == 0)
	{
		fprintf(cgiOut,"%s\n", ERR_OPEN_TPL_FILE); //fflush(cgiOut);
		exit(__LINE__);
	}

	// interpretazione dei dati e scrittura su cgiOut
	procTplData(tplString, cgiOut, &outputString);


	if((TntRequestType == RT_PREPROC) || (TntRequestType == RT_PREPROC_ALL))
	{
		fprintf(stdout,"%s", TPL_UPD_COMM_SUCC);fflush(stdout);
	}

	/**/if(usedbg){fprintf(debug, "Exiting cgiMain\n");fflush(debug);}

  ITannit_DisconnectSQL();

	return 1;
}



void dumpAround(char * refPtr, int strtChr, int chrnum)
{
/**/if(usedbg){fprintf(debug, "\n START OF MEMORY DUMP------------------------\n");fflush(debug);}
	int i = 0;
	for (i = 0; i < chrnum; i++)
	{
/**/if(usedbg){fprintf(debug, "%c", *( refPtr + strtChr + i) );fflush(debug);}
	}
/**/if(usedbg){fprintf(debug, "\n END OF MEMORY DUMP------------------------\n");fflush(debug);}
}


/*************************************************************************************
NOME			:queryIndex
attivita'	:scorre l'array di puntatori a QueryResult fino a trovare quello che ha l'id
					 (label) uguale alla stringa di input
return		:l'indice della query identificata da queryId
*************************************************************************************/
int queryIndex(char * queryId, int noexit)
{
	int qIndex = -1;
	if (queryId)
	{
		for (qIndex = 0; qIndex < QUERY_NUMBER; qIndex++)
		{
//**/if(usedbg && noexit){fprintf(debug, "queryIndex. queryId: %s; qIndex: %d;\n", queryId, qIndex);fflush(debug);}
			if (QueryResultSet[qIndex])
			{
				if (strcmp(QueryResultSet[qIndex]->id,queryId) == 0)
				{
					break;
				}
			}
		}
		if (qIndex == QUERY_NUMBER)
		{
			if (noexit)
			{
				return -1;
			}
			else
			{
				fprintf(cgiOut, "%s\n", retMsg(ERR_QUERY_NOT_FOUND) );
				EXIT(1);
			}
		}
	}
	return(qIndex);
}


/*************************************************************************************
NOME			:listTQRnames
attivita'	:scorre l'array di puntatori a QueryResult e stampa in debug i nomi
*************************************************************************************/
void listTQRnames()
{
	int qIndex = -1;
	for (qIndex = 0; qIndex < QUERY_NUMBER; qIndex++)
	{
		if (QueryResultSet[qIndex]->id)
		{
/**/if(usedbg){fprintf(debug, "listTQRnames :%d-%s\n", qIndex, QueryResultSet[qIndex]->id);fflush(debug);}
		}
	}
}


int pushStk (StrStack** stack,char *strg)
{
	StrStack *new_element;
	int       str_len;

	str_len=strlen(strg);

	if ( !( new_element = (StrStack*) malloc(sizeof(StrStack)) ) )
	{
		return 0;
	}
/**/if(alloctrc){fprintf(debug, "- - new_element:%d - - len:%d\n", new_element, sizeof(StrStack) );fflush(debug);}
///**/fprintf(cgiOut, "<br>strg=%d<br>", strg);
	new_element->item = strg;
	new_element->next = *stack;

	*stack = new_element;

	return 1;
}



int popStk  (StrStack** stack,char **strg)
{
	StrStack  *appo;

	if (!*stack)
	{
		*strg = 0;
		return 0;
	}

	appo = (*stack)->next;
	*strg = (*stack)->item;
	*stack = appo;
	return 1;
}


void trimIt(char * inputStr)
{
	int		j,i;
	char	*newStr;

	newStr = (char*) malloc( strlen(inputStr) * sizeof(char)  );if(!newStr) EXIT(23);
/**/if(alloctrc){fprintf(debug, "- - newStr:%d - - len:%d\n", newStr, strlen(inputStr) * sizeof(char) );fflush(debug);}

	for (j = 0; (inputStr[j] && inputStr[j]==' '); ++j);
	for (i = strlen(inputStr); ( i > 0 && inputStr[i-1]==' '); --i);

	if(j < i)
	{
		strncpy(newStr,&inputStr[j],i-j);
		newStr[i-j]=0;
		strcpy (inputStr, newStr);
	}
	else
	{
		inputStr = 0;
	}
}

/*************************************************************************************
NOME			:verVersion
attivita'	:nel caso nella stringa di get vi sia il tag che identifica la richiesta di
					 informazioni queste vengono restituite sull'output
return		:l'id del tipo di informazione richiesta
chiamante	:cgiMain
*************************************************************************************/
int verVersion()
{
	int infoId = 0;

	if (cgiFormInteger(INFO_TAG, &infoId, INFO_ID)==cgiFormSuccess)
	{
		switch (infoId)
		{
			case APPC_MSG_ID:
#ifdef _ITX_APPC
				fprintf(cgiOut, "%s\n", APPC_MSG);//fflush(cgiOut);
#endif
			case AUTHOR:
				fprintf(cgiOut, "%s\n", AUTHOR_MSG);//fflush(cgiOut);
			case OWNER:
				fprintf(cgiOut, "%s\n", OWNER_MSG);//fflush(cgiOut);
			case VERSION:
				fprintf(cgiOut, "%s\n", VERSION_MSG);//fflush(cgiOut);
		}
	}
	return infoId;
}


/*************************************************************************************
NOME			:getline
attivita'	:prende una linea di caratteri
return		:il numero di caratteri presi
chiamante	:tanti
*************************************************************************************/
int getline(char s[], int lim)
{
  int c, i=0, num;

//  for (i=0; (i<lim) && ((c=getchar())!=EOF) && (c!='\n'); i++) 
  for (i=0; (i<lim); i++) 
  {
    if ( (c=getchar()) == EOF ) 
      return EOF;
    if (c =='\n')
     break;
    s[i] = c;
  }
  if (c == '\n') {
    s[i] = c;
  }
  if ((i==0) && (c!='\n'))
    num = 0;
  else if (i == lim)
    num = i;
  else
    num = i+1;
  return num;
}


/*************************************************************************************
NOME			:checkDbPwd
attivita'	:confronta la password nel database (decriptata) con la password di input
return		:il numero di caratteri presi
chiamante	:lo stato di validazione
*************************************************************************************/
int checkDbPwd(char* login, char* pwd, char* extraField, char* extraVal)
{
	char dbClearPwd[256];
	char queryString[1024];
	char *dbPwd;
	int errCode;

  // Connessione al DB e acquisizione del valore della password
	sprintf(queryString, "SELECT %s FROM %s WHERE %s = '%s' AND %s = %s",
    PwdField, LoginTable, LoginField, login, extraField, extraVal);

	newAutoName();
	dbInterface(QueryLabel, queryString, 1, 1);
	dbPwd = storedData(1, PwdField, QueryLabel, &errCode);

  if ( errCode == ERR_COL_NOT_FOUND   ||
       errCode == ERR_QUERY_NOT_FOUND ||
       errCode == ERR_VOID_QUERY_NAME     )
		return errCode;

  if (dbPwd == NULL)									return NOT_VALIDATED;
  if (strcmp(dbPwd,REC_VAL_ZERO)==0)  return NOT_VALIDATED;

/**/if(usedbg){fprintf(debug, "decripting\n");fflush(debug);}
/**/if(usedbg){fprintf(debug, "dbPwd---%s\n", dbPwd);fflush(debug);}
//  tannitDecrypt(dbPwd, dbClearPwd);
/**/if(usedbg){fprintf(debug, "dbClearPwd---%s\n", dbClearPwd);fflush(debug);}
/**/if(usedbg){fprintf(debug, "pwd---%s\n", pwd);fflush(debug);}

  tannitDecrypt(dbPwd, dbClearPwd);

  if ( strcmp(dbClearPwd, pwd) != 0 )			return NOT_VALIDATED;

  return VALIDATED;
}


char * retMsg(int msgId)
{
	int i = 0;
	while (Message[i].msgId)
	{
		if ( Message[i].msgId == msgId )
		{
			break;
		}
		i++;
	}
	return Message[i].msg;
}


void newAutoName()
{
	sprintf(QueryLabel,"Auto%d", Rumble++);
}


int capturePreproc(char ** tplString, FILE ** refinedTplFp)
{
	char	userPKey[TPL_NAME_LEN];
	char	tplName[TPL_NAME_LEN];
	char	tplAbsName[TPL_NAME_LEN];
	char	*outputString = 0;		// buffer utilizzato da procData per appoggiare i dati interpretati del template

  /**/if(usedbg){fprintf(debug, "capturePreproc processing: START\n");fflush(debug);}

	// se si trova la key che comanda il preprocessing nella stringa di get
	// i teplate vengono letti nella directory CrudeTplDir
	if (cgiFormString(PrepKeyTag, userPKey, TPL_NAME_LEN) == cgiFormSuccess)
	{
		if (strcmp(userPKey, PrepropKey) == 0 )
		{
			if	(cgiFormString(TGT_TAG, tplName, TPL_NAME_LEN) == cgiFormSuccess)
			{
				sprintf(tplAbsName,"%s\\%s%s",TplDir, tplName, TPL_EXT);
				if ( ( *refinedTplFp = fopen(tplAbsName,"w") ) != 0 )
				{
					/**/if(usedbg){fprintf(debug, "capturePreproc processing: return RT_PREPROC_ALL\n");fflush(debug);}
					fprintf(stdout,"%s ", TPL_UPD_COMM_RECV);fflush(stdout);
					fprintf(stdout, "<font color=white>%s</font>", tplName);fflush(stdout);
					return RT_PREPROC_ALL;
				}
			}
			else if (cgiFormString(TPL_TAG, tplName, TPL_NAME_LEN) == cgiFormSuccess)
			{
				sprintf(tplAbsName,"%s\\%s%s",TplDir, tplName, TPL_EXT);
				if ( ( *refinedTplFp = fopen(tplAbsName,"w") ) != 0 )
				{
					/**/if(usedbg){fprintf(debug, "capturePreproc processing: return RT_PREPROC\n");fflush(debug);}
					fprintf(stdout,"%s ", TPL_UPD_COMM_RECV);fflush(stdout);
					fprintf(stdout, "<font color=white>%s</font>", tplName);fflush(stdout);
					return RT_PREPROC;
				}
			}
		}
		else if (strcmp(userPKey, PreviewKey) == 0 )
		{
			/**/if(usedbg){fprintf(debug, "capturePreproc processing: return RT_PREVIEW\n");fflush(debug);}
			return RT_PREVIEW;
		}
		else if (strcmp(userPKey, NormviewKey) == 0 )
		{
			/**/if(usedbg){fprintf(debug, "capturePreproc processing: return RT_BROWSE\n");fflush(debug);}
			return RT_BROWSE;
		}
		else EXIT(0);
	}

  /**/if(usedbg){fprintf(debug, "capturePreproc processing: return RT_BROWSE\n");fflush(debug);}
	return RT_BROWSE;
}


