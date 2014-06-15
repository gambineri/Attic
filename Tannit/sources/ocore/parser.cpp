/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: parser.cpp,v $
 * $Revision: 1.98 $  
 * $Author: massimo $
 * $Date: 2002-06-25 18:25:15+02 $
 | _-.-..-._-_-._--_-._.-._     AITECSA S.r.l.  2000
 |
 |
 | FILENAME    : parser.cpp
 | TAB         : 2 spaces
 |
 | DESCRIPTION : Parser implementations.
 |
 |
 */


#include "parser.h"
#include "cgiresolver.h"
#include "commands.h"
#include "tannitobj.h"



                        /****************
                              Parser
                         ****************/


//-----------------------------------------------------------------

Parser::Parser()
{
  for (int i=0; i < MAX_CMDS; i++)
    m_Command[i] = NULL;

  m_TotalCount      = 0;
  m_BaseCount       = 0;
  m_ForceExit       = 0;
  m_ForceReturn     = 0;
  m_TplVars.idx     = 0;
  m_StartCommandTag = START_COMMAND_TAG;
  m_PreprocRun      = false; // becoming obsolete
  m_RunType         = Normalrun;
  m_CurTpl          = NULL;
  m_ActualTplDir    = "";
}

//-----------------------------------------------------------------

Parser::~Parser()
{
  DestroyBaseCommands();
  if (m_CurTpl != NULL)
    delete m_CurTpl;
}

//-----------------------------------------------------------------

