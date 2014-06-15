/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include "cgic.h"
#include	"tannit.h"
#include	"extVars.h"
#include	"itxlib.h"

#include <windows.h>

#define cgiStrEq(a, b) (!strcmp((a), (b)))

#define LAST_CHANCHE_TIMER_ID     100
#define LAST_CHANCHE_SECONDS      10

typedef enum 
{
	cgiEscapeRest,
	cgiEscapeFirst,
	cgiEscapeSecond
} cgiEscapeState;

typedef enum 
{
	cgiUnescapeSuccess,
	cgiUnescapeMemory
} cgiUnescapeResultType;

extern cgiParseResultType itxCGIParseMultipartInput();

char* cgiServerSoftware;
char* cgiServerName;
char* cgiGatewayInterface;
char* cgiServerProtocol;
char* cgiServerPort;
char* cgiRequestMethod;
char* cgiPathInfo;
char* cgiPathTranslated;
char* cgiScriptName;
char* cgiQueryString;
char* cgiRemoteHost;
char* cgiRemoteAddr;
char* cgiAuthType;
char* cgiRemoteUser;
char* cgiRemoteIdent;
char* cgiContentType;
char* cgiAccept;
char* cgiUserAgent;
char* cgiReferrer;
int   cgiContentLength;

/*aggiunte aitecsa*/
char* cgiHttpHost;/*nome letterale dell'host*/
char* cgiCookie;

FILE* cgiIn;
FILE* cgiOut;

/* The first form entry. */
cgiFormEntry* cgiFormEntryFirst;

/* True if CGI environment was restored from a file. */
static int cgiRestored = 0;

static int            cgiHexValue[256];
static char*          cgiFindTarget = 0;
static cgiFormEntry*  cgiFindPos    = 0;



/************************* PROTOTIPI *************************/

static void               cgiGetenv(char **s, char *var);
static void               cgiSetupConstants();
static void               cgiFreeResources();
static int                cgiFirstNonspaceChar(char *s);
static int                cgiStrEqNc(char *s1, char *s2);
static int                cgiWriteString(FILE *out, char *s);
static int                cgiWriteInt(FILE *out, int i);
static int                cgiReadString(FILE *out, char **s);
static int                cgiReadInt(FILE *out, int *i);
static cgiParseResultType cgiParseGetFormInput();
static cgiParseResultType cgiParsePostFormInput();
static cgiParseResultType cgiParseFormInput(char *data, int length);
static cgiUnescapeResultType cgiUnescapeChars(char **sp, char *cp, int len);
static cgiFormResultType  cgiFormEntryString(cgiFormEntry *e, char *result, int max, int newlines);
static cgiFormEntry*      cgiFormEntryFindFirst(char *name);
static cgiFormEntry*      cgiFormEntryFindNext();


VOID CALLBACK LastChancheHandler(
  HWND hwnd,     // handle of window for timer messages
  UINT uMsg,     // WM_TIMER message
  UINT idEvent,  // timer identifier
  DWORD dwTime   // current system time
)
{
/**/if(usedbg){fprintf(debug, "Tannit Program Exiting through the LAST_CHANCHE_TIMER door...now is %d\n", (int)dwTime);fflush(debug);}
  exit(0);
//  EXIT(0);
}
 

