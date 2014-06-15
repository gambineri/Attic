/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxstring.cpp,v $
 * $Revision: 1.46 $  
 * $Author: administrator $
 * $Date: 2002-07-23 12:42:01+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "itxstring.h"


/*
 *  Costruzione/distruzione e funzioni ausiliarie
 */
itxString::itxString(int G)
{
  try
  {
    m_Str = NULL;
    m_Cursor = NULL;
    m_Bufsize = 0;
    m_Granularity = G;
    *this = '\0';
  }
  CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "itxString::itxString(int G)")
}

itxString::itxString(char* sz, int G)
{
  m_Str = NULL;
  m_Cursor = NULL;
  m_Bufsize = 0;
  m_Granularity = G;

  try
  {
    if (sz == NULL)
      *this = '\0';
    else
    {
      int l = strlen(sz);
      XALLOC(m_Str, m_Cursor, m_Bufsize, m_Granularity, l + 1);
      memcpy(m_Str, sz, l);
      m_Cursor  = m_Str + l;
      *m_Cursor = 0;
    }
  }
  CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "itxString::itxString(char* sz, int G)")
}

itxString::itxString(itxString& src, int G)
{
  m_Str = NULL;
  m_Cursor = NULL;
  m_Bufsize = 0;
  m_Granularity = G;

  try
  {
    if (src.m_Str == NULL)
      *this = '\0';
    else
    {
      register unsigned int l = (unsigned)src.m_Cursor - (unsigned)src.m_Str;
      if (m_Bufsize <= l)
        XALLOC(m_Str, m_Cursor, m_Bufsize, m_Granularity, l + 1);

      memcpy(m_Str, src.m_Str, l);
      m_Cursor  = m_Str + l;
      *m_Cursor = 0;
    }
  }
  CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "itxString::itxString(itxString& src, int G)")
}

itxString::~itxString()
{
  try
  {
    ITXFREE(m_Str);
  }
  //CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "itxString::~itxString()")
  //patched fabio--->31/05/2001
  CATCH_TO_NOTHING
}

int itxString::Space(int len, int c)
{
  try
  {
    if (len <= 0)
      return (m_Str == NULL ? 0 : 1);

    m_Cursor = m_Str;

    if ((int)m_Bufsize < len)
      XALLOC(m_Str, m_Cursor, m_Bufsize, m_Granularity, len);

    memset(m_Str, c, len);
    *(m_Str + m_Bufsize - 1) = 0;
  }
  CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "itxString::Space(int len, int c)")
  
  return (m_Str == NULL ? 0 : 1);
}

//Use CAREFULLY: only if you write on the internal buffer by anonymous means.
void itxString::UpdateCursor()
{
  try
  {
    m_Cursor = m_Str + strlen(m_Str);
  }
  CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "itxString::UpdateCursor()")
}

/*****************************************************************************
 - FUNCTION NAME: GetAt
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    int pos : position 0-based in the buffer string.
 -----------------------------------------------------------------------------
 - RETURN VALUE:
  Returns the integer ascii value of the char at pos position (0-based).
 *****************************************************************************/

/*--------------- Object Version --------------------------------------------*/
int itxString::GetAt(int pos)
{
//  if (pos < 0 || (unsigned)pos >= ((unsigned)m_Cursor - (unsigned)m_Str))
//    return -1;

  return m_Str[pos];
}

void itxString::SetAt(char a, int pos)
{
//  if (pos < 0 || (unsigned)pos >= ((unsigned)m_Cursor - (unsigned)m_Str))
//    return -1;

  m_Str[pos] = a;
}

/*****************************************************************************
 - FUNCTION NAME: Left
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    len : len to take.
 -----------------------------------------------------------------------------
 - RETURN VALUE:
 *****************************************************************************/

/*--------------- Object Version --------------------------------------------*/
itxString& itxString::Left(int len)
{
  try
  {
    if ((unsigned)len < ((unsigned)m_Cursor - (unsigned)m_Str) && len >= 0)
    {
      m_Cursor = m_Str + len;
      *m_Cursor = 0;
    }
  }
  CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "itxString::Left(int len)")

  return *this;
}

/*****************************************************************************
 - FUNCTION NAME: Right
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    len : len to take.
 -----------------------------------------------------------------------------
 - RETURN VALUE:
 *****************************************************************************/

/*--------------- Object Version --------------------------------------------*/
itxString& itxString::Right(int len)
{
  try
  {
    if  ((unsigned)len < ((unsigned)m_Cursor - (unsigned)m_Str) && len >= 0)
    {
      memmove(m_Str, m_Cursor - len, len + 1);
      m_Cursor = m_Str + len;
    }
  }
  CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "itxString::Right(int len)")

  return *this;
}

/*****************************************************************************
 - FUNCTION NAME: Left
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    len : len to take.
 -----------------------------------------------------------------------------
 - RETURN VALUE:
 *****************************************************************************/

