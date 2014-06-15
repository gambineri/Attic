/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/*
 | _-.-..-._-_-._--_-._.-._     AITECSA S.r.l.   2000
 |
 |
 | FILENAME    : cgiresolver.cpp
 | TAB         : 2 spaces
 |
 | DESCRIPTION : CGIResolver object implementation file.
 |
 |
 */


#ifdef FCGITANNIT
#include "fcgi_config.h"
#include "fcgi_stdio.h"
#else
#include <stdio.h>
#endif


#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "itxlib.h"
#include "cgiresolver.h"


//Sets to empty an environment variable if null: appo must be provided as an aux char*
#define GETENV(appo, varname)  (((appo) = getenv(varname)) == NULL ? "" : (appo))



/*****************************************************************************
 ---------------------------- CGIResolver METHODS ----------------------------
 *****************************************************************************/

CGIResolver::CGIResolver() : m_PRMConf(TANNIT_CONFIG_FILE)
{
	cgiIn             = stdin;
	cgiOut            = stdout;
  cgiFormEntryFirst = NULL;
  cgiFindTarget     = 0;
  cgiFindPos        = 0;

  m_Sys.FSSetMode(cgiOut, _O_BINARY);
  memset(cgiHexValue, 0, 256);

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

CGIResolver::~CGIResolver()
{
	cgiFreeResources();
}

void CGIResolver::GetEnvironment(bool must_trace)
{
  char* appo;

	cgiServerSoftware   = (char*)GETENV(appo, "SERVER_SOFTWARE");
	cgiServerName       = (char*)GETENV(appo, "SERVER_NAME");
	cgiGatewayInterface = (char*)GETENV(appo, "GATEWAY_INTERFACE");
	cgiServerProtocol   = (char*)GETENV(appo, "SERVER_PROTOCOL");
	cgiServerPort       = (char*)GETENV(appo, "SERVER_PORT");
	cgiRequestMethod    = (char*)GETENV(appo, "REQUEST_METHOD");
	cgiPathInfo         = (char*)GETENV(appo, "PATH_INFO");
	cgiPathTranslated   = (char*)GETENV(appo, "PATH_TRANSLATED");
	cgiScriptName       = (char*)GETENV(appo, "SCRIPT_NAME");
	cgiQueryString      = (char*)GETENV(appo, "QUERY_STRING");
	cgiHttpHost         = (char*)GETENV(appo, "HTTP_HOST");
	cgiRemoteHost       = (char*)GETENV(appo, "REMOTE_HOST");
	cgiRemoteAddr       = (char*)GETENV(appo, "REMOTE_ADDR");
	cgiAuthType         = (char*)GETENV(appo, "AUTH_TYPE");
	cgiRemoteUser       = (char*)GETENV(appo, "REMOTE_USER");
	cgiRemoteIdent      = (char*)GETENV(appo, "REMOTE_IDENT");
	cgiContentType      = (char*)GETENV(appo, "CONTENT_TYPE");
	cgiAccept           = (char*)GETENV(appo, "HTTP_ACCEPT");
	cgiCookie           = (char*)GETENV(appo, "HTTP_COOKIE");
	cgiUserAgent        = (char*)GETENV(appo, "HTTP_USER_AGENT");
	cgiReferrer         = (char*)GETENV(appo, "HTTP_REFERER");

	char* cgiContentLengthString = NULL;
	cgiContentLengthString = (char*)GETENV(appo, "CONTENT_LENGTH");
	cgiContentLength = atoi(cgiContentLengthString);

  //Trace values on the debug file
  if (must_trace)
  {
    DebugTrace2(DEFAULT, "cgiServerSoftware:%s\n",		cgiServerSoftware  );
    DebugTrace2(DEFAULT, "cgiServerName:%s\n",				cgiServerName      );
    DebugTrace2(DEFAULT, "cgiGatewayInterface:%s\n",	cgiGatewayInterface);
    DebugTrace2(DEFAULT, "cgiServerProtocol:%s\n",		cgiServerProtocol  );
    DebugTrace2(DEFAULT, "cgiServerPort:%s\n",				cgiServerPort      );
    DebugTrace2(DEFAULT, "cgiRequestMethod:%s\n",		  cgiRequestMethod   );
    DebugTrace2(DEFAULT, "cgiPathInfo:%s\n",					cgiPathInfo        );
    DebugTrace2(DEFAULT, "cgiPathTranslated:%s\n",		cgiPathTranslated  );
    DebugTrace2(DEFAULT, "cgiScriptName:%s\n",				cgiScriptName      );
    DebugTrace2(DEFAULT, "cgiQueryString:%s\n",			  cgiQueryString     );
    DebugTrace2(DEFAULT, "cgiHttpHost:%s\n",					cgiHttpHost        );
    DebugTrace2(DEFAULT, "cgiRemoteHost:%s\n",				cgiRemoteHost      );
    DebugTrace2(DEFAULT, "cgiRemoteAddr:%s\n",				cgiRemoteAddr      );
    DebugTrace2(DEFAULT, "cgiAuthType:%s\n",					cgiAuthType        );
    DebugTrace2(DEFAULT, "cgiRemoteUser:%s\n",				cgiRemoteUser      );
    DebugTrace2(DEFAULT, "cgiRemoteIdent:%s\n",			  cgiRemoteIdent     );
    DebugTrace2(DEFAULT, "cgiContentType:%s\n",			  cgiContentType     );
    DebugTrace2(DEFAULT, "cgiContentLength:%d\n",	    cgiContentLength   );
    DebugTrace2(DEFAULT, "cgiAccept:%s\n",						cgiAccept          );
    DebugTrace2(DEFAULT, "cgiReferrer:%s\n",					cgiReferrer        );
    DebugTrace2(DEFAULT, "cgiUserAgent:%s\n",				  cgiUserAgent       );
    DebugTrace2(DEFAULT, "Cookie:%s\n",               cgiCookie          );
  }
}

bool CGIResolver::ReadPostContent() 
{
	char*               input;

	if (!cgiContentLength) 
		return false;
	
	if ((input = (char*)malloc(cgiContentLength)) == NULL) 
		return false;	
	
  if (fread(input, 1, cgiContentLength, cgiIn) != (unsigned int)cgiContentLength) 
		return false;
	
  m_POSTbody.Strncpy(input, cgiContentLength);
	free(input);

	return true;
}

cgiParseResultType CGIResolver::cgiParseGetFormInput() 
{
	return cgiParseFormInput(cgiQueryString, cgiContentLength);
}

cgiParseResultType CGIResolver::cgiParseFormInput(char *data, int length) 
{
	/* Scan for pairs, unescaping and storing them as they are found. */
	int           pos = 0;
	cgiFormEntry* n;
	cgiFormEntry* l = 0;
	
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
		if (!n) 
			return cgiParseMemory;

		n->attr = attr;
		n->value = value;
		n->next = 0;

    DebugTrace2(DEFAULT, "Acquisizione parametro get (1): %s  value: %s\n", attr, value);

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

UnescapeResultType_t CGIResolver::cgiUnescapeChars(char **sp, char *cp, int len)
{
	char*          s;
	EscapeState_t  escapeState = cgiEscapeRest;
	int            escapedValue = 0;
	int            srcPos = 0;
	int            dstPos = 0;

	if ((s = (char*)malloc(len + 1)) == NULL)
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
        DebugTrace2(DEFAULT, "cgiUnescapeChars: 1-escapedValue=%d\n",escapedValue);
			  escapeState = cgiEscapeSecond;
			break;

			case cgiEscapeSecond:
			  escapedValue += cgiHexValue[ch];
        DebugTrace2(DEFAULT, "cgiUnescapeChars: 2-escapedValue=%d\n",escapedValue);
			  s[dstPos++] = escapedValue;
			  escapeState = cgiEscapeRest;
			break;
		}
		srcPos++;
	}
	s[dstPos] = '\0';
	*sp = s;
  DebugTrace2(DEFAULT, "cgiUnescapeChars: s=%s\n",s);

	return cgiUnescapeSuccess;
}		

