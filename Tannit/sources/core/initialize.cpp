/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/*****************************************************************************
                                 AITECSA S.R.L.
 - PROJECT			: itxWeb - Tannit
 - FILENAME			: initialize.c
 - TAB					: 2, no spaces
 - DESCRIPTION	:	funzioni di inizializzazione
 *****************************************************************************/

#include	"tannit.h"
#include "extVars.h"

#define ERR_POS_PAR_FILE			"Errore:impossibile posizionare il file di inizializzazione\n"

/*************************************************************************************
NOME			:initGlobals
attivita'	:inizializzazione delle variabili globali per la gestione dell'array di
					 puntatori a query, dei cicli, dei blocchi condizionati.
chiamante	:cgiMain.
*************************************************************************************/
int initGlobals()
{
	QueryCounter = 0;
	for (CndLevel = 0; CndLevel < CYC_NESTING_DEPTH; CndLevel++)		ValidBlock[CndLevel] = 1;
	for (CycLevel = 0; CycLevel < CYC_NESTING_DEPTH; CycLevel++)		ReadCycle[CycLevel] = 1;
	CndLevel = 0;
	CycLevel = 0;
	ReadCycle[0] = 1;
	LockLoop = 0;
	TplVars.idx = 0;
	TplNest = 0;
//**/if(usedbg){fprintf(debug, "initGlobals 1;\n");fflush(debug);}
  memset( QueryResultSet, 0, sizeof( TannitQueryResult* ) * QUERY_NUMBER);
//**/if(usedbg){fprintf(debug, "initGlobals 2;\n");fflush(debug);}

	return 1;
}


/*************************************************************************************
NOME			:readPar
attivita'	:ricerca nel 'initFile' del valore del parametro 'parId'; 'lanId' identifica
					 il suffisso eventuale per il linguaggio
chiamante	:readIniPars
*************************************************************************************/
char * readPar (const char * parId, const char * lanId, FILE * initFile){
	char *token, *retVal = 0;
	char fileLine[INIT_FILE_LINE_LEN];
	char static parValue[PAR_NAME_LEN];
	char langParId[PAR_NAME_LEN];
	fpos_t pos;

	// ogni volta che viene chiamato readPar viene messo il puntatore al file all'inizio
	pos = 0;
  if( fsetpos( initFile, &pos ) != 0 )
	{
		fprintf(cgiOut,"%s\n", ERR_POS_PAR_FILE);//fflush(cgiOut);
		EXIT(__LINE__);
	}

	// se lanId � diverso da "" si devono cercare i nomi dei parametri composti dal
	// nome originale concatenato con la stringa _lanId (es. BGMAIN diventa BGMAIN_E oppure BGMAIN_2)
	sprintf(langParId, "%s_%s", parId, lanId);
	parValue[0] = 0;

	// scansione delle linee del file
	while ( fgets (fileLine, INIT_FILE_LINE_LEN, initFile) != 0 )
	{
		// se il primo carattere e' '#' la linea non va letta: e' un commento
		if ( fileLine[0] == '#' )
			continue;

		// il segno di uguale determina la fine del token candidato a id del parametro:
		// si confronta il token con il parametro da cercare
		token = strtok (fileLine, "=");
		if ( ( strcmp( token, parId ) == 0 ) || ( strcmp( token, langParId ) == 0 ) )
		{
			retVal = strtok ('\0',"\n");

			if (retVal != NULL)		strcpy (parValue, retVal);
			else									strcpy (parValue, "");

			// nel caso il parametro individuato sia quello con la specificazione della lingua
			// si interrompe la ricerca in modo da dare precedenza alla versione con lingua
			// rispetto a quella di default (che viene scelta solo se non c'e' la versione con la lingua)
			if ( strcmp( token, langParId ) == 0 )
				break;
		}
	}

	// se il valore del parametro e' nullo si forza a ""
	if (retVal == NULL) return("");

	//**/if(usedbg){fprintf(debug, "returning:%s;\n", retVal);fflush(debug);}
	return (parValue);
}