/*--------------- Object Version --------------------------------------------*/
itxString& itxString::Mid(int start, int len)
{
  try
  {
    if ((unsigned)len <= ((unsigned)m_Cursor - (unsigned)m_Str - start) && len >= 0)
    {
      memmove(m_Str, m_Str + start, len + 1);
      m_Cursor = m_Str + len;
      *m_Cursor = 0;
    }
  }
  CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "itxString::Mid(int start, int len)")

  return *this;
}

/*****************************************************************************
 - FUNCTION NAME: Strcat
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    char* add: string to be added.
 -----------------------------------------------------------------------------
 - RETURN VALUE:
 *****************************************************************************/

/*--------------- Object Version --------------------------------------------*/

void itxString::Strcat(char* add)
{
  *this += add;
}

/*****************************************************************************
 - FUNCTION NAME: Strncpy
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    char*        src    : string from which the add is made.
    unsigned int nchars : Num of chars to add.
 -----------------------------------------------------------------------------
 - RETURN VALUE:
 *****************************************************************************/

/*--------------- Object Version --------------------------------------------*/

void itxString::Strncpy(char* src, unsigned int nchars)
{
  if (nchars == 0 || src == NULL)
    return;

  try
  {
    if (strlen(src) <= nchars)
      *this = src;
    else
    {
      if (nchars > m_Bufsize)
      {
        m_Cursor = m_Str;
        XALLOC(m_Str, m_Cursor, m_Bufsize, m_Granularity, nchars + 1);
/////////
/*  int saveG = (int)(m_Granularity); 
  (m_Granularity) = ((int)(((nchars + 1) - (m_Bufsize)) / (m_Granularity)) + 1) * (m_Granularity); 
  try 
  { 
    char* temp; 
    if ((temp = (char*)ITXALLOC((m_Bufsize) + (m_Granularity))) != NULL) 
    { 
      if ((m_Str) != NULL) 
      { 
        memcpy(temp, (m_Str), (m_Bufsize)); 
        (m_Cursor) = temp + ((unsigned)(m_Cursor) - (unsigned)(m_Str));
        ITXFREE(m_Str);
      }
      else 
        (m_Cursor) = temp; 
      (m_Str) = temp; 
      (m_Bufsize) += (m_Granularity); 
    } 
  } 
  catch(...){throw;}
  (m_Granularity) = saveG; 
*/
/////////

      }

      memcpy(m_Str, src, nchars);
      m_Str[nchars] = 0;
      m_Cursor += nchars;
    }
  }
  CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "itxString::Strncpy(char* src, unsigned int nchars)")
}

/*****************************************************************************
 - FUNCTION NAME: PurgeSubString
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    char* tobepurged: substring to be purged.
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:  
  Finds and clears all occurrences of the string pointed to from the parameter. 
 *****************************************************************************/

/*--------------- Object Version --------------------------------------------*/

void itxString::PurgeSubString(char* tobepurged)
{
  if (tobepurged == NULL)
    return;

  int     len2purge = strlen(tobepurged);
  char*   cursor = m_Str;

  try
  {
    while((cursor = strstr(cursor, tobepurged)) != NULL)
    {
      memmove(cursor, cursor + len2purge, (unsigned)m_Cursor - (unsigned)cursor - len2purge + 1);
      m_Cursor -= len2purge;
    }
  }
  CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "itxString::PurgeSubString(char* tobepurged)")
}

/*****************************************************************************
 - FUNCTION NAME: SubstituteSubString
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    char* base: string in which will be made the substitution.
    char* what: string to be searched.
    char* with: string for replacing.
 -----------------------------------------------------------------------------
 - OUTPUT PARAMETERS:
    char* retstr
 -----------------------------------------------------------------------------
 - RETURN VALUE:
  int:   0    : on success;
        -1    : on error;
         ret  : length of the new string, needed for allocation.
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:  
  Substitutes all occurrences of the string 'what' in the main string 'base'
  with the string 'with'. 
 *****************************************************************************/
int itxString::SubstituteSubString(char* base, char* what, char* with, char* retstr)
{
  char* pNextWhat = NULL;
  char* residual  = NULL;
  int   whatlen   = 0;

  if (base == NULL || what == NULL || with == NULL)
    return -1;

  if (strlen(base) == 0 || (whatlen = strlen(what)) == 0)
    return -1;

  if (retstr == NULL)
  {  
    int what_count = 0;
    pNextWhat = base;
    while ((pNextWhat = strstr(pNextWhat, what)) != NULL)
    {
      what_count++;
      pNextWhat += whatlen;
    }

    return (strlen(base) + what_count * (strlen(with) - strlen(what)));
  }

  pNextWhat = base;
  residual  = base;
  retstr[0] = 0;
  while ((pNextWhat = strstr(residual, what)) != NULL)
  {
    strncat(retstr, residual, (unsigned int)pNextWhat - (unsigned int)residual);
    strcat(retstr, with);    
    residual = pNextWhat + whatlen;
  }
	
	//Last piece
	strcat(retstr, residual);
	  
  return 0;
}

/*--------------- Object Version --------------------------------------------*/