void Parser::CreateBaseCommands(void* tannit)
{
  Tannit*               ptannit           = (Tannit*)tannit;
  CGIResolver*          pcgires           = (CGIResolver*)tannit;
  TQRManager*           ptqr_manager      = &ptannit->m_TQRManager;
  TQRODBCManager*       ptqrodbc_manager  = &ptannit->m_TQRODBCManager;
  TQRCollection*        ptqrcollection    = &ptannit->m_TQRCollection;
  itxSQLConnCollection* pconnections      = &ptannit->m_connections;
  itxSQLConnection*     podbcconnection   = &ptannit->m_odbcconnection;
  
  int i = 0;

//BEGIN CREATION BASE COMMANDS
  
  m_Command[i++] = (AbstractCommand*)new BC_StartConditionalBlock(START_CONDITIONAL_BLOCK, this, START_CND_BLK);
  m_Command[i++] = (AbstractCommand*)new BC_EndConditionalBlock();
  m_Command[i++] = (AbstractCommand*)new BC_AlternateConditionalBlock();
  m_Command[i++] = (AbstractCommand*)new BC_StartConditionalBlock(ELSIF_CONDITIONAL_BLOCK, this, ELSIF_CND_BLK);
  m_Command[i++] = (AbstractCommand*)new BC_PRMValue(PRMVALUE, pcgires);
  m_Command[i++] = (AbstractCommand*)new BC_Cgipath(CGIPATH, pcgires);
  m_Command[i++] = (AbstractCommand*)new BC_GetValue(this, pcgires);
  m_Command[i++] = (AbstractCommand*)new BC_AdjustQuotes(this);
  m_Command[i++] = (AbstractCommand*)new BC_ProcessExternFile(this, pcgires);
  m_Command[i++] = (AbstractCommand*)new BC_UCase();
  m_Command[i++] = (AbstractCommand*)new BC_LCase();
  m_Command[i++] = (AbstractCommand*)new BC_Trim();
  m_Command[i++] = (AbstractCommand*)new BC_SetVar(this);
  m_Command[i++] = (AbstractCommand*)new BC_GetVar(this);
  m_Command[i++] = (AbstractCommand*)new BC_GetCookie(this, pcgires);
  m_Command[i++] = (AbstractCommand*)new BC_UTime(this);
  m_Command[i++] = (AbstractCommand*)new BC_DMYTime(this);
  m_Command[i++] = (AbstractCommand*)new BC_Now(this);
  m_Command[i++] = (AbstractCommand*)new BC_FormatCurrency(this);
  m_Command[i++] = (AbstractCommand*)new BC_CheckForbiddenChars(this);
  m_Command[i++] = (AbstractCommand*)new BC_TraceUser(this, pcgires);
  m_Command[i++] = (AbstractCommand*)new BC_ODBCExecuteQuery(this, pcgires, ptqrodbc_manager);
  m_Command[i++] = (AbstractCommand*)new BC_TQRRecordFieldValue(this, ptqrcollection);
  m_Command[i++] = (AbstractCommand*)new BC_TQRRecordFieldValQuot(this, ptqrcollection);
  m_Command[i++] = (AbstractCommand*)new BC_CycleTQR(this, pcgires, ptqrcollection);
  m_Command[i++] = (AbstractCommand*)new BC_EndCycleTQR(this, ptqrcollection);
  m_Command[i++] = (AbstractCommand*)new BC_CycleFor(this, pcgires, ptqrcollection, ptqr_manager);
  m_Command[i++] = (AbstractCommand*)new BC_EndCycleFor(this, ptqrcollection);
  m_Command[i++] = (AbstractCommand*)new BC_ForIndex(this, ptqrcollection);
  m_Command[i++] = (AbstractCommand*)new BC_Exit(this);
  m_Command[i++] = (AbstractCommand*)new BC_TQRExist(this, ptqrcollection);
  m_Command[i++] = (AbstractCommand*)new BC_GetHide(this);
  m_Command[i++] = (AbstractCommand*)new BC_Crypt(this);
  m_Command[i++] = (AbstractCommand*)new BC_Decrypt(this);
  m_Command[i++] = (AbstractCommand*)new BC_RemoveTQR(this, ptqrcollection);
  m_Command[i++] = (AbstractCommand*)new BC_TQRStat(this, ptqrcollection);
  m_Command[i++] = (AbstractCommand*)new BC_Valid(this, pcgires, ptqrodbc_manager, ptqrcollection);
  m_Command[i++] = (AbstractCommand*)new BC_Return(this);
  m_Command[i++] = (AbstractCommand*)new BC_Flush(this, pcgires);
  m_Command[i++] = (AbstractCommand*)new BC_TQRFilt(this, ptqr_manager);
  m_Command[i++] = (AbstractCommand*)new BC_Recsel(this,  ptqrcollection, pcgires);
  m_Command[i++] = (AbstractCommand*)new BC_TQRMove(this, ptqrcollection);
  m_Command[i++] = (AbstractCommand*)new BC_Trans(this, pcgires, ptqrodbc_manager, ptqrcollection);
  m_Command[i++] = (AbstractCommand*)new BC_Left(this);
  m_Command[i++] = (AbstractCommand*)new BC_Right(this);
  m_Command[i++] = (AbstractCommand*)new BC_Strlen();
  m_Command[i++] = (AbstractCommand*)new BC_TQRStore(this, ptqr_manager);
  m_Command[i++] = (AbstractCommand*)new BC_TQRInsert(this, ptqrodbc_manager);
  m_Command[i++] = (AbstractCommand*)new BC_Pager(this, ptqr_manager);
  m_Command[i++] = (AbstractCommand*)new BC_Netprex(this);
  m_Command[i++] = (AbstractCommand*)new BC_Netgrab(this);
  m_Command[i++] = (AbstractCommand*)new BC_Rand(this);
  m_Command[i++] = (AbstractCommand*)new BC_MakeOp(this);
  m_Command[i++] = (AbstractCommand*)new BC_GetValueQuote(this, pcgires);
  m_Command[i++] = (AbstractCommand*)new BC_TQRSample(this, ptqr_manager);
  m_Command[i++] = (AbstractCommand*)new BC_NewConnection(this, pconnections);
  m_Command[i++] = (AbstractCommand*)new BC_SetConnection(this, ptqrodbc_manager, pconnections);
  m_Command[i++] = (AbstractCommand*)new BC_ResetConnection(this, ptqrodbc_manager, podbcconnection);
  m_Command[i++] = (AbstractCommand*)new BC_Array(this, ptqr_manager);
  m_Command[i++] = (AbstractCommand*)new BC_ArraySet(this, ptqr_manager);
  m_Command[i++] = (AbstractCommand*)new BC_ArrayGet(this, ptqr_manager);
  m_Command[i++] = (AbstractCommand*)new BC_RemoveChar(this);
  m_Command[i++] = (AbstractCommand*)new BC_Mid(this);
  m_Command[i++] = (AbstractCommand*)new BC_NumericalCmp(this);
  m_Command[i++] = (AbstractCommand*)new BC_Proc(this, pcgires);
  m_Command[i++] = (AbstractCommand*)new BC_Verinst(this);
  m_Command[i++] = (AbstractCommand*)new BC_CopyFile(this);
  m_Command[i++] = (AbstractCommand*)new BC_Currency(this);
  m_Command[i++] = (AbstractCommand*)new BC_Console(this, pcgires);
  m_Command[i++] = (AbstractCommand*)new BC_GetValueCast(this, pcgires);
  m_Command[i++] = (AbstractCommand*)new BC_GetPOSTbody(pcgires);
  m_Command[i++] = (AbstractCommand*)new BC_Mail(this);
  m_Command[i++] = (AbstractCommand*)new BC_Getenv(this);

//END CREATION BASE COMMANDS

  UpdateTotalCount();
  m_BaseCount = m_TotalCount;
}

