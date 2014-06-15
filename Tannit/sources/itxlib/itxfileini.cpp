/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxfileini.cpp,v $
 * $Revision: 1.2 $  
 * $Author: massimo $
 * $Date: 2002-06-07 11:08:26+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */

#include "itxfileini.h"


bool itxFileINI::Read(char* inifile)
{  
	FILE*   initFile;
	char    fileLine[INI_LINE_LEN];
	char*   token = NULL;
  char*   retVal = NULL;

  m_FileName = inifile;
  m_NumParams = 0;
  
  try
  {
    if (m_FileName.IsEmpty())
      return false;

	  if((initFile = fopen(m_FileName.GetBuffer(), "r")) == NULL) 
      return false;

    if (fseek(initFile, 0, SEEK_SET))
      return false;

	  while (fgets(fileLine, INI_LINE_LEN, initFile) != 0)
	  {
		  if (fileLine[0] == '#' || strstr(fileLine, "=") == NULL)
			  continue;

		  token = strtok(fileLine, "=");
      if (token != NULL)
      {
        m_Names[m_NumParams] = token;
        m_Names[m_NumParams].Trim();

			  token = strtok(NULL, "\n");
        if (token != NULL)
        {
          m_Values[m_NumParams] = token;
          m_Values[m_NumParams].Trim();
        }
        else
          m_Values[m_NumParams].SetEmpty();

        m_NumParams++;
      }
	  }

    fclose(initFile);
  }
  catch(...)
  {
    fprintf(stdout, "Exception in itxFileINI::Read\n");
    return false;
  }

  return true;
}

bool itxFileINI::GetINIValue(char* param_name, itxString* param_value)
{
  int    i = 0;

  while (i < m_NumParams && m_Names[i].Compare(param_name))
    i++; 

  if (i < m_NumParams)
  {
    *param_value = m_Values[i];
    return true;
  }

  return false;
}



