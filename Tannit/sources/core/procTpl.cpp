/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/*****************************************************************************
                                 AITECSA S.R.L.
 - PROJECT			: itxWeb - Tannit
 - FILENAME			: procTpl.c
 - TAB					: 2, no spaces
 - DESCRIPTION	:	gestione del parsing dei template
 *****************************************************************************/
#include	"tannit.h"
#include "extVars.h"

#define START_CYCLE				1
#define END_CYCLE				2
#define START_CND_BLK			3
#define ELSE_CND_BLK			4
#define END_CND_BLK				5
#define PREPROC_IT				6
#define NEVER_PREPROC_IT	    7

typedef struct
{
	char *cmdName;
//  int cmdFunction;
	void * (* cmdFunction)( int, char *, char * );
	int cmdType;
} CommandStructure;


CommandStructure CmdStruct[] = {
	{"exeQ",						execQuery,					0},
	{"_exeQ",						execQuery,					PREPROC_IT},
	{"exeIQ",						execInsQuery,				0},
	{"remQ",						remQuery,						0},
	{"totRecs",					writeTotalRecords,	0},
	{"actRec",					writeActualRec,			0},
	{"maxRecs",					writeMaxRecords,		0},
	{"firstRec",				writeFirstRecord,		0},
	{"moreRecs",				moreRecsMsg,				0},
	{"recVal",					recVal,							0},
	{"_recVal",					recVal,							PREPROC_IT},
	{"recCnd",					recValCnd,					0},
	{"_recCnd",					recValCnd,					PREPROC_IT},
	{"recSEL",					recValConf,					0},
	{"recLookS",				recValLookAsStr,		0},
	{"cycleQ",					cycleQuery,					START_CYCLE},
	{"endCQ",						endCycleQuery,			END_CYCLE},
	{"setRec",					setActualRow,				0},
	{"if",							startCndBlk,				START_CND_BLK},
	{"ifblk",						startCndBlk,				START_CND_BLK},
	{"^ifblk",					startCndBlk,				START_CND_BLK},
	{"else",						elseCndBlk,					ELSE_CND_BLK},
	{"elseblk",					elseCndBlk,					ELSE_CND_BLK},
	{"^elseblk",				elseCndBlk,					ELSE_CND_BLK},
	{"endif",						endCndBlk,					END_CND_BLK},
	{"endblk",					endCndBlk,					END_CND_BLK},
	{"^endblk",					endCndBlk,					END_CND_BLK},
	{"WEBURL",					writeWebUrl,				PREPROC_IT},
	{"WEBROOT",					writeHomeDir,				PREPROC_IT},
	{"cgipath",					writeCgiPath,				PREPROC_IT},
	{"IMGDIR",					writeImgDir,				PREPROC_IT},
	{"SSDIR",						writeSSDir,					PREPROC_IT},
	{"get",							getValue,						0},
	{"^get",						getValue,						0},
	{"_get",						getValue,						PREPROC_IT},
	{"getmad",					getmad, 						0},
	{"encr",						encrIt,							0},//deprecated
	{"auth",						authorize,					0},//deprecated
	{"crypt",						crypt,							0},
	{"decr",						decrypt,						0},
	{"valid",						validate, 					0},//validazione utenti
	{"validAleph", 			validAleph,       	0},//validazione utenti per il client Aleph
	{"exof",						exitOnFile,					0},
	{"chkIt",						syntChk,						0},
	{"setVar",					setVar,							0},
	{"getVar",					getVar,							0},
	{"getId",						extrIdG,						0},
	{"extId",						extrId,							0},
	{"trans",					  translateIt,				PREPROC_IT},
	{"prex",						processExternFile,	PREPROC_IT},
	{"^prex",						processExternFile,	0},
	{"setCoo",					setCookie,					0},
	{"CrAppDir",				writeCorrAppDir,		0},
	{"CrFileName",			writeCorrFileName,	0},
	{"inDomain",   			isInTheDomain,    	0},
	{"fontSt",	   			fontStyle,		    	PREPROC_IT},
	{"revBr",	   				revealBrowser,    	0},
	{"^revBr",	   			revealBrowser,    	0},
	{"StoreTQR",				StoreTQR,				    0},
	{"TQRInsert",				TQRInsert,			    0},
	{"getP00",				  getP00,   			    0},
	{"exeTP",				    exeTP,    			    0},
	{"fillmsk",				  fillmsk,   			    0},
	{"span",					  writeSpan, 			    PREPROC_IT},
	{"verCoo",					verCookie, 			    0},
	{"now",							now,			 			    0},
	{"exist",						exist,		 			    0},
	{"fconn",						firstConn,		 			0},
	{"ckcli",						ckcli,    		 			0},
	{"fddate",					formatDimDate, 			0},
	{"splitREA",				splitREA,			 			0},
	{"dimDataMagic",		dimDataMagic,	 			0},
	{"sottra",					subtractIt,		 			0},
	{"sumf",						sumFloat,			 			0},
	{"parval",					parval,				 			PREPROC_IT},
	{"tgt",							switchTplTgt,				PREPROC_IT},
	{"filtTqr",					filterTqr,				  0},
	{"gethide",					putGetHide, 				0},
	{"tqrval",					setTQRValues, 			0},
	{"pkdate",					pkdate, 						0},
	{"tunnel",					prmTunnel,					0},
	{"dfcur",						dimFormatCurrency,	0},
	{"divint",					divideInt,					0},
	{"fdcap",						formatDimCap,				0},
	{"fdnace",					formatDimNACE,			0},
	{"fdvia",						formatDimVia,				0},
	{"ucase",						uppercaseIt,				0},
	{"tplname",					putTplName,					0},
	{"fdnaz",						formatDimNaz,				0},
// vers 2.7 specific
	{"invoke",					invoke,     				0},
	{"traceU",					traceUser,					0},
	{"execmd",					execmd,     				0},
	{"getcoo",					getcoo,     				0},
	{"rtrim",					  rtrim,      				0},
	{"trim",					  trim,       				0},
	{"cfc",					    cfc,      			  	0},
	{"flush",					  flush,     			  	0},
	{"uTime",					  uTime,     			  	0},
	{"uTimeToDMY",		  uTimeToDMY,     		0},
	{"exeQnew",		      exeQnew,     		    0},
	{"olimpybet",		    olimpybet,     		  0},
	{"dbgmsg",  		    dbgmsg,        		  0},
	{"quotestr",  		  quotestr,      		  0},
	{NULL,							NULL,								0}
};