int itxString::SubstituteSubString(char* what, char* with)
{
  int ret = 0;

  try
  {
    itxString appo(*this);
    int newdim = SubstituteSubString(appo.m_Str, what, with, NULL);
  
    if ((int)m_Bufsize <= newdim)
      XALLOC(m_Str, m_Cursor, m_Bufsize, m_Granularity, newdim + 1);

    ret = SubstituteSubString(appo.m_Str, what, with, m_Str);

    //Set m_Cursor to the proper position
    m_Cursor = m_Str + strlen(m_Str);
  }
  CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "itxString::SubstituteSubString(char* what, char* with)")

  return ret;
}

/*****************************************************************************
 - FUNCTION NAME: FindSubString
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    char* what        : string to be searched.
    int   start_from  : zero-based position of the starting character 
                        (defaults to zero)
 -----------------------------------------------------------------------------
 - RETURN VALUE:
  int:  >=0  : on success: the position 0-based of the beginning of 'what';
         -1  : on error;
 *****************************************************************************/

/*--------------- Object Version --------------------------------------------*/

int itxString::FindSubString(char* what, int start_from)
{
  try
  {
    if (what != NULL)
    {
      char* pos = NULL;

      if (start_from < (int)((unsigned)m_Cursor - (unsigned)m_Str))
        pos = strstr(m_Str + start_from, what);

      return (pos == NULL ? 
              (int)((unsigned)m_Cursor - (unsigned)m_Str) : 
              (unsigned)pos - (unsigned)m_Str - start_from);
    }
    else
      return -1;
  }
  CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "itxString::FindSubString(char* what)")
}

/*****************************************************************************
 - FUNCTION NAME: IndexOf
  Slightly different from FindSubString: this function returns the index
  0-based of the argument substring or -1 if it cannot be found, in any case.
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    char* what        : sub-string to be searched.
 -----------------------------------------------------------------------------
 - RETURN VALUE:
  int:  >=0  : on success: the position 0-based of the beginning of 'what';
         -1  : if the string cannot be found;
 *****************************************************************************/

/*--------------- Object Version --------------------------------------------*/

int itxString::IndexOf(char* what)
{
  try
  {
    if (what != NULL && m_Str[0] != 0)
    {
      char* pos = NULL;

      if ((int)((unsigned)m_Cursor - (unsigned)m_Str) > 0)
        pos = strstr(m_Str, what);

      return (pos == NULL ? -1 : (unsigned)pos - (unsigned)m_Str);
    }
    else
      return -1;
  }
  CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "itxString::IndexOf(char* what)")
}

/*****************************************************************************
 - FUNCTION NAME: AdjustStr
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    char* base: string in which will be made the substitution.
 -----------------------------------------------------------------------------
 - OUTPUT PARAMETERS:
    char* retstr
 -----------------------------------------------------------------------------
 - RETURN VALUE:
  int:   0    : on success if retstr != NULL;
         ret  : on success if retstr == NULL, number of apexes, useful
                for pre-allocation.
        -1    : on error;
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:  
  Doubles the occurrences of the ' character in the string 'base'.
 *****************************************************************************/
int itxString::AdjustStr(char* base, char* retstr)
{
  char* pAux = NULL;

  if (base == NULL || strlen(base) == 0)
    return -1;

  if (retstr == NULL)
  {  
    int apex_count = 0;
    pAux = base;
    while (*pAux != 0)
    {
      if (*pAux == '\'')
        apex_count++;
      pAux++;
    }

    return apex_count;
  }

  pAux = base;
  while (*pAux != 0)
  {
    *retstr++ = *pAux;
    if (*pAux == '\'')
      *retstr++ = *pAux;
    pAux++;
  }
  *retstr = 0; //zero termination

  return 0;
}

/*--------------- Object Version --------------------------------------------*/

int itxString::AdjustStr()
{
  int ret = 0;
  try
  {
    itxString appo(*this);
    Space(Len() + AdjustStr(appo.m_Str, NULL));

    ret = AdjustStr(appo.m_Str, m_Str);
  
    //Set m_Cursor to the proper position
    m_Cursor = m_Str + strlen(m_Str);
  }
  CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "itxString::AdjustStr(char* what)")

  return ret;
}


/*****************************************************************************
 - FUNCTION NAME: RemoveLeadingZeros
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    char* base  : string in which will be made the substitution.
    int   zleft : number of zeros to be left anyway.
 -----------------------------------------------------------------------------
 - OUTPUT PARAMETERS:
    char* retstr
 -----------------------------------------------------------------------------
 - RETURN VALUE:
  int:   0 : on success;
        -1 : on error;
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:  
  Removes leading zeros in 'base' exeeding 'zleft' zeros.
 *****************************************************************************/
int itxString::RemoveLeadingZeros(char* base, int zleft, char* retstr)
{
  int   zeros = 0;
  char* p;

  if (base == NULL || retstr == NULL || strlen(base) == 0)
    return -1;
  
  p = base;
  while (*p++ == '0')
    zeros++;

  if (p == base || zeros < zleft)
    strcpy(retstr, base);
  else
    strcpy(retstr, base + zeros - zleft);

  return 0;
}