//-----------------------------------------------------------------

void Parser::DestroyBaseCommands()
{
  for (int i=0; i < m_BaseCount; i++)
  {
    delete m_Command[i];
    m_Command[i] = NULL;
  }
}

//-----------------------------------------------------------------

void Parser::UpdateTotalCount()
{
  m_TotalCount = 0;
  for (int i=0; i < MAX_CMDS; i++)
  {
    if (m_Command[i] != NULL)
      m_TotalCount++;
  }
}

//-----------------------------------------------------------------

int Parser::PickPar(char* inputStr, int position, itxString* returnStr)
{
	char* strCursor;
	int		parLen = 0;
  int   parNum = 0;
	int		parsOffstArr[PARAM_NUMBER] = {0};
	int		i = 0;
  int   searchPars = 1;

  // nel caso che non vi sia nessun parametro
  if (inputStr == NULL || strlen(inputStr) == 0)
    return PARAM_NOT_FOUND;
   
	// inizializzazione del cursore al primo carattere della stringa di input
	strCursor = inputStr;
	position--;

	while (*strCursor)
	{
		if (*strCursor == BLINDER)
      searchPars = (searchPars ? 0 : 1);

		if (searchPars && *strCursor == ',')
    {
      //gestione del carattere 'virgola' con il backslash (alla C)
      if ((unsigned)strCursor <= (unsigned)inputStr || *(strCursor - 1) != '\\')
			  parsOffstArr[++parNum] = (unsigned int)strCursor - (unsigned int)inputStr + 1;
    }

		strCursor++;
	}

  //si copia il parametro sulla stringa di uscita
  if (parNum > position)
	{
		parLen = parsOffstArr[position + 1] - parsOffstArr[position];
    returnStr->Strncpy(inputStr + parsOffstArr[position], parLen - 1);
	}
	else if (parNum == position)
		*returnStr = &inputStr[parsOffstArr[position]];
	else
		return PARAM_NOT_FOUND;

  returnStr->Trim(); //toggle tedious blanks off...
  returnStr->SubstituteSubString("\\,", ","); //eliminazione del backslash per la virgola

	return PARAM_FOUND;
}

//-----------------------------------------------------------------

int Parser::PickInt(char* inputstr, int position, int* retval)
{
  if (position <= 0 || retval == NULL)
    return 0;
  
  itxString retstr;
  retstr = "";

  if (PickPar(inputstr, position, &retstr) == PARAM_NOT_FOUND)
    return 0;

  *retval = atoi(retstr.GetBuffer());

  return 1;
}

//-----------------------------------------------------------------

int Parser::PickDouble(char* inputstr, int position, double* retval)
{
  if (position <= 0 || retval == NULL)
    return 0;
  
  itxString retstr;
  retstr = "";

  if (PickPar(inputstr, position, &retstr) == PARAM_NOT_FOUND)
    return 0;

  *retval = atof(retstr.GetBuffer());

  return 1;
}

//-----------------------------------------------------------------

int Parser::PickString(char* inputstr, int position, char* retval, int* bufdim)
{
  if (position <= 0 || bufdim == NULL)
    return 0;
  
  itxString retstr;
  retstr = "";

  if (PickPar(inputstr, position, &retstr) == PARAM_NOT_FOUND)
    return 0;

  if (retval == NULL)
  {
    *bufdim = retstr.Len() + 1;
    return 1;
  }

  strcpy(retval, retstr.GetBuffer());

  return 1;
}

//-----------------------------------------------------------------