int main(int argc, char *argv[])
{
	int   result;
	char* cgiContentLengthString;
	time_t      tm;
	struct tm*  today; 
	itxString   output;
  char pidStr[256];

  //Super mega ultra patch del timer come ultima sponda
//  int ret = SetTimer(NULL, LAST_CHANCHE_TIMER_ID, LAST_CHANCHE_SECONDS /* 1000*/, (TIMERPROC)LastChancheHandler);

	// Inizializzazione dell'eventuale file di debug
	usedbg = false;
	FILE * dbgnamefp;
  dbgnamefp = fopen(".\\cgi-bin\\dbgname.par", "r");
	if(dbgnamefp == 0)
	{
		dbgnamefp = fopen(".\\cgi-itx\\dbgname.par", "r");
		if(dbgnamefp == 0)
		{
			dbgnamefp = fopen("dbgname.par", "r");
		}
	}

	if(dbgnamefp != 0)
	{
		strcpy(DbgPath, readPar("DbgPath","", dbgnamefp));

		if (strstr(DbgPath, "MULTIDBG") != 0)
    {
	    time(&tm);
	    today = localtime(&tm);
      output.SetInt(tm);

      sprintf(pidStr, "%d", GetCurrentProcessId()),
      strcat(DbgPath, "_");
//      strcat(DbgPath, output.GetBuffer());
      strcat(DbgPath, pidStr);
      strcat(DbgPath, ".dbg");
    }

		fclose(dbgnamefp);

		if ((debug = fopen(DbgPath, "w")) != NULL)
			usedbg = true;

		if (usedbg)
		{
			if (strstr(DbgPath, "alloctrc") != 0 )
				alloctrc = true;
		}
	}

/**/if(usedbg){fprintf(debug, "Tannit Program Starting with pid = %d\n", GetCurrentProcessId());fflush(debug);}

//**/if(usedbg){fprintf(debug, "SetTimer returns %d\n", ret);fflush(debug);}
	
	cgiSetupConstants();
	cgiGetenv(&cgiServerSoftware, "SERVER_SOFTWARE");
	cgiGetenv(&cgiServerName, "SERVER_NAME");
	cgiGetenv(&cgiGatewayInterface, "GATEWAY_INTERFACE");
	cgiGetenv(&cgiServerProtocol, "SERVER_PROTOCOL");
	cgiGetenv(&cgiServerPort, "SERVER_PORT");
	cgiGetenv(&cgiRequestMethod, "REQUEST_METHOD");
	cgiGetenv(&cgiPathInfo, "PATH_INFO");
	cgiGetenv(&cgiPathTranslated, "PATH_TRANSLATED");
	cgiGetenv(&cgiScriptName, "SCRIPT_NAME");
	cgiGetenv(&cgiQueryString, "QUERY_STRING");
	cgiGetenv(&cgiHttpHost, "HTTP_HOST");
	cgiGetenv(&cgiRemoteHost, "REMOTE_HOST");
	cgiGetenv(&cgiRemoteAddr, "REMOTE_ADDR");
	cgiGetenv(&cgiAuthType, "AUTH_TYPE");
	cgiGetenv(&cgiRemoteUser, "REMOTE_USER");
	cgiGetenv(&cgiRemoteIdent, "REMOTE_IDENT");
	cgiGetenv(&cgiContentType, "CONTENT_TYPE");
	cgiGetenv(&cgiContentLengthString, "CONTENT_LENGTH");
	cgiContentLength = atoi(cgiContentLengthString);
	cgiGetenv(&cgiAccept, "HTTP_ACCEPT");
	cgiGetenv(&cgiReferrer, "HTTP_REFERER");
	cgiGetenv(&cgiUserAgent, "HTTP_USER_AGENT");

  if (cgiUserAgent != NULL)
  {
    if (strstr(cgiUserAgent, "Go!Zilla"))
    {
      printf("Go!Zilla not supported.");
/**/if(usedbg){fprintf(debug, "%s\n", "Go!Zilla not supported.");fflush(debug);}
      exit(23);
    }
    if (strstr(cgiUserAgent, "downloadaccelerator"))
    {
      printf("downloadaccelerator not supported.");
/**/if(usedbg){fprintf(debug, "%s\n", "downloadaccelerator not supported.");fflush(debug);}
      exit(23);
    }
  }
	cgiGetenv(&cgiCookie, "HTTP_COOKIE");

  sprintf(UserAgentSpy, cgiUserAgent); 
/**/if(usedbg){fprintf(debug, "cgiServerSoftware:%s\n",				cgiServerSoftware          );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiServerName:%s\n",						cgiServerName              );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiGatewayInterface:%s\n",			cgiGatewayInterface        );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiServerProtocol:%s\n",				cgiServerProtocol          );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiServerPort:%s\n",						cgiServerPort              );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiRequestMethod:%s\n",				cgiRequestMethod           );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiPathInfo:%s\n",							cgiPathInfo                );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiPathTranslated:%s\n",				cgiPathTranslated          );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiScriptName:%s\n",						cgiScriptName              );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiQueryString:%s\n",					cgiQueryString             );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiHttpHost:%s\n",							cgiHttpHost                );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiRemoteHost:%s\n",						cgiRemoteHost              );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiRemoteAddr:%s\n",						cgiRemoteAddr              );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiAuthType:%s\n",							cgiAuthType                );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiRemoteUser:%s\n",						cgiRemoteUser              );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiRemoteIdent:%s\n",					cgiRemoteIdent             );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiContentType:%s\n",					cgiContentType             );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiContentLengthString:%s\n",	cgiContentLengthString     );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiAccept:%s\n",								cgiAccept                  );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiReferrer:%s\n",							cgiReferrer                );fflush(debug);}
/**/if(usedbg){fprintf(debug, "cgiUserAgent:%s\n",						cgiUserAgent               );fflush(debug);}
/**/if(usedbg){fprintf(debug, "Cookie:%s\n",                  cgiCookie                  );fflush(debug);}

	cgiFormEntryFirst = 0;
	cgiIn = stdin;
	cgiOut = stdout;
	cgiRestored = 0;

	/* These five lines keep compilers from
		producing warnings that argc and argv
		are unused. They have no actual function. */
	if (argc) 
  {
		if (argv[0]) 
			cgiRestored = 0;
	}	

	if (cgiStrEqNc(cgiRequestMethod, "post")) 
  {
		if (cgiStrEqNc(cgiContentType, "application/x-www-form-urlencoded")) 
    {	
			if (cgiParsePostFormInput() != cgiParseSuccess) 
      {
				cgiFreeResources();
				return -1;
			}	
		}

/* -          C O D I C E   A I T E C S A           -*/
/*
    if (strstr( cgiContentType,"multipart/form-data" ) != NULL) 
    {
			if (itxCGIParseMultipartInput() != cgiParseSuccess) 
      {
				cgiFreeResources();
				return -1;
			}
		}
*/
/* - FINE - C O D I C E   A I T E C S A -  FINE - */

  } 
  else if (cgiStrEqNc(cgiRequestMethod, "get")) 
  {	
		/* The spec says this should be taken care of by
			the server, but... it isn't */
		cgiContentLength = strlen(cgiQueryString);
		if (cgiParseGetFormInput() != cgiParseSuccess) 
    {
/**/if(usedbg){fprintf(debug, "ATTENZIONE : cgiParseGetFormInput != cgiParseSuccess\n");fflush(debug);}
			cgiFreeResources();

			exit(-1);
		} 
	}

	result = cgiMain();
	cgiFreeResources();

	/* The following two lines allegedly produce better behavior
		when run with the CERN server on some platforms. 
		Remove them and experiment if you wish. */
	fflush(stdout);