/*--------------- Object Version --------------------------------------------*/

int itxString::RemoveLeadingZeros(int zleft)
{
  itxString appo(*this);
  int ret = RemoveLeadingZeros(appo.m_Str, zleft, m_Str);

  //Set m_Cursor to the proper position
  m_Cursor = m_Str + strlen(m_Str);

  return ret;
}


/*****************************************************************************
 - FUNCTION NAME: Capitalize
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    char* base  : input string.
 -----------------------------------------------------------------------------
 - OUTPUT PARAMETERS:
    char* retstr
 -----------------------------------------------------------------------------
 - RETURN VALUE:
  int:   0 : on success;
        -1 : on error;
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:  
  LTrims and capitalizes first non-blank character.
 *****************************************************************************/
int itxString::Capitalize(char* base, char* retstr)
{
  char* p;

  if (base == NULL || retstr == NULL || strlen(base) == 0)
    return -1;
  
  p = base;
  while (*p == ' ')
    p++;
  
  retstr[0] = toupper(p[0]);
  strcpy(&retstr[1], &p[1]);

  return 0;
}

/*--------------- Object Version --------------------------------------------*/

int itxString::Capitalize()
{
  itxString appo(*this);
  return Capitalize(appo.m_Str, m_Str);
}

/*****************************************************************************
 - FUNCTION NAME: RightInstr
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    char* base        : input string
    char* search_arg  : string to search
    int   start_from  : 1-based position in the 'base' array.
 -----------------------------------------------------------------------------
 - OUTPUT PARAMETERS:
 -----------------------------------------------------------------------------
 - RETURN VALUE:
  int:   ret  : on success the position 1-based of the last occurrence of
                'search_arg' in 'base';
         0    : 'search_arg' was not found;
        -1    : on error;
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:
      Executes an 'Instr' operation from the bottom of a string
 *****************************************************************************/
int itxString::RightInstr(char* base, char* search_arg)
{
  char* p = NULL;
  char* found = NULL;
  int   blen = 0;
  int   salen = 0;
  
  if (base == NULL       || (blen = strlen(base)) == 0 || 
      search_arg == NULL || (salen = strlen(search_arg)) == 0)
    return -1;

  p = base + strlen(base) - salen;  
  while ( ((unsigned)p >= (unsigned)(base - 1)) && 
          (found = strstr(p, search_arg)) == NULL )
    p--;
  
  if (found)
    return (unsigned)found - (unsigned)base + 1;
  else
    return 0;
}

/*--------------- Object Version --------------------------------------------*/

int itxString::RightInstr(char* search_arg)
{
  return RightInstr(m_Str, search_arg);
}

/*****************************************************************************
 - FUNCTION NAME: MaxTokenLen
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    char* base    : input string
    char* sep     : separator token
 -----------------------------------------------------------------------------
 - RETURN VALUE:
  int:   ret  : on success the maximum length of a token;
        -1    : on error;
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:
    Computes the maximum length of a token in 'base' using 'sep' as separator.
 *****************************************************************************/
int itxString::MaxTokenLen(char* base, char* sep)
{
  char* base2  = NULL;  
  char* token  = NULL;
  int   ret    = 0;
  int   toklen = 0;

  if ( base == NULL || strlen(base) == 0 || 
       sep == NULL  || strlen(sep) == 0 )
    return -1;
  
  if ((base2 = strdup(base)) == NULL)
    return -1;

  token = strtok(base2, sep);
  while (token != NULL)   
  {
    toklen = strlen(token);
    ret = (ret < toklen ? toklen : ret);
    token = strtok(NULL, sep);
  }
  
  free(base2);
  
  return ret;
}

/*--------------- Object Version --------------------------------------------*/

int itxString::MaxTokenLen(char* sep)
{
  return MaxTokenLen(m_Str, sep);
}

/*****************************************************************************
 - FUNCTION NAME: UCase
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    char* str     : string to convert
 -----------------------------------------------------------------------------
 - RETURN VALUE:
    char*         : converted string in the input buffer pointed by 'str' arg.
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:
    ToUppers chars in a string.
 *****************************************************************************/
char* itxString::UCase(char* str)
{ 
  char* ret = str;
  while (*str)
    *str++ = toupper(*str);

  return ret;
}

/*--------------- Object Version --------------------------------------------*/

itxString& itxString::UCase()
{
  UCase(m_Str);
  return *this;
}

/*****************************************************************************
 - FUNCTION NAME: LCase
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    char* str     : string to convert
 -----------------------------------------------------------------------------
 - RETURN VALUE:
    char*         : converted string in the input buffer pointed by 'str' arg.
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:
    ToLowers chars in a string.
 *****************************************************************************/
char* itxString::LCase(char* str)
{ 
  char* ret = str;
  while (*str)
    *str++ = tolower(*str);

  return ret;
}

/*--------------- Object Version --------------------------------------------*/

itxString& itxString::LCase()
{
  LCase(m_Str);
  return *this;
}

