/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#include <time.h>

#include "cgic.h"
#include "tannit.h"
#include "extVars.h"

extern cgiFormEntry *cgiFormEntryFirst;

void itxAuthorizeUpload()
{
	char	initFileName[INIT_FILE_NAME_LEN];				/*nome del file di inizializazione*/
	char	initFileN[INIT_FILE_NAME_LEN];					/*nome del file di inizializazione con l'estensione*/
	FILE  *initFile;															/*file di inizializazione*/
	static char	queryString[GET_PAR_VAL_LEN];
	char  *dbPwd = NULL;
	int   errCode = 0;

/**/if(usedbg){fprintf(debug, "itxAuthorizeUpload START\n");fflush(debug);}

  char login[GET_VAR_LEN];
  char cpwd[GET_VAR_LEN];

  if (cgiFormString(LOGIN_TAG, login, GET_VAR_LEN) != cgiFormSuccess)
  {
    sendTransStart(0, 0);
    sendBack(LOGIN_ERROR);
    EXIT(0);
  }

  if (cgiFormString(CPWD_TAG, cpwd, GET_VAR_LEN) != cgiFormSuccess)
  {
    sendTransStart(0, 0);
    sendBack(LOGIN_ERROR);
    EXIT(0);
  }

	//
	// Il nome del file di inizializzazione viene ricavato dal valore di get PAR_FILE_TAG se
	// PAR_FILE non e' presente nella get si usa il file di default DEF_PAR_FILE
	//
	if (cgiFormString(PAR_FILE_TAG, initFileName, GET_VAR_LEN)!=cgiFormSuccess)
		sprintf(initFileN,"%s.%s", DEF_PAR_FILE, DEF_PAR_FILE_EXT);
	else
		sprintf(initFileN,"%s.%s", initFileName, DEF_PAR_FILE_EXT);

  initFile = fopen(initFileN, "r");
	if(!initFile) EXIT(0);
	strcpy (Odbcdsn,						readPar("ODBCDSN"						,"" , initFile)	);/*odbc dsn*/
	strcpy (Odbcuid,						readPar("ODBCUID"						,"" , initFile)	);/*odbc uid*/
	strcpy (Odbcpwd,						readPar("ODBCPWD"						,"" , initFile)	);/*odbc pwd*/
	strcpy (LoginTable,					readPar("LoginTable"				,"" , initFile)	);/*tabella di verifica del login*/
	strcpy (PwdField,						readPar("PwdField"					,"" , initFile)	);/*campo password*/
	strcpy (LoginField,					readPar("LoginField"				,"" , initFile)	);/*campo login*/
	fclose(initFile);

	//
	// Controllo che la password nel DB sia la stessa di input
	//
	if ( checkDbPwd(login, cpwd, "1", "1") == NOT_VALIDATED )
  {
      sendTransStart(0, 0);
      sendBack(LOGIN_ERROR);
      EXIT(0);
  }

}

cgiParseResultType AddNewFormEntry(char* attr, char* value)
{
  cgiFormEntry *n;
  cgiFormEntry *l;
  cgiFormEntry *p;

	/* OK, we have a new pair, add it to the list. */
	n = (cgiFormEntry *) malloc(sizeof(cgiFormEntry));	
/**/if(alloctrc){fprintf(debug, "- - n:%d - - len:%d\n", n, sizeof(cgiFormEntry));fflush(debug);}
	if (!n) 
		return cgiParseMemory;

	n->attr = (char*) malloc(strlen(attr)+1);
/**/if(alloctrc){fprintf(debug, "- - n->attr:%d - - len:%d\n", n->attr, strlen(attr)+1);fflush(debug);}
	n->value = (char*) malloc(strlen(value)+1);
/**/if(alloctrc){fprintf(debug, "- - n->value:%d - - len:%d\n", n->value, strlen(value)+1);fflush(debug);}
	n->next = 0;

  strcpy(n->attr, attr);
  strcpy(n->value, value);

/**/if(usedbg){fprintf(debug, "Acquisizione parametro get: %s  value: %s\n", attr, value);fflush(debug);}

  l = cgiFormEntryFirst;
  p = 0;
  while(l != 0)
  {
    p = l;
    l = l->next;
  }

  if (p == 0)
    cgiFormEntryFirst = n;
  else
    p->next = n;

  return cgiParseSuccess;
}			

