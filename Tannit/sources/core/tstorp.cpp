/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

// AITECSA s.r.l.   2000
//    filename:     tstorp.cpp
//    description:  support for stored procedure and ODBC error check
//    project:      since tannit 2.7

#ifndef __ITX_TSTORP_CPP__
#define __ITX_TSTORP_CPP__
#endif

#include "windows.h"

#include "tannit.h"
#include "itannitc.h"
#include "extVars.h"
#include "itxtypes.h"

#define  ITX_TSTORP_SERVER_DOWN     "SERVER_DOWN"
#define  ITX_TSTORP_DUPLICATE_KEY   "DUPLICATE_KEY"
#define  ITX_TSTORP_H_ATTACK        "H_ATTACK"
//#define  ITXCOOVERS_EQ              "\x0D\x0A"
//#define  ITXCOOVERS_SEP             "\x0D\x0A"
#define  ITXCOOVERS_EQ              "="
#define  ITXCOOVERS_SEP             ";"

/*************************************************************************************
Categoria	: Gasp Command: *invoke(stProcName,parametri)
attivita'	: invoca la Stored Procedure di comando SQL Server
stProcName: nome della Store Procedure
parametri	:	stringa di parametri per la storedProcedure. Nota: la stringa contiene
						delle virgole va racchiusa tra doppi apici.
*************************************************************************************/
void* invoke(int vuoto1, char *vuoto2, char * inputStr)
{
	char* spname;
  char* sppar;
  char* returnvalue = NULL;
  itxString istr;

  char stm[ITX_QRS_MAX_QUERY_LEN];
  int errvalue;
  
  memset(stm, 0, ITX_QRS_MAX_QUERY_LEN);

  if(!pickPar(inputStr, 1, &spname	))	return(PARAM_NOT_FOUND_MSG);
  if(!pickPar(inputStr, 2, &sppar	))	return(PARAM_NOT_FOUND_MSG);

  istr.InBlinder(&sppar, BLINDER);

  ITannit_Create(QueryResultSet, &QueryCounter);
  
  if (!ITannit_ConnectSQL(Odbcdsn, Odbcuid, Odbcpwd))
  {  
    ITannit_Destroy();
    return ITX_TSTORP_SERVER_DOWN;
  }
  strcat(stm, spname);
  strcat(stm, " ");
  strcat(stm, sppar);
  
  ITannit_ExecuteSQL(stm);
  
  errvalue = ITannit_ErrorSQL(debug); 
////**/if(usedbg){fprintf(debug, "SQL Native Error: %d\n", errvalue);fflush(debug);}
 
  switch (errvalue)
  {
    case 0: // SUCCESS
    break;

    // Real SERVER ERROR
    case 6: // Server Down
      returnvalue = ITX_TSTORP_SERVER_DOWN;
    break;
    
    // vero check sulla chiave per l'utente accreditato
    case 2601:
    case 2627: // Primary Key Violation
    case 23000:
      returnvalue = ITX_TSTORP_DUPLICATE_KEY;
    break;

    // casi che potrebbero accadere su attacchi di hackers
    case 257: // Type Mismatch
    case 8144: // Too Many Arguments
    case 201: // Too Few Arguments
    default:
      returnvalue = ITX_TSTORP_H_ATTACK ;
    break;
  } 

  ITannit_DisconnectSQL();
  ITannit_Destroy();

  return returnvalue;
}

