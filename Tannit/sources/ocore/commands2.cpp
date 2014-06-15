/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: commands2.cpp,v $
 * $Revision: 1.33 $  
 * $Author: massimo $
 * $Date: 2002-06-25 18:25:15+02 $
 *
 * More command definition
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               aitecsa s.r.l. via baglivi 3 00161 roma italy
 *               aitecsa@aitecsa.it
 */

#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <time.h>

#include "itxtypes.h"
#include "commands.h"
#include "templatefile.h"




/*----------------------------------------------------------------------------
                              Rand
  ----------------------------------------------------------------------------*/

char* BC_Rand::Execute(char* istr)
{
	int maxvalue;
  int seed = 0;

  m_Output.SetEmpty();

	if(m_pParser->PickInt(istr, 1, &maxvalue) == PARAM_NOT_FOUND)
    return "";

	if(m_pParser->PickInt(istr, 2, &seed)	== PARAM_FOUND)
    srand((unsigned)seed);
  else
    srand((unsigned)time(NULL));

  m_Output.SetInt(rand() % maxvalue);

  return m_Output.GetBuffer();
}

/*----------------------------------------------------------------------------
                              BC_MakeOp
  ----------------------------------------------------------------------------*/

#define MAKEOPERATION(op, dleft, dright, resop)  \
  switch(op)  \
  { \
    case '%': \
      resop = ((int)dleft) % ((int)dright); \
    break;  \
    case '*': \
      resop = (dleft) * (dright); \
    break;  \
    case '/': \
      if (dright == 0.) \
        {throw; break;}  \
      resop = (dleft) / (dright); \
    break;  \
    case '+': \
      resop = (dleft) + (dright); \
    break;  \
    case '-': \
      resop = (dleft) - (dright); \
    break;  \
  }

bool BC_MakeOp::MakeMulDivs(itxListPtr* ops, itxListPtr* terms, char op)
{
  char*   aux;
  double* pdleft;
  double* pdright;
  double  resop;

  //Execute mult-divs
  ops->GetHead();
  aux = (char*)ops->GetNext();
  int i = 1;
  while (aux)
  {
    if (*aux == op)
    {
      pdleft  = (double*)terms->GetElement(i - 1);
      pdright = (double*)terms->GetElement(i);
//      MAKEOPERATION(op, *pdleft, *pdright, resop);
  switch(op)  
  { 
    case '%': 
      resop = ((int)*pdleft) % ((int)*pdright); 
    break;  
    case '*': 
      resop = (*pdleft) * (*pdright); 
    break;  
    case '/':
      if (*pdright == 0.)
      {
        DebugTrace2(IN_WARNING, "Divide by zero\n");
        m_pParser->m_ForceExit = 1;
      }
      else
        resop = (*pdleft) / (*pdright);
    break;  
    case '+': 
      resop = (*pdleft) + (*pdright); 
    break;  
    case '-': 
      resop = (*pdleft) - (*pdright); 
    break;  
  }

      ops->Remove(i);
      *pdleft = resop;
      terms->Remove(i);
      
      return true;
    }

    i++;
    aux = (char*)ops->GetNext();
  }

  return false;
}

