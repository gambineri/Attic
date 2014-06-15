/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* olimpybet.cpp 24/08/2000
 *
 * utility wrap for the public domain DES implementation by mr. Outerbridge
 *
 * filename:     tqrext.cpp
 * description:  tannitqueryresult manipolation functions from
 *               client inputs and various tannit template command extensions
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */
 
 AITECSA s.r.l.   2000
#ifndef __ITX_OLIMPYBET_CPP__
#define __ITX_OLIMPYBET_CPP__
#endif

#include "stdio.h"
#include "stdlib.h"
#include "memory.h"
#include "string.h"

#include "tannit.h"
#include "itannitc.h"
#include "extVars.h"
#include "itxtypes.h"

#define MATRICE_LEN   12
#define MATRICE_INC   1
#define MATRICE_LST   6
#define MATRICE_SPC   (MATRICE_LST - 1)
#define SYMBOL_NUM    26
#define MATRICE_NORM  32356 // max 65535 (4 chars)
#define MAX_SERIAL    (MATRICE_NORM * SYMBOL_NUM)

#define MAX_MLPFREQ   10

#define SPECIAL_NULL  1
#define SPECIAL_OLIM  2
#define SPECIAL_WRLD  5

#define OLBT_SUCCESSED      0
#define OLBT_ERR_CONN       1
#define OLBT_ERR_ODBC       2
#define OLBT_EVENT_CLOSED   3
#define OLBT_END_RC_TOKS    4
#define OLBT_END_BT_TOKS    5
#define OLBT_ERR_RECEIPT    6


char appo_matrice[MATRICE_LEN+1];
char errorcode[6];
  
bool Receipt(char* matrice, int serial)
{
  if (serial > MAX_SERIAL) return false;

  try
  {
    memset(matrice, '0', MATRICE_LEN);
    memset(appo_matrice, '\0', MATRICE_LEN + 1);
    matrice[MATRICE_LEN] = '\0';
    int serie = serial / MATRICE_NORM;
    matrice[0] = serie + 65; // 65 is ascii value for 'A'
    matrice[MATRICE_SPC] = matrice[0];
    int residual = serial % MATRICE_NORM;
    sprintf(appo_matrice, "%X", residual);
    int appolen = strlen(appo_matrice);
    memcpy(&matrice[MATRICE_SPC - appolen], appo_matrice, appolen);
    matrice[MATRICE_LST] = '\0';
    sprintf(appo_matrice, "%s", matrice);

    return true;
  }
  catch(...)
  {
    memset(appo_matrice, '\0', MATRICE_LEN + 1);
    return false;
  }
}