void CGIResolver::cgiFreeResources() 
{
	cgiFormEntry* c = cgiFormEntryFirst;
	cgiFormEntry* n;

	while (c) 
  {
    DebugTrace2(DEFAULT, "c->attr  :%s\n", c->attr);
    DebugTrace2(DEFAULT, "c->value :%s\n", c->value);
		n = c->next;
		free(c->attr);
		free(c->value);

/*+++++++++++++++attenzione !!! <-- e' una patch: capire perche' schioppa su questa free!*/
//free(c); 

    DebugTrace2(DEFAULT, "1 Freed\n");
		c = n;
	}
	
  DebugTrace2(DEFAULT, "cgiFreeResources - D O N E \n");
}

FormResultType_t CGIResolver::cgiFormString(char *name, char *result, int max) 
{
	cgiFormEntry* e;

	if ((e = cgiFormEntryFindFirst(name)) == NULL) 
  {
		strcpy(result, "");
		return cgiFormNotFound;
	}

	return cgiFormEntryString(e, result, max, 1);
}

//itxString version
FormResultType_t CGIResolver::cgiFormString(char *name, itxString* istrresult, int max) 
{
	cgiFormEntry* e;

	if ((e = cgiFormEntryFindFirst(name)) == NULL) 
  {
    *istrresult = "";
		return cgiFormNotFound;
	}

  istrresult->Space(strlen(e->value) + 1);
  FormResultType_t ret = cgiFormEntryString(e, istrresult->GetBuffer(), max, 1);
  istrresult->UpdateCursor();

	return ret;
}