/*****************************************************************************
 - FUNCTION NAME: EscapeChars
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    char*   source      : in
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:
    Genera una nuova stringa sostituendo ai caratteri speciali 
    i loro codici di escape. La versione statica torna un puntatore
    'destination' che DEVE essere disallocato dal chiamante.
 *****************************************************************************/

int itxString::EscapeChars(char* source, char** destination)
{
  itxString appo(source);

  try
  {
    appo.EscapeChars();

    if ((*destination = (char*)malloc(appo.Len() + 1)) == NULL)
      return 0;
    else
    {
      strcpy(*destination, appo.GetBuffer());
      return 1;
    }
  }
  CATCHALLMSG("Exception in itxString::EscapeChars(char* source, char** destination)")
}

/*--------------- Object Version --------------------------------------------*/

int itxString::EscapeChars()
{
	char* s;
  char* d;
	int   srcPos = 0;
	int   dstPos = 0;
	char  appo[64];

  try
  {
	  memset(appo, '\0', 64);

    s = m_Str;
    int srclen = strlen(s);
    int dstlen = srclen;
	  
    if (!s)
		  return 0;

    // count
	  while (srcPos < srclen) 
    {
		  unsigned char ch = s[srcPos];    
		  // se il carattere non e' alfanumerico
		  if(
				  (ch < 48)									||
				  ((ch > 57) && (ch < 65))	||
				  ((ch > 90) && (ch < 97))	||
				  (ch > 122)
			  )
		  {
			  dstlen += 2;
		  }
		  srcPos++;
	  }

    if ((d = (char*)ITXALLOC(dstlen + 1)) == NULL)
      return 0;
  
    // revert
	  srcPos = 0;
    while (srcPos < srclen) 
    {
		  unsigned char ch = s[srcPos];

		  if(
				  (ch < 48)									|| 
				  ((ch > 57) && (ch < 65))	|| 
				  ((ch > 90) && (ch < 97))	||
				  (ch > 122)
			  )
		  {
			  memset(appo, '\0', 64);
        sprintf(appo, "%%%X", ch);
        memcpy((char*)d + dstPos, appo, 3);
        dstPos += 3;
		  }
		  else
		  {
        d[dstPos] = ch;
        dstPos++;
		  }
		  srcPos++;
	  }

	  d[dstPos] = '\0';

    //Replace old buffer
    if ((int)m_Bufsize < dstlen)
      XALLOC(m_Str, m_Cursor, m_Bufsize, m_Granularity, dstlen + 1);
    memcpy(m_Str, d, dstPos);
    m_Cursor = m_Str + dstPos;
    *m_Cursor = 0;

    ITXFREE(d);
  }
  CATCHALLMSG("Exception in itxString::EscapeChars()")

  return (m_Str == NULL ? 0 : 1);
}

/*****************************************************************************
 - FUNCTION NAME: InBlinder
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    char**  source    : in-out
    char    blinder   : carattere di delimitazione
 -----------------------------------------------------------------------------
 - RETURN VALUE:
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:
    Torna la stringa in source epurata dei due delimitatori.
 *****************************************************************************/
void itxString::InBlinder(char** source, char blinder)
{
	char* s;
	int startPos = -1;
	int stopPos = -1;
	int pos;
	
  try
  {
    s = *source;
    int srclen = strlen(s);
    int dstlen = srclen;
	  
    if (!s)
		  return;

	  // find first blinder occurence
	  pos = 0;
	  while (pos < srclen && startPos == -1) 
    {
		  int ch = s[pos];
		  if (ch == blinder)
			  startPos = pos + 1;

		  pos++;
	  }

	  // find last blinder occurence
	  pos = srclen - 1;
	  while (pos >= 0 && stopPos == -1) 
    {
		  int ch = s[pos];
		  if (ch == blinder)
			  stopPos = pos;

		  pos--;
	  }
	  
	  if (stopPos < startPos)
		  return;

	  if (stopPos == -1 && startPos == -1)
		  return;

	  s[stopPos] = '\0';
	  (*source) += startPos;
  }
  CATCHALLMSG("Exception in itxString::InBlinder(char** source, char blinder)")

	return;
}		

/*--------------- Object Version --------------------------------------------*/

int itxString::InBlinder(char blinder)
{
  try
  {
    itxString oappo(*this);
    char* appo = oappo.m_Str;
    InBlinder(&appo, blinder);
    *this = appo;
  }
  CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "itxString::InBlinder(char blinder)")

  return (m_Str == NULL ? 0 : 1);
}

/*****************************************************************************
 - FUNCTION NAME: CompareNoCase
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    char* s1    : stringa 1
    char* s2    : stringa 2
 -----------------------------------------------------------------------------
 - RETURN VALUE: 1 se sono uguali, 0 altrimenti.
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:
    Confronto non case sensitive.
 *****************************************************************************/