/*************************************************************************************
NOME			:bufferTpl
attivita'	:ricava il nome del template dal parametro di get TPL_TAG, lo combina con
					 l'estensione di default TPL_EXT e lo concatena con il nome della directory
					 in cui si trova il template individuata dalla funzione fillExtPars;
					 apre il file individuato, alloca tplString e vi scrive i dati del file;
					 se � specificato optFile lo usa al posto di quello specificato nella
					 stringa di get (per ora usa sempre la estensione di default).
					 tplDir e' la directori dove prndere il template;
*************************************************************************************/
int bufferTpl(char ** tplString, char * optFile, char * tplDir)
{
	int		tplLen=0;
	long int contLen = 0;
	char	* dataBuffer = NULL;
	char	tplAbsName[TPL_ABS_NAME_LEN];
	char	tplName[TPL_NAME_LEN];
	FILE	*fTemplate;

  // se non e' assegnato il file da usare come template si chiede alla url il
  // parametro TPL_TAG per la definizione del nome del file; se non si trova
  // si assegna il default template

	/**/if(usedbg){fprintf(debug, "BUFFERTPL : STARTING\n");fflush(debug);}
  if (optFile)
		strcpy(tplName, optFile);
	else if (cgiFormString(TPL_TAG, tplName, TPL_NAME_LEN) != cgiFormSuccess)
		strcpy(tplName, DEFAULT_TPL);

	if(strpbrk( tplName, "./\\" ))
		EXIT(-1);

	memset(CurrentTpl[TplNest],0,TPL_NAME_LEN);
	strcpy(CurrentTpl[TplNest], tplName);

  // si compone il tpl con il nome della directory di origine
  // e con la estensione di default (TPL_EXT)
	sprintf(tplAbsName,"%s\\%s%s",tplDir, tplName, TPL_EXT);
//	sprintf(tplAbsName,"%s\\%s",tplDir, tplName);

  
	/**/if(usedbg){fprintf(debug, "BUFFERTPL : TEMPLATE %s\n", tplAbsName);fflush(debug);}
  // apertura e controllo del template da interpretare
	if ( ( fTemplate = fopen(tplAbsName,"r") ) == 0 )
		return 0;


  //
  // Valutazione della Content-length
  //
  fseek(fTemplate, 0, SEEK_SET);
  contLen = ftell(fTemplate);
  fseek(fTemplate, 0, SEEK_END);
  contLen = ftell(fTemplate) - contLen;
  rewind(fTemplate);

  dataBuffer = (char*) calloc(contLen + 4096, sizeof(char) );if ((dataBuffer) == 0) EXIT(MEMERR);
	/**/if(alloctrc){fprintf(debug, "+ + dataBuffer:%d + + len:%d\n", dataBuffer, contLen + 4096 );fflush(debug);}

  // si copiano tutti i caratteri del template su dataBuffer
	while ((dataBuffer[tplLen++] = fgetc(fTemplate)) != EOF);
	dataBuffer[tplLen-1]=0; // sostituzione del carattere EOF con 0

  *tplString = (char*) malloc(tplLen * sizeof(char) );if ((*tplString) == 0) EXIT(MEMERR);
	/**/if(alloctrc){fprintf(debug, "+ + *tplString:%d + + len:%d\n", *tplString, tplLen * sizeof(char) );fflush(debug);}
	if (dataBuffer == 0) return 0;
	strcpy(*tplString, dataBuffer);

	/**/if(usedbg){fprintf(debug, "BUFFERTPL : ENDING\n");fflush(debug);}
	return 1;
}


