/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* crypt 21/10/1999
 *
 * utility wrap for the public domain DES implementation by
 * mr. Outerbridge
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */
 
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "crypt.h"
#include "desx.h"

#define   ITX_DES_BLOK    8

void BlockHexToAscii(char* source, char* destination, int block_length)
{
  char chex[3];
  int i;

  for (i = 0; i < block_length; i++)
  {
    memset(chex, 0, 3);
    _itoa((int) ((unsigned char) source[i]), (char*) chex, 16);
    if (strlen(chex) < 2)
    {
      chex[1] = chex[0];
      chex[0] = '0';
    }
    memcpy(destination + 2 * i, chex, 2);
  }
}

void BlockAsciiToHex(char* source, char* destination, int block_length)
{
  char  chex[3];
  char* stop;
  int  i;
  char m;

  stop = &chex[2];
  for (i = 0; i < block_length; i++)
  {
    memset(chex, 0, 3);
    memcpy(chex, &source[i * 2], 2);
    m = (char) strtol(chex, &stop, 16);
    memcpy(destination + i, &m, 1);
  }
}


/*
// (in)  source string must be null terminated
// (out) the destination string is the hex representation of the ascii values of the 
//       characters of the crypted string
*/
unsigned short itxEncrypt(
	unsigned char* DESKey,
	unsigned char* Whitenings,
	unsigned char* source,
	unsigned char* destination)
{
	struct DESXKey dxKey;
	struct DESXContext dxCon;

  unsigned char blok[ITX_DES_BLOK];
  unsigned char* sourcepos = source;
  unsigned char* destinpos = destination;
  int sourcelen = 0;
  int iblok, nbloks = 0, residual = 0;

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
    BlockHexToAscii((char*) blok, (char*) (destination + 2 * iblok * ITX_DES_BLOK), ITX_DES_BLOK);    
  }

  if (residual != 0)
  {
    /* blanks added on tail to obtain well formed DES_BLOK */
    memset(blok, ' ', ITX_DES_BLOK);
    memcpy(blok, source + iblok * ITX_DES_BLOK, residual);
	  DESXEncryptBlock(&dxCon, blok, blok);
    BlockHexToAscii((char*) blok, (char*) (destination + 2 * iblok * ITX_DES_BLOK), ITX_DES_BLOK);    
  }

  /* make destination null-terminated */
  *(destination + 2 * (iblok + 1) * ITX_DES_BLOK) = 0;

  return 0;
}

/*
// (in) source string must be the hex representation of the ascii values of the 
//      characters of the crypted string
*/
unsigned short itxDecrypt(
	unsigned char* DESKey,
	unsigned char* Whitenings,
	unsigned char* source,
	unsigned char* destination)
{
	struct DESXKey dxKey;
	struct DESXContext dxCon;

  unsigned char blok[ITX_DES_BLOK];
  unsigned char* sourcepos = source;
  unsigned char* destinpos = destination;
  int sourcelen = 0;
  int iblok, nbloks = 0, i = 0;

  memcpy(dxKey.DESKey64, DESKey, ITX_DES_BLOK);
  memcpy(dxKey.Whitening64, Whitenings, ITX_DES_BLOK);

  DESXKeySetup(&dxCon, &dxKey);

  sourcelen = strlen((char*) source);
  nbloks = sourcelen / (ITX_DES_BLOK * 2);

  BlockAsciiToHex((char*) source, (char*) source, sourcelen / 2);

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
