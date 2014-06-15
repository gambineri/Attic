/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/*
 | _-.-..-._-_-._--_-._.-._     AITECSA S.r.l.   2000
 |
 |
 | FILENAME    : commands.cpp
 | TAB         : 2 spaces
 |
 | DESCRIPTION : main implementation file for all base command objects.
 |
 |
 */

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "desx.h"

#include "itxtypes.h"
#include "commands.h"
#include "templatefile.h"


#define CRYPT_KEY                   "itxsecur";
#define CRYPT_WHT                   "byFabioM";
#define ITXCOOVERS_EQ               "="
#define ITXCOOVERS_SEP              ";"
#define ITX_DES_BLOK                8







/*----------------------------------------------------------------------------
                              BC_StartConditionalBlock
  ----------------------------------------------------------------------------*/

/*
attivita'	:inizio del blocco condizionato; necessita di un comando *endif()/*endblk()
					 per la terminazione del blocco:
					 la condizione utilizza gli operatori
					 ==	uguale
					 != diverso
					 <	minore
					 >	maggiore
					 <=	minore-uguale
					 =>	uguale-maggiore
					 per il confronto tra i due termini.
					 Nel caso entrambi i termini del confronto siano convertibili a numeri
					 interi il confronto avviene tra interi. altrimenti tra stringhe.
					 Se la condizione � una stringa nulla l'if non ha successo.
					 Se la condizione non contiene nessun operatore e la stringa non � nulla
					 l'if ha successo.
					 Se il confronto ha successo il blocco viene elaborato dal parser ed i
					 comandi al suo interno non vengono eseguiti.
*/

char* BC_StartConditionalBlock::Execute(char* inputStr)
{
	char*       operators[] = {"==","!=","<","<=",">=",">"};
	char*       opPosition;
	itxString   leftOp;
  itxString   rightOp;
	int         opOrd = -1;
  int         result = 0;
  int         numops = sizeof(operators)/sizeof(char*);
  int         lopint = 0;
  int         ropint = 0;
  bool        numeric_conv_good = false;

	for (opOrd=0; opOrd<numops; opOrd++)
		if ((opPosition = strstr(inputStr, operators[opOrd])) != NULL)
			break;

	if (opOrd == numops)
	{
		// se non vi � operatore il blocco si considera TRUE se vi e' una qualsiasi stringa
    m_pParser->SetValidBlock(((strlen(inputStr) > 0) ? EXEC_BLK : DONT_EXEC_BLK));
		return NULL;
	}

  //extracts operands
  leftOp.Strncpy(inputStr, (unsigned)opPosition - (unsigned)inputStr);
  rightOp = (char*)((unsigned)opPosition + strlen(operators[opOrd]));
	leftOp.Trim();
  rightOp.Trim();

	// nel caso di operatori convertibili ad interi vengono confrontati i valori interi
	if ((lopint = atoi(leftOp.GetBuffer())) && (ropint = atoi(rightOp.GetBuffer())))
  {
    itxString appo;

    appo.SetInt(lopint);
    if (appo.Compare(&leftOp) == 0)
    {
      appo.SetInt(ropint);
      if (appo.Compare(&rightOp) == 0)
      {
        result = lopint - ropint;
        numeric_conv_good = true;
      }
    }
  }

  if (!numeric_conv_good)
    result = leftOp.Compare(&rightOp);

	switch (opOrd) 
  {
	case 0:
    m_pParser->SetValidBlock(result != 0 ? DONT_EXEC_BLK : EXEC_BLK);
		break;
	case 1:
    m_pParser->SetValidBlock(result != 0 ? EXEC_BLK : DONT_EXEC_BLK);
		break;
	case 2:
    m_pParser->SetValidBlock((result<0) ? EXEC_BLK : DONT_EXEC_BLK);
		break;
	case 3:
    m_pParser->SetValidBlock((result<=0) ? EXEC_BLK : DONT_EXEC_BLK);
		break;
	case 4:
    m_pParser->SetValidBlock((result>=0) ? EXEC_BLK : DONT_EXEC_BLK);
		break;
	case 5:
    m_pParser->SetValidBlock((result>0) ? EXEC_BLK : DONT_EXEC_BLK);
		break;
	}

  return NULL;
}


/*----------------------------------------------------------------------------
                              BC_GetValue
  ----------------------------------------------------------------------------*/

/*
attivita'	:restituisce il valore del parametro di get indicato
					 - parametro: nome del parametro di get di cui restituire il valore
					 - OPZdefault: valore di default ritornato nel caso non si trovi il
												 parametro.
						 N.B. se OPZdefault � il carattere '+' il valore
						 ricavato per il parametro viene modificato sostituendo tutti
						 i caratteri non alfanumerici con la stringa %xx dove xx �
						 il valore ascii esadecimale del carattere sostituito.
*/

char* BC_GetValue::Execute(char* istr)
{
  itxString defParVal;
  itxString getParName;
	int		    useDefault = 0;

  m_Output.SetEmpty();

  if (m_pParser->PickPar(istr, 1, &getParName) == PARAM_NOT_FOUND)
  {
    m_Output = PARAM_NOT_FOUND_MSG;
    return m_Output.GetBuffer();
  }

  if (m_pParser->PickPar(istr, 2, &defParVal) == PARAM_FOUND)
    useDefault = 1;

	if (m_pCGIRes->cgiFormString(getParName.GetBuffer(), &m_Output, 0) != cgiFormSuccess)
	{
		if (useDefault) // valore opzionale di default definito dall'utente
			m_Output = defParVal;
		else  // valore di default generico
			m_Output = GET_PAR_VAL;
	}

	if (!defParVal.IsNull() && strcmp(defParVal.GetBuffer(), "+") == 0)
	{
		if(!m_Output.IsNull() && strcmp(m_Output.GetBuffer(), "+") == 0)
			m_Output = "";

		m_Output.EscapeChars();
	}

  return m_Output.GetBuffer();
}

/*----------------------------------------------------------------------------
                              BC_GetHide
  ----------------------------------------------------------------------------*/
/*
attivita'	:nasconde i caratteri non alfanumerici del parametro trasformandoli in %xx
					 dove xx � il valore ascii esadecimale del carattere sostituito.
*/
char* BC_GetHide::Execute(char* istr)
{
  m_Output.SetEmpty();

  if (m_pParser->PickPar(istr, 1, &m_Output) == PARAM_NOT_FOUND)
    return NULL;

	m_Output.EscapeChars();

	return m_Output.GetBuffer();
}

/*----------------------------------------------------------------------------
                              BC_ProcessExternFile
  ----------------------------------------------------------------------------*/

/*
attivita'	:prosegue il parsing nel file specificato. Una volta concluso quest'ultimo
					 prosegue per il file di partenza.
*/
char* BC_ProcessExternFile::Execute(char* istr)
{
  itxString tplName;
  itxString tpldir;

  m_Output.SetEmpty();

  if (m_pParser->PickPar(istr, 1, &tplName) == PARAM_NOT_FOUND)
  {
    m_Output = PARAM_NOT_FOUND_MSG;
    return m_Output.GetBuffer();
  }

  //printf("%s<br>", tplName);
  if(m_pParser->m_ActualTplDir.IsEmpty())
    m_pCGIRes->m_PRMFile.GetPRMValue(TPLDIR, &tpldir);
  else
    tpldir = m_pParser->m_ActualTplDir;
  
  m_pParser->Run(&tpldir, &tplName);

  return m_Output.GetBuffer();
}


/*----------------------------------------------------------------------------
                              BC_SetVar
  ----------------------------------------------------------------------------*/

/*
attivita'	: assegna alla variabile varName il valore varValue.
*/

