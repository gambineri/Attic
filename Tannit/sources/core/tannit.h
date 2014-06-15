/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/*****************************************************************************
                                 AITECSA S.R.L.
 - PROJECT			: itxWeb - Tannit
 - FILENAME			: tannit.h
 - TAB				: 2, no spaces
 - DESCRIPTION	    : header  principale
 *****************************************************************************/
#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	<stdlib.h>
#include	<malloc.h>
#include	<time.h>
#include	<sys/types.h>
#include	<sys/timeb.h>

#include	"cgic.h"
#include	"tannitds.h"
//#include	"dbinterface.h"
#include	"tntcrypt.h"
#include	"itxlib.h"
#include	"tannitdefines.h"

//Identity
#define INFO_ID								0
#define	INFO_TAG							"wrinfo"
#define	VERSION								1
#define	VERSION_MSG						"<!-- Tannit: V 3.017 08.09.2000 -->\n"
#define	OWNER									2
#define	OWNER_MSG							"<!-- Code owned by :  A I T E C S A   S.R.L. -->\n"
#define	AUTHOR								3
#define	AUTHOR_MSG						"<!-- Like Sweetie Pie By The Stone - Alliance Everbody Knows I'm Known For Dropping Science -->\n"
#define	APPC_MSG_ID						4