int Parser::PickListString(char* inputstr, itxString** params)
{
  int position = 0;
  
  while (PickPar(inputstr, position + 1, params[position]) != PARAM_NOT_FOUND)
    position++;

  return position;
}

/*-----------------------------------------------------------------
attivita'	:
    scandisce i dati letti in tplString e li scrive su outputString.
*/
int Parser::ProcTplData(char* tplString, itxString* outputString)
{
  BaseCommand*  basecmd = NULL;
	itxString     commandRetStr;	  //stringa restituita dal comando eseguito (l'allocazione spetta al comando)
	itxString     transfArg;				//argomento di una funzione trasformato dalla chiamata ricorsiva
	itxString     argStr;						//argomento grezzo del comando
	char*         cmdStart;
	char*         paux;
	int	          dataIsChar = 1; 	//flag che indica se il dato da restituire � un semplice carattere
  int           i = 0;
	int		        cmdOrd = 0;
	int		        retStrLen = 0;

  if (tplString == NULL)
    return 0;
  transfArg.SetGranularity(1024);

  try
  {
	  while (*tplString != 0 && m_ForceExit == 0 && m_ForceReturn == 0)
	  { 
		  dataIsChar=1; // Di default non mi aspetto un comando: assumo che il dato 
		                // sia un carattere da copiare in uscita cos� come �

      if (m_CurTpl->m_RecursionZero == 0)
      {
        paux = tplString;
        tplString = strchr(paux, m_StartCommandTag);
        
        if (m_CurTpl->MustExec())
        {
          if(tplString == NULL)
          {
            *outputString += paux;
            return 0; //Reached the end of template
          }
          else
          {
            *tplString = '\0';
            *outputString += paux;
            *tplString = m_StartCommandTag;
          }
        }
      }

		  // se incontro il carattere di segnalazione comando
		  if (*tplString == m_StartCommandTag)
		  {
			  // verifico la sintassi del comando e metto i puntatori su tplString
			  // all'inizio del nome del comando e dell'argomento (termino le stringhe ponendo
			  // a zero i caratteri di inizio e fine argomento); tplString viene spostato a fine comando    
        if (VerifyCmdSynt(&tplString, &cmdOrd, &argStr, &cmdStart) == 1)
			  {
          if (cmdOrd < m_BaseCount)
          {
            if (m_CurTpl->CheckStartBlock(((BaseCommand*)m_Command[cmdOrd])->m_Type, cmdStart))
		          dataIsChar = 0; // si disattiva il flag che segnala la presenza di un singolo carattere
          }

				  // esecuzione comando
				  if (m_CurTpl->MustExec())
				  {
        	  // chiamata ricorsiva: l'argomento del comando viene interpretato e restituito su
					  // transfArg; il file pointer (secondo argomento) viene passato NULL per permettere
					  // il riconoscimento del livello interno di ricorsione
            transfArg = "";
            m_CurTpl->m_RecursionZero++;
					  ProcTplData(argStr.GetBuffer(), &transfArg);
            m_CurTpl->m_RecursionZero--;

            try //Esecuzione
            {
              TimeTrace("PRIMA DEL COMANDO");              
					    DebugTrace2(DEFAULT, "Executing: %s(%s)\n", m_Command[cmdOrd]->GetName(), transfArg.GetBuffer());
              commandRetStr = m_Command[cmdOrd]->Execute(transfArg.GetBuffer());
              TimeTrace("DOPO IL COMANDO");

              m_Command[cmdOrd]->Deallocate();
            }
            catch(char* msg)
            {
					    DebugTrace2(IN_ERROR, "Propagated message: '%s'\n", msg);
              throw;
            }
            catch(...)
            {
              itxSystem sys;
					    DebugTrace2((cmdOrd < m_BaseCount ? IN_ERROR : IN_WARNING),
                          "Parser::ProcTplData: Unhandled Exception during '%s - %d' COMMAND\n",
                          m_Command[cmdOrd]->GetName(), sys.BSGetLastError());
              throw;
            }

            // si disattiva il flag che segnala la presenza di un singolo carattere
					  dataIsChar = 0;
				  }

          if (cmdOrd < m_BaseCount)
          {
            if (m_CurTpl->CheckEndBlock(((BaseCommand*)m_Command[cmdOrd])->m_Type, &tplString))
		          dataIsChar = 0; // si disattiva il flag che segnala la presenza di un singolo carattere
          }
			  }
		  }

		  if (m_CurTpl->MustExec())
		  {
			  // se il carattere letto non � il principio di un comando va scritto in output cos� come �
			  if (dataIsChar) //&& *tplString != '\x0A')
					*outputString += (char) *tplString;
			  // il dato non � un singolo carattere ma una stringa
			  // risultato dell'elaborazione di un comando
			  else
			  {
				  // si copia la stringa di ritorno del comando sullo stream di uscita
					if (!commandRetStr.IsNull() && !commandRetStr.IsEmpty())
            *outputString += commandRetStr;
				  commandRetStr = "";					
			  }
		  }

		  tplString++;
	  } // while (*tplString != 0)
  }
  catch (...)
  {
		DebugTrace2(IN_ERROR, "Parser::ProcTplData: recursion level %d.\n", m_CurTpl->m_RecursionZero);
		DebugTrace2(IN_ERROR, "Parser::ProcTplData: transfArg = '%s'\n", transfArg.GetBuffer());
		DebugTrace2(IN_ERROR, "Parser::ProcTplData: commandRetStr = '%s'\n", commandRetStr.GetBuffer());
    m_ForceExit = 1;
    throw;
  }
   
	return 0;
}


