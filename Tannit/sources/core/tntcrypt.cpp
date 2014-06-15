/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#ifndef __TNT_CRYPT_CPP__
#define __TNT_CRYPT_CPP__
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "tannit.h"
#include "extVars.h"
#include "itxtypes.h"
#include "crypt.h"


extern FILE* debug;

unsigned char key[64] = "itxsecur";
unsigned char wht[64] = "byFabioM";
//unsigned char key[64] = "}!�00-r�";
//unsigned char wht[64] = "r�";


// (in)  usercode
// (out) userlabel - must be allocated by the user of the function
int BuildCryptUserLabel(unsigned char* usercode, unsigned char* userlabel)
{
  unsigned char label[128];
  unsigned char cryptlabel[128];
  time_t ltime;
//  unsigned short (_cdecl *pf)(unsigned char*, unsigned char*, unsigned char*, unsigned char*);
//  pf = (itxEncrypt);

  if (usercode == NULL)
    return FALSE;
  
  memset(label, 0, 128);
  memset(cryptlabel, 0, 128);

  time( &ltime );
  sprintf((char*) &label[0], "%s#%ld", usercode, ltime);
  
  itxEncrypt(&key[0], &wht[0], (unsigned char*) &label[0], (unsigned char*) &cryptlabel[0]);
//   pf(&key[0], &wht[0], (unsigned char*) &label[0], (unsigned char*) &cryptlabel[0]);

  sprintf((char*) &userlabel[0], "%s", &cryptlabel[0]);

  return TRUE;
}

// (in) usercode, userlabel
// (out) return value = difference between actual time and decrypted time stamp
//       return value 0 means Failure
long VerifyCryptUserLabel(unsigned char* usercode, unsigned char* userlabel)
{
  unsigned char decryptlabel[128];
  char* decryptcode = NULL;
  char* decrypttime = NULL;
  time_t ltime, ctime;

  if (usercode == NULL || userlabel == NULL)
    return 0;
  
  memset(decryptlabel, 0, 128);

  time( &ltime );
  
  itxDecrypt(&key[0], &wht[0], userlabel, decryptlabel);

  if ((decryptcode = strtok((char*) &decryptlabel[0], "#")) == NULL)
    return 0;
  
  if ((decrypttime = strtok(NULL, "#")) == NULL)
    return 0;

  if (!ISEQUAL((char*) &usercode[0], (char*) &decryptcode[0]))
    return 0;

  ctime = atol(decrypttime);    

  return (ltime - ctime);
}

extern void BlockAsciiToHex(char* source, char* destination, int block_length);

void tannitEncrypt(char* source, char* destination)
{
  itxEncrypt(&key[0], &wht[0], (unsigned char*) source, (unsigned char*) destination);

//BEG PROVE WIND  

  //"RrVMYZrK5Og="

//  char a[50] = {0};
//  BlockAsciiToHex(destination, a, strlen(destination));
//**/if(usedbg){fprintf(debug, "VALORE CRYPTATO IN HEX: %s\n", destination);fflush(debug);}
//**/if(usedbg){fprintf(debug, "VALORE CRYPTATO IN ASCII: %s\n", a);fflush(debug);}
  
//END PROVE WIND  
}

void tannitDecrypt(char* source, char* destination)
{
  itxDecrypt(&key[0], &wht[0], (unsigned char*) source, (unsigned char*) destination);
}
