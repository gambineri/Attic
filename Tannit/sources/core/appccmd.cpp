/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#include <stdarg.h>
#include "tannit.h"
#include "tannitds.h"
#ifdef _ITX_APPC
	#include "itannitc.h"
#endif	// _ITX_APPC
#include "extVars.h"

#ifdef _ITX_APPC
void iZLEADPAD(char* a, int b, char* c)
{
  char zcode[256];
  memset(zcode, '0', b);
  zcode[b+1] = 0;
  int  l = strlen(a);
  memcpy(zcode + b - l, a, l);
  sprintf(c, "%s", zcode);
}
#endif	// _ITX_APPC

/*************************************************************************************
NOME			:sendTransStart
Categoria	:upload/download service
attivita'	:invia un segnale di sincronizzazione al client aleph;
*************************************************************************************/
void sendTransStart(int download, int fileLen)
{
/**/if(usedbg){fprintf(debug, "sendTransStart STARTIN\n");fflush(debug);}
  if(strcmp(UserAgentSpy,"Aleph")==0 )
  {
/**/if(usedbg){fprintf(debug, "sendTransStart %d\n", fileLen);fflush(debug);}
    if (!download)
			cgiHeaderContentType("text/html");
/**/if(usedbg){fprintf(debug, "%s:%d;", ALEPH_START_COM, fileLen);fflush(debug);}
	  fprintf(cgiOut, "%s:%d;", ALEPH_START_COM, fileLen);
  }
}

/*************************************************************************************
NOME			:sendBack
Categoria	:upload/download service
attivita'	:invia un segnale di sincronizzazione al client aleph;
*************************************************************************************/
void sendBack(int flagCode)
{
/**/if(usedbg){fprintf(debug, "sendBack STARTIN\n");fflush(debug);}
  if(strcmp(UserAgentSpy,"Aleph")==0 )
  {
/**/if(usedbg){fprintf(debug, "sendBack %d\n", flagCode);fflush(debug);}
/**/if(usedbg){fprintf(debug, "%s:%d;\r\n", ALEPH_END_COM, flagCode );fflush(debug);}
	  fprintf(cgiOut, "%s:%d;\r\n", ALEPH_END_COM, flagCode );
	  //fflush(cgiOut);
		EXIT(0);
  }
}

