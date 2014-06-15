/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxstring.h,v $
 * $Revision: 1.42 $  
 * $Author: massimo $
 * $Date: 2002-06-11 11:01:45+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */

#ifndef __ITXSTRING_H__
#define __ITXSTRING_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "itxdefines.h"


// Defines
#define DEFAULT_GRANULARITY     10
#define INT_2_CHAR_LEN          21

//  This macro sends generic exception info about the current object status
#define CATCHALL(Str, Cursor, Bufsize, Granularity, method) catch(...) {  \
  char buf[256]; \
  char* content = (Str);  \
  if (strlen(content) > 20) content[20] = 0;  \
  sprintf(buf, "itxString crashed: content = '%s'; m_Str = 0x%p; m_Cursor = 0x%p; m_Bufsize = %d; m_Granularity = %d; Method = %s", \
          content, (Str), (Cursor), (Bufsize), (Granularity), (method)); \
  throw buf;  \
  }



//-------------------------------------------------
//----------------    itxString   -----------------
//-------------------------------------------------

class itxString
{
private:
	//Members
  char*         m_Str;
  char*         m_Cursor;         //points to the zero termination
  unsigned int  m_Bufsize;        //bytes currently allocated
  unsigned int  m_Granularity;

public:
	//ANSI C string Methods
	static int            SubstituteSubString(char* base, char* what, char* with, char* retstr);
	static int            AdjustStr(char* base, char* retstr);
	static int            RemoveLeadingZeros(char* base, int zleft, char* retstr);
	static int            Capitalize(char* base, char* retstr);
	static int            RightInstr(char* base, char* search_arg);
	static int            MaxTokenLen(char* base, char* sep);
  static int            CompareNoCase(char* s1, char* s2);
  static int            EscapeChars(char* source, char** destination);
  static void           InBlinder(char** source, char blinder);
  static char*          UCase(char* str);
  static char*          LCase(char* str);
  static char*          LTrim(char* str);
  static char*          RTrim(char* str);
  static char*          Trim(char* str);
  static void           HexToAscii(char* source, char* destination, int src_len);
  static void           AsciiToHex(char* source, char* destination, int src_len);
  
  //Object versions
	int                   SubstituteSubString(char* what, char* with);
	int                   FindSubString(char* what, int start_from = 0);
  int                   IndexOf(char* what);
	int                   AdjustStr();
	int                   RemoveLeadingZeros(int zleft = 0);
  int                   Capitalize();
	int                   RightInstr(char* search_arg);
	int                   MaxTokenLen(char* sep);
  itxString&            UCase();
  itxString&            LCase();
  itxString&            LTrim();
  itxString&            RTrim();
  itxString&            Trim();
  itxString&            Left(int len);
  itxString&            Right(int len);
  itxString&            Mid(int start, int len);
  itxString&            RmBlankChars();

  void                  SetEmpty();
  void                  SetInt(int val);
  int                   InBlinder(char blinder);
  int                   EscapeChars();
  int                   CompareNoCase(itxString* pistr);
  int                   CompareNoCase(char* pistr);
  int                   Compare(itxString* pistr);
  int                   Compare(char* pstr);
  void                  Strcat(char* add);
  void                  Strncpy(char* src, unsigned int nchars);
  void                  PurgeSubString(char* tobepurged);
  int                   GetAt(int pos);
  void                  SetAt(char a, int pos);
  char*                 CVFromDouble(double value, int mindecimal = 1);
  void                  GetToken(char sep, int pos, itxString* dest);
  void                  Currency(int invert = 1);
  int                   Space(int len, int c = ' ');
    
  //Object management
  inline char*          GetBuffer(){return m_Str;}
  inline int            Len(){return PTR_DISTANCE(m_Cursor, m_Str);}
  inline int            IsNull(){return (m_Str == NULL ? 1 : 0);}
  inline int            IsEmpty(){if (m_Str == NULL) return 0; else return (*m_Str == 0 ? 1 : 0);}
  void                  SetGranularity(unsigned int G){m_Granularity = G;}
  void                  UpdateCursor();