/*************************************************************************************
NOME			:verifyCmdSynt
stato			:NUOVA. da completare con un check congruo per la sintassi dei comandi
attivita'	:verifica che i primi caratteri di *tmplString siano quelli di uno dei
					 comandi e verifica la sintassi. cmdOrd identifica la posizione del comando
					 nell'array; argStr viene indirizzato su tplString all'inizio dell'argomento;
					 le stringhe vengono terminate ponendo a zero i caratteri di inizio e fine
					 argomento; tplString viene spostato alla fine del comando
return		:se *tmplString inizia con un comando restituisce 1, se non sono verificate le 
					 condizioni sintattiche torna 0, se non � stato chiuso l'argomento torna -1
chiamante	:procTplData
chiama		:
note			:
*************************************************************************************/
int verifyCmdSynt(char ** tplString, int * cmdOrd, char ** argStr, char ** cmdStart)
{
	int insideArg = 0;
	int argTagNestingLev=-1;
	int canBeCommand=1;
	int cmdLen = 0;
	int argLen = 0;
	int i=0;
	int cmdArgNotClosed=0;
	char * holdTplString;
	char * tmpArgP;
	char strToEval[CMD_LEN+1];

	// memorizzo la posizione di partenza della stringa e
	// copio in un buffer un numero di caratteri della stringa pari al massimo numero
	// consentito per il nome di un comando (+ il terminator)
	holdTplString = *tplString;
	strncpy(strToEval, *tplString+1, CMD_LEN+1);

	// prima condizione da soddisfare: devo trovare il carattere di inizio argomento
	for (cmdLen=0; cmdLen<=CMD_LEN+1; cmdLen++)
	{
		// la funzione � scettica: presuppone che la stringa non sia un comando
		canBeCommand=0;
		// se il carattere cmdLen � quello di inizio argomento..
		if (strToEval[cmdLen]==CMD_ARG_OPEN_TAG)
		{
			// se viene trovato il carattere di inizio argomento altrove che nel primo
			// carattere della stringa questa pu� essere un comando (altrimenti no,
			// l'argomento deve essere preceduto da un nome)
			if (cmdLen)
			{
				// l'argomento punta alla prima parentesi
				tmpArgP = *tplString + cmdLen + 1;
				// termino il buffer mettendo a zero il carattere dove ho trovato l'inizio argomento
				strToEval[cmdLen]=0;
				// sono all'interno dell'argomento
				insideArg=1;
				// sono entrato nel livello zero di annidamento parentesi
				argTagNestingLev=0;
				// condizione soddisfatta
				canBeCommand=1;
			}
			break;
		}
	}

	// se le condizioni fin qui richieste sono state soddisfatte
	if (canBeCommand)
	{
		// la funzione continua a essere scettica: il fatto che si sia trovato un carattere
		// di apertura argomento non la ha convinta
		canBeCommand=0;
		// si scorre l'array di CommandStructure
		// e si comparano le etichette dei comandi con quella isolata ora
		for(*cmdOrd = 0; CmdStruct[*cmdOrd].cmdName; (*cmdOrd)++)
		{
			if(!strcmp(strToEval, CmdStruct[*cmdOrd].cmdName))
			{
				// condizione soddisfatta
				canBeCommand=1;
				break;
			}
		}
	}
  
	// se le condizioni fin qui richieste sono state soddisfatte
	if (canBeCommand)
	{
		// la funzione continua a essere scettica
		canBeCommand=0;
		// sposto il puntatore all'inizio del presunto argomento
		*tplString = tmpArgP;
		// procedo fino ad incontrere il carattere di fine stringa o fino a quando non esco dall'argomento
		while (tmpArgP && insideArg) 
		{
			// il puntatore si sposta al carattere successivo
			(*tplString)++;
			// se si tratta di un carattere di apertura argomento
			if ( **tplString == CMD_ARG_OPEN_TAG )
			{
				// aumento il livello di annidamento parentesi
				argTagNestingLev++;
			}
			// se trovo il carattere di chiusura argomento
			else if (**tplString==CMD_ARG_CLOSE_TAG)
			{
				// se ho raggiunto il livello 0 ho esaurito l'argomento
				if (argTagNestingLev==0)
					insideArg=0;

				// diminuisco il livello di annidamento parentesi
				argTagNestingLev--;
			}
		}
		// se ho esaurito l'argomento prima di terminare la stringa tutto ok
		if (!insideArg)
			canBeCommand=1;
		else
			cmdArgNotClosed=1;
	}


	// la sintassi di comando e' stata verificata, occorre ancora verificare che non
	// ci si trovi in un ambiente di preprocessing nel qual caso solo alcuni comandi
	// sono ammessi alla esecuzione
	if (canBeCommand)
	{
		if (TntRequestType == RT_PREPROC)
		{
			if(CmdStruct[*cmdOrd].cmdType == PREPROC_IT)
				canBeCommand = 1;
			else
				canBeCommand = 0;
		}
		else if (TntRequestType == RT_PREPROC_ALL)
		{
			if(strstr(strToEval,"^")!=0)			 
				canBeCommand = 0;
		}
	}

	// se � definitivamente un comando
	if (canBeCommand)
	{
		// si fa puntare l'argomento al carattere successivo, il primo del argomento
		(tmpArgP)++;
		argLen = (*tplString) - (tmpArgP);
		*argStr = (char*) malloc ( argLen * sizeof(char) );if (!(*argStr)) EXIT(23);
		/**/if(alloctrc){fprintf(debug, "- - *argStr:%d - - len:%d\n", *argStr, argLen * sizeof(char) );fflush(debug);}
		strncpy( *argStr, tmpArgP, argLen + 1 );
		(*argStr)[argLen] = 0;
	}
	else
	{
		// non � un comando
		*cmdOrd = 0;
		// niente comando
		tmpArgP = NULL;
		// niente argomento: rimettiamo la stringa a posto
		*tplString = holdTplString;
	}
	*cmdStart = holdTplString - 1;

	// se � un comando torna 1, se non lo � torna 0, se non � stato chiuso l'argomento torna -1
	return canBeCommand-cmdArgNotClosed;
}