FormResultType_t CGIResolver::cgiFormStringNoNewlines(char *name, char *result, int max) 
{
	cgiFormEntry* e;

	if ((e = cgiFormEntryFindFirst(name)) == NULL) 
  {
		strcpy(result, "");
		return cgiFormNotFound;
	}

	return cgiFormEntryString(e, result, max, 0);
}

FormResultType_t CGIResolver::cgiFormStringMultiple(char* name, char*** result)
{
	char**        stringArray;
	cgiFormEntry* e;
	int i;
	int total = 0;

	/* Make two passes. One would be more efficient, but this
		function is not commonly used. The select menu and
		radio box functions are faster. */
	e = cgiFormEntryFindFirst(name);
	if (e != NULL) 
  {
		do 
    {
			total++;
		} 
    while ((e = cgiFormEntryFindNext()) != 0); 
	}

	stringArray = (char**)malloc(sizeof(char*) * (total + 1));

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

FormResultType_t CGIResolver::cgiFormStringSpaceNeeded(char *name, int *result) 
{
	cgiFormEntry* e;

	if ((e = cgiFormEntryFindFirst(name)) == NULL) 
  {
		*result = 1;
		return cgiFormNotFound; 
	}
	
  *result = strlen(e->value) + 1;
	return cgiFormSuccess;
}

FormResultType_t CGIResolver::cgiFormEntryString(cgiFormEntry *e, char *result, int max, int newlines) 
{
	char  *dp, *sp;
	int   truncated = 0;
	int   len = 0;
	int   avail = max - 1;
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


FormResultType_t CGIResolver::cgiFormInteger(char *name, int *result, int defaultV) 
{
	int           ch;
	cgiFormEntry* e;

	if ((e = cgiFormEntryFindFirst(name)) == NULL) 
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

FormResultType_t CGIResolver::cgiFormIntegerBounded(char *name, int *result, int min, int max, int defaultV) 
{
	FormResultType_t error = cgiFormInteger(name, result, defaultV);
	
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

FormResultType_t CGIResolver::cgiFormDouble(char *name, double *result, double defaultV) 
{
	int ch;
	cgiFormEntry* e;

	if ((e = cgiFormEntryFindFirst(name)) == NULL) 
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

FormResultType_t CGIResolver::cgiFormDoubleBounded(char *name, double *result, double min, double max, double defaultV) 
{
	FormResultType_t error = cgiFormDouble(name, result, defaultV);
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

FormResultType_t CGIResolver::cgiFormSelectSingle(char *name, char **choicesText, int choicesTotal, int *result, int defaultV)
{
	int i;
	cgiFormEntry* e;

	if ((e = cgiFormEntryFindFirst(name)) == NULL) 
  {
		*result = defaultV;
		return cgiFormNotFound;
	}
	
  for (i=0; (i < choicesTotal); i++) 
  {
    if (!strcmp(choicesText[i], e->value)) 
    {
			*result = i;
			return cgiFormSuccess;
		}
	}

	*result = defaultV;

	return cgiFormNoSuchChoice;
}

FormResultType_t CGIResolver::cgiFormSelectMultiple(char *name, char **choicesText, int choicesTotal, int *result, int *invalid)
{
	int i;
	int hits = 0;
	int invalidE = 0;

	for (i=0; (i < choicesTotal); i++) 
		result[i] = 0;
	
	cgiFormEntry* e;

	if ((e = cgiFormEntryFindFirst(name)) == NULL) 
  {
		*invalid = invalidE;
		return cgiFormNotFound;
	}
	
  do 
  {
		int hit = 0;
		for (i=0; (i < choicesTotal); i++) 
    {
			if (!strcmp(choicesText[i], e->value)) 
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

FormResultType_t CGIResolver::cgiFormCheckboxSingle(char *name)
{
	cgiFormEntry* e;

	if ((e = cgiFormEntryFindFirst(name)) == NULL) 
		return cgiFormNotFound;

	return cgiFormSuccess;
}

FormResultType_t CGIResolver::cgiFormCheckboxMultiple(char *name, char **valuesText, int valuesTotal, int *result, int *invalid)
{
	/* Implementation is identical to cgiFormSelectMultiple. */
	return cgiFormSelectMultiple(name, valuesText, valuesTotal, result, invalid);
}

FormResultType_t CGIResolver::cgiFormRadio(char *name, char **valuesText, int valuesTotal, int *result, int defaultV)
{
	/* Implementation is identical to cgiFormSelectSingle. */
	return cgiFormSelectSingle(name, valuesText, valuesTotal, result, defaultV);
}

void CGIResolver::cgiHeaderLocation(char *redirectUrl) 
{	
  fprintf(cgiOut, "Location: %s%c%c", redirectUrl, 10, 10); 
}

void CGIResolver::cgiHeaderStatus(int status, char *statusMessage) 
{
	fprintf(cgiOut, "Status: %d %s%c%c", status, statusMessage, 10, 10);
}

int CGIResolver::cgiWriteString(FILE *out, char *s) 
{
	unsigned int len = strlen(s);
	cgiWriteInt(out, len);
	if (fwrite(s, 1, len, out) != len) 
		return 0;

	return 1;
}

int CGIResolver::cgiWriteInt(FILE *out, int i) 
{
	if (!fwrite(&i, sizeof(int), 1, out)) 
		return 0;

	return 1;
}

cgiFormEntry* CGIResolver::cgiFormEntryFindFirst(char *name) 
{
	cgiFindTarget = name;
	cgiFindPos = cgiFormEntryFirst;

	return cgiFormEntryFindNext();
}

cgiFormEntry* CGIResolver::cgiFormEntryFindNext() 
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

int CGIResolver::cgiFirstNonspaceChar(char *s) 
{
	int len = strspn(s, " \n\r\t");
	return s[len];
}

void CGIResolver::cgiStringArrayFree(char **stringArray) 
{
	char *p;
	p = *stringArray;

	while (p) 
  {
		free(p);
		stringArray++;
		p = *stringArray;
	}
}	

void CGIResolver::ManageRequestMethod()
{
  itxString istr;

	if (istr.CompareNoCase(cgiRequestMethod, "post")) 
  {
    //Read from stdin
    if (!ReadPostContent())
      return;

		if (istr.CompareNoCase(cgiContentType, "application/x-www-form-urlencoded")) 
    {	
			if (cgiParseFormInput(m_POSTbody.GetBuffer(), cgiContentLength) != cgiParseSuccess) 
				return;
		}
    else if (istr.CompareNoCase(cgiContentType, "multipart/form-data") != 0) 
    {
//		  if (itxCGIParseMultipartInput() != cgiParseSuccess) 
//			  return;
	  }
  } 
  else if (istr.CompareNoCase(cgiRequestMethod, "get")) 
  {	
		/* The spec says this should be taken care of by the server, but... it isn't */
		cgiContentLength = strlen(cgiQueryString);
		if (cgiParseGetFormInput() != cgiParseSuccess) 
			return;
	}
}

void CGIResolver::OnStart()
{
}

void CGIResolver::Flush(char* outbuf, int outbufdim)
{
  fwrite(outbuf, 1, outbufdim, cgiOut);
  fflush(cgiOut);  
}

void CGIResolver::Flush(char* outbuf)
{
  fwrite(outbuf, 1, strlen(outbuf), cgiOut);
//  fprintf(cgiOut, "%s", outbuf);
  fflush(cgiOut);  
}

bool CGIResolver::FlushOnFile(char* path, char* fname, char* outbuf, char* ext, char mode)
{
  FILE* fp;
  itxString outputfile(path);
  itxString wmode;
  itxString preproc_enableext;

  m_PRMFile.GetPRMValue("enableext", &preproc_enableext);

  if (preproc_enableext.IsEmpty() || preproc_enableext.IsNull())
  {
    //Build up the complete output file path pathname
    if (outputfile.RightInstr(PATH_SEPARATOR) != outputfile.Len())
      outputfile += PATH_SEPARATOR;
    outputfile += fname;
    outputfile += ext;
  }
  else
    outputfile += fname;

  wmode = 'w';
  wmode += mode; 
  if ((fp = fopen(outputfile.GetBuffer(), wmode.GetBuffer())) == NULL)
  {
    DebugTrace2(IN_ERROR, "Unable to open %s file to flush output.", outputfile.GetBuffer());
    return false;
  }

  fprintf(fp, "%s", outbuf);
  fflush(fp);  
  fclose(fp);
  
  return true;
}

/*****************************************************************************
 ------------------------------  PRMFile METHODS  ----------------------------
 *****************************************************************************/
PRMFile::PRMFile()
{
  m_LanId.SetEmpty();
  m_FileName.SetEmpty();
  m_NumParams = 0;
}

PRMFile::PRMFile(char* prmfile)
{
  m_FileName = prmfile;
  m_NumParams = 0;
  m_FileDir = "";
  m_FilePath = "";
}

bool PRMFile::ReadPRM()
{  
	FILE*   initFile;
	char    fileLine[INIT_FILE_LINE_LEN];
	char*   token = NULL;
  char*   retVal = NULL;

  if (m_FileName.IsEmpty())
    return false;

	if((initFile = fopen(m_FileName.GetBuffer(), "r")) == NULL)
  {
    m_FilePath.SetEmpty();
    m_FilePath += m_FileDir;
    m_FilePath += m_FileName;
    if((initFile = fopen(m_FilePath.GetBuffer(), "r")) == NULL)
      return false;
  }

  if (fseek(initFile, 0, SEEK_SET))
    return false;

	// scansione delle linee del file
	while (fgets(fileLine, INIT_FILE_LINE_LEN, initFile) != 0)
	{
		// se il primo carattere e' '#' oppure non c'e` un uguale,
    // la linea non va letta: e' un commento o una linea (tipicamente) vuota
		if (fileLine[0] == '#' || strstr(fileLine, "=") == NULL)
			continue;

		// il segno di uguale determina la fine del token candidato a id del parametro:
		// si confronta il token con il parametro da cercare
		token = strtok(fileLine, "=");
    if (token != NULL)
    {
      m_PrmNames[m_NumParams] = token;
      m_PrmNames[m_NumParams].Trim();

			token = strtok(NULL, "\n");
      if (token != NULL)
      {
        m_PrmValues[m_NumParams] = token;
        m_PrmValues[m_NumParams].Trim();
      }
      else
        m_PrmValues[m_NumParams].SetEmpty();

      m_NumParams++;
    }
	}

  fclose(initFile);

  return true;
}

bool PRMFile::MergePRM(itxString* pathname)
{
	FILE*     initFile;
	char      fileLine[INIT_FILE_LINE_LEN];
	char*     token = NULL;
  char*     retVal = NULL;
  int       idx;
  itxString newToken;

  if (pathname->IsEmpty())
    return false;

	if((initFile = fopen(pathname->GetBuffer(), "r")) == NULL) 
  {
    m_FilePath.SetEmpty();
    m_FilePath += m_FileDir;
    m_FilePath += *pathname;
    if((initFile = fopen(m_FilePath.GetBuffer(), "r")) == NULL)
      return false;
  }

  if (fseek(initFile, 0, SEEK_SET))
    return false;

	// scansione delle linee del file
	while (fgets(fileLine, INIT_FILE_LINE_LEN, initFile) != 0 && m_NumParams < MAX_PRM_PARS)
	{
		// se il primo carattere e' '#' oppure non c'e` un uguale,
    // la linea non va letta: e' un commento o una linea (tipicamente) vuota
		if (fileLine[0] == '#' || strstr(fileLine, "=") == NULL)
			continue;

		// il segno di uguale determina la fine del token candidato a id del parametro:
		// si confronta il token con il parametro da cercare
		token = strtok(fileLine, "=");
    if (token != NULL)
    {
      newToken = token;
      newToken.Trim();
      if ((idx = GetIndexByName(newToken.GetBuffer())) == -1)
      {
        idx = m_NumParams;
        m_NumParams++;
      }

      m_PrmNames[idx] = newToken;

			token = strtok(NULL, "\n");
      if (token != NULL)
      {
        m_PrmValues[idx] = token;
        m_PrmValues[idx].Trim();
      }
      else
        m_PrmValues[idx].SetEmpty();
    }
	}

  fclose(initFile);

  return true;
}

bool PRMFile::Clone(PRMFile* destobj)
{
  if (destobj == NULL)
    return false;

  int i = 0;

  try
  {
    for (i=0; i<MAX_PRM_PARS; i++)
    {
      destobj->m_PrmNames[i] = m_PrmNames[i];
      destobj->m_PrmValues[i] = m_PrmValues[i];
    }

    destobj->m_LanId = m_LanId;
    destobj->m_FileName = m_FileName;
    destobj->m_NumParams = m_NumParams;
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "PRMFile::Clone: unhandled exception.");
    return false;
  }

  return true;
}

void PRMFile::SetLanguageId(char* lid)
{
  m_LanId = lid;  
}

void PRMFile::SetPRMFileName(char* fname)
{
  m_FileName = fname;
}

void PRMFile::GetLanguageId(itxString* plid)
{
  *plid = m_LanId;
}

void PRMFile::GetPRMFileName(itxString* pfname)
{
  *pfname = m_FileName;
}

bool PRMFile::GetPRMValue(itxString* param_name, itxString* param_value)
{
  int    i = 0;
  char*  pname = param_name->GetBuffer();
  
  if (pname == NULL)  
    return false;

  while (i < m_NumParams && m_PrmNames[i].Compare(pname))
    i++;

  if (i < m_NumParams)
  {
    *param_value = m_PrmValues[i];
    return true;
  }

  return false;
}

bool PRMFile::GetPRMValue(int index, itxString* param_value)
{
  if (index < m_NumParams)
  {
    *param_value = m_PrmValues[index];
    return true;
  }

  return false;
}

bool PRMFile::GetPRMValue(char* param_name, itxString* param_value)
{
  int    i = 0;

  while (i < m_NumParams && m_PrmNames[i].Compare(param_name))
    i++; 

  if (i < m_NumParams)
  {
    *param_value = m_PrmValues[i];
    return true;
  }

  return false;
}
 
bool PRMFile::GetPRMValue(char* param_name, char* param_value, int* bufdim)
{
  if (param_name == NULL || bufdim == NULL)
    return false;

  int i = 0;
  while (i < m_NumParams && m_PrmNames[i].Compare(param_name))
    i++;

  if (i < m_NumParams)
  {
    if (param_value == NULL)
      *bufdim = m_PrmValues[i].Len() + 1;
    else
      strcpy(param_value, m_PrmValues[i].GetBuffer());

    return true;
  }

  return false;
}

bool PRMFile::GetPRMName(int index, itxString* param_value)
{
  if (index < m_NumParams)
  {
    *param_value = m_PrmNames[index];
    return true;
  }

  return false;
}

int PRMFile::GetIndexByName(char* param_name)
{
  int i = 0;
  while (i < m_NumParams && m_PrmNames[i].Compare(param_name))
    i++;

  return (i == m_NumParams ? -1 : i);
}

void PRMFile::ClearNamesAndValues()
{
  int i;

  for (i=0; i<MAX_PRM_PARS; i++)
    m_PrmNames[i].SetEmpty();
  
  for (i=0; i<MAX_PRM_PARS; i++)
    m_PrmValues[i].SetEmpty();
}

void PRMFile::SetAuxiliaryDirectory(char* dir_path)
{
  m_FileDir = dir_path;
  m_FileDir +="\\";
}

itxString PRMFile::GetAuxiliaryDirectory()
{
  return m_FileDir;
}