  //Operator =
  void operator=(itxString& src)
  {
    try
    {
      if (src.m_Str == NULL)
      {
        if (m_Str != NULL)
        {
          *m_Str = '\0';
          m_Cursor = m_Str;
        }
      }
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
    CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "operator=(itxString& src)")
  }
  
  //Operator =
  void operator=(char c)
  {
    try
    {
      if (m_Bufsize <= 1)        
        XALLOC(m_Str, m_Cursor, m_Bufsize, m_Granularity, 2);

      *m_Str    = c;
      m_Cursor  = m_Str + (c ? 1 : 0);
      *m_Cursor = 0;
    }
    CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "operator=(char c)")
  }

  //Operator =
  void operator=(char* psrc)
  {
    try
    {
      if (psrc == NULL)
      {
        if (m_Str != NULL)
        {
          *m_Str = '\0';
          m_Cursor = m_Str;
        }
      }
      else
      {
        register unsigned int l = strlen(psrc);
        if (m_Bufsize <= l)        
          XALLOC(m_Str, m_Cursor, m_Bufsize, m_Granularity, l + 1);

        memcpy(m_Str, psrc, l);
        m_Cursor  = m_Str + l;
        *m_Cursor = 0;
      }
    }
    CATCHALL(m_Str, m_Cursor, m_Bufsize, m_Granularity, "operator=(char* psrc)")
  }

  //Operator +=
  itxString& operator+=(itxString& add)
  {
    register unsigned int l = (unsigned)add.m_Cursor - (unsigned)add.m_Str;
    if ((m_Bufsize - (unsigned)m_Cursor + (unsigned)m_Str) <= l)
      XALLOC(m_Str, m_Cursor, m_Bufsize, m_Granularity, m_Bufsize + l + 1);

    memcpy(m_Cursor, add.m_Str, l);
    m_Cursor  += l;
    *m_Cursor = 0;

    return *this;
  }

  //Operator +=
  itxString& operator+=(char* padd)
  {
    if (padd != NULL)
    {
      register unsigned int l = strlen(padd);
      if ((m_Bufsize - (unsigned)m_Cursor + (unsigned)m_Str) <= l)
        XALLOC(m_Str, m_Cursor, m_Bufsize, m_Granularity, m_Bufsize + l + 1);

      memcpy(m_Cursor, padd, l);
      m_Cursor  += l;
      *m_Cursor = 0;
    }

    return *this;
  }

  //Operator +=
  itxString& operator+=(char c)
  {
    if ((m_Bufsize - (unsigned)m_Cursor + (unsigned)m_Str) <= 1)
      MALLOC(m_Str, m_Cursor, m_Bufsize, m_Granularity);
  
    *m_Cursor = c;
    m_Cursor++;
    *m_Cursor = 0;

    return *this;
  }

  //Operator +=
  itxString& operator+=(int a)
  {
    char padd[24];

    sprintf(padd, "%d", a);
    register unsigned int l = strlen(padd);
    if ((m_Bufsize - (unsigned)m_Cursor + (unsigned)m_Str) <= l)
      XALLOC(m_Str, m_Cursor, m_Bufsize, m_Granularity, m_Bufsize + l + 1);

    memcpy(m_Cursor, padd, l);
    m_Cursor  += l;
    *m_Cursor = 0;

    return *this;
  }

	//Construction/Destruction
  itxString(char* sz, int G = DEFAULT_GRANULARITY);
  itxString(itxString& src, int G = DEFAULT_GRANULARITY);
  itxString(int G = DEFAULT_GRANULARITY);
  ~itxString();

  //InsAt
  void InsAt(char* padd, int pos) // pos = 0 means insert at the head
  {
    if (padd == NULL)
      return;

    register unsigned int len = this->Len();
    register unsigned int l = strlen(padd);
    if ((m_Bufsize - (unsigned)m_Cursor + (unsigned)m_Str) <= l)
      XALLOC(m_Str, m_Cursor, m_Bufsize, m_Granularity, m_Bufsize + l + 1);

    memmove(m_Str + pos + l, m_Str + pos, len - pos);
    memcpy(m_Str + pos, padd, l);
    m_Cursor  += l;
    *m_Cursor = 0;
  }
};


#endif // __ITXSTRING_H__

