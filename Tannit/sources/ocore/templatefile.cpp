/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/*****************************************************************************
                                AITECSA S.R.L. 
 -----------------------------------------------------------------------------
 - FILENAME    : templatefile.cpp
 - TAB         : 2 spaces
 -----------------------------------------------------------------------------
 - DESCRIPTION :  
    Template file wrapper object impl. 
 *****************************************************************************/

#include "defines.h"

#include "auxfile.h"
#include "templatefile.h"
#include "itxlib.h"


//-------------------------------------------------------------------------------

void TemplateFile::InitMembers()
{
	for (m_CndLevel=0; m_CndLevel < CYC_NESTING_DEPTH; m_CndLevel++)
  {
    m_ValidBlock[m_CndLevel] = EXEC_BLK;
    m_ReadCycle[m_CndLevel] = 1;
  }

	m_CndLevel = 0;
	m_CycLevel = 0;

  m_Tpl.SetEmpty();
  m_ContentLen    = 0;
  m_RecursionZero = 0;
  m_Output.SetGranularity(100000);
  m_Output.SetEmpty();
}

//-------------------------------------------------------------------------------

TemplateFile::TemplateFile(char* tpl_path_name, char* ext)
{
  InitMembers();

  m_PathName = tpl_path_name;
  m_TplFound = Read(ext);
}

//-------------------------------------------------------------------------------

TemplateFile::TemplateFile(itxString* tpl_path_name, char* ext)
{
  InitMembers();

  m_PathName = *tpl_path_name;
  m_TplFound = Read(ext);
}

//-------------------------------------------------------------------------------

TemplateFile::TemplateFile(char* tpl_content, int finto)
{
  InitMembers();

  char appo[20];
  sprintf(appo, "%p", tpl_content);
  m_PathName      = appo;

  m_ContentLen    = strlen(tpl_content);
  m_TplFound      = 1;

  //Purge cpp-style comment lines and sets m_Tpl to the purged buffer
  PurgeComments(tpl_content);
}

//-------------------------------------------------------------------------------

/*
attivita'	:apre il file individuato, alloca tplString e vi scrive i dati del file;
*/
int TemplateFile::Read(char* ext)
{
	long      numread = 0;
	char*     dataBuffer = NULL;
	FILE*     fTemplate;

  if (m_PathName.IsNull() || m_PathName.IsEmpty())
    return 0;

  //N.B. questa linea di codice lega 
  //la classe TemplateFile all'estensione 'htm' dei template.
	m_PathName += ext;

  //Apertura e controllo del template da interpretare
  if ((fTemplate = fopen(m_PathName.GetBuffer(),"rb")) == 0)
		return 0;

  //Valutazione della Content-length
  fseek(fTemplate, 0, SEEK_SET);
  m_ContentLen = ftell(fTemplate);
  fseek(fTemplate, 0, SEEK_END);
  m_ContentLen = ftell(fTemplate) - m_ContentLen;
  rewind(fTemplate);

  if ((dataBuffer = (char*)calloc(m_ContentLen + 1, sizeof(char))) == NULL) 
    return 0;

  //lettura template
  if ((numread = fread(dataBuffer, sizeof(char), m_ContentLen, fTemplate)) < m_ContentLen)
    return 0;
  
	dataBuffer[m_ContentLen] = 0; // sostituzione del carattere EOF con 0

  //Purge cpp-style comment lines and sets m_Tpl to the purged buffer
  PurgeComments(dataBuffer); 

  free(dataBuffer);
  fclose(fTemplate);

  //Purge LF
//  m_Tpl.PurgeSubString("\n");

	return 1;
}

//-------------------------------------------------------------------------------

void TemplateFile::PurgeComments(char* cursor)
{
  try
  {
    char* slash1 = cursor;

    slash1 = strstr(cursor, TNT_COMMENT);

    while(slash1 != NULL)
    {
      *slash1++ = 0;
      m_Tpl += cursor;
      cursor = slash1;

      if ((cursor = strchr(cursor, CR)) != NULL)
      {
        slash1 = (++cursor);   
        slash1 = strstr(cursor, TNT_COMMENT);
      }
      else
        slash1 = NULL;
    }

    m_Tpl += cursor;
  }
  catch(...)
  {
    DebugTrace2(IN_ERROR, "Unhandled Exception in TemplateFile::PurgeComments\n");
  }
}

//-------------------------------------------------------------------------------
int TemplateFile::GetContent(char* tplstr)
{
  try
  {
    if (tplstr)
      strcpy(tplstr, GetContentBuffer());
  }
  catch(...)
  {
    return 0;
  }
    
  return 1;
}

//-------------------------------------------------------------------------------
itxString* TemplateFile::GetContent(itxString* tplstr)
{
  try
  {
    if (tplstr)
      *tplstr = GetContentBuffer();
  }
  catch(...)
  {
    return NULL;
  }
    
  return tplstr;
}

//-----------------------------------------------------------------

int TemplateFile::CheckStartBlock(int type, char* cmdStart)
{
  int ret = 0;

	if (type & START_CND_BLK)
	{
		m_CndLevel++;

		// si ereditano le colpe dei padri
		if (m_ValidBlock[m_CndLevel-1] != EXEC_BLK)
      m_ValidBlock[m_CndLevel] = m_ValidBlock[m_CndLevel-1];
    
    ret = 1;
	}
	else if (type & ELSE_CND_BLK)
	{
		if (m_ValidBlock[m_CndLevel] == DONT_EXEC_BLK)
			m_ValidBlock[m_CndLevel] = EXEC_BLK;
		else if (m_ValidBlock[m_CndLevel] == EXEC_BLK)
			m_ValidBlock[m_CndLevel] = DONT_EXEC_BLK;

		// si ereditano le colpe dei padri
		if (m_ValidBlock[m_CndLevel-1] != EXEC_BLK)
      m_ValidBlock[m_CndLevel] = m_ValidBlock[m_CndLevel-1];

    ret = 1;
	}
	else if (type & ELSIF_CND_BLK)
	{
		if (m_ValidBlock[m_CndLevel] == DONT_EXEC_BLK)
			m_ValidBlock[m_CndLevel] = EXEC_BLK;
		else if (m_ValidBlock[m_CndLevel] == EXEC_BLK)
			m_ValidBlock[m_CndLevel] = STOP_EXEC_BLK;

		// si ereditano le colpe dei padri
		if (m_ValidBlock[m_CndLevel-1] != EXEC_BLK)
      m_ValidBlock[m_CndLevel] = m_ValidBlock[m_CndLevel-1];

    ret = 1;
	}

	if (m_ValidBlock[m_CndLevel] == EXEC_BLK && (type & START_CYCLE))
	{
		m_CycLevel++;
    m_CycleStk.Push(cmdStart);
    ret = 1;
	}
  
  return ret;
}

//-----------------------------------------------------------------

int TemplateFile::CheckEndBlock(int type, char** tplString)
{
  int ret = 0;

	if ((m_ValidBlock[m_CndLevel] == EXEC_BLK) && (type & END_CYCLE))
	{
		if (m_ReadCycle[m_CycLevel])
			m_CycleStk.Pop((void**)tplString);
		else
		{
			m_CycleStk.Pop(NULL);
			m_ReadCycle[m_CycLevel] = 1;
		}
		m_CycLevel--;

    ret = 1;
	}

	if	(type & END_CND_BLK)
	{
		m_ValidBlock[m_CndLevel] = EXEC_BLK;
		m_CndLevel--;

    ret = 1;
	}
  
  return ret;
}