/*
versione 3.017  - eliminate alcune operazioni valide solo per AsWeb e corretta la EXIT.
versione 3.016  - Aggiunta quotestr
versione 3.015  - Aggiunta stampa del pid nel nome del file di debug
versione 3.014  - pecciata la exof: eliminato fflush
versione 3.013  - Aggiunta stampa del pid nella prima printf su dbg (requires windows.h...)
versione 3.012  - Patchone exit
versione 3.011  - Aggiunta la funzione dbgmsg
versione 3.010  - Introdotta la funzione EXIT!
versione 3.009  - Divieto a Go!Zilla
versione 3.008  - Baco sulla TQR User in olimpybet eliminato
versione 3.007  - Multidebug con utime
versione 3.006b - aggiunta la *trim(a)
versione 3.005b - aggiunta la *sumf(a,b,dec)
versione 3.004b - aggiunto il comando *olimpybet() nell'ambito del progetto Olimpy-Bet per Wind
versione 3.003b - aggiunto il comando *SSDIR()
versione 3.002b - modificate getmad ed exeQ: la getmad raddoppia anche i doppi apici, la exeQ
									li rimette a posto ( "" -> " )
versione 3.001b - corretto il GRAVISSIMO baco del parser per la pseudo-riallocazione del buffer
									di uscita
versione 2.712b - rinominata la funzione di upload receiveFile in receiveAPPCFile; creata la
									receive file, per ora inoperativa.
versione 2.711b - la storeTQR elimina eventuali apici dal parametro buffer.
versione 2.710b - aggiunta la funzione cfc (check for chars);
									aggiunta la funzione flush (per lo svuotamento del buffer di output);
									aggiunta la funzione uTime (universal time).
versione 2.702b - aggiunta la funzione execmd, modificata la setCookie e aggiunti i comandi: 
									getcoo, rtrim
versione 2.701b - aggiunta la funzione traceUser
versione 2.70b  - versione con alcune estensioni per il supporto esteso di Store Procedure SQL Server
                  e del controllo dell'eventuale errore ODBC
versione 2.60b	- sviluppo a dimensione per asweb;
versione 2.50b	- Creata la pazzesca funzionalita' del preprocessing;
									introdotte le funzioni *_nomeFunz: sempre preprocessabili ma equivalenti
									alle preesistenti.
versione 2.01b	- l'ereditarieta' dei blocchi condizionati viene attivata a livello di parser
									e non piu' al livello delle singole funzioni ifblk ed elseblk
versione 2.004b	- Corrette tutte le chiamate a storedData(0,..) con storedData(1,..).
                  Eliminato il Dump da una funzione.
                  Corretta la fillmsk (ITannit_CreateEnv al posto di ITannit_CreateEnv).
versione 2.003b	- Aggiunta la funzione *recValLookAsStr ed aggiunto alla recValCnd il parametro
									per la concatenazione in coda.
									La recVal del record 0 (quello pirima del primo) viene forzata a "".
versione 2.002b	- Aggiornati: *ifblk - da true senza operatore e con una stringa diversa da null
															*fddate- previsto il caso gmmaaaa
versione 2.001b	- Aggiunto l'else;
									aggiunta la funzione setRec (nota: la cycleQ non resetta pi� il TQR
									alla sua uscita);
									cambiata la setVar in modo che possa modificare le variabili
									gi� dichiarate
versione 2.0b		- Completamente rivisti gli if e cycle (rimossi, tra l'altro, PANIC e DAMAGE!)
versione 1.266b - aggiunto il comando GASP *fconn per la prima connessione con AS400 (dimensione),
									aggiunto il messaggio "appc enabled" al messaggio di info
versione 1.265b - aggiunti i comandi GASP *getP00, *fillmsk, *exeTP, *exist nel file tqrext.cpp ed 
                  introdotta la funzione itxEscapeChars nel file cgic.cpp;
versione 1.264b - cambiate le malloc di dbInterface in CALLOC per inizializzare le righe;
versione 1.263b - supermegapatch su endCQ per contrastare i casini del patch PANIC;
									(patch id: DAMAGE)
versione 1.262b - aggiunte le funzioni: *totRecs(dati), *actRec(dati), *maxRecs(dati),
									*firstRec(dati), *moreRecs(dati,msg)
versione 1.261b - !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
									pecchate le cycleQ e endCQ!!!
									Monitorare la sitazione (patch id: PANIC)
									!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
versione 1.260b - introdotto il nuovo comando *recCnd per concatenare in testa alla recVal una stringa;
versione 1.259b - introdotto il nuovo comando *getmad per raddoppiare gli apici in una stringa;
versione 1.258b - attivato il comando *remQ(nome_query) (funzione remQuery) nella funzionalita' di
									renaming del TQR (Tannit Query Resultset);
versione 1.257b - aggiunte le due funzioni StoreTQR e TQRInsert. per l'implementazione delle due funzioni
                  � stato aggiunto il file tqrext.cpp.
versione 1.256b - patched version (senza le free critiche)
versione 1.255b - aggiunto il comando *cgipath() con la funzione writeCgiPath che scrive l'intero path
									di arrivo alla cgi; aggiunto il parametro CgiName; aggiunto messaggio di errore al
									fallimento di remBlindingChar (funzione per la cancellazione dei caratteri di inizio
									e fine stringa della query)
versione 1.254b - realizzata la funzione di servizio dumpAround per la visualizzazione della memoria
									attivalta la funzionalita' per il tracing opzionale dell'allocazione dei puntatori
									(se il nome specificato per il file di debug contiene la stringa 'alloctrc' vengono
									tracciati tutti gli indirizzi di tutte le malloc effettuate ed il numero di bytes
									assegnati)
versione 1.253b - attivato l'uso del file di debug da file aggiuntivo (dbgname.par) di inizializzazione
									definita la funzione remQ per la cancellazione delle strutture di dati da query
									(non ancora attiva)
versione 1.252b - aggiunta la funzione revealBrowser (*revBr())
versione 1.251b - predisposto il bool usedbg per inibire da file par l'uso del debug (si devono
									vaferizzare tutte le fprintf a debug)
versione 1.25b -	Realizzati i commenti #ifdef _ITX_APPC nel file appccmp.cpp per isolare la versione
									indipendente dallo SNA Server
versione 1.24b -	nome del file di debug parametrizzato e proveniente dal parametro DbgPath nell par file
versione 1.23b -	Situazione al 17 gennaio 2000 - AS400
versione 1.0b -		Rilascio (beta) per dimensione con comunicazione SNASrever - AS400
versione 0.9b -		versione stravolta da commentare ancora
versione 0.73b -	funzione/comando translateIt per la gestione delle lingue
versione 0.72b -	funzione/comando execInsQuery per le query di inserimento con campo identity:
									la funzione esegue una query extra per conoscere il valore del massimo del
									campo label e lo rende disponibile nella tabella "C" + "nome query"
versione 0.7b -		compilata in C++ e collegata alla lib itannit (modulo per la comunicazione AS400)
versione 0.58b -	comandi getId e extId
versione 0.58b -	comandi setVar e getVar
versione 0.56b -	messi nel file di inizializzazione il nome della tabella di
									login ed i campi login e password per la funzione authorize
versione 0.55b -	funzionalit� internazionali per il file di inizializzazione
*/