//	sleep(1);

	/**/if(usedbg){fprintf(debug, "TANNIT - D O N E \n");fflush(debug);}
	EXIT(2);
  return result;
}

static void cgiGetenv(char** s, char* var)
{
	*s = getenv(var);
	if (!(*s)) 
		*s = "";
}

static cgiParseResultType cgiParsePostFormInput() 
{
	char*               input;
	cgiParseResultType  result;

	if (!cgiContentLength) 
		return cgiParseSuccess;
	
  input = (char*)malloc(cgiContentLength);

/**/if(alloctrc){fprintf(debug, "- - input:%d - - len:%d\n", input, cgiContentLength );fflush(debug);}

	if (!input) 
		return cgiParseMemory;	
	
  if (fread(input, 1, cgiContentLength, cgiIn) != (unsigned int)cgiContentLength) 
		return cgiParseIO;
	
  result = cgiParseFormInput(input, cgiContentLength);

/**/if(alloctrc){fprintf(debug, "* * input:%d\n", input);fflush(debug);}

	free(input);

	return result;
}

static cgiParseResultType cgiParseGetFormInput() 
{
	return cgiParseFormInput(cgiQueryString, cgiContentLength);
}

static cgiParseResultType cgiParseFormInput(char *data, int length) 
{
	/* Scan for pairs, unescaping and storing them as they are found. */
	int pos = 0;
	cgiFormEntry *n;
	cgiFormEntry *l = 0;
	while (pos != length) 
  {
		int foundEq = 0;
		int foundAmp = 0;
		int start = pos;
		int len = 0;
		char *attr;
		char *value;
		
    while (pos != length) 
    {
			if (data[pos] == '=') 
      {
				foundEq = 1;
				pos++;
				break;
			}
			pos++;
			len++;
		}
		if (!foundEq) 
			break;

		if (cgiUnescapeChars(&attr, data+start, len) != cgiUnescapeSuccess) 
			return cgiParseMemory;

		start = pos;
		len = 0;
		while (pos != length) 
    {
			if (data[pos] == '&') 
      {
				foundAmp = 1;
				pos++;
				break;
			}
			pos++;
			len++;
		}

		/* The last pair probably won't be followed by a &, but
			that's fine, so check for that after accepting it */
		if (cgiUnescapeChars(&value, data+start, len) != cgiUnescapeSuccess) 
			return cgiParseMemory;

		/* OK, we have a new pair, add it to the list. */
		n = (cgiFormEntry *) malloc(sizeof(cgiFormEntry));	
/**/if(alloctrc){fprintf(debug, "- - n:%d - - len:%d\n", n, sizeof(cgiFormEntry));fflush(debug);}
		if (!n) 
			return cgiParseMemory;

		n->attr = attr;
		n->value = value;
		n->next = 0;

/**/if(usedbg){fprintf(debug, "Acquisizione parametro get (1): %s  value: %s\n", attr, value);fflush(debug);}

		if (!l) 
			cgiFormEntryFirst = n;
    else 
 			l->next = n;
		
    l = n;
		if (!foundAmp) 
			break;
	}

	return cgiParseSuccess;
}

