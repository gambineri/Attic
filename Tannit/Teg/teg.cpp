/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: teg.cpp,v $
 * $Revision: 1.0 $  
 * $Author: fabio $
 * $Date: 2001-01-09 19:11:08+01 $
 *
 * Tannit Extension Manager Command Implementation
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */

#include <windows.h>
#include <stdio.h>

#include "teg.h"


BOOL APIENTRY DllMain(HANDLE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
{
  return TRUE;
}


//[[BEGIN_GENERATED_CODE]]

teg_header    cmd1("teg_header");
teg_load      cmd2("teg_load");

TNTAPI*    g_pTNTAPI; //tnt hook object pointer

void __declspec(dllexport) TannitHandshake(AbstractCommand** ppCommands, TNTAPI* pTNTAPI)
{
  ppCommands[0] = (AbstractCommand*)&cmd1;
  ppCommands[1] = (AbstractCommand*)&cmd2;
  g_pTNTAPI = pTNTAPI;
}

//[[END_GENERATED_CODE]]

char* teg_header::Execute(char* inputstr)
{
  char* modulename = NULL;
  int bufdim;

  if (TNTPickString(inputstr, 1, NULL, &bufdim))
  {
    modulename = (char*)calloc(bufdim, 1);
    TNTPickString(inputstr, 1, modulename, &bufdim);
  }
  
  m_Output.SetEmpty();

  m_Output =  "// Tannit Extension Manager 1.0\r\n"; 
  m_Output += "// \r\n";
  m_Output += "// aitecsa\r\n";
  m_Output += "// ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.\r\n";
  m_Output += "//                 � aitecsa s.r.l. via baglivi 3 00161 roma italy\r\n"; 
  m_Output += "//                 aitecsa@aitecsa.it\r\n";
  m_Output += "//                 all rights reserved\r\n";
  m_Output += "// \r\n";
  m_Output += "// \r\n";
  m_Output += "// This software is automatically generated using TEG 1.0\r\n";  
  m_Output += "// \r\n";
  m_Output += "// Module Extension: ";
  m_Output += modulename;
  m_Output += "\r\n";
  m_Output += "// \r\n";

  free(modulename); 
  return m_Output.GetBuffer();
}

char* teg_load::Execute(char* inputstr)
{
  char* modulename = NULL;
  char* commandlist = NULL;
  FILE* fp = NULL; 
  int bufdim;

  if (TNTPickString(inputstr, 1, NULL, &bufdim))
  {
    modulename = (char*)calloc(bufdim, 1);
    TNTPickString(inputstr, 1, modulename, &bufdim);
  }

  if (TNTPickString(inputstr, 2, NULL, &bufdim))
  {
    commandlist = (char*)calloc(bufdim, 1);
    TNTPickString(inputstr, 2, commandlist, &bufdim);
  }
  
  if (!TNT_TQRExist(commandlist))
  {
    itxString cmdstring;

    cmdstring.SetEmpty();
    int icmd = 1;
    char cmdname[64];
    if ((fp = fopen(modulename, "r")) != NULL)
    {
      while (fscanf(fp, "%s", &cmdname[0]) != EOF)
      {
        cmdstring += cmdname;
        cmdstring += '#';
        icmd++;
      }
    
      TNTLoadDataBuffer(commandlist, 2, '#', '$', cmdstring.GetBuffer());    
      fclose(fp);
    }
  }
  else
    TNTRewind(commandlist);

  return 0;
}