typedef struct StrStack_t
{
	char								*item;
	struct StrStack_t   *next;
} StrStack;

typedef struct TplVars
{
	int			idx;
	char	* values[TPL_VARS_NUM];
	char	* names[TPL_VARS_NUM];
}	TplVarsStrct;

typedef struct MessagesStruct
{
	int		msgId;
	char	msg[ERR_MSG_LEN];
} MessagesSt;

/*command functions*/
void*		writeWebUrl				(	int,		char*,		char*	);
void*		writeHomeDir			(	int,		char*,		char*	);
void*		writeCgiPath			(	int,		char*,		char*	);
void*		writeImgDir				(	int,		char*,		char*	);
void*		writeSSDir				(	int,		char*,		char*	);
void*		execQuery					(	int,		char*,		char*	);
void*		execInsQuery			(	int,		char*,		char*	);
void*		remQuery					(	int,		char*,		char*	);
void*		writeTotalRecords	(	int,		char*,		char*	);
void*		writeActualRec		(	int,		char*,		char*	);
void*		writeMaxRecords		(	int,		char*,		char*	);
void*		writeFirstRecord	(	int,		char*,		char*	);
void*		moreRecsMsg				(	int,		char*,		char*	);
void*		recVal						(	int,		char*,		char*	);
void*		recValCnd					(	int,		char*,		char*	);
void*		recValConf				(	int,		char*,		char*	);
void*		recValLookAsStr		(	int,		char*,		char*	);
void*		cycleQuery				(	int,		char*,		char*	);
void*		endCycleQuery			(	int,		char*,		char*	);
void*		setActualRow			(	int,		char*,		char*	);
void*		startCndBlk				(	int,		char*,		char*	);
void*		elseCndBlk				(	int,		char*,		char*	);
void*		endCndBlk					(	int,		char*,		char*	);
void*		getValue					(	int,		char*,		char*	);
void*		getmad  					(	int,		char*,		char*	);
void*		encrIt						(	int,		char*,		char*	);
void*		crypt 						(	int,		char*,		char*	);
void*		decrypt 					(	int,		char*,		char*	);
void*		authorize					(	int,		char*,		char*	);
void*		validate					(	int,		char*,		char*	);
void*		exitOnFile				(	int,		char*,		char*	);
void*		syntChk						(	int,		char*,		char*	);
void*		setVar						(	int,		char*,		char*	);
void*		getVar						(	int,		char*,		char*	);
void*		extrIdG						(	int,		char*,		char*	);
void*		extrId						(	int,		char*,		char*	);
void*		translateIt			  (	int,		char*,		char*	);
void*		processExternFile	(	int,		char*,		char*	);
void*		setCookie	  			(	int,		char*,		char*	);
void*		writeCorrAppDir		(	int,		char*,		char*	);
void*		writeCorrFileName	(	int,		char*,		char*	);
void*		isInTheDomain     (	int,		char*,		char*	);
void*   validAleph        (	int,		char*,		char*	);
void*   fontStyle	        (	int,		char*,		char*	);
void*		revealBrowser     (	int,		char*,		char*	);
void*		StoreTQR  			  (	int,		char*,		char*	);
void*		TQRInsert  			  (	int,		char*,		char*	);
void*		getP00    			  (	int,		char*,		char*	);
void*		exeTP     			  (	int,		char*,		char*	);
void*		fillmsk    			  (	int,		char*,		char*	);
void*		writeSpan  			  (	int,		char*,		char*	);
void*		verCookie  			  (	int,		char*,		char*	);
void*		now				 			  (	int,		char*,		char*	);
void*		exist			 			  (	int,		char*,		char*	);
void*		firstConn	 			  (	int,		char*,		char*	);
void*   ckcli							(	int,		char*,		char*	);
void*		formatDimDate			(	int,		char*,		char*	);
void*		splitREA					(	int,		char*,		char*	);
void*		dimDataMagic			(	int,		char*,		char*	);
void*		subtractIt 				(	int,		char*,		char*	);
void*		sumFloat	 				(	int,		char*,		char*	);
void*		parval		 				(	int,		char*,		char*	);
void*		switchTplTgt			(	int,		char*,		char*	);
void*		filterTqr   			(	int,		char*,		char*	);
void*		putGetHide   			(	int,		char*,		char*	);
void*   setTQRValues      ( int,    char*,    char* );
void*   pkdate						( int,    char*,    char* );
void*   prmTunnel					( int,    char*,    char* );
void*   dimFormatCurrency	( int,    char*,    char* );
void*   divideInt					( int,    char*,    char* );
void*   formatDimCap			( int,    char*,    char* );
void*   formatDimNACE			( int,    char*,    char* );
void*   formatDimVia			( int,    char*,    char* );
void*   uppercaseIt				( int,    char*,    char* );
void*   putTplName				( int,    char*,    char* );
void*   formatDimNaz			( int,    char*,    char* );
void*   traceUser					( int,    char*,    char* );
//version 2.7 specific
void*   invoke      			( int,    char*,    char* );
void*   execmd     	  		( int,    char*,    char* );
void*   getcoo     	  		( int,    char*,    char* );
void*   rtrim     	  		( int,    char*,    char* );
void*   trim      	  		( int,    char*,    char* );
void*   cfc       	  		( int,    char*,    char* );
void*   flush       	  	( int,    char*,    char* );
void*   uTime       	  	( int,    char*,    char* );
void*   uTimeToDMY   	  	( int,    char*,    char* );
void*   olimpybet   	  	( int,    char*,    char* );
void*   dbgmsg       	  	( int,    char*,    char* );
void*   quotestr       	  ( int,    char*,    char* );