cgiUnescapeResultType cgiUnescapeChars(char **sp, char *cp, int len)
{
	char*           s;
	cgiEscapeState  escapeState = cgiEscapeRest;
	int             escapedValue = 0;
	int             srcPos = 0;
	int             dstPos = 0;

	s = (char*)malloc(len + 1);

/**/if(alloctrc){fprintf(debug, "- - s:%d - - len:%d\n", s, len + 1);fflush(debug);}

	if (!s)
		return cgiUnescapeMemory;

	while (srcPos < len) 
  {
		int ch = cp[srcPos];
		switch (escapeState) 
    {
			case cgiEscapeRest:
			  if (ch == '%') 
				  escapeState = cgiEscapeFirst;
        else if (ch == '+') 
				  s[dstPos++] = ' ';
        else 
				  s[dstPos++] = ch;	
			break;
			
      case cgiEscapeFirst:
			  escapedValue = cgiHexValue[ch] << 4;
        /**/if(usedbg){fprintf(debug, "cgiUnescapeChars: 1-escapedValue=%d\n",escapedValue);fflush(debug);}
			  escapeState = cgiEscapeSecond;
			break;

			case cgiEscapeSecond:
			  escapedValue += cgiHexValue[ch];
        /**/if(usedbg){fprintf(debug, "cgiUnescapeChars: 2-escapedValue=%d\n",escapedValue);fflush(debug);}
			  s[dstPos++] = escapedValue;
			  escapeState = cgiEscapeRest;
			break;
		}
		srcPos++;
	}
	s[dstPos] = '\0';
	*sp = s;
/**/if(usedbg){fprintf(debug, "cgiUnescapeChars: s=%s\n",s);fflush(debug);}

	return cgiUnescapeSuccess;
}		

static void cgiSetupConstants() 
{
	for (int i=0; (i < 256); i++) 
		cgiHexValue[i] = 0;

	cgiHexValue['0'] = 0;	
	cgiHexValue['1'] = 1;	
	cgiHexValue['2'] = 2;	
	cgiHexValue['3'] = 3;	
	cgiHexValue['4'] = 4;	
	cgiHexValue['5'] = 5;	
	cgiHexValue['6'] = 6;	
	cgiHexValue['7'] = 7;	
	cgiHexValue['8'] = 8;	
	cgiHexValue['9'] = 9;
	cgiHexValue['A'] = 10;
	cgiHexValue['B'] = 11;
	cgiHexValue['C'] = 12;
	cgiHexValue['D'] = 13;
	cgiHexValue['E'] = 14;
	cgiHexValue['F'] = 15;
	cgiHexValue['a'] = 10;
	cgiHexValue['b'] = 11;
	cgiHexValue['c'] = 12;
	cgiHexValue['d'] = 13;
	cgiHexValue['e'] = 14;
	cgiHexValue['f'] = 15;
}

static void cgiFreeResources() 
{
	cgiFormEntry *c = cgiFormEntryFirst;
	cgiFormEntry *n;

	while (c) 
  {
//**/if(usedbg){fprintf(debug, "c->attr  :%s\n", c->attr);fflush(debug);}
//**/if(usedbg){fprintf(debug, "c->value :%s\n", c->value);fflush(debug);}
		n = c->next;
/**/if(alloctrc){fprintf(debug, "* * c->attr:%d - %s\n", c->attr, c->attr);fflush(debug);}
		free(c->attr);
/**/if(alloctrc){fprintf(debug, "* * c->value:%d - %s\n", c->value, c->value);fflush(debug);}
		free(c->value);

/*+++++++++++++++attenzione !!! <-- e' una patch: capire perche' schioppa su questa free!*/
/**/if(alloctrc){fprintf(debug, "* * c:%d\n", c);fflush(debug);}
//free(c); 

//**/if(usedbg){fprintf(debug, "1 Freed\n");fflush(debug);}
		c = n;
	}
	
  /* If the cgi environment was restored from a saved environment,
		then these are in allocated space and must also be freed */
	if (cgiRestored) 
  {
		free(cgiServerSoftware);
		free(cgiServerName);
		free(cgiGatewayInterface);
		free(cgiServerProtocol);
		free(cgiServerPort);
		free(cgiRequestMethod);
		free(cgiPathInfo);
		free(cgiPathTranslated);
		free(cgiScriptName);
		free(cgiQueryString);
		free(cgiRemoteHost);
		free(cgiRemoteAddr);
		free(cgiAuthType);
		free(cgiRemoteUser);
		free(cgiRemoteIdent);
		free(cgiContentType);
		free(cgiAccept);
		free(cgiUserAgent);
	}
/**/if(usedbg){fprintf(debug, "cgiFreeResources - D O N E \n");fflush(debug);}
}