/*-----------------------------------------------------------------
NOME			:VerifyCmdSynt
attivita'	:verifica che i primi caratteri di *tplString siano quelli di uno dei
					 comandi e verifica la sintassi. cmdOrd identifica la posizione del comando
					 nell'array; argStr viene indirizzato su tplString all'inizio dell'argomento;
					 le stringhe vengono terminate ponendo a zero i caratteri di inizio e fine
					 argomento; tplString viene spostato alla fine del comando
return		:se *tplString inizia con un comando restituisce 1, se non sono verificate le 
					 condizioni sintattiche torna 0, se non � stato chiuso l'argomento torna -1
*/
int Parser::VerifyCmdSynt(char** tplString, int* cmdOrd, itxString* argStr, char** cmdStart)
{
	int       insideArg             = 0;
	int       argTagNestingLev      = -1;
	int       blinderCount          = 0;
	int       canBeCommand          = 1;
	int       cmdLen                = 0;
	int       argLen                = 0;
	int       i                     = 0;
	int       cmdArgNotClosed       = 0;
	char*     holdTplString         = NULL;
	char*     tmpArgP               = NULL;
	itxString strToEval;

  strToEval.Space(CMD_LEN+1);

	// memorizzo la posizione di partenza della stringa e
	// copio in un buffer un numero di caratteri della stringa pari al massimo numero
	// consentito per il nome di un comando (+ il terminator)
	holdTplString = *tplString;
  strToEval.Strncpy(*tplString + 1, CMD_LEN + 1);
  
	// prima condizione da soddisfare: devo trovare il carattere di inizio argomento '('
  char opentag[2] = {CMD_ARG_OPEN_TAG, '\0'};
  strToEval.Left(strToEval.FindSubString(opentag));
  cmdLen = strToEval.Len();

	tmpArgP = *tplString + cmdLen + 1;  // l'argomento punta alla prima parentesi
	insideArg=1;        // sono all'interno dell'argomento
	argTagNestingLev=0; // sono entrato nel livello zero di annidamento parentesi

  strToEval.RTrim();

	// se le condizioni fin qui richieste sono state soddisfatte
	if (canBeCommand)
	{
		// la funzione continua a essere scettica: il fatto che si sia trovato un carattere
		// di apertura argomento non la ha convinta
		canBeCommand = 0;
		// si scorre l'array di CommandStructure
		// e si comparano le etichette dei comandi con quella isolata ora
		for(*cmdOrd = 0; *cmdOrd < m_TotalCount; (*cmdOrd)++)
		{
			if(!strToEval.Compare(m_Command[*cmdOrd]->GetName()))
			{
				canBeCommand = 1; // condizione soddisfatta
				break;
			}
		}
	}

	// se le condizioni fin qui richieste sono state soddisfatte
	if (canBeCommand)
	{
		canBeCommand=0; // la funzione continua a essere scettica
		// sposto il puntatore all'inizio del presunto argomento
		*tplString = tmpArgP;
		// procedo fino ad incontrare il carattere di fine stringa o fino a quando non esco dall'argomento
//		while (tmpArgP && insideArg)
		while (**tplString != '\0' && insideArg)
    {
			// il puntatore si sposta al carattere successivo
			(*tplString)++;

			if (**tplString == BLINDER)
      {
        blinderCount++;
        continue;
      }

			// se si tratta di un carattere di apertura argomento
			if (**tplString == CMD_ARG_OPEN_TAG && !(blinderCount % 2))				
				argTagNestingLev++; // aumento il livello di annidamento parentesi			
			else if (**tplString==CMD_ARG_CLOSE_TAG && !(blinderCount % 2))  // se trovo il carattere di chiusura argomento
			{
				// se ho raggiunto il livello 0 ho esaurito l'argomento
				if (argTagNestingLev==0)
					insideArg=0;

				// diminuisco il livello di annidamento parentesi
				argTagNestingLev--;
			}
		}
		// se ho esaurito l'argomento prima di terminare la stringa tutto ok
		if (!insideArg)
			canBeCommand=1;
		else
			cmdArgNotClosed=1;
	}

	// se � definitivamente un comando
	if (canBeCommand)
	{
		tmpArgP++;  // si fa puntare l'argomento al carattere successivo, il primo del argomento
		argLen = (unsigned)*tplString - (unsigned)tmpArgP;
    if (argLen == 0)
      argStr->SetEmpty();
    else
      argStr->Strncpy(tmpArgP, argLen);

    //Se il comando e` seguito immediatamente da un CR, da un LF o da una coppia CRLF, lo skippo 
    if (*(*tplString + 1) == '\r')
      (*tplString)++;
    if (*(*tplString + 1) == '\n')
      (*tplString)++;
	}
	else
	{
		*cmdOrd = 0;  // non � un comando
		tmpArgP = NULL; // niente comando
		*tplString = holdTplString; // niente argomento: rimettiamo la stringa a posto
	}

	*cmdStart = holdTplString - 1;

	// se � un comando torna 1, se non lo � torna 0, se non � stato chiuso l'argomento torna -1
	return (canBeCommand - cmdArgNotClosed);
}