int sendFile() //download to the remote operator
{
	char	*corrCode;
  FILE * fp;
  char  buffer[4096];
  long  int byteRead, totalBytesRead=0;
	char	cpwd[GET_PAR_VAL_LEN], login[GET_PAR_VAL_LEN];
  char  fileName[GET_PAR_VAL_LEN], queryString[GET_PAR_VAL_LEN];
  int   errCode;
  char  *dndStatus;

/**/if(usedbg){fprintf(debug, "sendFile STARTIN'\n");fflush(debug);}
/**/if(usedbg){fprintf(debug, "---------------->Llu=%s; Rtp=%s\n", Llu, Rtp);fflush(debug);}

	// Lettura parametri di ingresso
  if (cgiFormString( LOGIN_TAG, login, 0) != cgiFormSuccess)
  {
    sendBack(LOGIN_ERROR);
    EXIT(0);
  }
  if (cgiFormString( CPWD_TAG, cpwd, 0) != cgiFormSuccess)
  {
    sendBack(LOGIN_ERROR);
    EXIT(0);
  }

	//
	// Controllo che la password nel DB sia la stessa di input
	//
	if ( checkDbPwd(login, cpwd, "1", "1") == NOT_VALIDATED )
  {
      sendTransStart(0, 0);
      sendBack(LOGIN_ERROR);
      EXIT(0);
  }

  //
	// Il codice del corrispondente relativo al login viene messo sulla stringa corrCode
	//
	sprintf(queryString,"select %s from %s	where %s = '%s'",
		CorrCodeField, CorrTable, CorrUserField, login);
	dbInterface("corrUpload", queryString, 1, 1);
	corrCode = storedData(1, CorrCodeField, "corrUpload", &errCode);

  char ccode[7];
  memset(ccode, '\0', 7);
#ifdef _ITX_APPC
  iZLEADPAD(corrCode, 3, ccode);
  ITannit_CreateEnv(QueryResultSet, &QueryCounter, Llu, Rtp);
  ITannit_StoreRecord("C00", "C00", login, NULL);
  ITannit_StoreRecord("C02", "C02", login, ccode, NULL);
  iZLEADPAD(corrCode, 6, ccode);
  ITannit_SetUser(ccode);
#endif	// _ITX_APPC

  sprintf(fileName, ".\\dnddir\\%s.new", login);

  // Si chiede all'SQL se e' disponibile un file gia' ricevuto da AS400
  sprintf(queryString,"select %s from %s	where %s = '%s'",
		CorrDndStatus, CorrTable, CorrUserField, login);
	dbInterface("getDndStatus", queryString, 1, 1);
	dndStatus = storedData(1, CorrDndStatus, "getDndStatus", &errCode);


//se lo stato e' : file non presente su disco lo si chiede all'AS400
/**/if(usedbg){fprintf(debug, "Z-queryString: %s\n", queryString);fflush(debug);}
/**/if(usedbg){fprintf(debug, "Colonna: %s\n", CorrDndStatus);fflush(debug);}
/**/if(usedbg){fprintf(debug, "dndStatus: %s\n", dndStatus);fflush(debug);}

#ifdef _ITX_APPC
  if ( strcmp(dndStatus, DND_FILE_READY) != 0 )
  {
    if (!ITannit_ReceiveFileFromAS400(fileName, debug))
    {
        char* e00 = NULL;
        e00 = storedData(1, "f1", "E00", &errCode);


        if (e00 == NULL)
        {
         //connessione con AS400 fallita o impossibile scrivere file su disco
          sendTransStart(0, 0);
          sendBack(SERVER_ERROR);
          EXIT(0);
        }
        else
        {
         //connessione con AS400 fallita o impossibile scrivere file su disco
          sendTransStart(0, 0);
          sendBack(USER_NOT_ENABLED);
          EXIT(0);
        }
    }

    fileAvail = storedData(1, "f4", "C01", &errCode);

	  if ( strcmp(fileAvail, "Y") != 0 )
    {
/**/if(usedbg){ITannit_Dump(debug);}
			//file not available on the AS400
			sendTransStart(0, 0);
			sendBack(NO_FILE_AVAIL);
			EXIT(0);
    }

    //si comunica all'SQL che i dati sono pronti per essere inviati
	  sprintf(queryString,"update %s	set %s = %s  where %s = '%s'",
      CorrTable, CorrDndStatus, DND_FILE_READY, CorrUserField, login);
	  dbInterface("dndStatusReady", queryString, 1, 1);
//if(usedbg){fprintf(debug, "Y-queryString: %s\n", queryString);fflush(debug);}
  
  }
/**/if(usedbg){ITannit_Dump(debug);}
#endif	// _ITX_APPC

  if ( (fp = fopen(fileName,"rb")) == NULL )
  {
    sendBack(FATAL_ERROR);
    EXIT(0);
  }

  fprintf(cgiOut, "Content-disposition: filename=pcorbak\n");
	cgiHeaderContentType("application/itxApplication");

  //
  // Valutazione del numero di byte del file
  //
  long int fileLen = 0;
  fileLen = ftell(fp);
  fseek(fp, 0, SEEK_END);
  fileLen = ftell(fp) - fileLen;
  rewind(fp);

	sendTransStart(1, fileLen);
  //fflush(cgiOut);
  while (!feof(fp))
  {
    byteRead = fread(buffer, sizeof(char), 4096, fp);
    fwrite(buffer,  sizeof(char), byteRead, cgiOut);
    fwrite(buffer,  sizeof(char), byteRead, debug);
    //fflush(cgiOut);
    totalBytesRead += byteRead;
  }

  fclose(fp);
#ifdef _ITX_APPC
/**/if(usedbg){ITannit_Dump(debug);}
  ITannit_Destroy();
#endif	// _ITX_APPC
  sendBack(FILE_DOWNLOADED);
	return 1;
}