void InitStructs(STARTUPINFO& LtStart, 
                 PROCESS_INFORMATION& LtProc,
                 SECURITY_ATTRIBUTES& LProcessAttributes,
                 SECURITY_ATTRIBUTES& LThreadAttributes,
                 HANDLE& StreamFromChild)
{                        
  SECURITY_ATTRIBUTES sa;

  sa.nLength = sizeof(sa);
  sa.lpSecurityDescriptor = NULL;
  sa.bInheritHandle = TRUE;

  BOOL ret = CreatePipe(&StreamFromChild, &LtStart.hStdOutput, &sa, 0);
/*
  LtStart.hStdOutput = 
              CreateFile("cico", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                         &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
*/

////**/if(usedbg){fprintf(debug, "LtStart.hStdOutput: %p\n", LtStart.hStdOutput);fflush(debug);}
  
  /* Inizializzazione della struttura STARTUPINFO */
  LtStart.cb = sizeof(LtStart);
  LtStart.lpReserved = NULL;
  LtStart.lpDesktop = NULL;
  LtStart.lpTitle = NULL;
  LtStart.dwX = 0;
  LtStart.dwY = 0;
  LtStart.dwXSize = 0;
  LtStart.dwYSize = 0;
  LtStart.dwXCountChars = 0;
  LtStart.dwYCountChars = 0;
  LtStart.dwFillAttribute = 0;
  LtStart.dwFlags = STARTF_USESTDHANDLES;
  LtStart.wShowWindow = 0;
  LtStart.cbReserved2 = 0;
  LtStart.lpReserved2 = 0;
  LtStart.hStdError = 0;
  
  /* Inizializzazione della struttura PROCESS_INFORMATION */
  LtProc.hProcess = 0;
  LtProc.hThread = 0;
  
  /* Inizializzazione della struttura SECURITY_ATTRIBUTES per il processo */
  LProcessAttributes.nLength = sizeof(LProcessAttributes);
  LProcessAttributes.bInheritHandle = TRUE;
  LProcessAttributes.lpSecurityDescriptor = NULL;
  
  /* Inizializzazione della struttura SECURITY_ATTRIBUTES per il thread */
  LThreadAttributes.nLength = sizeof(LThreadAttributes);
  LThreadAttributes.bInheritHandle = TRUE;
  LThreadAttributes.lpSecurityDescriptor = NULL;
  
}

/*****************************************************************************
 - FUNCTION NAME: ShellCommand
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION: Lancia un eseguibile e non torna fino a che non
    si e` concluso il processo. Inibito l'uso della console del processo
    padre per output del processo figlio (DETACHED_PROCESS).
 -----------------------------------------------------------------------------
 - RETURN VALUES: 
      ON SUCCESS: exit code del processo lanciato.
      ON FAILURE: -1.
 *****************************************************************************/
