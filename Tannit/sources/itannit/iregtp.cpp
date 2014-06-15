/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "itxtypes.h"
#include "iregtp.h"

extern TPAS400 tpAS400[NUM_TPAS400];

void itxTP::Default()
{
  for (int i = 0; i < NUM_TPAS400; i++)
  {
    m_tpas400[i].id = (unsigned short) tpAS400[i].id;
    m_tpas400[i].n_fields = tpAS400[i].n_fields;
    memcpy(m_tpas400[i].name, tpAS400[i].name, TPAS400_NAME_LEN);
  }
}

bool itxTP::Load(char* filename)
{
  FILE* f = NULL;
  int itp = 1;
  char appo[256];
  
  if ((f = fopen(filename, "r")) == NULL)
  {
    Default();
    return TRUE;
  }

  while (!feof(f))
  {
    fscanf(f, "%s", appo);
    if (strlen(appo) >= TPAS400_NAME_LEN) return FALSE;
    sprintf(m_tpas400[itp].name, "%s", appo);
    fscanf(f, "%d", &m_tpas400[itp].n_fields);
    if (m_tpas400[itp].n_fields <= 0) return FALSE;
    itp++;

    if (itp >= MAX_NUM_TPAS400) return FALSE;
  }
  
  m_tpnumber = itp;
  return TRUE;
}

unsigned short itxTP::GetTPID(char* tpname)
{
  unsigned short id = 0;

  if (tpname != NULL)
  {
    while (strcmp(m_tpas400[id].name, tpname) != 0 && id < MAX_NUM_TPAS400)
      id++;           
  }
  return id;
}

char* itxTP::GetTPName(unsigned short id)
{
  char* name = NULL;

  if (id > 0 && id < m_tpnumber)
    name = (char*) &(m_tpas400[id].name);
  
  return name;
}

int itxTP::GetTPNFields(char* name)
{
  unsigned short id = 0;
  
  if ((id = GetTPID(name)) != 0 && id != MAX_NUM_TPAS400)
    return m_tpas400[id].n_fields;
  return 0;
}