int itxString::CompareNoCase(char *s1, char *s2) 
{
  if (s1 == NULL || s2 == NULL)
    return 0;

  try
  {
	  while(1) 
    {
		  if (!(*s1)) 
      {
			  if (!(*s2)) 
				  return 1;
        else 
				  return 0;
		  } 
      else if (!(*s2)) 
			  return 0;
		  
      if (isalpha(*s1)) 
      {
			  if (tolower(*s1) != tolower(*s2)) 
				  return 0;
		  } 
      else if ((*s1) != (*s2)) 
			  return 0;
		  
      s1++;
		  s2++;
	  }
  }
  CATCHALLMSG("Exception in itxString::CompareNoCase(char *s1, char *s2)")
}

/*--------------- Object Version --------------------------------------------*/

int itxString::CompareNoCase(itxString* pistr)
{
  itxString appo(*this);
  return CompareNoCase(appo.m_Str, pistr->m_Str);
}

int itxString::CompareNoCase(char* pistr)
{
  itxString appo(*this);
  return CompareNoCase(appo.m_Str, pistr);
}

/*****************************************************************************
 - FUNCTION NAME: Compare
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    itxString* s1    : stringa da confrontare con this.
 -----------------------------------------------------------------------------
 - RETURN VALUE: quello di strcmp.
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:
    Confronto case sensitive.
 *****************************************************************************/

/*--------------- Object Version --------------------------------------------*/

int itxString::Compare(itxString* pistr)
{
  try
  {
    if (this->m_Str == NULL && pistr->m_Str == NULL)
      return 0;

    if (this->m_Str == NULL || pistr->m_Str == NULL)
      return 1;
  }
  CATCHALLMSG("Exception in itxString::Compare(itxString* pistr)")

  return strcmp(this->m_Str, pistr->m_Str);
}

int itxString::Compare(char* pstr)
{
  try
  {
    if (this->m_Str == NULL && pstr == NULL)
      return 0;

    if (this->m_Str == NULL || pstr == NULL)
      return 1;
  }
  CATCHALLMSG("Exception in itxString::Compare(char* pstr)")

  return strcmp(this->m_Str, pstr);
}

/*****************************************************************************
 - FUNCTION NAME: LTrim
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    char* s1    : stringa da L-trimmare
 -----------------------------------------------------------------------------
 - RETURN VALUE:
  stringa L-trimmata
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:
 *****************************************************************************/
char* itxString::LTrim(char* str)
{
  try
  {
    char* p = str;
  
    while(*p == ' ')
      p++;

    strcpy(str, p);
  }
  catch(...)
  {    
    return str;
  }
  
  return str;
}

/*--------------- Object Version --------------------------------------------*/

itxString& itxString::LTrim()
{
  LTrim(m_Str);

  //Set m_Cursor to the proper position
  m_Cursor = m_Str + strlen(m_Str);

  return *this;
}

/*****************************************************************************
 - FUNCTION NAME: RTrim
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    char* str    : stringa da R-trimmare
 -----------------------------------------------------------------------------
 - RETURN VALUE:
  stringa R-trimmata
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:
 *****************************************************************************/
char* itxString::RTrim(char* str)
{
  try
  {
    char* p = str + strlen(str) - 1;
  
    while(*p == ' ')
      p--;
    
    *(++p) = 0;
  }
  catch(...)
  {
    return str;
  }

  return str;
}

/*--------------- Object Version --------------------------------------------*/

itxString& itxString::RTrim()
{
  RTrim(m_Str);

  //Set m_Cursor to the proper position
  m_Cursor = m_Str + strlen(m_Str);

  return *this;
}

/*****************************************************************************
 - FUNCTION NAME: Trim
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
    char* str    : stringa da trimmare
 -----------------------------------------------------------------------------
 - RETURN VALUE:
  stringa trimmata
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:
 *****************************************************************************/
char* itxString::Trim(char* str)
{
  try
  {
    if (str == NULL)
      return NULL;

    //L
    char* p = str;
    while(*p == ' ')
      p++;
    strcpy(str, p);

    //R
    p = str + strlen(str) - 1;
    while(*p == ' ')
      p--;
    *(++p) = 0;
  }
  catch(...)
  {
    return str;
  }

  return str;
}

/*--------------- Object Version --------------------------------------------*/

itxString& itxString::Trim()
{
  Trim(m_Str);

  //Set m_Cursor to the proper position
  m_Cursor = m_Str + strlen(m_Str);

  return *this;
}

itxString& itxString::RmBlankChars()
{
  char* str = m_Str;

  try
  {
    if (str == NULL)
      return *this;

    //L
    char* p = str;
    while(*p == ' ' || *p == 0x0A || *p == 0x0D || *p == '\t')
      p++;
    strcpy(str, p);

    //R
    p = str + strlen(str) - 1;
    while(*p == ' ' || *p == 0x0A || *p == 0x0D || *p == '\t')
      p--;
    *(++p) = 0;
  }
  catch(...)
  {
    return *this;
  }

  //Set m_Cursor to the proper position
  m_Cursor = m_Str + strlen(m_Str);

  return *this;
}


/*****************************************************************************
 - FUNCTION NAME: SetEmpty
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
 -----------------------------------------------------------------------------
 - RETURN VALUE:
  stringa contenente il carattere '\0' (stringa vuota).
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:
 *****************************************************************************/