int BetTrisInput(char* user, int event, int amount, char* gold, char* silver, char* bronze, int special, FILE* log)
{
  int result = OLBT_SUCCESSED;
  int  seriale, maxscommesse, nsco, nmlpf, mlpbase;
  int mlpfreq;
  int gpronostico, grecord;
  char receipt[MATRICE_LEN+1];
  char query[1024];
  TannitQueryResult* qres;

  if (
       (user == NULL) ||
       (!(event > 0 && event < 301)) ||
       (amount != 1) ||
       (gold == NULL) ||
       (silver == NULL) ||
       (bronze == NULL) ||
       (ISNULL(gold)) ||
       (ISNULL(silver)) ||
       (ISNULL(bronze)) ||
       (!(special == 0 || special == 1))
     )
     return OLBT_ERR_RECEIPT;

  memset(query, '\0', 1024);

  //**/if(usedbg){fprintf(debug, "Connetction...\n");fflush(debug);}
  if (ITannit_ConnectSQL(Odbcdsn, Odbcuid, Odbcpwd))
  {
  //**/if(usedbg){fprintf(debug, "Manual Commit...\n");fflush(debug);}
    if (ITannit_ManualCommitSQL())
    {
  //**/if(usedbg){fprintf(debug, "Select scalari...\n");fflush(debug);}
      // recupero i valori scalari globali
      if (ITannit_ExecuteSQLQuery(Odbcdsn, Odbcuid, Odbcpwd, "Serial", "SELECT * FROM Scalari WITH (TABLOCKX)", 1, 512) != ITXFAILED)
      {
        qres= ITannit_FindQuery("Serial");
        seriale = atoi(ITannit_GetValue(qres, 1, "Seriale"));
        maxscommesse = atoi(ITannit_GetValue(qres, 1, "MaxScommesse"));

        // calcolo della ricevuta
  //**/if(usedbg){fprintf(debug, "Calculate receipt...\n");fflush(debug);}
        if (Receipt(receipt, seriale))
        {
          // prendo i valori interessanti della finale
  //**/if(usedbg){fprintf(debug, "Select Finali...\n");fflush(debug);}
          sprintf(query, "SELECT NumeroScommesse, MlpBase, MlpFrequenza, Chiusa FROM Finali WHERE ID = %d", event);
          if (ITannit_ExecuteSQLQuery(Odbcdsn, Odbcuid, Odbcpwd, "Finale", query, 1, 512) != ITXFAILED)
          { 
  //**/if(usedbg){fprintf(debug, "Finali selected...\n");fflush(debug);}
            qres = ITannit_FindQuery("Finale");
  //**/if(usedbg){fprintf(debug, "1...%x\n", qres);fflush(debug);}
            nsco = atoi(ITannit_GetValue(qres, 1, "NumeroScommesse"));
  //**/if(usedbg){fprintf(debug, "2...\n");fflush(debug);}
            mlpbase = atoi(ITannit_GetValue(qres, 1, "MlpBase"));
  //**/if(usedbg){fprintf(debug, "3...\n");fflush(debug);}
            mlpfreq = atoi(ITannit_GetValue(qres, 1, "MlpFrequenza"));
  //**/if(usedbg){fprintf(debug, "4...\n");fflush(debug);}
            int chiusa = atoi(ITannit_GetValue(qres, 1, "Chiusa"));
            
  //**/if(usedbg){fprintf(debug, "Check on Chiusa...\n");fflush(debug);}
            if (chiusa == 0)
            {

              nsco++;
          
              if (nsco > maxscommesse) 
                maxscommesse = nsco;

              // ricalcolo del moltiplicatore di frequenza
              nmlpf = (int)(MAX_MLPFREQ - (int)((float)((float) (nsco * MAX_MLPFREQ)/ (float) maxscommesse)));

              // aggiorno la finale con i nuovi valori
              sprintf(query, "UPDATE Finali SET NumeroScommesse = %d, MlpFrequenza = %d WHERE ID = %d", nsco, nmlpf, event);
  //**/if(usedbg){fprintf(debug, "Update Finali...\n");fflush(debug);}
              if (ITannit_ExecuteSQLQuery(Odbcdsn, Odbcuid, Odbcpwd, 0, query, 1, 512) != ITXFAILED)
              { 
                // inserisco il pronostico utente
  //**/if(usedbg){fprintf(debug, "Insert Pronostico...\n");fflush(debug);}
                sprintf(query, "INSERT INTO Pronostici (Utente, Finale, MlpBase, MlpFrequenza, GettoniGiocati, Matrice, Oro, Argento, Bronzo, GiocataRecord) VALUES ('%s', %d, %d, %d, %d, '%s', '%s', '%s', '%s', %d)", user, event, mlpbase, mlpfreq, amount, receipt, gold, silver, bronze, special);
                if (ITannit_ExecuteSQLQuery(Odbcdsn, Odbcuid, Odbcpwd, 0, query, 1, 512) != ITXFAILED)
                {
                  // aggiorno lo scalare seriale e il massimo numero di scommesse
                  seriale += MATRICE_INC;
                  sprintf(query, "UPDATE Scalari SET Seriale = %d, MaxScommesse = %d", seriale, maxscommesse);
  //**/if(usedbg){fprintf(debug, "Update Scalari...\n");fflush(debug);}
                  if (ITannit_ExecuteSQLQuery(Odbcdsn, Odbcuid, Odbcpwd, 0, query, 1, 512) != ITXFAILED)
                  {
                    // aggiornamento dei gettoni utente
                    sprintf(query, "SELECT GettoniPronostico, GettoniRecord FROM Partecipanti WHERE Utente = '%s'", user);
  //**/if(usedbg){fprintf(debug, "Select Partecipanti...\n");fflush(debug);}
                    if (ITannit_ExecuteSQLQuery(Odbcdsn, Odbcuid, Odbcpwd, "User", query, 1, 512) != ITXFAILED)
                    { 
                      qres = ITannit_FindQuery("User");
                      if (qres->totalRows > 0)
                      {
                        gpronostico = atoi(ITannit_GetValue(qres, 1, "GettoniPronostico"));
                        grecord = atoi(ITannit_GetValue(qres, 1, "GettoniRecord"));
                        if (gpronostico > 0)
                        {
                          gpronostico--;
                          if (!(grecord <= 0 && special != 0))
                          {
                            if (special != 0)
                              grecord--;
                    
                            sprintf(query, "UPDATE Partecipanti SET GettoniPronostico = %d, GettoniRecord = %d WHERE Utente = '%s'", gpronostico, grecord, user);
    //**/if(usedbg){fprintf(debug, "Update Partecipanti...\n");fflush(debug);}
                            if (ITannit_ExecuteSQLQuery(Odbcdsn, Odbcuid, Odbcpwd, 0, query, 1, 512) != ITXFAILED)
                              ITannit_CommitSQL();
                            else
                              result = OLBT_ERR_ODBC; //update partecipanti failed
                          }
                          else
                            result = OLBT_END_RC_TOKS; // wrong parameters (no more record tokens)
                        }
                        else
                          result = OLBT_END_BT_TOKS; // wrong parameters (no more bet-tokens)
                      }
                      else
                        result = OLBT_ERR_RECEIPT; // � arrivato un valore user non presente nel DB
                    }
                    else
                      result = OLBT_ERR_ODBC; // select from partecipanti failed
                  }
                  else
                    result = OLBT_ERR_ODBC; // update scalari failed
                }
                else
                  result = OLBT_ERR_ODBC; // insert pronostico failed
              }
              else
                result = OLBT_ERR_ODBC; // update finali
            }
            else
              result = OLBT_EVENT_CLOSED; // finale chiusa               
          }
          else
            result = OLBT_ERR_ODBC; // select from finali failed                
        }
        else
          result = OLBT_ERR_RECEIPT; // calcolo ricevuta failed
      }
      else
        result = OLBT_ERR_ODBC; // select scalari failed
    }
    else
      result = OLBT_ERR_ODBC; // Manual commit mode failed
  }
  else
    result = OLBT_ERR_CONN; // connection failed

  if (result != OLBT_SUCCESSED)
  {
  //**/if(usedbg){fprintf(debug, "Error Condition...\n");fflush(debug);}
    memset(appo_matrice, '\0', MATRICE_LEN + 1);
    ITannit_ErrorSQL(log);
    ITannit_RollbackSQL();
    ITannit_AutoCommitSQL();
  }

  return result;  
}