char* ShellCommand(char* sCommandLine)
{
  STARTUPINFO         tStart;
  PROCESS_INFORMATION tProc;
  SECURITY_ATTRIBUTES ProcessAttributes;
  SECURITY_ATTRIBUTES ThreadAttributes;
  int                 lret = 0;
  unsigned long       exit_code_proc = 0;
  HANDLE              StreamFromChild;

  /* Inizializzazione strutture */
  InitStructs(tStart, tProc, ProcessAttributes, ThreadAttributes, StreamFromChild);

  /* Creazione del processo */
  if ((lret = CreateProcess(NULL, sCommandLine, &ProcessAttributes, &ThreadAttributes,
                            TRUE, NORMAL_PRIORITY_CLASS | DETACHED_PROCESS, FALSE, 
                            NULL, &tStart, &tProc)) == FALSE)
    return 0;

  /* Attesa della fine del processo */
  if ((lret = WaitForSingleObject(tProc.hProcess, INFINITE)) == WAIT_FAILED)
    return 0;
  
  /* Richiesta del valore di ritorno del processo figlio 
  if (GetExitCodeProcess(tProc.hProcess, &exit_code_proc) == 0)
    return -1; */

  LPVOID lpAddress = VirtualAlloc(NULL, 1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
////**/if(usedbg){fprintf(debug, "lpAddress: <%p>\n", lpAddress);fflush(debug);}

////**/if(usedbg){fprintf(debug, "tStart.hStdOutput: %p\n", tStart.hStdOutput);fflush(debug);}
  
  if (lpAddress == NULL)
    return 0;

  memset(lpAddress, 0, 1024 );

  DWORD bytes2read = 1024;
  DWORD bytesread = 0;
  BOOL ret = ReadFile(StreamFromChild, lpAddress, bytes2read, &bytesread, NULL);

////**/if(usedbg){fprintf(debug, "ReadFile ret: %d\n", ret);fflush(debug);} 
////**/if(usedbg){fprintf(debug, "OUT: <%s>\n", lpAddress);fflush(debug);} 
  
  /* Chiusura del processo */
  CloseHandle(tProc.hProcess);
  
  return (char*)lpAddress;
}


void* execmd(int vuoto1, char *vuoto2, char * inputStr)
{
  char* command;

  if(!pickPar(inputStr, 1, &command))	return(PARAM_NOT_FOUND_MSG);
////**/if(usedbg){fprintf(debug, "COMMAND execmd: <%s>\n", command);fflush(debug);} 

  return ShellCommand(command);
}


/*************************************************************************************
Categoria	:	gasp command *getcoo(cooName)
attivita'	:	torna il cookie specificato
*************************************************************************************/
void* getcoo(int vuoto1, char *vuoto2, char * inputStr)
{
  char* cooName = NULL;
  char* cooAttr = NULL;
  char* pcoo = NULL;
	char* pcomma = NULL;
  static char cooValue[256];
  int retval = 0;

  memset(cooValue, '\0', 256);

//**/if(usedbg){fprintf(debug, "COMMAND getcoo\n");fflush(debug);} 
//**/if(usedbg){fprintf(debug, "  cgiCookie : %s\n", cgiCookie);fflush(debug);} 

	if(	!pickPar(inputStr, 1, &cooName)	)		return(PARAM_NOT_FOUND_MSG);
  retval = pickPar(inputStr, 2, &cooAttr);

  if ((pcoo = strstr(cgiCookie, cooName)) != 0)
	{
		if ((pcoo = strstr(pcoo, ITXCOOVERS_EQ)) != 0)
    {    
		  pcoo++;
      if ((pcomma = strstr(pcoo, ITXCOOVERS_SEP)) != 0)
      {
        memcpy(cooValue, pcoo, (size_t) (pcomma - pcoo));
        pcoo = pcomma;
      }
      else
        memcpy(cooValue, pcoo, strlen(pcoo));
    }
	}

  // Cookie Name found, let see if is enough
  if (retval != PARAM_NOT_FOUND)
  {
    memset(cooValue, '\0', 256);
    if ((pcoo = strstr(pcoo, cooAttr)) != 0)
	  {
		  if ((pcoo = strstr(pcoo, ITXCOOVERS_EQ)) != 0)
      {    
		    pcoo++;
        if ((pcomma = strstr(pcoo, ITXCOOVERS_SEP)) != 0)
        {
          memcpy(cooValue, pcoo, (size_t) (pcomma - pcoo));
        }
        else
          memcpy(cooValue, pcoo, strlen(pcoo));
      }
	  }
  }

  return cooValue;
}

/*************************************************************************************
Categoria	:	gasp command *rtrim(str)
attivita'	:	fa il trim dei caratteri spazio della stringa str
*************************************************************************************/
void* rtrim(int vuoto1, char *vuoto2, char * inputStr)
{
  char* str;

  if(!pickPar(inputStr, 1, &str))	return(PARAM_NOT_FOUND_MSG);
////**/if(usedbg){fprintf(debug, "COMMAND rtrim\n");fflush(debug);} 
  return ITannit_RTrim(str);
}

/*************************************************************************************
Categoria	:	gasp command *trim(str)
attivita'	:	fa il trim dei caratteri spazio della stringa str
*************************************************************************************/
void* trim(int vuoto1, char *vuoto2, char * inputStr)
{
  char* str;

  if(!pickPar(inputStr, 1, &str))	return(PARAM_NOT_FOUND_MSG);
////**/if(usedbg){fprintf(debug, "COMMAND rtrim\n");fflush(debug);} 
  return ITannit_Trim(str);
}

/*************************************************************************************
Categoria	:gasp command *cfc(teststring)
attivita'	:cerca i caratteri contenuti nella stringa ForbiddenChars nella teststring
return		:TANNIT_TRUE se trova i caratteri; TANNIT_FALSE se non trova i caratteri;
*************************************************************************************/
void* cfc(int vuoto1, char *vuoto2, char * inputStr)
{
  char* found = NULL;

  found = strpbrk(inputStr, ForbiddenChars);
  if (found != NULL)
    return TANNIT_TRUE;
  return TANNIT_FALSE;
}

void* flush(int vuoto1, char *vuoto2, char * inputStr)
{
  fflush(cgiOut);
  return 0;
}