/*************************************************************************************
NOME			:readIniPars
attivita'	:funzione per la lettura dei parametri dal file .ini
chiamante	:cgiMain
chiama		:readPar
note			:assegna valori a molte variabili globali; apre il file all'inizio e lo
					 chiude alla fine
*************************************************************************************/
int readIniPars()
{
	char	lanId[LANID_LEN];
	char	initFileName[INIT_FILE_NAME_LEN];				/*nome del file di inizializazione*/
	char	initFileN[INIT_FILE_NAME_LEN];					/*nome del file di inizializazione con l'estensione*/
	FILE *initFile;																/*file di inizializazione*/

	// il nome del file di inizializzazione viene ricavato dal valore di get PAR_FILE_TAG se
	// PAR_FILE non e' presente nella get si usa il file di default DEF_PAR_FILE
	if (cgiFormString(PAR_FILE_TAG, initFileName, GET_VAR_LEN)!=cgiFormSuccess)
		sprintf(initFileN,"%s.%s", DEF_PAR_FILE, DEF_PAR_FILE_EXT);
	else
		sprintf(initFileN,"%s.%s", initFileName, DEF_PAR_FILE_EXT);

	initFile = fopen(initFileN, "r");
	if(!initFile) return(0);

	// estrazione dalla stringa di get del valore del parametro opzionale
	// CONC_LAN_ID_TAG (concatenated language id)
	memset(lanId, 0, LANID_LEN);
	if (cgiFormString(CONC_LAN_ID_TAG, lanId, LANID_LEN - 1) != cgiFormSuccess)	strcpy(lanId,"");

	strcpy (WebUrl,							readPar("WEBURL"						,lanId, initFile)	);//riferimento al sito web
	strcpy (WebHome,						readPar("WEBHOME"						,lanId, initFile)	);//riferimento alla root web
	strcpy (CgiDir,							readPar("CgiDir"						,lanId, initFile)	);//nome della directory cgi
	strcpy (CgiName,						readPar("CgiName"						,lanId, initFile)	);//nome dell'eseguibile cgi
	strcpy (ImgDir,							readPar("IMGDIR"						,lanId, initFile)	);//directory delle immagini
	strcpy (SSDir,							readPar("SSDIR"							,lanId, initFile)	);//directory degli style sheet

	strcpy (TplDir,							readPar("TPLDIR"						,lanId, initFile)	);// directory dei template
	strcpy (CrudeTplDir,				readPar("CrudeTplDir"				,lanId, initFile)	);// directory dei template da preprocessare
	strcpy (PrepropKey,					readPar("PrepropKey"				,lanId, initFile)	);// chiave di autorizzazione al preprocessing
	strcpy (PreviewKey,					readPar("PreviewKey"				,lanId, initFile)	);// chiave di autorizzazione al previewing
	strcpy (NormviewKey,				readPar("NormviewKey"				,lanId, initFile)	);// chiave di autorizzazione alla visualizzazione standard
	strcpy (PrepKeyTag,					readPar("PrepKeyTag"				,lanId, initFile)	);// tag della get per la chiave di autorizzazione al preprocessing

	strcpy (FileDir,						readPar("FILEDIR"						,lanId, initFile)	);// directory dei file
	strcpy (DbgPath,						readPar("DbgPath"						,lanId, initFile)	);// path del file di debug

	strcpy (CooTimeDelay,				readPar("CooTimeDelay"			,lanId, initFile)	);
	strcpy (CooURLEscape,				readPar("CooURLEscape"			,lanId, initFile)	);

	strcpy (BgMain,							readPar("BGMAIN"						,lanId, initFile)	);// background principale
	strcpy (BgMenu,							readPar("BGMENU"						,lanId, initFile)	);// background menu
	strcpy (BgTop,							readPar("BGTOP"							,lanId, initFile)	);/*background top*/
	strcpy (BgLeft,							readPar("BGLEFT"						,lanId, initFile)	);/*background left*/
	strcpy (BgRight,						readPar("BGRIGHT"						,lanId, initFile)	);/*background right*/
	strcpy (BgBott,							readPar("BGBOTT"						,lanId, initFile)	);/*background bottom*/
	strcpy (Odbcdsn,						readPar("ODBCDSN"						,lanId, initFile)	);/*odbc dsn*/
	strcpy (Odbcuid,						readPar("ODBCUID"						,lanId, initFile)	);/*odbc uid*/
	strcpy (Odbcpwd,						readPar("ODBCPWD"						,lanId, initFile)	);/*odbc pwd*/
	strcpy (LoginTable,					readPar("LoginTable"				,lanId, initFile)	);/*tabella di verifica del login*/
	strcpy (PwdField,						readPar("PwdField"					,lanId, initFile)	);/*campo password*/
	strcpy (LoginField,					readPar("LoginField"				,lanId, initFile)	);/*campo login*/

	strcpy (ExtrTable,					readPar("ExtrTable"					,lanId, initFile)	);/*tabella di default per l'estrazione dell'id*/
	strcpy (IdField,						readPar("IdField"						,lanId, initFile)	);/*campo id da cui estrarre il valore*/
	strcpy (ExtrField,					readPar("ExtrField"					,lanId, initFile)	);/*campo di default da usare come condizione per l'identificazione del record di cui estrarre l'id*/
	strcpy (LangTagGet,					readPar("LangTagGet"				,lanId, initFile)	);/*etichetta della stringa di get per la lingua*/
	strcpy (LangTable,					readPar("LangTable"					,lanId, initFile)	);/*tabella delle lingue*/
	strcpy (LangNameField,			readPar("LangNameField"			,lanId, initFile)	);/*campo nome della tabella delle lingue*/
	strcpy (LangCodeField,			readPar("LangCodeField"			,lanId, initFile)	);/*campo codice della tabella delle lingue*/
	strcpy (TransTable,					readPar("TransTable"				,lanId, initFile)	);/*tabella per le traduzioni*/
	strcpy (TransTagField,			readPar("TransTagField"			,lanId, initFile)	);/*campo etichetta della tabella per le traduzioni*/
	strcpy (DefaultLanguageId,	readPar("DefaultLanguageId"	,lanId, initFile)	);/*id della lingua di default*/
	strcpy (FFace1,							readPar("FFace1"						,lanId, initFile)	);
	strcpy (FFace2,							readPar("FFace2"						,lanId, initFile)	);
	strcpy (FSize,							readPar("FSize"							,lanId, initFile)	);
	strcpy (FStyle,							readPar("FStyle"						,lanId, initFile)	);
	strcpy (FColor,							readPar("FColor"						,lanId, initFile)	);
	strcpy (FDecor,							readPar("FDecor"						,lanId, initFile)	);
	strcpy (Lheight,						readPar("Lheight"						,lanId, initFile)	);
	strcpy (TargetTplField,			readPar("TargetTplField"		,lanId, initFile)	);
	strcpy (TplTable,			      readPar("TplTable"		      ,lanId, initFile)	);
	strcpy (TplTableId,			    readPar("TplTableId"		    ,lanId, initFile)	);
	strcpy (TplTableName,			  readPar("TplTableName"		  ,lanId, initFile)	);
	strcpy (ContextField,			  readPar("ContextField"		  ,lanId, initFile)	);
	strcpy (ContextTag,					readPar("ContextTag"				,lanId, initFile)	);

  strcpy (UploadDir,          readPar("UploadDir"	        ,lanId, initFile)	);/*directory di destinazione dei file di upload*/
  strcpy (AllowUpload,        readPar("AllowUpload"	      ,lanId, initFile)	);

  strcpy (CorrTable,          readPar("CorrTable" 	      ,lanId, initFile)	);/*tabella dei corrispondenti*/
  strcpy (CorrUserField,      readPar("CorrUserField"     ,lanId, initFile)	);/*campo user della tabella dei corrispondenti*/
  strcpy (CorrDndStatus,      readPar("CorrDndStatus"			,lanId, initFile)	);/*campo DndStatus della tabella dei corrispondenti*/
  strcpy (CorrCodeField,      readPar("CorrCodeField"     ,lanId, initFile)	);/*campo codice della tabella dei corrispondenti*/
  strcpy (CorrAppDir,				  readPar("CrAppDir"					,lanId, initFile)	);/*nome della directory del disco locale del corrispondente*/
  strcpy (CorrFileName,			  readPar("CrFileName"				,lanId, initFile)	);/*nome del file nel disco locale del corrispondente*/
  strcpy (CorrDnDir,					readPar("CorrDnDir"					,lanId, initFile)	);/*nome della directory sul server relativa alla directory di esecuzione della cgi per il download dei file*/

  strcpy (Llu,    			  		readPar("Llu"	      				,lanId, initFile)	);/*parametro per la redirezione del link all'as400 tra ambiente di test e di produzione*/
  strcpy (Rtp,		      			readPar("Rtp"				      	,lanId, initFile)	);/*parametro per la redirezione del link all'as400 tra ambiente di test e di produzione*/

  strcpy (ForbiddenChars,		  readPar("ForbiddenChars"		,lanId, initFile)	);


  fclose(initFile);
	return(1);
}

