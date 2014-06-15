/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */


#ifndef __DEFINES_H__
#define __DEFINES_H__


//Compilation options

//#define MAINT_1           //check if must use dbg
//#define TIMETRACE         //check if must print timing stats


#define AITECSA_DISCLAIMER    "<!--\r\
    This file has been created from the Tannit 4.0 Application owned, produced and distributed\r\
    uniquely by AITECSA S.r.l. This copy of Tannit 4.0 is intended to run only on machines owned\r\
    by explicitly authorized users, owners of a run time license of Tannit 4.0 product.\r\
    Any other use is discouraged and will be persecuted in the terms established by the current laws. \r\
    FGPC00013 FMPC00001 MGPC00005 \r\
-->\r"

/*
  FMPC00001 - fixed conversion of double data type;

  FGPC00013 - Modificata la netgrab e la netprex: il browser agent non  pi
              "Aitecsa Thunder v 1.45" ma "Thunder v 1.45"
  FGPC00012 - Modificata la netgrab: ora torna il numero di bytes scaricati
  FGPC00010 - Aggiunto il comando getcast
  FGPC00010 - Aggiunta l'opzione refferrer nel comando traceu
  FGPC00009 - Impostata la variabile di abiente m_ActualTplDir del Parser ed attivato
              l'allineamento della directory del *prex in caso di preprocessing con chiave
  FGPC00008 - Modificata pesantemente la Tannit::OnStart aggiungendo le funzioni
              Tannit::ManageContext e Tannit::ManageOutput.
              Il preprocessing  ora condizionabile da chiavi all'interno del file
              dei parametri
  FGPC00007 - Invertito l'ordine tra la lettura del file tannit.conf e l'acquisizione
              dei parametri d'ambiente; questo evita che il programma si blocchi qualora
              non trovi tannit.conf cadendo nel trappolone per il modo command line
  FGPC00006 - Risolta diversamente la MGPC00001: non viene pi impostata la directory
              di esecuzione del programma ma vengono definite le directory ausiliarie
              dove cercare i file tannit.conf e prm nel caso non vengano trovati in quella
              corrente.
  FGPC00005 - Modificato netgrab aggiunto il parametro dei secondo di timeout
  FGPC00004 - modificata la *proc: ora se nel prm e' presente il parametro ForceProcCmdTag 
              non usa la PickPar ma processa direttamente il buffer;
  FGPC00003 - modificata la newconn: ritorna un valore con il 5 parametro;
  FGPC00002 - *netgrab(url,maxbytes,tofilename) command added;
  FGPC00001 - test version of the *copyf(fromfile,tofile) command added;

  MGPC00005 - aggiunto il comando mail
  MGPC00004 - comando tqrstat: aggiunte info sui campi; macro facilities in tnt.h
              correzzione bug: le variabili in prm e con ora sono trimmate a sx e dx.
  MGPC00003 - modifica funzionalita` di inquiry, tplservice, response mime type.
  MGPC00002 - WIN32: aggiunta funzionalita` di inquiry.
  MGPC00001 - WIN32: modifica alla directory in cui vengono prelevati i file conf e prm: 
              ora sempre dalla dir in cui risiede Tannit.
*/

// Messaggi per la comunicazione con il client download/upload "Aleph"
#define FILE_UPLOADED								1
#define FILE_DOWNLOADED							2
#define DND_CONFIRMED								3
#define NO_FILE_AVAIL								4
#define LOGIN_SUCCESS								5
#define UNABLE_TO_UPLOAD						1002
#define SERVER_ERROR    						1003
#define USER_NOT_ENABLED 						1004
#define FATAL_ERROR									1999
#define ALEPH_START_COM             "ITX_START_OF_TRASMISSIONS_GOOD_LUCK"
#define ALEPH_END_COM               "ITX_END_OF_TRASMISSIONS_RETURNING_CODE"

// chiavi per l'invio e la ricezione di file
#define	EXCNG_FILE_TAG        "excng"
#define	SEND_FILE_GRANT       "92"
#define	RECEIVE_APPC_FILE_GRANT    "93"
#define	DND_CONFIRM_GRANT			"94"
#define	RECEIVE_FILE_GRANT    "95"

// Chiavi per la comunicazione con Baal: TntRequestTypes
#define RT_BROWSE							0
#define RT_PREPROC						1
#define RT_PREPROC_ALL				2
#define RT_PREVIEW						3
#define TPL_UPD_COMM_RECV			"<html><body bgcolor=#44AAFF><font color=Blue><b>Tpl:"
#define TPL_UPD_COMM_SUCC			"<br><Font size=1 color=red>E' il direttore che vi parla: sbaraccate ed andatevene in vacanza</Font></b></BODY></HTML>"