cgiFormResultType cgiFormString(char *name, char *result, int max) 
{
	cgiFormEntry* e = cgiFormEntryFindFirst(name);

	if (!e) 
  {
		strcpy(result, "");
		return cgiFormNotFound;
	}

	return cgiFormEntryString(e, result, max, 1);
}

cgiFormResultType cgiFormStringNoNewlines(char *name, char *result, int max) 
{
	cgiFormEntry* e = cgiFormEntryFindFirst(name);

	if (!e) 
  {
		strcpy(result, "");
		return cgiFormNotFound;
	}

	return cgiFormEntryString(e, result, max, 0);
}

cgiFormResultType cgiFormStringMultiple(char* name, char*** result)
{
	char**        stringArray;
	cgiFormEntry* e;
	int i;
	int total = 0;

	/* Make two passes. One would be more efficient, but this
		function is not commonly used. The select menu and
		radio box functions are faster. */
	e = cgiFormEntryFindFirst(name);
	if (e != 0) 
  {
		do 
    {
			total++;
		} 
    while ((e = cgiFormEntryFindNext()) != 0); 
	}

	stringArray = (char**)malloc(sizeof(char*) * (total + 1));

/**/if(alloctrc){fprintf(debug, "- - stringArray:%d - - len:%d\n", stringArray, sizeof(char *) * (total + 1));fflush(debug);}

	if (!stringArray) 
  {
		*result = 0;
		return cgiFormMemory;
	}

	/* initialize all entries to null; the last will stay that way */
	for (i=0; (i <= total); i++) 
		stringArray[i] = 0;

	/* Now go get the entries */
	e = cgiFormEntryFindFirst(name);

  if (e) 
  {
		i = 0;
		do 
    {
			int max = strlen(e->value) + 1;
			stringArray[i] = (char *) malloc(max);
      /**/if(alloctrc){fprintf(debug, "- - stringArray[%d]:%d - - len:%d\n", i, stringArray[i], max );fflush(debug);}
			if (stringArray[i] == 0) 
      {
				/* Memory problems */
				cgiStringArrayFree(stringArray);
				*result = 0;
				return cgiFormMemory;
			}	
			strcpy(stringArray[i], e->value);
			cgiFormEntryString(e, stringArray[i], max, 1);
			i++;
		} 
    while ((e = cgiFormEntryFindNext()) != 0);

		*result = stringArray;

		return cgiFormSuccess;
	} 
  else 
  {
		*result = stringArray;

		return cgiFormNotFound;
	}	
}

cgiFormResultType cgiFormStringSpaceNeeded(char *name, int *result) 
{
	cgiFormEntry* e = cgiFormEntryFindFirst(name);
	if (!e) 
  {
		*result = 1;
		return cgiFormNotFound; 
	}
	
  *result = strlen(e->value) + 1;
	return cgiFormSuccess;
}

static cgiFormResultType cgiFormEntryString(cgiFormEntry *e, char *result, int max, int newlines) 
{
	char  *dp, *sp;
	int   truncated = 0;
	int   len = 0;
	int   avail = max-1;
	int   crCount = 0;
	int   lfCount = 0;	

	dp = result;
	sp = e->value;	

	while (1) 
  {
		int ch;
		ch = *sp;
		if (len == avail) 
    {
			truncated = 1;
			break;
		}	 
		
    /* Fix the CR/LF, LF, CR nightmare: watch for
			consecutive bursts of CRs and LFs in whatever
			pattern, then actually output the larger number 
			of LFs. Consistently sane, yet it still allows
			consecutive blank lines when the user
			actually intends them. */
		if ((ch == 13) || (ch == 10)) 
    {
			if (ch == 13) 
				crCount++;
      else 
				lfCount++;
		} 
    else 
    {
			if (crCount || lfCount) 
      {
				int lfsAdd = crCount;
				
        if (lfCount > crCount) 
					lfsAdd = lfCount;
				
        /* Stomp all newlines if desired */
				if (!newlines) 
					lfsAdd = 0;

				while (lfsAdd) 
        {
					if (len == avail) 
          {
						truncated = 1;
						break;
					}
					*dp = 10;
					dp++;
					lfsAdd--;
					len++;		
				}
				crCount = 0;
				lfCount = 0;
			}
			
      if (ch == '\0') 				
				break;  /* The end of the source string */

			*dp = ch;
			dp++;
			len++;
		}
		sp++;	
	}	
	*dp = '\0';
	if (truncated) 
		return cgiFormTruncated;
  else if (!len) 
		return cgiFormEmpty;
  else 
		return cgiFormSuccess;
}