/*************************************************************************************
NOME			:receiveAPPCFile
Categoria	:upload/download service
attivita'	:upload from the remote operator per i corrispondenti
return		:
*************************************************************************************/
int receiveAPPCFile()
{
	char	*corrCode;
	char	login[GET_PAR_VAL_LEN];
	char	cpwd[GET_PAR_VAL_LEN];
	char	queryString[GET_PAR_VAL_LEN];
	int		errCode;
  char	uploadFileName[GET_PAR_VAL_LEN];

/**/if(usedbg){fprintf(debug, "receiveAPPCFile STARTING\n");fflush(debug);}
/**/if(usedbg){fprintf(debug, "---------------->Llu=%s; Rtp=%s\n", Llu, Rtp);fflush(debug);}

	sendTransStart(0, 0);

	// Lettura parametri di ingresso per il client aleph
	if (cgiFormString( LOGIN_TAG, login, 0) != cgiFormSuccess)			sendBack(LOGIN_ERROR);
	if (cgiFormString( CPWD_TAG, cpwd, 0) != cgiFormSuccess)				sendBack(LOGIN_ERROR);
	if (cgiFormString( ITX_UPLOAD_FILE_TAG, uploadFileName, 0) != cgiFormSuccess)
																																	sendBack(LOGIN_ERROR);

	//
	// Controllo che la password nel DB sia la stessa di input
	//
	if ( checkDbPwd(login, cpwd, "1", "1") == NOT_VALIDATED )
	{
		sendTransStart(0, 0);
		sendBack(LOGIN_ERROR);
		EXIT(0);
	}

  //
	// Il codice del corrispondente relativo al login viene messo sulla stringa corrCode
	//
	sprintf(queryString,"select %s from %s	where %s = '%s'",
		CorrCodeField, CorrTable, CorrUserField, login);
	dbInterface("corrUpload", queryString, 1, 1);
	corrCode = storedData(1, CorrCodeField, "corrUpload", &errCode);

#ifdef _ITX_APPC
  ITannit_CreateEnv(QueryResultSet, &QueryCounter, Llu, Rtp);

  char ccode[7];
  memset(ccode, '\0', 7);
  iZLEADPAD(corrCode, 3, ccode);
  ITannit_StoreRecord("C00", "C00", login, NULL);
  ITannit_StoreRecord("C03", "C03", login, ccode, NULL);
  iZLEADPAD(corrCode, 6, ccode);

  ITannit_SetUser(ccode);
  if(!ITannit_SendFileToAS400(uploadFileName, debug))
  {
    char* e00 = NULL;
    e00 = storedData(1, "f1", "E00", &errCode);

    if (e00 == NULL)
    {
     //connessione con AS400 fallita o impossibile scrivere file su disco
      sendTransStart(0, 0);
      sendBack(SERVER_ERROR);
      EXIT(0);
    }
    else
    {
     //connessione con AS400 fallita o impossibile scrivere file su disco
      sendTransStart(0, 0);
      sendBack(USER_NOT_ENABLED);
      EXIT(0);
    }
  }

/**/if(usedbg){ITannit_Dump(debug);fflush(debug);}
  ITannit_Destroy();
#endif	// _ITX_APPC

  // svuotamento del file
  FILE * fp;
  fp = fopen(uploadFileName, "w");
  fclose(fp);



/**/if(usedbg){fprintf(debug, "receiveAPPCFile EXITING\n");fflush(debug);}
	sendBack(FILE_UPLOADED);
  return (1);
}

/*************************************************************************************
Categoria	:upload/download service
attivita'	:upload from the remote operator [non operativo]
return		:
*************************************************************************************/
int receiveFile()
{
  return (1);
}

int dndConfirm()
{
	char	*corrCode;
	char	login[GET_PAR_VAL_LEN];
	char	cpwd[GET_PAR_VAL_LEN];
	char	queryString[GET_PAR_VAL_LEN];
	int		errCode;

/**/if(usedbg){fprintf(debug, "dndConfirm STARTING\n");fflush(debug);}
/**/if(usedbg){fprintf(debug, "---------------->Llu=%s; Rtp=%s\n", Llu, Rtp);fflush(debug);}

	sendTransStart(0, 0);

	// Lettura parametri di ingresso
  if (cgiFormString( LOGIN_TAG, login, 0) != cgiFormSuccess)			sendBack(LOGIN_ERROR);
  if (cgiFormString( CPWD_TAG, cpwd, 0) != cgiFormSuccess)				sendBack(LOGIN_ERROR);

	//
	// Controllo che la password nel DB sia la stessa di input
	//
	if ( checkDbPwd(login, cpwd, "1", "1") == NOT_VALIDATED )
  {
      sendTransStart(0, 0);
      sendBack(LOGIN_ERROR);
      EXIT(0);
  }

	//
	// Il codice del corrispondente relativo al login viene messo sulla stringa corrCode
	//
	sprintf(queryString,"select %s from %s	where %s = '%s'",
		CorrCodeField, CorrTable, CorrUserField, login);
	dbInterface("corrUpload", queryString, 1, 1);
	corrCode = storedData(1, CorrCodeField, "corrUpload", &errCode);

#ifdef _ITX_APPC
  ITannit_CreateEnv(QueryResultSet, &QueryCounter, Llu, Rtp);
  char ccode[7];
  memset(ccode, '\0', 7);
  ITannit_StoreRecord("C00", "C00", login, NULL);
  iZLEADPAD(corrCode, 6, ccode);
  ITannit_SetUser(ccode);
  ITannit_SendHOK(debug, ITX_APPC_PC_FTX);
/**/if(usedbg){ITannit_Dump(debug);}
  ITannit_Destroy();
#endif	// _ITX_APPC
  //si comunica all'SQL che i dati sono stati inviati
	sprintf(queryString,"UPDATE %s	SET %s = %s, ultimoUpload=getDate() WHERE %s = '%s'",
    CorrTable, CorrDndStatus, DND_FILE_DONE, CorrUserField, login);
	dbInterface("dndStatusDone", queryString, 1, 1);

/**/if(usedbg){fprintf(debug, "dndConfirm EXITING\n");fflush(debug);}
	sendBack(DND_CONFIRMED);
  return (1);
}