TemplateFile* Parser::LoadTemplate(itxString* tpldir, itxString* pname, char* ext)
{
  if (pname->IsEmpty() || pname->IsNull())
    return NULL;

  TemplateFile* tpl = NULL;

  itxString templ = *tpldir;

  try
  {
    if (templ.RightInstr(PATH_SEPARATOR) != templ.Len())
      templ += PATH_SEPARATOR;

    templ += *pname;
    tpl = new TemplateFile(templ.GetBuffer(), ext);
  
    if (tpl->Exist() == 0)
    {   
      DebugTrace2(IN_WARNING, "Template %s not found.\n", templ.GetBuffer());
      delete tpl;
      tpl = NULL;
    }
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "Unhandled exception in Parser::LoadTemplate while opening %s.\n", templ.GetBuffer());
    throw;
  }

  return tpl;
}

//-----------------------------------------------------------------

void Parser::Run(itxString* tpldir, itxString* pname, char* ext)
{
  TemplateFile* new_tpl       = NULL;
  TemplateFile* popped_tpl    = NULL;

	// interpretazione dei dati
  try
  {
    if ((new_tpl = LoadTemplate(tpldir, pname, ext)) != NULL)
    {
      m_TplStack.Push(new_tpl);
      m_CurTpl = new_tpl;

      g_DebugFile.m_CurrentTemplate = m_CurTpl->m_PathName;
      DebugTrace2(TEMPLATE, "Parser::Run: template is: %s\n", m_CurTpl->m_PathName.GetBuffer());

      try
      {
  	    ProcTplData(m_CurTpl->GetContentBuffer(), &(m_CurTpl->m_Output));
      }
      catch(...)
      {
		    DebugTrace2(IN_ERROR, "Unhandled exception during ProcTplData: now context is Run(itxString*, itxString*, char*), going on...\n");
      }

      //Return from template was forced, reset dedicated variable
      if(m_ForceReturn != 0)
        m_ForceReturn = 0;

      //Finish with this template
      m_TplStack.Pop((void**)&popped_tpl);
      
      if (popped_tpl != new_tpl)
        DebugTrace2(DEFAULT, "Template mismatch in Parser::Run : expected %s, found %s\n", 
                   new_tpl->m_PathName.GetBuffer(), popped_tpl->m_PathName.GetBuffer());

      //Reassign the currente template with the topmost of the stack
      if ((m_CurTpl = (TemplateFile*) m_TplStack.Top()) != NULL)
      {
        m_CurTpl->m_Output += new_tpl->m_Output;
        DebugTrace2(TEMPLATE, "Parser::Run: template is: %s\n", m_CurTpl->m_PathName.GetBuffer());
        delete new_tpl;
      } 
      else        
        m_CurTpl = new_tpl;

      g_DebugFile.m_CurrentTemplate = m_CurTpl->m_PathName;
    }
  }
  catch(...)
  {
		DebugTrace2(IN_ERROR, "Unhandled exception in Parser::Run. Current template is '%s'\n", pname->GetBuffer());
    throw;
  }
}