cgiFormResultType cgiFormInteger(char *name, int *result, int defaultV) 
{
	int ch;
	cgiFormEntry* e = cgiFormEntryFindFirst(name);

	if (!e) 
  {
		*result = defaultV;
		return cgiFormNotFound; 
	}	
	if (!strlen(e->value)) 
  {
		*result = defaultV;
		return cgiFormEmpty;
	}
	ch = cgiFirstNonspaceChar(e->value);
	
  if (!(isdigit(ch)) && (ch != '-') && (ch != '+')) 
  {
		*result = defaultV;
		return cgiFormBadType;
	} 
  else 
  {
		*result = atoi(e->value);
		return cgiFormSuccess;
	}
}

cgiFormResultType cgiFormIntegerBounded(char *name, int *result, int min, int max, int defaultV) 
{
	cgiFormResultType error = cgiFormInteger(name, result, defaultV);
	
  if (error != cgiFormSuccess) 
		return error;

	if (*result < min) 
  {
		*result = min;
		return cgiFormConstrained;
	} 
	
  if (*result > max) 
  {
		*result = max;
		return cgiFormConstrained;
	} 
	
  return cgiFormSuccess;
}

cgiFormResultType cgiFormDouble(char *name, double *result, double defaultV) 
{
	int ch;
	cgiFormEntry* e = cgiFormEntryFindFirst(name);

	if (!e) 
  {
		*result = defaultV;
		return cgiFormNotFound; 
	}	
	if (!strlen(e->value)) 
  {
		*result = defaultV;
		return cgiFormEmpty;
	} 
	ch = cgiFirstNonspaceChar(e->value);
	
  if (!(isdigit(ch)) && (ch != '.') && (ch != '-') && (ch != '+')) 
  {
		*result = defaultV;
		return cgiFormBadType;
	} 
  else 
  {
		*result = atof(e->value);
		return cgiFormSuccess;
	}
}

cgiFormResultType cgiFormDoubleBounded(char *name, double *result, double min, double max, double defaultV) 
{
	cgiFormResultType error = cgiFormDouble(name, result, defaultV);
	if (error != cgiFormSuccess) 
		return error;

	if (*result < min) 
  {
		*result = min;
		return cgiFormConstrained;
	} 
	
  if (*result > max) 
  {
		*result = max;
		return cgiFormConstrained;
	} 
	
  return cgiFormSuccess;
}

cgiFormResultType cgiFormSelectSingle(char *name, char **choicesText, int choicesTotal, int *result, int defaultV)
{
	int i;
	cgiFormEntry* e = cgiFormEntryFindFirst(name);

  if (!e) 
  {
		*result = defaultV;
		return cgiFormNotFound;
	}
	
  for (i=0; (i < choicesTotal); i++) 
  {

    if (cgiStrEq(choicesText[i], e->value)) 
    {

			*result = i;
			return cgiFormSuccess;
		}
	}

	*result = defaultV;

	return cgiFormNoSuchChoice;
}

cgiFormResultType cgiFormSelectMultiple(char *name, char **choicesText, int choicesTotal, int *result, int *invalid)
{
	cgiFormEntry* e = cgiFormEntryFindFirst(name);
	int i;
	int hits = 0;
	int invalidE = 0;

	for (i=0; (i < choicesTotal); i++) 
		result[i] = 0;
	
	if (!e) 
  {
		*invalid = invalidE;
		return cgiFormNotFound;
	}
	
  do 
  {
		int hit = 0;
		for (i=0; (i < choicesTotal); i++) 
    {
			if (cgiStrEq(choicesText[i], e->value)) 
      {
				result[i] = 1;
				hits++;
				hit = 1;
				break;
			}
		}

		if (!(hit)) 
			invalidE++;
	} 
  while ((e = cgiFormEntryFindNext()) != 0);

	*invalid = invalidE;

	if (hits) 
		return cgiFormSuccess;
  else 
		return cgiFormNotFound;
}

cgiFormResultType cgiFormCheckboxSingle(char *name)
{
	cgiFormEntry* e = cgiFormEntryFindFirst(name);

	if (!e) 
		return cgiFormNotFound;

	return cgiFormSuccess;
}

cgiFormResultType cgiFormCheckboxMultiple(char *name, char **valuesText, int valuesTotal, int *result, int *invalid)
{
	/* Implementation is identical to cgiFormSelectMultiple. */
	return cgiFormSelectMultiple(name, valuesText, valuesTotal, result, invalid);
}

