/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#ifndef __ITX_ITNCNV_CPP__
#define __ITX_ITNCNV_CPP__
#endif

#include <string.h>

#include "itannitdef.h"

void itxOemToChar(char* source, char* destination)
{
  char* p;

  p = source;
  while ((p = strchr(p, 0xAB)) != NULL)
    *p = ITX_APPC_FIELD_SEP; 

  p = source;
  while ((p = strchr(p, 0xAA)) != NULL)
    *p = ITX_APPC_RECORD_SEP;

  p = source;
  while ((p = strchr(p, 0xBA)) != NULL)
    *p = ITX_APPC_ENDINFO_SEP;

  p = source;
  while ((p = strchr(p, 0x85)) != NULL)
//    *p = '�';
    *p = (char) 0x7B;

  p = source;
  while ((p = strchr(p, 0x8A)) != NULL)
//    *p = '�';
    *p = (char) 0x7D;

  p = source;
  while ((p = strchr(p, 0x8D)) != NULL)
//    *p = '�';
    *p = (char) 0x7E;

  p = source;
  while ((p = strchr(p, 0x7C)) != NULL)
//    *p = '|';
    *p = (char) 0xE3;

  p = source;
  while ((p = strchr(p, 0x5C)) != NULL)
//    *p = '\';
    *p = (char) 0xA7;

  p = source;
  while ((p = strchr(p, 0x40)) != NULL)
//    *p = '@';
    *p = (char) 0xDD;

  p = source;
  while ((p = strchr(p, 0x23)) != NULL)
//    *p = '#';
    *p = (char) 0xD9;

  p = source;
  while ((p = strchr(p, 0xF5)) != NULL)
//    *p = '�';
    *p = (char) 0x40;

  p = source;
  while ((p = strchr(p, 0x5B)) != NULL)
//    *p = '[';
    *p = (char) 0xCA;

  p = source;
  while ((p = strchr(p, 0x5D)) != NULL)
//    *p = ']';
    *p = (char) 0xA9;

  p = source;
  while ((p = strchr(p, 0x87)) != NULL)
//    *p = '�';
    *p = (char) 0x5C;

  p = source;
  while ((p = strchr(p, 0x97)) != NULL)
//    *p = '�';
    *p = (char) 0x60;

  p = source;
  while ((p = strchr(p, 0x95)) != NULL)
//    *p = '�';
    *p = (char) 0x7C;

  p = source;
  while ((p = strchr(p, 0x82)) != NULL)
//    *p = '�';
    *p = (char) 0x5D;

  p = source;
  while ((p = strchr(p, 0x9C)) != NULL)
//    *p = '�';
    *p = (char) 0x23;

  p = source;
  while ((p = strchr(p, 0xF8)) != NULL)
//    *p = '�';
    *p = (char) 0x5B;

}

void itxCharToOem(char* source, char* destination)
{
  char* p;

  p = source;
  while ((p = strchr(p, ITX_APPC_FIELD_SEP)) != NULL)
    *p = (char) 0xAB;

  p = source;
  while ((p = strchr(p, ITX_APPC_RECORD_SEP)) != NULL)
    *p = (char) 0xAA;

  p = source;
  while ((p = strchr(p, ITX_APPC_ENDINFO_SEP)) != NULL)
    *p = (char) 0xBA;


  p = source;
  while ((p = strchr(p, 0x7B)) != NULL)
//    *p = '�';
    *p = (char) 0x85;

  p = source;
  while ((p = strchr(p, 0x7C)) != NULL)
//    *p = '�';
    *p = (char) 0x95;

  p = source;
  while ((p = strchr(p, 0x40)) != NULL)
//    *p = '�';
    *p = (char) 0xF5;

  p = source;
  while ((p = strchr(p, 0x7D)) != NULL)
//    *p = '�';
    *p = (char) 0x8A;

  p = source;
  while ((p = strchr(p, 0x5D)) != NULL)
//    *p = '�';
    *p = (char) 0x82;

  p = source;
  while ((p = strchr(p, 0x7E)) != NULL)
//    *p = '�';
    *p = (char) 0x8D;

  p = source;
  while ((p = strchr(p, 0xE3)) != NULL)
//    *p = '|';
    *p = (char) 0x7C;

  p = source;
  while ((p = strchr(p, 0x23)) != NULL)
//    *p = '�';
    *p = (char) 0x9C;

  p = source;
  while ((p = strchr(p, 0x5C)) != NULL)
//    *p = '�';
    *p = (char) 0x87;

  p = source;
  while ((p = strchr(p, 0x5B)) != NULL)
//    *p = '�';
    *p = (char) 0xF8;

  p = source;
  while ((p = strchr(p, 0xD9)) != NULL)
//    *p = '#';
    *p = (char) 0x23;

  p = source;
  while ((p = strchr(p, 0x60)) != NULL)
//    *p = '�';
    *p = (char) 0x97;

  p = source;
  while ((p = strchr(p, 0xCA)) != NULL)
//    *p = '[';
    *p = (char) 0x5B;

  p = source;
  while ((p = strchr(p, 0xA9)) != NULL)
//    *p = ']';
    *p = (char) 0x5D;

  p = source;
  while ((p = strchr(p, 0xA7)) != NULL)
//    *p = '\';
    *p = (char) 0x5C;

  p = source;
  while ((p = strchr(p, 0xDD)) != NULL)
//    *p = '@';
    *p = (char) 0x40;
}


// it works for null-terminating string
void itxEBCDToASCII(char* source)
{
  char* p;

  p = source;
  while ((p = strchr(p, ITX_APPC_FIELD_SEP)) != NULL)
    *p = (char) 0xAB;

  p = source;
  while ((p = strchr(p, ITX_APPC_RECORD_SEP)) != NULL)
    *p = (char) 0xAA;

  p = source;
  while ((p = strchr(p, ITX_APPC_ENDINFO_SEP)) != NULL)
    *p = (char) 0xBA;

  p = source;
  while ((p = strchr(p, 0x7B)) != NULL)
    *p = '�';

  p = source;
  while ((p = strchr(p, 0x7C)) != NULL)
    *p = '�';

  p = source;
  while ((p = strchr(p, 0x40)) != NULL)
    *p = '�';

  p = source;
  while ((p = strchr(p, 0x7D)) != NULL)
    *p = '�';

  p = source;
  while ((p = strchr(p, 0x5D)) != NULL)
    *p = '�';

  p = source;
  while ((p = strchr(p, 0x7E)) != NULL)
    *p = '�';

}




