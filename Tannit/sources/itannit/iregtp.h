/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#ifndef __IREGTP_H__
#define __IREGTP_H__

#include <memory.h>

#include "tptypes.h"

typedef struct NewTPAS400
{
  unsigned short id;
  char           name[TPAS400_NAME_LEN];
  short          n_fields;
} NewTPAS400;

class itxTP
{
public:
  NewTPAS400 m_tpas400[MAX_NUM_TPAS400];
  int        m_tpnumber;

  itxTP() 
  {
    memset(m_tpas400, '\0', MAX_NUM_TPAS400 * sizeof(NewTPAS400));
    m_tpnumber = 0;
  }

  void Default();
  bool Load(char* filename);
  unsigned short GetTPID(char* name);
  char* GetTPName(unsigned short id);
  int GetTPNFields(char* name);
};

#endif //__IREGTP_H__