/*************************************************************************************
NOME			:procTplData
stato			:nuova; da completare i filtri per le funzioni ciclo e condizionale
attivita'	:scandisce i dati letti in tplString e li scrive su outputStream se si trova
					 alla sommit� dei processi ricorsivi (li spedisce direttramente all'otput del
					 programma); li scrive su outputString se viene chiamata da se stessa
					 (restituisce i risultati al livello superiore per mezzo di una stringa)
chiamante	:main (poi: cgiMain), procTplData (ricorsiva);
chiama		:verifyCmdSynt, procTplData, runCmd;
note			:
*************************************************************************************/
int procTplData(char * tplString, FILE * outputStream, char ** outputString) 
{
	int	dataIsChar=1;							//flag che indica se il dato da restituire � un semplice carattere
	char * commandRetStr = NULL;	//puntatore che verr� indirizzato al valore restituito dal comando eseguito (l'allocazione spetta al comando)
	char * transfArg;							//argomento di una funzione trasformato dalla chiamata ricorsiva
	char * cmdStart;
	char * argStr;								//argomento grezzo del comando
	char * outStrCrs;							//cursore, si sposta partendo da *outputString
	char * deadLine;
	int		 tplLen, i=0;
	int		 cmdOrd = 0;
	int		 retStrLen = 0;
	char * stampella;
  

	// se non esiste outputStream sono all'interno di una ricorsione:
	// non scriver� su stream di uscita ma su *outputString
	if (!outputStream)
	{
		// lunghezza della stringa ricevuta (� l'argomento grezzo di una funzione)
		tplLen=strlen(tplString);
//**/if(usedbg){fprintf(debug, "PARSER- tplLen:%d\n", tplLen);fflush(debug);}

		// prima allocazione: viene riservato lo spazio della stringa grezza
		// (tale rimarr� se la stringa grezza non contiene comandi e quindi non viene modificata)
		*outputString=(char*)malloc( (tplLen)* sizeof(char));if (!(*outputString)) EXIT(MEMERR);
		/**/if(alloctrc){fprintf(debug, "- - *outputString:%d - - len:%d\n", *outputString, (tplLen)* sizeof(char) );fflush(debug);}

		// inizializzazione del cursore
		outStrCrs = *outputString;
	}

	while (*tplString != 0)
	{
		//**/if(usedbg){fprintf(debug, "%c", *tplString);fflush(debug);}

		// di default non mi aspetto un comando: assumo che il dato sia un carattere da copiare
		// in uscita cos� come �
		dataIsChar=1;

		// se incontro il carattere di segnalazione comando
		if (*tplString==START_COMMAND_TAG)
		{

			// verifico la sintassi del comando e metto i puntatori su tplString
			// all'inizio del nome del comando e dell'argomento (termino le stringhe ponendo
			// a zero i caratteri di inizio e fine argomento); tplString viene spostato a fine comando
      if (verifyCmdSynt(&tplString, &cmdOrd, &argStr, &cmdStart))
			{
				//
				if (CmdStruct[cmdOrd].cmdType == START_CND_BLK)
				{
					CndLevel++;

					// si ereditano le colpe dei padri
					if (ValidBlock[CndLevel-1] == 0)		ValidBlock[CndLevel] = 0;

					// si disattiva il flag che segnala la presenza di un singolo carattere
					dataIsChar = 0;
				}
				else if (CmdStruct[cmdOrd].cmdType == ELSE_CND_BLK)
				{
					if (ValidBlock[CndLevel] == 0)
						ValidBlock[CndLevel] = 1;
					else if (ValidBlock[CndLevel] == 1)
						ValidBlock[CndLevel] = 0;

					// si ereditano le colpe dei padri
					if (ValidBlock[CndLevel-1] == 0)		ValidBlock[CndLevel] = 0;

					// si disattiva il flag che segnala la presenza di un singolo carattere
					dataIsChar = 0;
				}

				if	(	(ValidBlock[CndLevel] !=0) && 
							(CmdStruct[cmdOrd].cmdType == START_CYCLE)	)
				{
					CycLevel++;
					pushStk(&cycleStk, cmdStart);
					// si disattiva il flag che segnala la presenza di un singolo carattere
					dataIsChar = 0;
				}


				//**/if(usedbg){fprintf(debug, "\nFound:%s - CycLevel %d=%d;CndLevel %d=%d\n", CmdStruct[cmdOrd].cmdName, CycLevel, ReadCycle[CycLevel], CndLevel,ValidBlock[CndLevel]);fflush(debug);}
				// esecuzione comando
				if (ValidBlock[CndLevel] && ReadCycle[CycLevel])
				{
					// chiamata ricorsiva: l'argomento del comando viene interpretato e restituito su
					// transfArg; il file pointer (secondo argomento) viene passato NULL per permettere
					// il riconoscimento del livello interno di ricorsione
					procTplData( argStr, NULL, &transfArg);
					/**/if(usedbg){fprintf(debug, "Executing:%s - ", CmdStruct[cmdOrd].cmdName );fflush(debug);}
					commandRetStr = (char*) CmdStruct[cmdOrd].cmdFunction( 0, 0, transfArg);
					// si disattiva il flag che segnala la presenza di un singolo carattere
					dataIsChar = 0;
				}


				if	(	(ValidBlock[CndLevel] !=0) && 
							(CmdStruct[cmdOrd].cmdType == END_CYCLE)	)
				{
					if ( ReadCycle[CycLevel] )
						popStk(&cycleStk, &tplString);
					else
					{
						popStk(&cycleStk, &deadLine);
						ReadCycle[CycLevel] = 1;
					}

					CycLevel--;
					// si disattiva il flag che segnala la presenza di un singolo carattere
					dataIsChar = 0;
				}

				if	(CmdStruct[cmdOrd].cmdType == END_CND_BLK)
				{
					ValidBlock[CndLevel] = 1;
					CndLevel--;
					// si disattiva il flag che segnala la presenza di un singolo carattere
					dataIsChar = 0;
				}
			
			
			}
		}

		if (ValidBlock[CndLevel] && ReadCycle[CycLevel])
		{
			// se il carattere letto non � il principio di un comando va scritto in output cos� come �
			if(dataIsChar)
			{
				// se esiste l'output stream (livello zero di ricorsione)
				if (outputStream) 
				{
					//**/if(usedbg){fprintf(debug, "%c", *tplString);fflush(debug);}
					fputc(*tplString, outputStream);
//					fflush(outputStream);
				}
				// se non esiste lo stream di uscita (livelli successivi di ricorsione)
				else
				{
					*outStrCrs++ = *tplString;
				}
			}

			// il dato non � un singolo carattere ma una stringa
			// risultato dell'elaborazione di un comando
			else 
			{
				// se esiste l'output stream (livello zero di ricorsione)
				if (outputStream)
				{
					i=0;
					// si copia la stringa di ritorno del comando sullo stream di uscita
					if (commandRetStr)
					{
						while ( commandRetStr[i])
						{
							//**/if(usedbg){fprintf(debug, "%c", commandRetStr[i]);fflush(debug);}
							fputc(commandRetStr[i++], outputStream);
						}
					}
//					fflush(outputStream);
				}
				// se non esiste lo stream di uscita (livelli successivi di ricorsione)
				else
				{
					if (commandRetStr && (retStrLen = strlen(commandRetStr)))
					{
						// si termina outStrCrs (sulla posizione finale della stringa)
						*outStrCrs = 0;

            stampella = (char*) malloc( ( tplLen + retStrLen + 1 ) * sizeof(char));if (!(stampella)) EXIT(MEMERR);
						tplLen = tplLen + retStrLen + 1;
//**/if(usedbg){fprintf(debug, "\nPARSER- Stampella Len:%d\n", tplLen + retStrLen + 1);fflush(debug);}
            if (*outputString)
            {
              strcpy(stampella, *outputString);
            }
						//BOH *outputString = (char*)realloc( *outputString , ( tplLen + retStrLen + 1) * sizeof(char));if (!(*outputString)) EXIT(23);/*aggiungo alla dimensione inizialmente prevista per *outputString un numero di byte pari alla lunghezza della stringa restituita dal comando*/

						// aggiungo il risultato del comando
						strcat(stampella, commandRetStr);
            *outputString = stampella;

						// si risposta il puntatore alla fine
						// (per continuare ad aggiungere dati)
            outStrCrs = strchr(*outputString, 0);
					}
				}
				commandRetStr = NULL;					
			}
		}
		tplString++;
	}

	if (!outputStream) 
	{	// nel caso sia in un livello di ricorsione
		// prima di uscire si termina la stringa costruita
		*outStrCrs = 0;
	}

	return 0;
}