char* BC_MakeOp::Execute(char* istr)
{
  itxListPtr    listop;
  itxListPtr    listnums;
  itxString     appo;
  char*         aux;
  char*         begterm = NULL;
  char          termstr[64];
  int           opcnt;
  int           i;
  int           j;
  int           k;
  unsigned int  termlen;
	itxString     input;
  double*       terms;
  double        result = 0.;

  m_Output.SetEmpty();

	if(m_pParser->PickPar(istr, 1, &input) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

  input.Trim();
  i = input.GetAt(0);
  if (i != '+' && i != '-')
  {
    appo = "+";
    appo += input;
  }
  else
    appo = input;

  aux = appo.GetBuffer();
    
  //Build the stack of (pointers to) operators in the input string expression
  opcnt = 0;
  while ((aux = strpbrk(aux, "+/-*%")) != NULL)
  {
    listop.Append(aux);
    aux++;
    opcnt++;
  }

  try
  {
    //Extract numbers
    terms = new double[opcnt + 1];
    begterm = appo.GetBuffer();
    listop.GetHead();
    aux  = (char*)listop.GetNext();
    i = 0;
    while (begterm && *begterm != 0)
    {
      if (aux)
      {
        termlen = (unsigned)aux - (unsigned)begterm;

        //must make a strong trim... (to interpret things like "-   1")
        j = 0;
        k = 0;
        while(j<termlen)
        {
          if (begterm[j] != ' ')
            termstr[k++] = begterm[j];
          j++;
        }
        termstr[k] = 0;
      }
      else
        strcpy(termstr, begterm);
      
      terms[i] = atof(termstr);
      listnums.Append(&terms[i]);
      begterm = (aux ? (++aux) : NULL);
      aux = (char*)listop.GetNext();
      i++;
    }

    //Mults, Divs and Mods
    while (MakeMulDivs(&listop, &listnums, '/'));
    while (MakeMulDivs(&listop, &listnums, '*'));
    while (MakeMulDivs(&listop, &listnums, '%'));

    //Sums  
    aux  = (char*)listop.GetHead();
    double* pd  = (double*)listnums.GetHead();
    while (aux && pd)
    {
      result += (*aux == '-' ? -1*(*pd) : *pd);
      aux = (char*)listop.GetNext();
      pd = (double*)listnums.GetNext();
    }
  }
  catch(...)
  {
    DebugTrace2(IN_WARNING, "Malformed expression or divide by zero\n");
  }

  delete [] terms;

  char resstr[64] = {0};
	if(m_pParser->PickPar(istr, 2, &appo) != PARAM_NOT_FOUND)
    sprintf(resstr, appo.GetBuffer(), result);
  else
    sprintf(resstr, "%f", result);

  m_Output = resstr;

  return m_Output.GetBuffer();
}

/*----------------------------------------------------------------------------
                              NewConnection
  ----------------------------------------------------------------------------*/

char* BC_NewConnection::Execute(char* istr)
{
	itxString name;
	itxString dsn;
	itxString uid;
	itxString pwd;
	itxString failuremsg;
  bool skipFailure = false;

  m_Output.SetEmpty();

	itxSQLConnection* newconn = NULL;
  
  m_pParser->PickPar(istr, 1, &name);
	m_pParser->PickPar(istr, 2, &dsn);
	m_pParser->PickPar(istr, 3, &uid);
	m_pParser->PickPar(istr, 4, &pwd);
	if(m_pParser->PickPar(istr, 5, &failuremsg) != PARAM_NOT_FOUND)
  {
    skipFailure = true;
  }

  try
  {
    newconn = new itxSQLConnection();
    
    if (newconn->Create(name.GetBuffer(), dsn.GetBuffer(), uid.GetBuffer(), pwd.GetBuffer()))
      m_pconnections->Put(newconn);
    else
    {
      if(skipFailure )
        m_Output=failuremsg;
      else
        DebugTrace2(IN_WARNING, "Unable to open new connection.\n");
    }  

  }
  catch(...)
  {
    DebugTrace2(IN_WARNING, "Unable to open new connection.\n");
  }  

  return m_Output.GetBuffer();
}

/*----------------------------------------------------------------------------
                              SetConnection
  ----------------------------------------------------------------------------*/

char* BC_SetConnection::Execute(char* istr)
{
	itxString name;
	itxSQLConnection* setconn = NULL;
  
  m_pParser->PickPar(istr, 1, &name);

  try
  {
    setconn = m_pconnections->Get(name.GetBuffer());
    m_pTQRODBCManager->SetConnect(setconn);
  }
  catch(...)
  {
    DebugTrace2(IN_WARNING, "Unable to open set connection.\n");
  }  

  return NULL;
}

char* BC_ResetConnection::Execute(char* istr)
{
  m_pTQRODBCManager->SetConnect(m_podbcconnection);
  return NULL;
}

/*----------------------------------------------------------------------------
                              Array
  ----------------------------------------------------------------------------*/

char* BC_Array::Execute(char* istr)
{
	itxString arrayname;
  int ncols, nrows;
  
	if(m_pParser->PickPar(istr, 1, &arrayname) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickInt(istr, 2, &nrows) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;  
	if(m_pParser->PickInt(istr, 3, &ncols) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;  

  try
  {
    TQR* qres = m_pTQRManager->CreateTQR(arrayname.GetBuffer(), ncols);
    for (int irow = 0; irow < nrows; irow++)
    {
      qres->AddTail();
    }
  }
  catch(...)
  {
    DebugTrace2(IN_COMMAND, "Unable to create array.\n");
  }

  return 0;
};

/*----------------------------------------------------------------------------
                              ArraySet
  ----------------------------------------------------------------------------*/

char* BC_ArraySet::Execute(char* istr)
{
	itxString arrayname;
  int irow;
  itxString value;
  itxString icol;
  
	if(m_pParser->PickPar(istr, 1, &arrayname) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickInt(istr, 2, &irow) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;  
	if(m_pParser->PickPar(istr, 3, &icol) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;  
	if(m_pParser->PickPar(istr, 4, &value) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

  try
  {
    itxString colname;

    colname = "f";
    colname += icol.GetBuffer();

    TQR* qres = m_pTQRManager->Get(arrayname.GetBuffer());
    qres->MoveTo(irow);
    qres->SetCurrentRecordField(colname.GetBuffer(), value.GetBuffer());
  }
  catch(...)
  {
    DebugTrace2(IN_COMMAND, "Unable to set array value.\n");
  }

  return 0;
};

/*----------------------------------------------------------------------------
                              ArrayGet
  ----------------------------------------------------------------------------*/

char* BC_ArrayGet::Execute(char* istr)
{
	itxString arrayname;
  int irow;
  itxString icol;
  
  m_Output.SetEmpty();

	if(m_pParser->PickPar(istr, 1, &arrayname) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickInt(istr, 2, &irow) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;  
	if(m_pParser->PickPar(istr, 3, &icol) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;  

  try
  {
    itxString colname;

    colname = "f";
    colname += icol.GetBuffer();

    TQR* qres = m_pTQRManager->Get(arrayname.GetBuffer());
    qres->MoveTo(irow);
    m_Output =qres->GetCurrentRecordField(colname.GetBuffer());
  }
  catch(...)
  {
    DebugTrace2(IN_COMMAND, "Unable to get array value.\n");
  }

  return m_Output.GetBuffer();
};

/*----------------------------------------------------------------------------
                              RemoveChar
  ----------------------------------------------------------------------------*/

char* BC_RemoveChar::Execute(char* istr)
{
  itxString string;
  itxString rchar;
  itxString schar;

  itxString a1;
  itxString a2;

  m_Output.SetEmpty();

	if(m_pParser->PickPar(istr, 1, &string) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickPar(istr, 2, &rchar) == PARAM_NOT_FOUND)
    rchar = "\"";
	if(m_pParser->PickPar(istr, 3, &schar) == PARAM_NOT_FOUND)
    schar = "";

  a1 = rchar;
  a2 = rchar;

  if (a1.Left(1).Compare("$") == 0 && a2.Right(1).Compare("$") == 0)
    rchar.InBlinder('$');

  a1 = schar;
  a2 = schar;

  if (a1.Left(1).Compare("\"") == 0 && a2.Right(1).Compare("\"") == 0)
    schar.InBlinder('"');

  try
  {
    string.SubstituteSubString(rchar.GetBuffer(), schar.GetBuffer());
  }
  catch(...)
  {
    DebugTrace2(IN_COMMAND, "Unable to remove char from string %s\n", string.GetBuffer());
  }

  m_Output = string;
  return m_Output.GetBuffer();
};


/*----------------------------------------------------------------------------
                              Verinst:
                              Comando da levare una volta capito cosa fa..
  ----------------------------------------------------------------------------*/

char* BC_Verinst::Execute(char* istr)
{
  itxString user;
  itxString code;

	if(m_pParser->PickPar(istr, 1, &user) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickPar(istr, 2, &code) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

  if (user.Compare("GALCO_Web_Ges") == 0) 
    if (code.Compare("0644254002") == 0)
      return 0;
  else
    if (code.Compare("3357539405") == 0)
      return 0;
  
  m_pParser->m_ForceExit = 1;

  return 0;
};


/*----------------------------------------------------------------------------
                              CopyFile
  ----------------------------------------------------------------------------*/

char* BC_CopyFile::Execute(char* istr)
{
  itxString from;
  itxString to;
  FILE *fdfrom;
  FILE *fdto;

	if(m_pParser->PickPar(istr, 1, &from) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;
	if(m_pParser->PickPar(istr, 2, &to) == PARAM_NOT_FOUND)
    return PARAM_NOT_FOUND_MSG;

  fdfrom = fopen(from.GetBuffer(),"rb");

  if (!fdfrom)
    return NULL;

  // Open R/W by owner, R by everyone else

  fdto = fopen(to.GetBuffer(),"wb");

  if (fdto)
  {
    int bufsiz = -1;

    // Use the largest buffer we can get
    for (bufsiz = 0x4000; bufsiz >= 128; bufsiz >>= 1)
    {
      char *buffer;

      buffer = (char *) malloc(bufsiz);

      if (buffer)
      {
        while (1)
        {
          int n;
          n = fread(buffer, sizeof(char), bufsiz, fdfrom);

          // the error messages are not handled but three cases are ready for future developing ...
          if(ferror(fdfrom)!=0)     // if error
          {
            break;
          }
          if (n != fwrite(buffer, sizeof(char), (unsigned) n, fdto)) // write error
          {
            break;
          }
          if (feof(fdfrom)!=0)      // if end of file
          {
            break;
          }
        }
        free(buffer);
        break;
      }
    }

  }

  fclose(fdto);
  fclose(fdfrom);

  return NULL;
};

/*----------------------------------------------------------------------------
                              Console
  ----------------------------------------------------------------------------*/

char* BC_Console::Execute(char* istr)
{
	char inbuf[1024];

  try
  {
    //Can't accept web requests for this command...sorry.
    if (!(m_pCGIRes->cgiServerSoftware == NULL || 
          *(m_pCGIRes->cgiServerSoftware) == 0))
      return NULL;
        
    m_pCGIRes->Flush("\n\nEntering Tannit console mode...\n\nTNT>");

    bool goon = true;
    itxString command;
    command.SetEmpty();

    while (goon && fgets(inbuf, 1000, m_pCGIRes->cgiIn))
    {
      inbuf[strlen(inbuf)-1] = 0;
      command = inbuf;

      if (strcmp(inbuf, "?") == 0 || strcmp(inbuf, "help") == 0)
      {
        itxString cmdlist;
        char      auxstr[30];

        cmdlist.SetEmpty();

        for (int ncmd = 0, i = 1; ncmd < m_pParser->m_TotalCount; ncmd++, i++)
        {
          memset(auxstr, ' ', 30);
          cmdlist += m_pParser->m_Command[ncmd]->GetName();
          auxstr[30 - strlen(m_pParser->m_Command[ncmd]->GetName())] = 0;

          if ((i%3) == 0)
          {
            i = 0;
            cmdlist += "\n";
          }
          else
            cmdlist += auxstr;
        }

        m_pCGIRes->Flush(cmdlist.GetBuffer());
      }
      else if (strcmp(inbuf, "exit") == 0)
      { 
        m_pCGIRes->Flush("Exiting Tannit console mode.\n");
        goon = false;
      }
      else
      {
        command += "*flush()";
        m_pParser->Run(command.GetBuffer());
      }

      if (goon)
        printf("\nTNT>");
    }
  }
  catch(...)
  {
    DebugTrace2(IN_COMMAND, "Console generic exception\n");
  }
  
  return m_Output.GetBuffer();
}

/*----------------------------------------------------------------------------
                              GetPOSTbody
  ----------------------------------------------------------------------------*/

char* BC_GetPOSTbody::Execute(char* istr)
{
  return m_pCGIRes->m_POSTbody.GetBuffer();
}

/*----------------------------------------------------------------------------
                              Mail
  ----------------------------------------------------------------------------*/

char* BC_Mail::Execute(char* istr)
{
  itxString smtp_server;
  itxString domain;
  itxString sender;
  itxString subject;
  itxString message;
  itxString attachments;
  itxString recipient;
  int       result = -1;

	m_pParser->PickPar(istr, 1, &smtp_server);
  m_pParser->PickPar(istr, 2, &domain);
  m_pParser->PickPar(istr, 3, &sender);
  m_pParser->PickPar(istr, 4, &subject);
  m_pParser->PickPar(istr, 5, &message);
  m_pParser->PickPar(istr, 6, &attachments);

  sender.PurgeSubString("\n");
  sender.PurgeSubString("\r");
  attachments.PurgeSubString("\n");
  attachments.PurgeSubString("\r");
  subject.PurgeSubString("\n");
  subject.PurgeSubString("\r");

  smtp_server.Trim();
  domain.Trim();
  sender.Trim();
  subject.Trim();
  message.Trim();
  attachments.Trim();

  if (smtp_server.Len() == 0)
    return "";

  itxSmtp smtp(smtp_server.GetBuffer(), domain.GetBuffer(), sender.GetBuffer());
  
  try
  {
    int i = 7;
    while (m_pParser->PickPar(istr, i, &recipient) == PARAM_FOUND)
    {
      recipient.PurgeSubString("\n");
      recipient.PurgeSubString("\r");
      recipient.Trim();
      smtp.AddRcpt(recipient.GetBuffer());
      recipient.SetEmpty();
      i++;
    }
  
    if (i == 7)
      return "";

    smtp.SetSubject(subject.GetBuffer());
    smtp.SetBody(message.GetBuffer());

    // Attachments
    i = 0;
    itxString attfile;
    while (i >= 0)
    {
      attfile.SetEmpty();
      attachments.GetToken('|', i++, &attfile);
      attfile.Trim();
      if (attfile.Len() == 0)
        i = -1;
      else
        smtp.AddAttachment(attfile.GetBuffer());
    }

    if ((result = smtp.Mail()) >= ITXM_CRITICAL_ERROR)
      DebugTrace2(IN_COMMAND, "Unable to send mail - SMTP error code: %d.\n", result);
  }
  catch(...)
  {
    DebugTrace2(IN_WARNING, "Unable to send mail - SMTP error %d.\n", result);
  }  

  return "";
}