void* exeQnew(int vuoto1,char *vuoto2, char * inputStr);
/*command support function*/
int pickPar(char* inputStr, int position, char ** returnStr);

/*program functions*/
int     cgiMain           ();
int     queryIndex        (char * queryId, int noexit);
int     pushStk           (StrStack** ,char*);
int     popStk            (StrStack** ,char**	);
int			bufferTpl					(	char **, char*, char *	);
int			procTplData				(	char*,	FILE*,		char**);
char*		readPar						(	const char *,const char * , FILE * );
int			readIniPars				();
void		trimIt						(char * );
int			verVersion				();
int			initGlobals				();
int			popStk            (StrStack** stack, char **strg );
int			pushStk           (StrStack** stack, char  *strg );
int     getline           (char s[], int lim);
int     sendFile          ();
int			dndConfirm				();
int     receiveFile				();
int     receiveAPPCFile   ();
void    sendBack          (int flagCode);
void    sendTransStart    (int download, int fileLen);
int			checkDbPwd				(char* login, char* cpwd, char*, char*);
char*		retMsg						(int msgId);
void		newAutoName				();
void		dumpAround				(char * refPtr, int strtChr, int chrnum);
void		redirectExit			(char* filename);
void		listTQRnames			();
int			capturePreproc		(char ** tplString, FILE ** refinedTplFp);
char*		storedData        (int , char * , char * , int *);
int			dbInterface       (char * , char * , int , int );
void    EXIT              (int retval);