char* BC_SetVar::Execute(char* istr)
{
	itxString varName;
  itxString varValue;

  if (m_pParser->PickPar(istr, 1, &varName) == PARAM_NOT_FOUND ||
      m_pParser->PickPar(istr, 2, &varValue) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

  if (varName.IsNull() || varName.IsEmpty())
  {
    DebugTrace2(IN_COMMAND | IN_WARNING, "Command setvar invoked with empty variable name.\n");
		return NULL;
  }

  if (varValue.IsNull() || varValue.IsEmpty())
    DebugTrace2(IN_COMMAND, "Command setvar invoked with empty variable value.\n");

  int i;
	for (i=0; i<m_pParser->m_TplVars.idx; i++)
	{
		if (!m_pParser->m_TplVars.names[i].IsNull())
		{
			if (strcmp(m_pParser->m_TplVars.names[i].GetBuffer(), varName.GetBuffer()) == 0)
				break;
		}
	}

	if (i == m_pParser->m_TplVars.idx)
	{
		if (i < TPL_VARS_NUM)
		{
			m_pParser->m_TplVars.names[i]  = varName;
			m_pParser->m_TplVars.values[i] = varValue;
			m_pParser->m_TplVars.idx++;
		}
		else
			return NULL;
	}
	else
		m_pParser->m_TplVars.values[i] = varValue ;

	return NULL;
}

/*----------------------------------------------------------------------------
                              BC_GetVar
  ----------------------------------------------------------------------------*/

/*
attivita'	:restituisce il valore della variabile varName.
*/

char* BC_GetVar::Execute(char* istr)
{
	itxString varName;

  m_Output.SetEmpty();

  if (m_pParser->PickPar(istr, 1, &varName) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

  m_pParser->PickPar(istr, 2, &m_Output);

  if (varName.IsNull() || varName.IsEmpty())
  {
    DebugTrace2(IN_COMMAND | IN_WARNING, "Command getvar invoked with empty variable name.\n");
		return NULL;
  }

	for (int i=0; i < m_pParser->m_TplVars.idx; i++)
	{
		if (
        !m_pParser->m_TplVars.names[i].IsNull() &&
        m_pParser->m_TplVars.names[i].Compare(&varName) == 0
       )
    {
  		if (!m_pParser->m_TplVars.values[i].IsEmpty())
        m_Output = m_pParser->m_TplVars.values[i];
      break;
    }
	}

	return m_Output.GetBuffer();
}

/*----------------------------------------------------------------------------
                              BC_GetCookie
  ----------------------------------------------------------------------------*/

/*
attivita'	:	torna il cookie specificato
*/

char* BC_GetCookie::Execute(char* istr)
{
	itxString cooName;
  itxString cooAttr;
  char*     pcoo = NULL;
	char*     pcomma = NULL;

  m_Output.SetEmpty();

  if (m_pParser->PickPar(istr, 1, &cooName) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

  DebugTrace2(IN_COMMAND, "COMMAND getcoo\n");
  DebugTrace2(IN_COMMAND, "  cgiCookie : %s\n", m_pCGIRes->cgiCookie);

  if ((pcoo = strstr(m_pCGIRes->cgiCookie, cooName.GetBuffer())) != NULL)
	{
		if ((pcoo = strstr(pcoo, ITXCOOVERS_EQ)) != NULL)
    {    
		  pcoo++;
      if ((pcomma = strstr(pcoo, ITXCOOVERS_SEP)) != NULL)
      {
        m_Output.Strncpy(pcoo, (size_t) (pcomma - pcoo));
        pcoo = pcomma;
      }
      else
        m_Output.Strncpy(pcoo, strlen(pcoo));
    }
	}

  if (m_pParser->PickPar(istr, 2, &cooAttr) != PARAM_NOT_FOUND)
  { // Cookie Name found, let see if is enough
    m_Output = "";
    if ((pcoo = strstr(pcoo, cooAttr.GetBuffer())) != NULL)
	  {
		  if ((pcoo = strstr(pcoo, ITXCOOVERS_EQ)) != NULL)
      {    
		    pcoo++;
        if ((pcomma = strstr(pcoo, ITXCOOVERS_SEP)) != NULL)
          m_Output.Strncpy(pcoo, (size_t)(pcomma - pcoo));
        else
          m_Output.Strncpy(pcoo, strlen(pcoo));
      }
	  }
  }

  return m_Output.GetBuffer();
}

/*----------------------------------------------------------------------------
                              BC_UTime
  ----------------------------------------------------------------------------*/

/*
attivit�	:	trasforma data, mese e anno in universal time (secondi passati dalla
						mezzanotte del 1.1.1970)
*/
char* BC_UTime::Execute(char* istr)
{ 
	itxString par;  //Collects year, month, day, hour, min, sec in this order
	struct tm datar;
	time_t    datas;
	char      ret[100];

  m_Output.SetEmpty();

  datar.tm_hour  = 0;
  datar.tm_isdst = -1;
	datar.tm_mday  = 0;
  datar.tm_min   = 0;
	datar.tm_mon	 = 0;
  datar.tm_sec   = 0;
  datar.tm_wday  = 0;
  datar.tm_yday  = 0;
  datar.tm_year  = 0;

  par.SetEmpty();

  if (m_pParser->PickPar(istr, 1, &par) && !par.IsNull() && !par.IsEmpty())
	  datar.tm_year  = atoi(par.GetBuffer()) - 1900;

  if (m_pParser->PickPar(istr, 2, &par) && !par.IsNull() && !par.IsEmpty())
	  datar.tm_mon   = atoi(par.GetBuffer()) - 1;

  if (m_pParser->PickPar(istr, 3, &par) && !par.IsNull() && !par.IsEmpty())
	  datar.tm_mday  = atoi(par.GetBuffer());

  if (m_pParser->PickPar(istr, 4, &par) && !par.IsNull() && !par.IsEmpty())
	  datar.tm_hour  = atoi(par.GetBuffer());

  if (m_pParser->PickPar(istr, 5, &par) && !par.IsNull() && !par.IsEmpty())
	  datar.tm_min   = atoi(par.GetBuffer());

  if (m_pParser->PickPar(istr, 6, &par) && !par.IsNull() && !par.IsEmpty())
	  datar.tm_sec   = atoi(par.GetBuffer());

	datas = mktime(&datar);

	sprintf(ret, "%ld", datas);
  m_Output = ret;

	return m_Output.GetBuffer();
}

/*----------------------------------------------------------------------------
                              BC_DMYTime
  ----------------------------------------------------------------------------*/

/*
attivita'	: trasforma una stringa che rappresenta una universal time (secondi
						passati dalla mezzanotte del 1.1.1970) in formato normale oppure, se
						presente un secondo parametro di valore d,m,y, torna il giorno, o il mese, 
						o l'anno.
*/

char* BC_DMYTime::Execute(char* istr)
{
	char       ret[100];
  itxString  utime;
  itxString  tag;
	struct tm* dt; 
	time_t     timet;

  m_Output.SetEmpty();
	memset(ret, '\0', 100);

  if (m_pParser->PickPar(istr, 1, &utime) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

  timet = (time_t)atol(utime.GetBuffer());
	dt    = localtime(&timet);
  //dt->tm_mon--;

  if (dt == NULL)
    return "";
      
  if (m_pParser->PickPar(istr, 2, &tag) == PARAM_FOUND)
  {
		if (tag.Compare("d") == 0)
			strftime(ret, 100,"%d", dt); 
		else if (tag.Compare("m") == 0)
			strftime(ret, 100,"%m", dt); 
		else if (tag.Compare("y") == 0)
			strftime(ret, 100,"%Y", dt); 
  }
  else
    strftime(ret, 100, "%Y-%m-%d %H:%M:%S", dt);

  m_Output = ret;

  return m_Output.GetBuffer();
}



/*----------------------------------------------------------------------------
                              BC_Now
  ----------------------------------------------------------------------------*/

/*
attivita'	: restituisce il numero di secondi passati dalla mezzanote del 1.1.1970
						(universal time); se � presente il parametro opzionale restituisce, a
						seconda del valore di quest'ultimo,
						d - il giorno corrente
						m - il mese corrente
						y - l'anno corrente
*/

char* BC_Now::Execute(char* istr)
{
	time_t      tm;
	struct tm*  today; 
	itxString   part;

  m_Output.SetEmpty();
	time(&tm);
	today = localtime(&tm);

  m_Output.Space(15);
	sprintf(m_Output.GetBuffer(), "%ld", tm);

	if(m_pParser->PickPar(istr, 1, &part))
	{
		if (part.Compare("d") == 0)
			strftime(m_Output.GetBuffer(), 15,"%d", today); 
		else if (part.Compare("m") == 0)
			strftime(m_Output.GetBuffer(), 15,"%m", today); 
		else if (part.Compare("y") == 0)
			strftime(m_Output.GetBuffer(), 15,"%Y", today);
	}

  m_Output.Trim();

  return m_Output.GetBuffer();
}

/*----------------------------------------------------------------------------
                              BC_FormatCurrency
  ----------------------------------------------------------------------------*/

/*
attivita'	: ritorna la valuta source formattata con le interpunzioni tra le migliaia
						ed il numero di decimali specificato
*/

char* BC_FormatCurrency::Execute(char* istr)
{
	itxString source;
  itxString decimaliSt;
	int		    decimali = 0;

  m_Output.SetEmpty();

	if(m_pParser->PickPar(istr, 1, &source) == PARAM_NOT_FOUND)
    return "";

	if(m_pParser->PickPar(istr, 2, &decimaliSt)	== PARAM_FOUND)
    decimali = atoi(decimaliSt.GetBuffer());

	if(decimali < 0)
		return "";

  char* ret = Format(source.GetBuffer(), decimali, 1);
  if (ret)
  {
    m_Output = ret;
    free(ret);
  }

  return m_Output.GetBuffer();
}

//---------------------------------------------------------------------

char* BC_FormatCurrency::Format(char* num, int mant_len, int want_sep)
{
  const char* sep = ".";
  char*       ret = NULL;
  int         numlen = strlen(num);
  int         first_pt = 0;
  
  if (num == NULL || numlen == 0)
    return NULL;

  if ((ret = (char*)calloc(3 * ((numlen >= mant_len ? numlen : mant_len) + 1), 1)) == NULL)
    return NULL;

  if (strstr(num, ",") != NULL)
  {
    strcpy(ret, num);
    return ret;
  }
  
  if (numlen > mant_len)  //Il numero e` > 1 ?
  {
    if (numlen - mant_len > 3 && want_sep)  //Gestione separatore 3 cifre
    {
      first_pt = (numlen - mant_len) % 3;
      if (first_pt > 0)
      {
        memcpy(ret, num, first_pt);
        strcat(ret, sep);
      }

      int i;
      for (i = first_pt; i<numlen - mant_len; i+=3)
      {
        memcpy(ret + strlen(ret), num + i, 3);
        strcat(ret, sep);
      }
      if ((i = strlen(ret)) > 0)
        ret[i - 1] = 0;
      
      //Se c'e` parte decimale, metto la virgola e l'attacco
      if (mant_len > 0)
      {
        strcat(ret, ",");
        strcat(ret, num + numlen - mant_len);
      }
    }
    else
    {
      memcpy(ret, num, numlen - mant_len);
      strcat(ret, ",");
      strcat(ret, num + numlen - mant_len);
    }
  }
  else
  {
    strcat(ret, "0,");
    memset(ret + 2, '0', mant_len - numlen);
    strcat(ret, num);
    
    //remove nonsense final zeros
    char* p = &ret[strlen(ret) - 1];
    while (*p == '0')
      p--;
    *(++p) = 0;
  }

  if (ret[strlen(ret) - 1] == ',')
    ret[strlen(ret) - 1] = 0;

  return ret;
}

/*----------------------------------------------------------------------------
                              BC_CheckForbiddenChars
  ----------------------------------------------------------------------------*/

/*
attivita'	:cerca i caratteri contenuti nella stringa ForbiddenChars nella teststring
return		:TANNIT_TRUE se trova i caratteri; TANNIT_FALSE se non trova i caratteri;
*/

char* BC_CheckForbiddenChars::Execute(char* istr)
{
  itxString str2check;
  itxString ForbiddenChars;

	if(m_pParser->PickPar(istr, 1, &str2check) == PARAM_NOT_FOUND ||
     m_pParser->PickPar(istr, 2, &ForbiddenChars) == PARAM_NOT_FOUND)
    return TANNIT_FALSE;

  return (char*)(strpbrk(str2check.GetBuffer(), ForbiddenChars.GetBuffer()) == NULL ? TANNIT_FALSE : TANNIT_TRUE);
}

/*----------------------------------------------------------------------------
                              BC_TraceUser
  ----------------------------------------------------------------------------*/

/*
attivita'	:restituisce informazioni sul client a seconda del parametro feature.
					 feature=brname->nome del browser
					 feature=brver ->versione del browser (solo parte intera)
					 feature=osname->nome del sistema operativo
					 feature=ipnum ->numero ip
					 feature=ipname->nome, quando risolvibile, della macchina remota
*/

char* BC_TraceUser::Execute(char* istr)
{
	itxString feature;
	itxString brname;
  itxString brver;
  itxString osname;
	char*     cursor;

  m_Output.SetEmpty();

	if(m_pParser->PickPar(istr, 1, &feature) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

	if ((cursor = strstr(m_pCGIRes->cgiUserAgent, "Mozilla/")) != NULL)
	{
		if (cursor = strstr(m_pCGIRes->cgiUserAgent, "MSIE"))
		{
			brname = BROWSER_MSIE;
			brver = &cursor[5];
			if(strstr(m_pCGIRes->cgiUserAgent, "Windows 98"))
				osname = "MS Windows 98";
			else if(strstr(m_pCGIRes->cgiUserAgent, "Windows 95"))
				osname = "MS Windows 95";
			else if(strstr(m_pCGIRes->cgiUserAgent, "Windows NT"))
				osname = "MS Windows NT";
			else if(strstr(m_pCGIRes->cgiUserAgent, "Windows 3.1"))
				osname = "MS Windows 3.1";
			else if(strstr(m_pCGIRes->cgiUserAgent, "Mac_"))
				osname = "Apple Macintosh";
		}
		else if (cursor = strstr(m_pCGIRes->cgiUserAgent, "Opera"))
			brname = BROWSER_OPERA;
		else if (cursor = strstr(m_pCGIRes->cgiUserAgent, "Gecko"))
			brname = "Gecko";
		else
		{
			brname = BROWSER_NETSCAPE;
			brver = &m_pCGIRes->cgiUserAgent[8];
			if(strstr(m_pCGIRes->cgiUserAgent, "Win98"))
				osname = "MS Windows 98";
			else if(strstr(m_pCGIRes->cgiUserAgent, "Win95"))
				osname = "MS Windows 95";
			else if(strstr(m_pCGIRes->cgiUserAgent, "WinNT"))
				osname = "MS Windows NT";
			else if(strstr(m_pCGIRes->cgiUserAgent, "Win16"))
				osname = "MS Windows 3.1";
			else if(strstr(m_pCGIRes->cgiUserAgent, "Macintosh"))
				osname = "Macintosh";
			else if(strstr(m_pCGIRes->cgiUserAgent, "Linux"))
				osname = "Linux";
			else if(strstr(m_pCGIRes->cgiUserAgent, "SunOS"))
				osname = "SunOS";
		}
	}

	if (feature.Compare("brname") == 0)
		m_Output = brname;
	else if (feature.Compare("brver") == 0)
  {
		m_Output = brver;
    int semicol = brver.FindSubString(";");
    if (semicol >= 0)
      m_Output.Strncpy(brver.GetBuffer(), semicol);
    else
      m_Output = brver;
  }
	else if (feature.Compare("osname") == 0)
		m_Output = osname;
	else if (feature.Compare("ipnum") == 0)
		m_Output = m_pCGIRes->cgiRemoteAddr;
	else if (feature.Compare("ipname") == 0)
		m_Output = m_pCGIRes->cgiRemoteHost;
	else if (feature.Compare("referrer") == 0)
		m_Output = m_pCGIRes->cgiReferrer;

  return m_Output.GetBuffer();
}

/*----------------------------------------------------------------------------
                              BC_ODBCExecuteQuery
  ----------------------------------------------------------------------------*/

/*
attivita'	:esegue una chiamata sql all'odbc di default specificato nel file di
					 inizializazione; gli eventuali risultati vengono messi in una struttura
					 rintracciabile per il nome della query
par 1			:nome della query (costituira' l'etichetta di identificazione dei risultati)
par 2			:query sql compresa tra doppi apici
par 3	opz	:primo record del resultset da memorizzare nella struttura (opzionale,
					 valore di default = STARTING_ROW, tipicamente 1)
par 4	opz :numero di record del resultset da memorizzare nella struttura (opzionale,
					 valore di default = a ROWS_TO_STORE, tipicamente 512)
*/

char* BC_ODBCExecuteQuery::Execute(char* istr)
{
	itxString queryName;
  itxString queryString;
  itxString firstRecord;
  itxString recsToStore;

	int firstRecInt = STARTING_ROW;
  int recsToStoreInt = ROWS_TO_STORE;

	if(m_pParser->PickPar(istr, 1, &queryName) == PARAM_NOT_FOUND ||
     m_pParser->PickPar(istr, 2, &queryString) == PARAM_NOT_FOUND)		
    return PARAM_NOT_FOUND_MSG;

	if(m_pParser->PickPar(istr, 3, &firstRecord) == PARAM_FOUND)
    firstRecInt = atoi(firstRecord.GetBuffer());

	if(m_pParser->PickPar(istr, 4, &recsToStore) == PARAM_FOUND)
    recsToStoreInt = atoi(recsToStore.GetBuffer());

	queryString.InBlinder('\"');
	queryString.SubstituteSubString("\"\"", "\"");

  m_pTQRODBCManager->Execute(queryName.GetBuffer(), queryString.GetBuffer(), firstRecInt, recsToStoreInt);

  return NULL;
}

/*
char* BC_ODBCExecuteQuery::Execute(char* istr)
{
	itxString* queryName = new itxString();
  itxString* queryString = new itxString();
  itxString* firstRecord = new itxString();
  itxString* recsToStore = new itxString();

	int firstRecInt = STARTING_ROW;
  int recsToStoreInt = ROWS_TO_STORE;

	if(m_pParser->PickPar(istr, 1, queryName) == PARAM_NOT_FOUND ||
     m_pParser->PickPar(istr, 2, queryString) == PARAM_NOT_FOUND)		
    return PARAM_NOT_FOUND_MSG;

	if(m_pParser->PickPar(istr, 3, firstRecord) == PARAM_FOUND)
    firstRecInt = atoi(firstRecord->GetBuffer());

	if(m_pParser->PickPar(istr, 4, recsToStore) == PARAM_FOUND)
    recsToStoreInt = atoi(recsToStore->GetBuffer());

	queryString->InBlinder('\"');
	queryString->SubstituteSubString("\"\"", "\"");

  m_pTQRODBCManager->Execute(queryName->GetBuffer(), queryString->GetBuffer(), firstRecInt, recsToStoreInt);

	delete queryName;
  delete queryString;
  delete firstRecord;
  delete recsToStore;

  return NULL;
}
*/

/*----------------------------------------------------------------------------
                              BC_TQRRecordFieldValue
  ----------------------------------------------------------------------------*/

char* BC_TQRRecordFieldValue::Execute(char* istr)
{
	itxString tqrname;
  itxString colname;

  m_Output.SetEmpty();

	if(m_pParser->PickPar(istr, 1, &tqrname) == PARAM_NOT_FOUND ||
     m_pParser->PickPar(istr, 2, &colname) == PARAM_NOT_FOUND)		
    return PARAM_NOT_FOUND_MSG;

  m_Output = m_pTQRCollection->GetCurrentRecordField(tqrname.GetBuffer(), colname.GetBuffer());
  if( m_Output.IsNull() || m_Output.IsEmpty() )
    m_pParser->PickPar(istr, 3, &m_Output);

	return m_Output.GetBuffer();
}

/*----------------------------------------------------------------------------
                              BC_TQRRecordFieldValQuot
  ----------------------------------------------------------------------------*/

char* BC_TQRRecordFieldValQuot::Execute(char* istr)
{
	itxString tqrname;
  itxString colname;
  itxString appo;

  m_Output.SetEmpty();

	if(m_pParser->PickPar(istr, 1, &tqrname) == PARAM_NOT_FOUND ||
     m_pParser->PickPar(istr, 2, &colname) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

  m_Output = m_pTQRCollection->GetCurrentRecordField(tqrname.GetBuffer(), colname.GetBuffer());
  if( m_Output.IsNull() || m_Output.IsEmpty() )
  {
    m_pParser->PickPar(istr, 3, &m_Output);
  }
  else
  {
    m_Output.AdjustStr();
    m_Output.SubstituteSubString("\"", "\"\"");
    appo  = '\'';
    appo += m_Output;
    appo += '\'';
    m_Output = appo;
  }

  return m_Output.GetBuffer();
}


/*----------------------------------------------------------------------------
                              BC_CycleTQR
  ----------------------------------------------------------------------------*/

/*
attivita'	:definisce il punto di partenza di un ciclo che viene effettuato sui record
					 del TQR specificato. Il comando di fine ciclo, necessario pena un
					 comportamento imprevedibile del template, � *endcycle(TQRName).
					 TQRName							: nome del tqr;
*/

char* BC_CycleTQR::Execute(char* istr)
{
	itxString queryName;

	if (m_pParser->GetCycleBlock(-1) == 0)
	{
		m_pParser->SetCycleBlock(0);
		return NULL;
	}

	if(m_pParser->PickPar(istr, 1, &queryName) == PARAM_NOT_FOUND)
  {
    DebugTrace2(IN_WARNING, "Parameter not found in cycle command.\n");
    return PARAM_NOT_FOUND_MSG;
  }

  TQR* tqr = (TQR*) m_pParser->GetCycleBlock(0);
  if (!ISVALIDPTR(tqr))
  {
    if ((tqr = m_pTQRCollection->Retrieve(queryName.GetBuffer())) == NULL)
    {
      DebugTrace2(IN_COMMAND, "Cannot find TQR named '%s'.\n", queryName.GetBuffer());
		  m_pParser->SetCycleBlock(0);
      return NULL;
    }
  }

	if (tqr->GetTotalRows() != 0)
	{
	  if (tqr->GetCurrentRecord() != NULL)
		  m_pParser->SetCycleBlock((int)tqr);
	  else
		  m_pParser->SetCycleBlock(0);
	}
  else
  {
    m_pParser->SetCycleBlock(0);
    DebugTrace2(IN_COMMAND, "TQR '%s' is empty.\n", tqr->GetName().GetBuffer());
  }

  return NULL;  
}

/*----------------------------------------------------------------------------
                              BC_EndCycleTQR
  ----------------------------------------------------------------------------*/

char* BC_EndCycleTQR::Execute(char* istr)
{
  TQR* tqr = (TQR*) m_pParser->GetCycleBlock(0);
  if (ISVALIDPTR(tqr))
    m_pTQRCollection->MoveNext(tqr);

  return NULL;  
}


/*----------------------------------------------------------------------------
                              BC_CycleFor
  ----------------------------------------------------------------------------*/

/*
attivita'	: definisce il punto d'entrata di un ciclo for:
            la variabile counter, il valore iniziale, il valore finale e lo step
*/

char* BC_CycleFor::Execute(char* istr)
{
	itxString counter;
  itxString tqrname;
  int initialvalue, finalvalue, step;
  TQR* tqr;

	if(m_pParser->PickPar(istr, 1, &counter) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickInt(istr, 2, &initialvalue) == PARAM_NOT_FOUND)		
    return PARAM_NOT_FOUND_MSG;    
	if(m_pParser->PickInt(istr, 3, &finalvalue) == PARAM_NOT_FOUND)		
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickInt(istr, 4, &step) == PARAM_NOT_FOUND)		
    step = 1;
  
  tqrname = TQR_FOR_PREFIX;
  tqrname += counter;

  tqr = m_pTQRCollection->Retrieve(tqrname.GetBuffer());

  // se il TQR* associato al counter � NULL, allora devo comunque creare il tqr
  if (tqr == NULL)
  {
    tqr = m_pTQRManager->CreateTQR(tqrname.GetBuffer(), 1);
    m_pTQRManager->SetColumnAttributes(tqr, 0, counter.GetBuffer(), 0, 10);
  
    itxString icounter; 
    if (step > 0)
      for (int i = initialvalue; i <= finalvalue; i += step)
      {
        icounter.SetInt(i);
        m_pTQRManager->AddTail(tqr);
        m_pTQRManager->SetCurrentRecordField(tqr, counter.GetBuffer(), icounter.GetBuffer());
      }
    else
      for (int i = initialvalue; i >= finalvalue; i += step)
      {
        icounter.SetInt(i);
        m_pTQRManager->AddTail(tqr);
        m_pTQRManager->SetCurrentRecordField(tqr, counter.GetBuffer(), icounter.GetBuffer());
      }

    m_pTQRManager->MoveFirst(tqr);
  }

	if (m_pParser->GetCycleBlock(-1) == 0)
		m_pParser->SetCycleBlock(0);
  else
  {
    if (tqr == NULL)
		  m_pParser->SetCycleBlock(0);
    else if (tqr->GetCurrentRecord() != NULL)
		  m_pParser->SetCycleBlock(1);
	  else
		  m_pParser->SetCycleBlock(0);
  }  
    
  // riposizionamento ad inizio TQR in caso di esaurimento dei cicli
  if (tqr != NULL && m_pParser->GetCycleBlock() == 0)
    m_pTQRCollection->Remove(tqrname.GetBuffer());
  
  return NULL;
}

/*----------------------------------------------------------------------------
                              BC_EndCycleFor
  ----------------------------------------------------------------------------*/

char* BC_EndCycleFor::Execute(char* istr)
{
	itxString counter;
  itxString tqrname;

	if(m_pParser->PickPar(istr, 1, &counter) == PARAM_NOT_FOUND)		
  {
    DebugTrace2(IN_WARNING, "endfor needs argument.\n");
    m_pParser->m_ForceExit = 1;
    return PARAM_NOT_FOUND_MSG;
  }

  tqrname = TQR_FOR_PREFIX;
  tqrname += istr;

  TQR* tqr;
  if ((tqr = m_pTQRCollection->Retrieve(tqrname.GetBuffer())) != NULL)
    m_pTQRCollection->MoveNext(tqr);

  return NULL;
}

/*----------------------------------------------------------------------------
                              BC_ForIndex
  ----------------------------------------------------------------------------*/

char* BC_ForIndex::Execute(char* istr)
{
  itxString tqrname;

  m_Output.SetEmpty();

  tqrname = TQR_FOR_PREFIX;
  tqrname += istr;

  TQR* tqr;
  if ((tqr = m_pTQRCollection->Retrieve(tqrname.GetBuffer())) != NULL)
    m_Output = tqr->GetCurrentRecord()->FieldValue(0);

  return m_Output.GetBuffer();  
}

/*----------------------------------------------------------------------------
                              BC_ExitOnFile
  ----------------------------------------------------------------------------*/

/*
attivita'	:interrompe il processing e forza Tannit verso l'uscita.
*/

char* BC_Exit::Execute(char* istr)
{
  m_pParser->m_ForceExit = 1;
  return NULL;
}

/*----------------------------------------------------------------------------
                              BC_TQRExist
  ----------------------------------------------------------------------------*/

char* BC_TQRExist::Execute(char* istr)
{
  itxString tqrname;

	if(m_pParser->PickPar(istr, 1, &tqrname) == PARAM_NOT_FOUND)		
    return PARAM_NOT_FOUND_MSG;

  TQR* tqr;
  if ((tqr = m_pTQRCollection->Retrieve(tqrname.GetBuffer())) != NULL)
    m_Output = TANNIT_TRUE;
  else
    m_Output = TANNIT_FALSE;

  return m_Output.GetBuffer();
}

/*----------------------------------------------------------------------------
                              BC_Valid
  ----------------------------------------------------------------------------*/

/*
attivita'	:autorizzazione utenti, fa uso della tabella del database specificata nel
					 file dei parametri al valore logintable.
					- login		  : identificatore dell'utente candidato alla autenticazione;
											  viene usato per selezionare il record uguagliandolo al campo
											  specificato nel file di inizializzazione nel parametro loginfield
					- pwd			  : password dell utente candidato alla autenticazione;
											  viene usata per convalidare il record selezionato dal campo login
											  uguagliandola al valore del campo specificato nel file di
											  inizializzazione nel parametro pwdfield
					- extraField: campo addizionale della tabella;
					- extraVal  : valore del campo addizionale da verificare se specificato
*/
char* BC_Valid::Execute(char* istr)
{
  itxString loginSt;
  itxString passwd;
  itxString extraField;
  itxString extraVal;
  itxString retMsg;
  int       validated = 0;
  
  m_Output.SetEmpty();

	if(m_pParser->PickPar(istr, 1, &loginSt) == PARAM_NOT_FOUND ||
     m_pParser->PickPar(istr, 2, &passwd) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

	if(m_pParser->PickPar(istr, 3, &extraField) == PARAM_NOT_FOUND)
    extraField = "1";

	if(m_pParser->PickPar(istr, 4, &extraVal) == PARAM_NOT_FOUND)
    extraVal = "1";

	// Controllo che la password nel DB sia la stessa di input
	validated = CheckDbPwd(&loginSt, &passwd, &extraField, &extraVal, &retMsg);
  DebugTrace2(IN_COMMAND, "%s\n", retMsg.GetBuffer());

  if (validated == 0)
  {
    itxString tpldir;
    itxString tplName;

    tplName = INVALID_LOGIN_FILE;
    m_pCGIRes->m_PRMFile.GetPRMValue(TPLDIR, &tpldir);

    //Parser is now redirected on the login error template
    //and after that, forced to pop the current (unvalidated) template.
    m_pParser->Run(&tpldir, &tplName);
    m_pParser->m_ForceReturn = 1;
	}

  return m_Output.GetBuffer();
}

/*
attivita'	:confronta la password nel database con la password di input;
           l'utente del comando DEVE sapere se su db la pwd e` criptata
           oppure no, e passare al comando un valore criptato, oppure no.
return		:il numero di caratteri presi
chiamante	:lo stato di validazione
*/

int BC_Valid::CheckDbPwd(itxString* login, 
                      itxString* pwd, 
                      itxString* extraField, 
                      itxString* extraVal, 
                      itxString* retMsg)
{
	itxString queryString;
	itxString appo;
	itxString logintable;
  itxString pwdcolname;
  char*     temp_tqr_name = "validation_tqr";
	char*     dbPwd;
  int       ret = 0;

  queryString += "SELECT ";
  
  m_pCGIRes->m_PRMFile.GetPRMValue("pwdfield",   &appo);
  pwdcolname = appo;
  
  queryString += appo;
  queryString += " FROM ";

  m_pCGIRes->m_PRMFile.GetPRMValue("logintable", &appo);
  
  logintable   = appo;
  queryString += appo;
  queryString += " WHERE ";

  m_pCGIRes->m_PRMFile.GetPRMValue("loginfield", &appo);

  queryString += appo;
  queryString += " = ";

  if (login->FindSubString("'") < 0 && LoginWantsQuotes(&logintable, &appo))
  {
    queryString += "'";
    queryString += *login;
    queryString += "'";
  }
  else
    queryString += *login;

  queryString += " AND ";
  queryString += *extraField;
  queryString += " = ";
  queryString += *extraVal;

  DebugTrace2(IN_COMMAND, "%s\n", queryString.GetBuffer());

  // Connessione al DB e acquisizione del valore della password
  if (m_pTQRODBCManager->Execute(temp_tqr_name, queryString.GetBuffer()) == ITXFAILED)
    return 0;

  TQR* tqr;
  if ((tqr = m_pTQRCollection->Retrieve(temp_tqr_name)) != NULL)
  {
    dbPwd = m_pTQRCollection->GetCurrentRecordField(temp_tqr_name, pwdcolname.GetBuffer());
    if(dbPwd == NULL || strcmp(dbPwd, "") == 0)
    {
      DebugTrace2(IN_COMMAND, "Password field value is null or empty.");
      *retMsg = ERR_COL_NOT_FOUND;
    }
  }
  else
  {
    DebugTrace2(IN_COMMAND | IN_ERROR, "Internal tqr not found.");
    *retMsg = ERR_QUERY_NOT_FOUND;
  }

  if (pwd->Compare(dbPwd) == 0)
  {
    ret = 1;
    DebugTrace2(IN_COMMAND, "Current login has been validated.\n");
  }
  else
    DebugTrace2(IN_COMMAND, "Current login has NOT been validated.\n");

  m_pTQRCollection->Remove(temp_tqr_name);
  
  return ret;
}

bool BC_Valid::LoginWantsQuotes(itxString* logintable, itxString* loginfield)
{
  char*     info_tqr_name = "info_tqr";
  itxString inquiry;
  bool      ret = false;

  try
  {
    inquiry  = "SELECT * FROM ";
    inquiry += *logintable;

    m_pTQRODBCManager->Inquiry(info_tqr_name, inquiry.GetBuffer());

    TQR* tqrinfo;
    if ((tqrinfo = m_pTQRCollection->Retrieve(info_tqr_name)) != NULL)
    {
      if (!m_pTQRODBCManager->IsNumeric(tqrinfo, loginfield->GetBuffer()))
        ret = true;

      m_pTQRCollection->Remove(info_tqr_name);
    }
  }
  catch(...)  
  {
    DebugTrace2(IN_COMMAND | IN_ERROR, "%s\n", "Unhandled exception in BC_Valid::LoginWantsQuotes.");
    throw;
  }

  return ret;
}


/*----------------------------------------------------------------------------
                              BC_Crypt
  ----------------------------------------------------------------------------*/

char* BC_Crypt::Execute(char* istr)
{
	itxString     toBeCrypted;
  unsigned char key[64] = CRYPT_KEY;
  unsigned char wht[64] = CRYPT_WHT;

  m_Output.SetEmpty();

	if(m_pParser->PickPar(istr, 1, &toBeCrypted) == PARAM_NOT_FOUND)		
    return PARAM_NOT_FOUND_MSG;

  m_Output.Space(10 * toBeCrypted.Len() * sizeof(char), 0);

  itxEncrypt(&key[0], &wht[0], (unsigned char*)toBeCrypted.GetBuffer(), (unsigned char*)m_Output.GetBuffer());

  return m_Output.GetBuffer();
}

/*
// (in)  source string must be null terminated
// (out) the destination string is the hex representation of the ascii values of the 
//       characters of the crypted string
*/
unsigned short BC_Crypt::itxEncrypt(unsigned char* DESKey, unsigned char* Whitenings,
	                                  unsigned char* source, unsigned char* destination)
{
	struct DESXKey dxKey;
	struct DESXContext dxCon;

  unsigned char blok[ITX_DES_BLOK];
  unsigned char* sourcepos = source;
  unsigned char* destinpos = destination;
  int sourcelen = 0;
  int iblok, nbloks = 0, residual = 0;
  itxString appo;

  memcpy(dxKey.DESKey64, DESKey, ITX_DES_BLOK);
  memcpy(dxKey.Whitening64, Whitenings, ITX_DES_BLOK);

  DESXKeySetup(&dxCon, &dxKey);

  sourcelen = strlen((char*) source);
  nbloks = sourcelen / ITX_DES_BLOK;
  residual = sourcelen % ITX_DES_BLOK;

  for (iblok = 0; iblok < nbloks; iblok++)
  {
    memcpy(blok, source + iblok * ITX_DES_BLOK, ITX_DES_BLOK);
	  DESXEncryptBlock(&dxCon, blok, blok);
    appo.HexToAscii((char*) blok, (char*) (destination + 2 * iblok * ITX_DES_BLOK), ITX_DES_BLOK);
  }

  if (residual != 0)
  {
    /* blanks added on tail to obtain well formed DES_BLOK */
    memset(blok, ' ', ITX_DES_BLOK);
    memcpy(blok, source + iblok * ITX_DES_BLOK, residual);
	  DESXEncryptBlock(&dxCon, blok, blok);
    appo.HexToAscii((char*) blok, (char*) (destination + 2 * iblok * ITX_DES_BLOK), ITX_DES_BLOK);    
  }

  /* make destination null-terminated */
  *(destination + 2 * (iblok + 1) * ITX_DES_BLOK) = 0;

  return 0;
}

/*----------------------------------------------------------------------------
                              BC_Decrypt
  ----------------------------------------------------------------------------*/

char* BC_Decrypt::Execute(char* istr)
{
	itxString toBeDecr;
  unsigned char key[64] = CRYPT_KEY;
  unsigned char wht[64] = CRYPT_WHT;

  m_Output.SetEmpty();

	if(m_pParser->PickPar(istr, 1, &toBeDecr) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

  m_Output.Space(toBeDecr.Len() * sizeof(char), 0);

  itxDecrypt(&key[0], &wht[0], (unsigned char*)toBeDecr.GetBuffer(), (unsigned char*)m_Output.GetBuffer());

  return m_Output.GetBuffer();
}

/*
// (in) source string must be the hex representation of the ascii values of the 
//      characters of the crypted string
*/
unsigned short BC_Decrypt::itxDecrypt(unsigned char* DESKey, unsigned char* Whitenings,
	                                    unsigned char* source, unsigned char* destination)
{
	struct DESXKey dxKey;
	struct DESXContext dxCon;

  unsigned char blok[ITX_DES_BLOK];
  unsigned char* sourcepos = source;
  unsigned char* destinpos = destination;
  int sourcelen = 0;
  int iblok, nbloks = 0, i = 0;
  itxString appo;

  memcpy(dxKey.DESKey64, DESKey, ITX_DES_BLOK);
  memcpy(dxKey.Whitening64, Whitenings, ITX_DES_BLOK);

  DESXKeySetup(&dxCon, &dxKey);

  sourcelen = strlen((char*) source);
  nbloks = sourcelen / (ITX_DES_BLOK * 2);

  appo.AsciiToHex((char*) source, (char*) source, sourcelen / 2);

  for (iblok = 0; iblok < nbloks; iblok++)
  {
    memcpy(blok, source + iblok * ITX_DES_BLOK, ITX_DES_BLOK);
  	DESXDecryptBlock(&dxCon, blok, blok);
    memcpy(destination + iblok * ITX_DES_BLOK, blok, ITX_DES_BLOK);
  }

  *(destination + iblok * ITX_DES_BLOK) = 0;

  /* eventually trims the blanks added on tail */
  i = 0;
  while ((*(destination + iblok * ITX_DES_BLOK + i - 1)) == ' ')
  {
    *(destination + iblok * ITX_DES_BLOK + i - 1) = 0;
    i--;
  }

  return 0;
}

/*----------------------------------------------------------------------------
                              BC_RemoveTQR
  ----------------------------------------------------------------------------*/

char* BC_RemoveTQR::Execute(char* istr)
{
  itxString tqrname;

	if(m_pParser->PickPar(istr, 1, &tqrname) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

  m_pTQRCollection->Remove(tqrname.GetBuffer());

  return NULL;  
}

/*----------------------------------------------------------------------------
                              BC_TQRStat
  ----------------------------------------------------------------------------*/

char* BC_TQRStat::Execute(char* istr)
{
  itxString tqrname;
  itxString stat;

  m_Output.Space(GET_PAR_VAL_LEN, '\0');

	if(m_pParser->PickPar(istr, 1, &tqrname) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickPar(istr, 2, &stat) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

  int icol;
	if(m_pParser->PickInt(istr, 3, &icol) == PARAM_NOT_FOUND)
    icol = -1;

  TQR* tqr;
  if ((tqr = m_pTQRCollection->Retrieve(tqrname.GetBuffer())) != NULL)
  {
    if (stat.Compare(TOTRECS) == 0)
    	m_Output.SetInt(tqr->GetTotalRows());
    else if (stat.Compare(ACTUALROW) == 0)
    	m_Output.SetInt(tqr->GetActualRow());
    else if (stat.Compare(ROWSTOSTORE) == 0)
    	m_Output.SetInt(tqr->GetRowsToStore());
    else if (stat.Compare(STARTINGROW) == 0)
    	m_Output.SetInt(tqr->GetStartingRow());
    else if (stat.Compare(MORERECS) == 0)
    {
      if (tqr->GetMoreDBRows())
        return TANNIT_TRUE;
      else  
        return TANNIT_FALSE;
    }
    else if (stat.Compare(SOURCECOUNT) == 0)
    	m_Output.SetInt(tqr->GetSourceRecordCount());
    else if (stat.Compare(ACTUALPAGE) == 0)
    	m_Output.SetInt(tqr->GetActualPage());
    else if (stat.Compare(TOTALPAGES) == 0)
    	m_Output.SetInt(tqr->GetTotalPages());
    else if (stat.Compare(LASTPAGEROW) == 0)
    	m_Output.SetInt(tqr->GetLastPageRow());
    else if (stat.Compare(NEXTPAGEROW) == 0)
    	m_Output.SetInt(tqr->GetNextPageRow());
    else if (stat.Compare(PREVPAGEROW) == 0)
    	m_Output.SetInt(tqr->GetPrevPageRow());
    else if (stat.Compare(COLCOUNT) == 0) //TQR info: column count
    	m_Output.SetInt(tqr->GetColsNum());
    else if (stat.Compare(COLNAME) == 0)  //TQR info: column name
        m_Output = tqr->GetColName(icol);
    else if (stat.Compare(COLTYPE) == 0)  //TQR info: column type
        m_Output.SetInt(tqr->GetColType(icol));
    else if (stat.Compare(COLSIZE) == 0)  //TQR info: column size
        m_Output.SetInt(tqr->GetColSize(icol));
  }

  return m_Output.GetBuffer();  
}

/*----------------------------------------------------------------------------
                              Return
  ----------------------------------------------------------------------------*/
char* BC_Return::Execute(char* istr)
{
  m_pParser->m_ForceReturn = 1;
  return NULL;
}

/*----------------------------------------------------------------------------
                              Flush
  ----------------------------------------------------------------------------*/

char* BC_Flush::Execute(char* istr)
{
  TemplateFile* ptpl = NULL;

  try
  {
    m_pParser->m_TplStack.SetCursorBottom();
    TemplateFile* ptpl = (TemplateFile*)m_pParser->m_TplStack.Bottom();

    while(ptpl != NULL)
    {
      m_pCGIRes->Flush(ptpl->m_Output.GetBuffer());
      ptpl->m_Output.SetEmpty();
      ptpl = (TemplateFile*)m_pParser->m_TplStack.NextUp();
    }
  }
  catch(...)
  {
    DebugTrace2(IN_COMMAND | IN_ERROR, "BC_Flush::Execute: ptpl = %p", ptpl);
    itxString appo;
    ptpl->GetContent(&appo);
    DebugTrace2(IN_COMMAND | IN_ERROR, "BC_Flush::Execute:\n%s",  appo.GetBuffer());
  }

  return NULL;
}


/*----------------------------------------------------------------------------
                              BC_TQRMove
  ----------------------------------------------------------------------------*/

char* BC_TQRMove::Execute(char* istr)
{
  itxString tqrname;
  itxString action;
  int row;

	if(m_pParser->PickPar(istr, 1, &tqrname) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickPar(istr, 2, &action) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickInt(istr, 3, &row) == PARAM_NOT_FOUND)
    row = 0;

  TQR* tqr;
  if ((tqr = m_pTQRCollection->Retrieve(tqrname.GetBuffer())) != NULL)
  {
    if (action.Compare(FIRST) == 0)
    	tqr->MoveFirst();
    else if (action.Compare(LAST) == 0)
    	tqr->MoveLast();
    else if (action.Compare(NEXT) == 0)
    	tqr->MoveNext();
    else if (action.Compare(TO) == 0)
    	tqr->MoveTo(row);
  }

  return NULL;  
}

/*----------------------------------------------------------------------------
                              BC_TQRFilt
  ----------------------------------------------------------------------------*/

char* BC_TQRFilt::Execute(char* istr)
{
	itxString source;
  itxString field;
  itxString value;
  itxString destination;

	if(m_pParser->PickPar(istr, 1, &source) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickPar(istr, 2, &field) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickPar(istr, 3, &value) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickPar(istr, 4, &destination) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

  TQR* tqr;
  tqr = m_pTQRManager->Filter(source.GetBuffer(),
                              field.GetBuffer(),
                              value.GetBuffer(),
                              destination.GetBuffer());

  if (tqr == NULL)
    DebugTrace2(IN_COMMAND, "Unable to filter tqr %s\n", source.GetBuffer());

  return NULL;  
}

/*----------------------------------------------------------------------------
                              BC_TQRSample
  ----------------------------------------------------------------------------*/

char* BC_TQRSample::Execute(char* istr)
{
	itxString source;
  itxString destination;
  int destMaxRecs, seed;

	if(m_pParser->PickPar(istr, 1, &source) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickPar(istr, 2, &destination) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickInt(istr, 3, &destMaxRecs) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickInt(istr, 4, &seed) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

  TQR* tqr;
  tqr = m_pTQRManager->Sample(source.GetBuffer(),
                              destination.GetBuffer(),
                              destMaxRecs,
                              seed);

  if (tqr == NULL)
    DebugTrace2(IN_COMMAND, "Unable to sample tqr %s\n", source.GetBuffer());

  return NULL;
}

/*----------------------------------------------------------------------------
                              BC_Recsel
  ----------------------------------------------------------------------------*/

/*
attivita'	:analogo a recVal restituisce il valore del campo specificato per il record
					 corrente del resultset specificato; aggiunge la stringa " SELECTED" se il
					 dato e' uguale al dato determinato dalla seconda coppia di parametri
par 1			:nome del resultset da identificare
par 2			:nome del campo
par 3			:nome del resultset da identificare per il confronto; se il nome e' "get"
					 il dato di confronto viene cercato nella stringa di get
par 4			:nome del campo da utilizzare per il confronto
*/

char* BC_Recsel::Execute(char* istr)
{
	itxString queryName;
  itxString queryField;
  itxString cnfField;
  itxString cnfSrc;

  m_Output.SetEmpty();

	if(m_pParser->PickPar(istr, 1, &queryName) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickPar(istr, 2, &queryField) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickPar(istr, 3, &cnfSrc) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickPar(istr, 4, &cnfField) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

	itxString firstVal;
  itxString secVal;
	
  firstVal = m_pTQRCollection->GetCurrentRecordField(queryName.GetBuffer(), queryField.GetBuffer());
  if (firstVal.IsEmpty())
    return " ";

  if (cnfSrc.Compare("get") == 0)
  {
		if (m_pCGIRes->cgiFormString(cnfField.GetBuffer(), &secVal, 0) != cgiFormSuccess)
			secVal = GET_PAR_VAL;		//valore di default generico
  }
  else
    secVal = m_pTQRCollection->GetCurrentRecordField(cnfSrc.GetBuffer(), cnfField.GetBuffer());

  m_Output  = firstVal;
	if (firstVal.Compare(&secVal) == 0)
		m_Output += " SELECTED";

	return m_Output.GetBuffer();
}

/*----------------------------------------------------------------------------
                              BC_Trans
  ----------------------------------------------------------------------------*/

/*
attivita'	:traduce la phraseTag nella lingua corrente. Fa uso dei parametri
					 LangTable, LangNameField, LangCodeField del file di inizializzazione per
					 definire tabella dei linguaggi, campo del nome della lingua, campo del
					 codice della lingua. Ricava il valore del campo LangNameField per il record
					 in cui il campo LangCodeField corrisponde al valore del parametro di get
					 identificato dal valore del parametro LangTagGet del file di inizializzazione.
					 Il nome del campo della lingua � quello da estrarre dalla tabella delle
					 traduzioni (definita dal parametro TransTable nell'file di inizializzazione)
					 in corrispondenza del valore del campo TransTagField (sempre nel file dei
					 parametri) che uguaglia la stringa di input del comando phraseTag.
					 OPZforceLang, � un parametro che forza la lingua al valore specificato.
*/

char* BC_Trans::Execute(char* istr)
{
	itxString translatedPhrase;
  itxString phraseTag;
  itxString forceLang;
  itxString queryString;
  itxString userlang;
  itxString langName;

  itxString LangTable;          //PRM
  itxString LangTagGet;         //PRM
  itxString LangCodeField;      //PRM
  itxString DefaultLanguageId;  //PRM
  itxString LangNameField;      //PRM
  itxString TransTable;         //PRM
  itxString TransTagField;      //PRM
  
  char*     temp_tqr_name = "trans_tqr";
  TQR*      tqr;
  
  m_Output.SetEmpty();

  //Parse input string
	if(m_pParser->PickPar(istr, 1, &phraseTag) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickPar(istr, 2, &forceLang) == PARAM_NOT_FOUND)
    forceLang.SetEmpty();

  //Push on the stack of this functions needed prm variables.
  LangTable.SetEmpty();
  LangTagGet.SetEmpty();
  LangCodeField.SetEmpty();
  DefaultLanguageId.SetEmpty();
  LangNameField.SetEmpty();
  TransTable.SetEmpty();
  TransTagField.SetEmpty();
    
  m_pCGIRes->m_PRMFile.GetPRMValue("LangTable", &LangTable);
  m_pCGIRes->m_PRMFile.GetPRMValue("LangTagGet", &LangTagGet);
  m_pCGIRes->m_PRMFile.GetPRMValue("LangCodeField", &LangCodeField);
  m_pCGIRes->m_PRMFile.GetPRMValue("LangNameField", &LangNameField);
  m_pCGIRes->m_PRMFile.GetPRMValue("DefaultLanguageId", &DefaultLanguageId);
  m_pCGIRes->m_PRMFile.GetPRMValue("TransTable", &TransTable);
  m_pCGIRes->m_PRMFile.GetPRMValue("TransTagField", &TransTagField);

  if (LangTable.IsEmpty()         || LangTagGet.IsEmpty()     || LangCodeField.IsEmpty()  || 
      DefaultLanguageId.IsEmpty() || LangNameField.IsEmpty()  || LangTable.IsEmpty()      || 
      LangCodeField.IsEmpty()     || TransTable.IsEmpty()     || TransTagField.IsEmpty())
  {
    DebugTrace2(IN_COMMAND, "Empty prm variables found.");
    return NULL;
  }
  
  //Get from http get string the value of the current language
	if (!forceLang.IsEmpty())
		userlang = forceLang;
  else
  {
    userlang.SetEmpty();
	  if (m_pCGIRes->cgiFormString(LangTagGet.GetBuffer(), &userlang, 0) != cgiFormSuccess)
		  userlang = DefaultLanguageId;	
  }
  
  //Query the name of the language to use as field name in the translator table
  queryString = "SELECT ";
  queryString += LangNameField;
  queryString += " FROM ";
  queryString += LangTable;
  queryString += " WHERE ";
  queryString += LangCodeField;
  queryString += " = ";
  queryString += userlang;

  if (m_pTQRODBCManager->Execute(temp_tqr_name, queryString.GetBuffer()) == ITXFAILED)
    return NULL;

  if ((tqr = m_pTQRCollection->Retrieve(temp_tqr_name)) != NULL)
  {
	  langName = m_pTQRCollection->GetCurrentRecordField(temp_tqr_name, LangNameField.GetBuffer());
    if(langName.IsNull() || langName.IsEmpty())
      DebugTrace2(IN_COMMAND, "Language name field value is null or empty.");

    //can safely remove the tqr becasue langName is an itxString
    m_pTQRCollection->Remove(temp_tqr_name);
  }
  else
    DebugTrace2(IN_COMMAND | IN_ERROR, "Internal tqr not found.");

  //Query the value of the translation
  queryString = temp_tqr_name;
  queryString += ", SELECT ";
  queryString += langName;
  queryString += " FROM ";
  queryString += TransTable;
  queryString += " WHERE ";
  queryString += TransTagField;
  queryString += " = '";
  queryString += phraseTag;
  queryString += "'";

  BC_ODBCExecuteQuery::Execute(queryString.GetBuffer());

  if ((tqr = m_pTQRCollection->Retrieve(temp_tqr_name)) != NULL)
  {
	  m_Output = m_pTQRCollection->GetCurrentRecordField(temp_tqr_name, langName.GetBuffer());
    if(m_Output.IsNull() || m_Output.IsEmpty())
      DebugTrace2(IN_COMMAND, "Language name field value is null or empty.");

    m_pTQRCollection->Remove(temp_tqr_name);
  }
  else
    DebugTrace2(IN_COMMAND | IN_ERROR, "Internal tqr not found.");

  return m_Output.GetBuffer();
}


/*----------------------------------------------------------------------------
                              TQRStore
  ----------------------------------------------------------------------------*/

char* BC_TQRStore::Execute(char* istr)
{
	itxString tqrname;
  int nfields;
  itxString sep;
  itxString buffer;
  
  char fieldsep, recsep;

	if(m_pParser->PickPar(istr, 1, &tqrname) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickInt(istr, 2, &nfields) == PARAM_NOT_FOUND)
    nfields = 0;
	if(m_pParser->PickPar(istr, 3, &sep) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickPar(istr, 4, &buffer) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

  buffer.InBlinder('\x27');

  char* psep = sep.GetBuffer();
  fieldsep = psep[0];
  recsep = psep[1];

  m_pTQRManager->LoadDataBuffer(tqrname.GetBuffer(), nfields, recsep, fieldsep, buffer.GetBuffer());
  m_pTQRManager->MoveFirst(m_pTQRManager->Get(tqrname.GetBuffer()));

  return 0;
};

/*----------------------------------------------------------------------------
                              TQRInsert
  ----------------------------------------------------------------------------*/

char* BC_TQRInsert::Execute(char* istr)
{
	itxString tqrname;
	itxString tablename;
  
	if(m_pParser->PickPar(istr, 1, &tqrname) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickPar(istr, 2, &tablename) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

  m_pTQRODBCManager->BulkInsert(tablename.GetBuffer(), tqrname.GetBuffer(), NULL);

  return 0;
};


/*----------------------------------------------------------------------------
                              Pager
  ----------------------------------------------------------------------------*/

char* BC_Pager::Execute(char* istr)
{
	itxString tqrSrcName;
	itxString tqrDstName;
  int pagesToShow;

	if(m_pParser->PickPar(istr, 1, &tqrSrcName) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickInt(istr, 2, &pagesToShow) == PARAM_NOT_FOUND)
    pagesToShow = 10;

  TQR* tqrSrc;
  TQR* tqrDst;

  if((tqrSrc = m_pTQRManager->Get(tqrSrcName.GetBuffer())) == NULL)
  {
    DebugTrace2(IN_COMMAND, "Cannot find TQR named '%s'.\n", tqrSrcName.GetBuffer());
    return 0;
  }
  
  tqrDstName = tqrSrcName;
  tqrDstName += "_pg";

  tqrDst = m_pTQRManager->CreateTQR(tqrDstName.GetBuffer(), 2);

  tqrDst->SetColumnAttributes(0, "pagenum", 0, 10);
  tqrDst->SetColumnAttributes(1, "startrow", 0, 10);

  m_pTQRManager->SetRowsParam(tqrDst, 1, pagesToShow, 1, 0);
  
	itxString buff;
  int sourceTotPages = tqrSrc->GetTotalPages();

  for(int i = 1; i <= pagesToShow && i <= sourceTotPages; i++)
  {
    buff.SetInt(i);
    tqrDst->AddTail();
    tqrDst->SetCurrentRecordField(0, buff.GetBuffer());

    buff.SetInt((tqrSrc->GetRowsToStore() * (i - 1)) + 1);
    tqrDst->SetCurrentRecordField(1, buff.GetBuffer());
  }
  tqrDst->MoveFirst();

  return 0;
}

/*----------------------------------------------------------------------------
                              Netprex
  ----------------------------------------------------------------------------*/

char* BC_Netprex::Execute(char* istr)
{
	itxString url;
  itxString cmd_tag;

  m_Output.SetEmpty();

	if(m_pParser->PickPar(istr, 1, &url) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickPar(istr, 2, &cmd_tag) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

  try
  {
    itxHttp connection("Thunder v 1.45");
    connection.GET(url.GetBuffer());
    m_Output = connection.GetDataReceived();

    char oldtag = m_pParser->m_StartCommandTag;
    m_pParser->m_StartCommandTag = cmd_tag.GetAt(0);
    m_pParser->Run(m_Output.GetBuffer());
    m_pParser->m_StartCommandTag = oldtag;
  }
  catch(itxException* e)
  {
    DebugTrace2(IN_COMMAND, "ITX_EXCEPTION: %d '%s'.\n", e->m_errornumber, e->m_procedure);
  }
  catch(...)
  {
    DebugTrace2(IN_COMMAND, "Netprex generic exception\n");
  }
  
  return NULL;
}


/*----------------------------------------------------------------------------
                              NetGrab
  ----------------------------------------------------------------------------*/

char* BC_Netgrab::Execute(char* istr)
{
	itxString url;
  int maxbytes;
  int maxsecs;
  itxString to;
  char *myBuffer;
  FILE *fdto;

  m_Output.SetEmpty();

	if(m_pParser->PickPar(istr, 1, &url) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickInt(istr, 2, &maxbytes) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickInt(istr, 3, &maxsecs) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

  try
  {
    itxHttp connection("Thunder v 1.45");
    myBuffer = (char *) calloc(maxbytes, sizeof(char));

    int datalen = connection.GETBin(url.GetBuffer(), &myBuffer, maxbytes, 4096, maxsecs);

    if(m_pParser->PickPar(istr, 4, &to) != PARAM_NOT_FOUND)
    {
      to.InBlinder('"');
      fdto = fopen(to.GetBuffer(),"wb");
      if (fdto)
        fwrite(myBuffer, sizeof(char), datalen, fdto);
      fclose(fdto);
    }
    free(myBuffer);
  
    if (datalen > 0)
      sprintf(m_Output.GetBuffer(),"%d", datalen);
  }
  catch(itxException* e)
  {
    DebugTrace2(IN_COMMAND, "ITX_EXCEPTION: %d '%s'.\n", e->m_errornumber, e->m_procedure);
  }
  catch(...)
  {
    DebugTrace2(IN_COMMAND, "NetGrab generic exception\n");
  }

  
  return m_Output.GetBuffer();
}



/*----------------------------------------------------------------------------
                              Proc
  ----------------------------------------------------------------------------*/

char* BC_Proc::Execute(char* istr)
{
	itxString buffer;
  itxString cmd_tag;

  m_Output.SetEmpty();

  m_pCGIRes->m_PRMFile.GetPRMValue("ForceProcCmdTag", &cmd_tag);

  if (cmd_tag.IsEmpty())
  {
    if(m_pParser->PickPar(istr, 1, &buffer) == PARAM_NOT_FOUND)
      return PARAM_NOT_FOUND_MSG;
    if(m_pParser->PickPar(istr, 2, &cmd_tag) == PARAM_NOT_FOUND)
      return PARAM_NOT_FOUND_MSG;
   	buffer.InBlinder('\"');
  }
  else
  {
    buffer = istr;
  }

  try
  {
    char oldtag = m_pParser->m_StartCommandTag;
    m_pParser->m_StartCommandTag = cmd_tag.GetAt(0);
    m_pParser->Run(buffer.GetBuffer());
    m_pParser->m_StartCommandTag = oldtag;
  }
  catch(...)
  {
    DebugTrace2(IN_COMMAND, "Proc generic exception\n");
  }
  
  return m_Output.GetBuffer();
}


/*----------------------------------------------------------------------------
                              BC_GetValueQuote
  ----------------------------------------------------------------------------*/

/*
attivita'	:restituisce il valore del parametro di get indicato e sostituisce
           nella stringa risultante i caratteri apice con i doppio apice
					 - parametro: nome del parametro di get di cui restituire il valore
					 - OPZdefault: valore di default ritornato nel caso non si trovi il
												 parametro.
						 N.B. se OPZdefault � il carattere '+' il valore
						 ricavato per il parametro viene modificato sostituendo tutti
						 i caratteri non alfanumerici con la stringa %xx dove xx �
						 il valore ascii esadecimale del carattere sostituito.
*/

char* BC_GetValueQuote::Execute(char* istr)
{
  itxString defParVal;
  itxString getParName;
	int		    useDefault = 0;

  m_Output.SetEmpty();

  if (m_pParser->PickPar(istr, 1, &getParName) == PARAM_NOT_FOUND)
  {
    m_Output = PARAM_NOT_FOUND_MSG;
    return m_Output.GetBuffer();
  }

  if (m_pParser->PickPar(istr, 2, &defParVal) == PARAM_FOUND)
    useDefault = 1;

	if (m_pCGIRes->cgiFormString(getParName.GetBuffer(), &m_Output, 0) != cgiFormSuccess)
	{
		if (useDefault) // valore opzionale di default definito dall'utente
			m_Output = defParVal;
		else  // valore di default generico
			m_Output = GET_PAR_VAL;
	}

	if (!defParVal.IsNull() && strcmp(defParVal.GetBuffer(), "+") == 0)
	{
		if(!m_Output.IsNull() && strcmp(m_Output.GetBuffer(), "+") == 0)
			m_Output = "";

		m_Output.EscapeChars();
	}

  m_Output.AdjustStr();
  m_Output.SubstituteSubString("\"", "\"\"");

  return m_Output.GetBuffer();
}

/*----------------------------------------------------------------------------
                              BC_GetValueCast
  ----------------------------------------------------------------------------*/

/*
attivita'	: restituisce il valore del parametro di get indicato e trasformando
            la stringa di input nel formato richiesto
*/

char* BC_GetValueCast::Execute(char* istr)
{
  itxString defParVal;
  itxString outputFormatIndicator;
  itxString getParName;
  char  *mySource;
  char  *mySourceCrs;
  char  *myDestin;
  char  *myDestinCrs;
	int   useDefault = 0;
  int   dotsNumber = 0;
  FormResultType_t formReadStatus;

  m_Output.SetEmpty();

  if (m_pParser->PickPar(istr, 1, &outputFormatIndicator) == PARAM_NOT_FOUND)
  {
    m_Output = PARAM_NOT_FOUND_MSG;
    return m_Output.GetBuffer();
  }

  if (m_pParser->PickPar(istr, 2, &getParName) == PARAM_NOT_FOUND)
  {
    m_Output = PARAM_NOT_FOUND_MSG;
    return m_Output.GetBuffer();
  }

  if (m_pParser->PickPar(istr, 3, &defParVal) == PARAM_FOUND)
    useDefault = 1;

	formReadStatus = m_pCGIRes->cgiFormString(getParName.GetBuffer(), &m_Output, 0);

  if(strcmp(outputFormatIndicator.GetBuffer(), "q") == 0)
  {
    m_Output.AdjustStr();
    m_Output.SubstituteSubString("\"", "\"\"");
  }
  else if(strcmp(outputFormatIndicator.GetBuffer(), "num") == 0)
  {
    mySource = (char *) calloc(m_Output.Len(), sizeof(char));
    myDestin = (char *) calloc(m_Output.Len()+1, sizeof(char));
    strcpy(mySource, m_Output.GetBuffer());
    mySourceCrs = mySource;
    myDestinCrs = myDestin;
    dotsNumber = 0;
    
    while(*mySourceCrs != 0)
    {
      if(isdigit(*mySourceCrs) == 0)
      {
        if((*mySourceCrs == ',') || (*mySourceCrs == '.'))
        {
          if(dotsNumber == 0)
          {
            *myDestinCrs = '.';
          }
          else
          {
            myDestinCrs--;
          }
          dotsNumber++;
        }
        else
        {
          myDestinCrs--;
        }
      }
      else
      {
        *myDestinCrs = *mySourceCrs;
      }
      myDestinCrs++;
      mySourceCrs++;
    }

    if (mySource)
      free(mySource);

    if (myDestin)
    {
      m_Output = myDestin;
      free(myDestin);
    }
  }

	if ((formReadStatus != cgiFormSuccess) || m_Output.IsEmpty())
	{
		if (useDefault) // valore opzionale di default definito dall'utente
			m_Output = defParVal;
		else  // valore di default generico
			m_Output = GET_PAR_VAL;
	}

	if (!defParVal.IsNull() && strcmp(defParVal.GetBuffer(), "+") == 0)
	{
		if(!m_Output.IsNull() && strcmp(m_Output.GetBuffer(), "+") == 0)
			m_Output = "";

		m_Output.EscapeChars();
	}
  
  return m_Output.GetBuffer();
}

char* BC_Currency::Execute(char* istr)
{
  int invert = 0;

  m_Output.SetEmpty();

  if (m_pParser->PickPar(istr, 1, &m_Output) == PARAM_NOT_FOUND)
    return 0;

  if (m_pParser->PickInt(istr, 2, &invert) == PARAM_NOT_FOUND)
    m_Output.Currency();
  else
    m_Output.Currency(-1);

  return m_Output.GetBuffer();
}