/*--------------- Object Version --------------------------------------------*/

void itxString::SetEmpty()
{
  *this = '\0';
  m_Cursor = m_Str;
}


/*****************************************************************************
 - FUNCTION NAME: SetInt
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
 -----------------------------------------------------------------------------
 - RETURN VALUE:
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:
    sprintf di un intero sul buffer interno.
 *****************************************************************************/

/*--------------- Object Version --------------------------------------------*/
void itxString::SetInt(int val)
{
  try
  {
    m_Cursor = m_Str;
    if (m_Bufsize < INT_2_CHAR_LEN)
      XALLOC(m_Str, m_Cursor, m_Bufsize, m_Granularity, INT_2_CHAR_LEN);
    sprintf(m_Str, "%d", val);
    RTrim(m_Str);
    m_Cursor = m_Str + strlen(m_Str);
  }
  CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "itxString::SetInt(int val)")
}


/*--------------- Object Version --------------------------------------------*/

char* itxString::CVFromDouble(double value, int mindecimal)
{
  char buffer[50];
  char newbuffer[50];
  int  len;
  unsigned int prec = 10;

  double frac, pint;
  char c;
  double cvalue = 0.0005;

  try
  {
    if (fabs(value) > 0.0005)
    {
      sprintf(buffer, "%f", value);
      char* dot = strstr(buffer, ".");

      // remove tailing zeros
      len = strlen(dot);
      int ic = 0;
      while ((c = dot[len - ic - 1]) == '0')
      {
        dot[len - ic - 1] = '\0';
        ic ++;
      }

      int sign = 1;
      len = strlen(dot);
      if (len > 6)
      {
        frac = fabs(modf(value, &pint));
        if (frac < 0) sign = -1;
        bool converge = false;
        double ceilvalue, floorvalue;

        int idec = 1;
        double trial;
        while(!converge && idec < len + 1)
        {
          trial = frac * 10 * idec;
          ceilvalue = ceil(trial);
          floorvalue = floor(trial);
          if ((ceilvalue - trial) < cvalue)
          {
            converge = true;
            frac = ceilvalue / (10 * idec);
          }
          else if ((trial - floorvalue) < cvalue)
          {
            converge = true;
            frac = floorvalue / (10 * idec);
          }
          idec++;
        }
        value = pint + sign * frac;
      }        

      sprintf(buffer, "%f", value);
      dot = strstr(buffer, ".");

      // remove tailing zeros
      len = strlen(dot);
      ic = 0;
      while ((c = dot[len - ic - 1]) == '0')
      {
        dot[len - ic - 1] = '\0';
        ic ++;
      }

      len = strlen(buffer);
      if (buffer[len - 1] == '.')
        buffer[len - 1] = '\0';
      else 
      {
        sprintf(newbuffer, "%s", buffer);
        dot = strstr(newbuffer, ".");

        // trunc at contiguous zeros group
        bool groupfound = false;
        len = strlen(dot);
        ic = 0;
        int iz = 0;
        while (!groupfound)
        {
          if (dot[ic + 1] == '0')
            iz++;
          else
            iz = 0;
          if (iz == 2 || ic == len)
          {
            groupfound = true;
            dot[ic] = '\0';
          }
          ic++;
        }
      }

      double newvalue = atof(newbuffer);
      if (fabs(value - newvalue) < 0.0005)
        sprintf(buffer, "%s", newbuffer);

      //roundup if necessary
  /*
      if (len > 6)
      {    
        buf = _ecvt(value, prec, &dec, &sign);
        if (dec < 0)
          cd = 4 - dec;
        else if (dec >= 0 && dec < 3)
          cd = 4 - dec;
        else if (dec >= 3 && dec < 6)
          cd = mindecimal + 1;
        else
          cd = mindecimal;
        sprintf(format, "%%.%df", cd); 
        sprintf(buffer, format, value);
      }
  */
    }
    else if (fabs(value) > 0.0000005)
    {
      sprintf(buffer, "%.12f", value);
      char* dot = strstr(buffer, ".");

      // remove tailing zeros
      len = strlen(dot);
      int ic = 0;
      while ((c = dot[len - ic - 1]) == '0')
      {
        dot[len - ic - 1] = '\0';
        ic ++;
      }

    }
    else
      sprintf(buffer, "%s", "0");
  }
  CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "itxString::CVFromDouble(double value, int mindecimal)")

  *this = &buffer[0];
  return m_Str;
}

/*****************************************************************************
 - FUNCTION NAME: GetToken
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS: 
    char sep : separatore
    int pos : posizione del token
    itxString* dest : destinazione
 -----------------------------------------------------------------------------
 - RETURN VALUE:
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION: 
    la posizione 0, sia nel caso di stringhe inizianti con il separatore che non,
    ritorna il primo token 
 *****************************************************************************/