cgiFormResultType cgiFormRadio(char *name, char **valuesText, int valuesTotal, int *result, int defaultV)
{
	/* Implementation is identical to cgiFormSelectSingle. */
	return cgiFormSelectSingle(name, valuesText, valuesTotal, result, defaultV);
}

void cgiHeaderLocation(char *redirectUrl) 
{	
  fprintf(cgiOut, "Location: %s%c%c", redirectUrl, 10, 10); 
}

void cgiHeaderStatus(int status, char *statusMessage) 
{
	fprintf(cgiOut, "Status: %d %s%c%c", status, statusMessage, 10, 10);
}

void cgiHeaderContentType(char *mimeType) 
{
	fprintf(cgiOut, "Content-type: %s%c%c", mimeType, 10, 10);
}

#ifndef NO_SYSTEM

int cgiSaferSystem(char *command) 
{
	char  *s;
	char  *sp;
	int   i;
	int   len = (strlen(command) * 2) + 1;

	s = (char *) malloc(len);

/**/if(alloctrc){fprintf(debug, "- - s:%d - - len:%d\n", s, len );fflush(debug);}

	if (!s) 
		return -1;

	sp = s;
	for (i=0; (i < len); i++) 
  {
		if (command[i] == ';') 
    {
			*sp = '\\';
			sp++;
		} 
    else if (command[i] == '|') 
    {
			*sp = '\\';
			sp++;
		}
		
    *sp = command[i];
		sp++;
	}

	*sp = '\0';

	return system(s);
}		

#endif /* NO_SYSTEM */

cgiEnvironmentResultType cgiWriteEnvironment(char *filename) 
{
	FILE *out;
	cgiFormEntry *e;
	/* Be sure to open in binary mode */
	out = fopen(filename, "wb");
	if (!out) 
		return cgiEnvironmentIO;  /* Can't create file */

	if (!cgiWriteString(out, cgiServerSoftware)) 
		goto error;
	
  if (!cgiWriteString(out, cgiServerName)) 
		goto error;
	
  if (!cgiWriteString(out, cgiGatewayInterface)) 
		goto error;
	
  if (!cgiWriteString(out, cgiServerProtocol)) 
		goto error;
	
  if (!cgiWriteString(out, cgiServerPort)) 
		goto error;
	
  if (!cgiWriteString(out, cgiRequestMethod)) 
		goto error;
	
  if (!cgiWriteString(out, cgiPathInfo)) 
		goto error;

	if (!cgiWriteString(out, cgiPathTranslated)) 
		goto error;
	
  if (!cgiWriteString(out, cgiScriptName)) 
		goto error;
	
  if (!cgiWriteString(out, cgiQueryString)) 
		goto error;
	
  if (!cgiWriteString(out, cgiRemoteHost)) 
		goto error;
	
  if (!cgiWriteString(out, cgiRemoteAddr)) 
		goto error;
	
  if (!cgiWriteString(out, cgiAuthType)) 
		goto error;
	
  if (!cgiWriteString(out, cgiRemoteUser)) 
		goto error;
	
  if (!cgiWriteString(out, cgiRemoteIdent)) 
		goto error;
	
  if (!cgiWriteString(out, cgiContentType)) 
		goto error;
	
  if (!cgiWriteString(out, cgiAccept))
		goto error;
	
  if (!cgiWriteString(out, cgiUserAgent)) 
		goto error;
	
  if (!cgiWriteInt(out, cgiContentLength)) 
		goto error;

	e = cgiFormEntryFirst;
	while (e) 
  {
		if (!cgiWriteString(out, e->attr)) 
			goto error;

		if (!cgiWriteString(out, e->value)) 
			goto error;

		e = e->next;
	}

	fclose(out);
	return cgiEnvironmentSuccess;

error:
	fclose(out);
	/* If this function is not defined in your system,
		you must substitute the appropriate 
		file-deletion function. */
	_unlink(filename);
	return cgiEnvironmentIO;
}

static int cgiWriteString(FILE *out, char *s) 
{
	unsigned int len = strlen(s);
	cgiWriteInt(out, len);
	if (fwrite(s, 1, len, out) != len) 
		return 0;

	return 1;
}

static int cgiWriteInt(FILE *out, int i) 
{
	if (!fwrite(&i, sizeof(int), 1, out)) 
		return 0;

	return 1;
}