/*************************************************************************************
NOME			:	olimpybet
Categoria	: Gasp Command: olimpybet
attivita'	: inserisce la scommessa per il gioco Olimpy-Bet
valori di ritorno:
          se ha successo ritorna una stringa di 6 caratteri alfanumerici
            che rappresenta la ricevuta della scommessa
          se fallisce ritorna un carattere nel seguente set:
            '1' : connessione con il DBMS fallita
            '2' : istruzione ODBC fallita
            '3' : gara chiusa
            '4' : gettoni record insufficienti
            '5' : gettoni pronostici insufficienti
            '6' : errore nel calcolo della ricevuta
*************************************************************************************/
void* olimpybet(int vuoto1, char *vuoto2, char * inputStr)
{
	char* user;
  int event;
  char* gold;
  char* silver;
  char* bronze;
  int special;
  
  char* c_event;
  char* c_special;
  int   betdone;

  if(!pickPar(inputStr, 1, &user))	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 2, &c_event))	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 3, &gold))	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 4, &silver))	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 5, &bronze))	return(PARAM_NOT_FOUND_MSG);
	if(!pickPar(inputStr, 6, &c_special))	return(PARAM_NOT_FOUND_MSG);

  event = atoi(c_event);
  special = atoi(c_special);

  ITannit_Create(QueryResultSet, &QueryCounter);

  /**/if(usedbg){fprintf(debug, "Entering BetTrisInput\n");fflush(debug);}
  betdone = BetTrisInput(user, event, 1, gold, silver, bronze, special, debug);
  /**/if(usedbg){fprintf(debug, "Exiting BetTrisInput return value: %d\n", betdone);fflush(debug);}
  
  ITannit_Destroy();  

  if (betdone == OLBT_SUCCESSED)
    return appo_matrice;
  else
  {
    memset(errorcode, '\0', 6);
    sprintf(errorcode, "%d", betdone);
    return errorcode;
  }
}