void itxString::GetToken(char sep, int pos, itxString* dest)
{
  if (dest == NULL || pos < 0)
    return;

  try
  {
    char* pc    = m_Str;
    int   count = 0;

    while (count < pos && *pc != 0)
    {
      if (*pc == sep)
        count++;
     pc++;
    }
    
    if (count < pos)
     return;

    *dest = pc;
    count = 0;

    while (*pc != sep && *pc != 0)
    {
      count++;
      pc++;
    }

    dest->Left(count);
  }
  CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "itxString::GetToken(char sep, int pos, itxString* dest)")
}

// converts the XXXXXXX.YYYY format string in X.XXX.XXX,YYYY
// converts the X.XXX.XXX,YYYY format string in XXXXXXX.YYYY if invert = -1
void itxString::Currency(int invert)
{
  if (invert == -1)
  {
    InBlinder('"');
    SubstituteSubString(".", "");
    SubstituteSubString(",", ".");
  }
  else
  { 
    int len = this->Len();
    int dotpos = FindSubString(".");
  
    if (len > dotpos + 3)
    {
      itxString sfrac;
      sfrac = this->GetBuffer();
      sfrac.Right(len - dotpos - 1);
      sfrac.Left(3);
      
      double dfrac = atof(sfrac.GetBuffer())/10.;
      double dceil = ceil(dfrac);
      double dfloor = floor(dfrac);
      
      int rfrac;
      if (dceil - dfrac > 0.5)
        rfrac = (int)dfloor;
      else 
        rfrac = (int)dceil;

      Left(dotpos + 1);

      if (rfrac < 10.)
        *this += "0";
      *this += rfrac;
    }
    SubstituteSubString(".", ",");
    int dotins = dotpos - 3;
    int stop = (GetAt(0) == '-' ? 1 : 0);
    while (dotins > stop)
    {
      InsAt(".", dotins);
      dotins -= 3;
    }
    if (FindSubString(",") == Len())
      Strcat(",00");
    if (FindSubString(",") == (Len() - 2))
      Strcat("0");
  }
}
/*****************************************************************************
 - FUNCTION NAME: HexToAscii
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
 -----------------------------------------------------------------------------
 - RETURN VALUE:
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:
 *****************************************************************************/

void itxString::HexToAscii(char* source, char* destination, int src_len)
{
  char chex[3];
  int i;

  if (source == NULL || destination == NULL)
    return;

  try
  {
    for (i = 0; i < src_len; i++)
    {
      memset(chex, 0, 3);

      sprintf(chex, "%x", (unsigned char) source[i]);

      if (strlen(chex) < 2)
      {
        chex[1] = chex[0];
        chex[0] = '0';
      }
      memcpy(destination + 2 * i, chex, 2);
    }
  }
  CATCHALLMSG("itxString::HexToAscii(char* source, char* destination, int src_len)")
}

/*****************************************************************************
 - FUNCTION NAME: AsciiToHex
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
 -----------------------------------------------------------------------------
 - RETURN VALUE:
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:
 *****************************************************************************/
void itxString::AsciiToHex(char* source, char* destination, int src_len)
{
  char  chex[3];
  char* stop;
  int  i;
  char m;

  if (source == NULL || destination == NULL)
    return;

  try
  {
    stop = &chex[2];
    for (i = 0; i < src_len; i++)
    {
      memset(chex, 0, 3);
      memcpy(chex, &source[i * 2], 2);
      m = (char) strtol(chex, &stop, 16);
      memcpy(destination + i, &m, 1);
    }
  }
  CATCHALLMSG("itxString::HexToAscii(char* source, char* destination, int src_len)")
}



/*---------------------------------------------------------------------------*
															WRAPPER FUNCTIONS
 *---------------------------------------------------------------------------*/


int itxString_SubstituteSubString(char* base, char* what, char* with, char* retstr)
{
	return itxString::SubstituteSubString(base, what, with, retstr);
}

int itxString_AdjustStr(char* base, char* retstr)
{
	return itxString::AdjustStr(base, retstr);
}

int itxString_RemoveLeadingZeros(char* base, int zleft, char* retstr)
{
	return itxString::RemoveLeadingZeros(base, zleft, retstr);
}

int itxString_Capitalize(char* base, char* retstr)
{
	return itxString::Capitalize(base, retstr);
}

int itxString_RightInstr(char* base, char* search_arg, int start_from)
{
	return itxString_RightInstr(base, search_arg, start_from);
}

int itxString_MaxTokenLen(char* base, char* sep)
{
	return itxString::MaxTokenLen(base, sep);
}

char* itxString_UCase(char* str)
{
	return itxString::UCase(str);
}

char* itxString_LCase(char* str)
{
	return itxString::LCase(str);
}

int itxString_EscapeChars(char* source, char** destination)
{
  return itxString::EscapeChars(source, destination);
}

void itxString_InBlinder(char **source, char blinder)
{
  itxString::InBlinder(source, blinder);
}

int itxString_CompareNoCase(char* s1, char* s2)
{
  return itxString::CompareNoCase(s1, s2);
}

char* itxString_LTrim(char* str)
{
  return itxString::LTrim(str);
}

char* itxString_RTrim(char* str)
{
  return itxString::RTrim(str);
}

char* itxString_Trim(char* str)
{
  return itxString::Trim(str);
}