//-----------------------------------------------------------------

void Parser::Run(char* content)
{
  TemplateFile* new_tpl       = NULL;
  TemplateFile* popped_tpl    = NULL;

  if (content == NULL)
    return;
	
  // interpretazione dei dati
  try
  {
    if ((new_tpl = new TemplateFile(content, 0)) != NULL)
    {
      m_TplStack.Push(new_tpl);
      m_CurTpl = new_tpl;

      g_DebugFile.m_CurrentTemplate = m_CurTpl->m_PathName;
      DebugTrace2(TEMPLATE, "Parser::Run: current template is coming from a memory buffer: %s\n", 
                  m_CurTpl->m_PathName.GetBuffer());

      try
      {
	      ProcTplData(m_CurTpl->GetContentBuffer(), &(m_CurTpl->m_Output));
      }
      catch(...)
      {
		    DebugTrace2(IN_ERROR, "Unhandled exception during ProcTplData: now context is Run(char*), going on...\n");
      }

      //Return from template was forced, reset dedicated variable
      if(m_ForceReturn != 0)
        m_ForceReturn = 0;

      //Finish with this template
      m_TplStack.Pop((void**)&popped_tpl);
      
      if (popped_tpl != new_tpl)
        DebugTrace2(DEFAULT, "Template mismatch in Parser::Run : expected %s, found %s\n", 
                   new_tpl->m_PathName.GetBuffer(), popped_tpl->m_PathName.GetBuffer());

      //Reassign the currente template with the topmost of the stack
      if ((m_CurTpl = (TemplateFile*) m_TplStack.Top()) != NULL)
      {
        m_CurTpl->m_Output += new_tpl->m_Output;
        DebugTrace2(TEMPLATE, "Parser::Run: template is: %s\n", m_CurTpl->m_PathName.GetBuffer());
        delete new_tpl;
      } 
      else        
        m_CurTpl = new_tpl;

      g_DebugFile.m_CurrentTemplate = m_CurTpl->m_PathName;
    }
  }
  catch(...)
  {
		DebugTrace2(IN_ERROR, "Unhandled exception in Parser::Run. Current template is '%p'\n", content);
    throw;
  }
}

//-----------------------------------------------------------------

void Parser::SetValidBlock(int val)
{
  m_CurTpl->m_ValidBlock[m_CurTpl->m_CndLevel] = val;
}

//-----------------------------------------------------------------

void Parser::SetCycleBlock(int val)
{
  m_CurTpl->m_ReadCycle[m_CurTpl->m_CycLevel] = val;
}

//-----------------------------------------------------------------

int Parser::GetCycleBlock(int offset)
{
  return m_CurTpl->m_ReadCycle[m_CurTpl->m_CycLevel + offset];
}

//-----------------------------------------------------------------

void Parser::AddVar(char* varName, char* varValue)
{
  int i;
	for (i=0; i < m_TplVars.idx; i++)
	{
		if (!m_TplVars.names[i].IsNull())
		{
			if (strcmp(m_TplVars.names[i].GetBuffer(), varName) == 0)
				break;
		}
	}

	if (i == m_TplVars.idx)
	{
		if (i < TPL_VARS_NUM)
		{
			m_TplVars.names[i]  = varName;
			m_TplVars.values[i] = varValue;
			m_TplVars.idx++;
		}
		else
			return;
	}
	else
		m_TplVars.values[i] = varValue ;
}
