cgiParseResultType itxCGIParseMultipartInput()
{
  int totBytesRead = 0, bytesread = 0;
  char buffer[BUFFER_SIZE];
  char bound[128];
  char finalBound[128];
  bool file_continue = true;

/**/if(usedbg){fprintf(debug, "itxCGIParseFormInput STARTING\n");fflush(debug);}
	if (cgiContentLength == 0) 
  {
		return cgiParseSuccess;
	}

  bytesread=getline(buffer,BUFFER_SIZE);
  buffer[bytesread-1]=0;
  strcpy(bound, buffer); /*definizione della stringa di boundary*/
  sprintf(finalBound, "%s--", bound); /*definizione della stringa finale*/

/**/if(usedbg){fprintf(debug, "itxCGIParseFormInput finalBound:%s\n", finalBound);fflush(debug);}
/**/if(usedbg){fprintf(debug, "itxCGIParseFormInput bound:%s\n", bound);fflush(debug);}
/**/if(usedbg){fprintf(debug, "Lettura Multipart Iniziante\n");fflush(debug);}

  while (file_continue)
  {
    bytesread=getline(buffer,BUFFER_SIZE);
    buffer[bytesread-1]=0;
/**/if(usedbg){fprintf(debug, "%s\n", buffer);fflush(debug);}

    if (strcmp(finalBound, buffer) == 0)
      file_continue = false;
    else
      if (strcmp(bound, buffer) != 0)
      {
        //siamo nella zona utile

        // deve essere presente la seguente stringa
        if ( strstr(buffer, "Content-Disposition: form-data; name=\"") == NULL)
				{
/**/if(usedbg){fprintf(debug, "cgiParseResultType returning A\n");fflush(debug);}
          return cgiParseIO;
				}

        // verifichiamo se si tratta di un file
        if ( strstr(buffer, "filename=") != NULL)
        {
          // check su login e cpwd
          // this function force exit on login error
          itxAuthorizeUpload();

          // eliminiamo l'eventuale Content-type e la riga vuota

          // per Netscape la seguente � una stringa vuota
          bytesread=getline(buffer, BUFFER_SIZE);
        	buffer[bytesread-1]=0;
/**/if(usedbg){fprintf(debug, "1-%s\n", buffer);fflush(debug);}

          // Explorer indica il tipo: supportiamo solo file testo
          if (strstr(buffer, "Content-Type: application") != 0)
          {
/**/if(usedbg){fprintf(debug, "cgiParseResultType returning B\n");fflush(debug);}
            return cgiParseIO;
          }
          else if (strstr(buffer, "Content-Type:") != 0)
          {
            // eliminiamo la riga vuota
            bytesread=getline(buffer,BUFFER_SIZE);
            buffer[bytesread-1]=0;
/**/if(usedbg){fprintf(debug, "2-%s\n", buffer);fflush(debug);}
          }

          char filename[GET_PAR_VAL_LEN];
          bool eof = false;
          bool opened = true;
          int currFileCnt = 0;
          FILE* fp;

          //generazione del nome del file destinazione dei dati di upload
          while(opened)
          {
				    sprintf(filename, "%sitxBuf%d.upl", CORR_UPLOAD_DIR, currFileCnt);
/**/if(usedbg){fprintf(debug, "filename:%s\n", filename);fflush(debug);}
            if ((fp = fopen(filename, "r"))  == NULL)
              opened = false;
            else
            {
//            sleep(1);
              fclose(fp);
              currFileCnt++;
            }
          }
          
          //  scarica il file 
          if ( ( fp = fopen(filename, "wb") ) == 0 )
					{
						sendTransStart(0, 0);
						sendBack(FATAL_ERROR);
						EXIT(0);
					}
          while(!eof)
          {
            bytesread = getline(buffer,BUFFER_SIZE);
            buffer[bytesread] = 0;
/**/if(usedbg){fprintf(debug, "3-%s", buffer);fflush(debug);}
/**/if(usedbg){fprintf(debug, "\nbytesread:%d\n", bytesread);fflush(debug);}

            if ( (strstr(buffer, finalBound) != 0) || (bytesread == EOF))
            {
              file_continue = false;
              eof = true;
            }
            else if (strstr(buffer, bound) != 0)
            {
              eof = true;
            }
            else
						{
              fwrite(buffer, 1, bytesread, fp);
						}
          }
          fclose(fp);
          AddNewFormEntry(ITX_UPLOAD_FILE_TAG, filename);

        }
        else
        {
          // si tratta di una coppia (variabile, valore)
          char* attr;
          char value[GET_PAR_VAL_LEN];

          strtok( buffer, "\"" );
          attr = strtok( NULL, "\"" );

          // leggiamo la riga vuota
          bytesread=getline(buffer,BUFFER_SIZE);
/**/if(usedbg){fprintf(debug, "4-%s", buffer);fflush(debug);}
          
          // recuperiamo il valore
          bytesread=getline(buffer,BUFFER_SIZE);
					if (bytesread > 0)
						memcpy(value, buffer, bytesread-1);
          value[bytesread-1] = 0;
/**/if(usedbg){fprintf(debug, "5-%s", buffer);fflush(debug);}

          AddNewFormEntry(attr, value);
        }
      }
  }

/**/if(usedbg){fprintf(debug, "\nLettura Multipart Finita\n");fflush(debug);}
	return cgiParseSuccess;
}
