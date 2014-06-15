/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxbuffer.h,v $
 * $Revision: 1.3 $  
 * $Author: massimo $
 * $Date: 2002-06-12 14:37:33+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */


#ifndef __ITXBUFFER_H__
#define __ITXBUFFER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "itxdefines.h"


// Defines
#define DEFAULT_GRANULARITY     10

//  This macro sends generic exception info about the current object status
#define CATCHALLBUF(Buf, Cursor, Bufsize, Granularity, method) catch(...) {  \
  char appobuf[256]; \
  sprintf(appobuf, "itxBuffer crashed: m_Buf = 0x%p; m_Cursor = 0x%p; m_Bufsize = %d; m_Granularity = %d; Method = %s", \
          (Buf), (Cursor), (Bufsize), (Granularity), (method)); \
  throw appobuf;  \
  }



//-------------------------------------------------
//----------------    itxBuffer   -----------------
//-------------------------------------------------

class itxBuffer
{
private:
	//Members
  char*         m_Buf;
  char*         m_Cursor;         //points to the end of data
  unsigned int  m_Bufsize;        //bytes currently allocated
  unsigned int  m_Granularity;

public:
  void                  SetEmpty();
  int                   Space(int len, int c = ' ');
  void                  InsAt(char* padd, unsigned int bsize, int pos);
  itxBuffer&            Append(char* padd, unsigned int bsize);
  void                  SetBuffer(char* psrc, unsigned int bsize);
    
  //Object management
  inline char*          GetBuffer(){return m_Buf;}
  inline int            Len(){return PTR_DISTANCE(m_Cursor, m_Buf);}
  inline bool           IsEmpty(){return (m_Buf == NULL);}
  inline void           SetGranularity(unsigned int G){m_Granularity = G;}
  inline void           UpdateCursor(unsigned int relpos){m_Cursor += relpos;}

	//Construction/Destruction
  itxBuffer(void* buf, int bsize, int G = DEFAULT_GRANULARITY);
  itxBuffer(itxBuffer& src, int G = DEFAULT_GRANULARITY);
  itxBuffer(int G = DEFAULT_GRANULARITY);
  ~itxBuffer();

  //Operator =
  void operator=(itxBuffer& src)
  {
    try
    {
      if (src.m_Buf == NULL)
        SetEmpty();
      else
      {
        register unsigned int l = PTR_DISTANCE(src.m_Cursor, src.m_Buf);
        if (m_Bufsize <= l)
          XALLOC(m_Buf, m_Cursor, m_Bufsize, m_Granularity, l);

        memcpy(m_Buf, src.m_Buf, l);
        m_Cursor  = m_Buf + l;
      }
    }
    CATCHALLBUF(m_Buf, m_Cursor, m_Bufsize, m_Granularity, "operator=(itxBuffer& src)")
  }
  
  //Operator +=
  itxBuffer& operator+=(itxBuffer& add)
  {
    try
    {
      register unsigned int l = PTR_DISTANCE(add.m_Cursor, add.m_Buf);
      if ((m_Bufsize - (unsigned)m_Cursor + (unsigned)m_Buf) <= l)
        XALLOC(m_Buf, m_Cursor, m_Bufsize, m_Granularity, m_Bufsize + l);

      memcpy(m_Cursor, add.m_Buf, l);
      m_Cursor  += l;
    }
    CATCHALLBUF(m_Buf, m_Cursor, m_Bufsize, m_Granularity, "operator+=(itxBuffer& add)")

    return *this;
  }
};


#endif // __ITXBUFFER_H__