// Errori di interrogazione del database
#define	ERR_MSG_LEN						256
#define	ERRORS_EDGE						0			// limite superiore per i return che definiscono errori
#define	DATA_VALUE_ON_ERROR		"itxdbdatadefval"

#define ERR_COL_NOT_FOUND			"Error: field not found."
#define ERR_QUERY_NOT_FOUND		"Error: query not found."
#define ERR_VOID_QUERY_NAME		"Error: unable to read query name."
#define LOGIN_ERROR						"Error: login failed."
#define ERR_PARAM_NEEDED			"Error: parameter needed."

// Errori di sintassi GASP
#define	ERR_REM_BLINDCH				"Error: double quotes expected."
#define ERR_OPEN_INI_FILE			"Error: conf or prm file(s) not found: remember to put them in the same Tannit directory.\n"
#define ERR_OPEN_TPL_FILE			"Error: template not found or empty.\n"
#define PARAMETER_NOT_FOUND		"notfound"
#define	FIELD_NAME_NOT_FOUND	"Error: field name not found in get string"
#define	INVALID_LOGIN_FILE		"loginerror"
#define PARAM_NOT_FOUND_MSG		"Error: parameter not found"
#define PAR_NOT_FOUND					"paramfileNotFound"

// Errori generici
#define CRYPTING_ERROR				"Crypting Error"
#define MEMERR								23
#define MEMORY_ERROR_MSG			"Memory allocation Error"

//Messaggi durante il preprocessing
#define PP_STARTED            "Now preprocessing...<br>"
#define PP_FINISHED           "Preprocess operation completed.<br>"
#define PP_CANT_WRITEOUTPUT   "Error while attempting to write on the output file.<br>"
#define PP_IN_OUT_DIRS_EQ     "Can't generate output file: preprocess and template directories are the same.<br>"
#define PP_NOT_ALLOWED        "Preprocessing not allowed"
#define WWW_PP_STARTED            "<html><head><title>PREPROCESSING</title><STYLE>body {border-width: 1px; border-color: #226622; border-style: solid; margin: 0px 0px 0px 0px; background-color: #55aa55; font-family: tahoma; font-size: 8pt;}</STYLE></head><body>"
#define WWW_PP_FINISHED           "completed</body></html>"
#define WWW_PP_CANT_WRITEOUTPUT   "<table width=100% bgcolor=#aa6666><tr><td>Error while attempting to write on the output file.</td></tr></table>"
#define WWW_PP_IN_OUT_DIRS_EQ     "<table width=100% bgcolor=#bb7777><tr><td>Can't generate output file: preprocess and template directories are the same.</td></tr></table>"
#define WWW_PP_NOT_ALLOWED        "<table width=100% bgcolor=#cc8888><tr><td>Preprocessing not allowed</td></tr></table>"


// Etichette
#define TANNIT_CONFIG_FILE    "tannit.conf"
#define BROWSER_OPERA					"Opera"
#define BROWSER_MSIE					"MS Internet Explorer"
#define BROWSER_NETSCAPE			"Netscape Comunicator"
#define ITX_MISINTER					"itxmisi"
#define ISEXPLORER						"explorer"
#define ISNETSCAPE						"netscape"
#define LOGIN_TAG             "login"
#define CPWD_TAG              "cpwd"
#define TPLDIR                "tpldir"
#define ADDON                 "addon"
#define	BLINDER								'"'
#define CONC_LAN_ID_TAG				"lid"
#define DEF_PAR_FILE					"tannit"
#define DEF_PAR_FILE_EXT			"prm"
#define PAR_FILE_TAG					"prm"
#define DEFAULT_TPL						"tannit"
#define TGT_TAG								"tgt"
#define TPL_TAG								"tpl"
#define TPL_SERVICE_TAG				"tplservice"
#define MIME_TAG							"mime"
#define PREPROC_TPL_TAG       "preproc"
#define PREPROC_DIR_PRM_NAME  "preprocdir"
#define DEBUG_2_VIDEO_TAG     "forcedebug"
#define INQUIRY_TAG           "diagnostic"
#define TPL_EXT								".htm"
#define TEG_EXT								".teg"
#define DEFAULT_MIME_TYPE			"text/html"
#define START_COMMAND_TAG			'*'
#define PREPROC_COMMAND_TAG		'#'
#define TEG_COMMAND_TAG		    '' //Tannit Extension Generator
#define CMD_ARG_OPEN_TAG			'('
#define CMD_ARG_CLOSE_TAG			')'
#define PARAM_NOT_FOUND				0
#define PARAM_FOUND						1
#define PARAM_NUMBER					128
#define CND_OP_NUM						6
#define GET_PAR_VAL						""
#define REC_VAL_ZERO					""
#define DND_FILE_DONE         "2"
#define DND_FILE_READY        "1"
#define	ITX_UPLOAD_FILE_TAG   "itxFile"
#define CORR_UPLOAD_DIR				"./itxCrUp/"
#define ITXCOO_SEP            "#"
#define TANNIT_TRUE           "true"
#define TANNIT_FALSE          "false"
#define TNT_COMMENT           "$$"
#define LF                    0xA
#define CR                    0xD
#define TQR_FOR_PREFIX        "tqr_"