cgiEnvironmentResultType cgiReadEnvironment(char *filename) 
{
	FILE *in;
	cgiFormEntry *e, *p;

	/* Free any existing data first */
	cgiFreeResources();

	/* Be sure to open in binary mode */
	in = fopen(filename, "rb");
	if (!in) 		
		return cgiEnvironmentIO;  /* Can't access file */

	if (!cgiReadString(in, &cgiServerSoftware)) 
		goto error;

	if (!cgiReadString(in, &cgiServerName)) 
		goto error;

	if (!cgiReadString(in, &cgiGatewayInterface)) 
		goto error;

	if (!cgiReadString(in, &cgiServerProtocol)) 
		goto error;

	if (!cgiReadString(in, &cgiServerPort)) 
		goto error;

	if (!cgiReadString(in, &cgiRequestMethod)) 
		goto error;

	if (!cgiReadString(in, &cgiPathInfo)) 
		goto error;

	if (!cgiReadString(in, &cgiPathTranslated)) 
		goto error;

	if (!cgiReadString(in, &cgiScriptName)) 
		goto error;

	if (!cgiReadString(in, &cgiQueryString)) 
		goto error;

	if (!cgiReadString(in, &cgiRemoteHost)) 
		goto error;

	if (!cgiReadString(in, &cgiRemoteAddr)) 
		goto error;

	if (!cgiReadString(in, &cgiAuthType)) 
		goto error;

	if (!cgiReadString(in, &cgiRemoteUser)) 
		goto error;

	if (!cgiReadString(in, &cgiRemoteIdent)) 
		goto error;

	if (!cgiReadString(in, &cgiContentType)) 
		goto error;

	if (!cgiReadString(in, &cgiAccept)) 
		goto error;

	if (!cgiReadString(in, &cgiUserAgent)) 
		goto error;

	if (!cgiReadInt(in, &cgiContentLength)) 
		goto error;

	p = 0;
	while (1) 
  {
		e = (cgiFormEntry *) malloc(sizeof(cgiFormEntry));
/**/if(alloctrc){fprintf(debug, "- - e:%d - - len:%d\n", e, sizeof(cgiFormEntry) );fflush(debug);}
		if (!e) 
    {
			cgiFreeResources();
			fclose(in);
			return cgiEnvironmentMemory;
		}
		
    if (!cgiReadString(in, &e->attr)) 
    {
			/* This means we've reached the end of the list. */
/**/if(alloctrc){fprintf(debug, "* * e:%d\n", e);fflush(debug);}
			free(e);
			break;
		}
		
    if (!cgiReadString(in, &e->value)) 
    {
/**/if(alloctrc){fprintf(debug, "* * e:%d\n", e);fflush(debug);}
			free(e);
			goto error;
		}
		e->next = 0;
		if (p) 
			p->next = e;
    else 
			cgiFormEntryFirst = e;

		p = e;
	}

	fclose(in);
	cgiRestored = 1;
	return cgiEnvironmentSuccess;
error:
	cgiFreeResources();
	fclose(in);
	return cgiEnvironmentIO;
}

static int cgiReadString(FILE *in, char **s) 
{
	unsigned int len;

	cgiReadInt(in, (int *) &len);
	*s = (char *) malloc(len + 1);

/**/if(alloctrc){fprintf(debug, "- - *s:%d - - len:%d\n", *s, len + 1 );fflush(debug);}

	if (!(*s)) 
		return 0;

	if (fread(*s, 1, len, in) != len) 
		return 0;

	(*s)[len] = '\0';

	return 1;
}

static int cgiReadInt(FILE *out, int *i) 
{
	if (!fread(i, sizeof(int), 1, out)) 
		return 0;

	return 1;
}

static int cgiStrEqNc(char *s1, char *s2) 
{
	while(1) 
  {
		if (!(*s1)) 
    {
			if (!(*s2)) 
				return 1;
      else 
				return 0;
		} 
    else if (!(*s2)) 
			return 0;
		
    if (isalpha(*s1)) 
    {
			if (tolower(*s1) != tolower(*s2)) 
				return 0;
		} 
    else if ((*s1) != (*s2)) 
			return 0;
		
    s1++;
		s2++;
	}
}

static cgiFormEntry *cgiFormEntryFindFirst(char *name) 
{
	cgiFindTarget = name;
	cgiFindPos = cgiFormEntryFirst;

	return cgiFormEntryFindNext();
}

static cgiFormEntry* cgiFormEntryFindNext() 
{
	while (cgiFindPos) 
  {
		cgiFormEntry* c = cgiFindPos;
		cgiFindPos = c->next;
		if (!strcmp(c->attr, cgiFindTarget))
			return c;
	}
	return 0;
}

static int cgiFirstNonspaceChar(char *s) 
{
	int len = strspn(s, " \n\r\t");
	return s[len];
}

void cgiStringArrayFree(char **stringArray) 
{
	char *p;
	p = *stringArray;

	while (p) 
  {
/**/if(alloctrc){fprintf(debug, "* * p:%d\n", p);fflush(debug);}
		free(p);
		stringArray++;
		p = *stringArray;
	}
}	