// Numerici
#define MAX_DSN								32
#define MAX_CRYPTING_BUFFER		256
#define	BUFFER_SIZE						10000
#define	LANG_ID_LEN						8
#define	ID_FIELD_LEN					255
#define	EXTRID_FIELD_LEN			256
#define	TPL_VARS_NUM					64
#define	CYC_NESTING_DEPTH			64
#define	SELECTED_ADD_ON				32
#define LANID_LEN							4						/*lunghezza max dell'identificatore di linguaggio*/
#define INIT_FILE_LINE_LEN		512					/*lunghezza massima di una riga del file di inizializzazione*/
#define PAR_NAME_LEN					64					/*lunghezza massima del nome di un parametro nel file di inizializzazione*/
#define PAR_VALUE_LEN					256					/*lunghezza massima del valore di un parametro nel file di inizializzazione*/
#define INIT_FILE_NAME_LEN		256
#define HOME_DIR_LEN					64
#define IMG_DIR_LEN						64
#define SS_DIR_LEN						64
#define GET_VAR_LEN						128
#define TPL_LEN								250000
#define TPL_ABS_NAME_LEN			1024
#define MAX_TPL_NESTS					24
#define TPL_NAME_LEN					256
#define CMD_LEN								64
#define GET_PAR_VAL_LEN				4096
#define ITXCOO_TIME_DELAY     10000 // in secondi
#define PRM_TUNNEL_TAG				"KOAN"
#define MAX_PRM_PARS          1000   //Max num di params in un prm
#define MAX_CMDS              1024
#define MAX_EXT_MODULES       50

// TQR Statistic
#define TOTRECS                      "TOTRECS"
#define ACTUALROW                    "ACTUALROW"
#define ACTUALPAGE                   "ACTUALPAGE"
#define ROWSTOSTORE                  "ROWSTOSTORE"
#define STARTINGROW                  "STARTINGROW"
#define MORERECS                     "MORERECS"
#define SOURCECOUNT                  "SOURCECOUNT"
#define TOTALPAGES                   "TOTALPAGES"
#define LASTPAGEROW                  "LASTPAGEROW"
#define NEXTPAGEROW                  "NEXTPAGEROW"
#define PREVPAGEROW                  "PREVPAGEROW"

// TQR cursor movement
#define FIRST                        "FIRST"
#define LAST                         "LAST"
#define NEXT                         "NEXT"
#define TO                           "TO"

// TQR info
#define COLCOUNT                     "COLCOUNT"
#define COLNAME                      "COLNAME"
#define COLTYPE                      "COLTYPE"
#define COLSIZE                      "COLSIZE"


// Base Command 'type' definitions
#define DEFAULT_BC_TYPE   0
#define START_CYCLE				1
#define END_CYCLE					2
#define START_CND_BLK			4
#define ELSE_CND_BLK			8
#define END_CND_BLK				16
#define ELSIF_CND_BLK			32

//Blocking - non blocking execution states
#define DONT_EXEC_BLK     0
#define EXEC_BLK          1
#define STOP_EXEC_BLK     2

//Inquiry
#define INQ_TOK_SEP       '*'
#define INQ_CONN_OK       "OK"
#define INQ_CONN_NOK      "NOK"


#ifdef DOWNLOAD
#pragma message("WARNING: you are compiling a time limited version. Disable DOWNLOAD option for no time expiration.")
#endif


#endif  /* __DEFINES_H__ */



