/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxbuffer.cpp,v $
 * $Revision: 1.1 $  
 * $Author: massimo $
 * $Date: 2002-06-11 15:57:58+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               © aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 *               developed by mr.blue
 */

#include "itxbuffer.h"


/*
 *  Costruzione/distruzione e funzioni ausiliarie
 */
itxBuffer::itxBuffer(int G)
{
  try
  {
    m_Buf         = NULL;
    m_Cursor      = NULL;
    m_Bufsize     = 0;
    m_Granularity = G;
  }
  CATCHALLBUF(m_Buf, m_Cursor, m_Bufsize, m_Granularity, "itxBuffer::itxBuffer(int G)")
}

itxBuffer::itxBuffer(void* buf, int bsize, int G)
{
  try
  {
    m_Buf         = NULL;
    m_Cursor      = NULL;
    m_Bufsize     = 0;
    m_Granularity = G;

    if (buf != NULL)
    {
      XALLOC(m_Buf, m_Cursor, m_Bufsize, m_Granularity, bsize);
      memcpy(m_Buf, buf, bsize);
      m_Cursor  = m_Buf + bsize;
    }
  }
  CATCHALLBUF(m_Buf, m_Cursor, m_Bufsize, m_Granularity, "itxBuffer::itxBuffer(void* buf, int bsize, int G)")
}

itxBuffer::itxBuffer(itxBuffer& src, int G)
{
  try
  {
    m_Buf         = NULL;
    m_Cursor      = NULL;
    m_Bufsize     = 0;
    m_Granularity = G;

    if (src.m_Buf != NULL)
    {
      register unsigned int l = PTR_DISTANCE(src.m_Cursor, src.m_Buf);
      if (m_Bufsize <= l)
        XALLOC(m_Buf, m_Cursor, m_Bufsize, m_Granularity, l);

      memcpy(m_Buf, src.m_Buf, l);
      m_Cursor  = m_Buf + l;
    }
  }
  CATCHALLBUF(m_Buf, m_Cursor, m_Bufsize, m_Granularity, "itxBuffer::itxBuffer(itxBuffer& src, int G)")
}

itxBuffer::~itxBuffer()
{
  try
  {
    ITXFREE(m_Buf);
  }
  CATCHALLBUF(m_Buf, m_Cursor, m_Bufsize, m_Granularity, "itxBuffer::itxBuffer(itxBuffer& src, int G)")
//  CATCH_TO_NOTHING
}

int itxBuffer::Space(int len, int c)
{
  try
  {
    if (len <= 0)
      return (m_Buf == NULL ? 0 : 1);

    if ((int)m_Bufsize < len)
      XALLOC(m_Buf, m_Cursor, m_Bufsize, m_Granularity, len);

    memset(m_Buf, c, len);
  }
  CATCHALLBUF(m_Buf, m_Cursor, m_Bufsize, m_Granularity, "itxBuffer::Space(int len, int c)")
  
  return (m_Buf == NULL ? 0 : 1);
}

/*****************************************************************************
 - FUNCTION NAME: SetEmpty
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
 -----------------------------------------------------------------------------
 - RETURN VALUE:
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:
 *****************************************************************************/
void itxBuffer::SetEmpty()
{
  ITXFREE(m_Buf); 
  m_Buf         = NULL;
  m_Cursor      = NULL;
  m_Bufsize     = 0;
}

/*****************************************************************************
 - FUNCTION NAME: SetBuffer
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
 -----------------------------------------------------------------------------
 - RETURN VALUE:
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:
 *****************************************************************************/
void itxBuffer::SetBuffer(char* psrc, unsigned int bsize)
{
  try
  {
    if (psrc == NULL)
      SetEmpty();
    else
    {
      if (m_Bufsize <= bsize)        
        XALLOC(m_Buf, m_Cursor, m_Bufsize, m_Granularity, bsize);

      memcpy(m_Buf, psrc, bsize);
      m_Cursor  = m_Buf + bsize;
    }
  }
  CATCHALLBUF(m_Buf, m_Cursor, m_Bufsize, m_Granularity, "operator=(char* psrc, int bsize)")
}

/*****************************************************************************
 - FUNCTION NAME: AddTail
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
 -----------------------------------------------------------------------------
 - RETURN VALUE:
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:
 *****************************************************************************/
itxBuffer& itxBuffer::Append(char* padd, unsigned int bsize)
{
  try
  {
    if (padd != NULL)
    {
      if ((m_Bufsize - (unsigned)m_Cursor + (unsigned)m_Buf) <= bsize)
        XALLOC(m_Buf, m_Cursor, m_Bufsize, m_Granularity, m_Bufsize + bsize);

      memcpy(m_Cursor, padd, bsize);
      m_Cursor  += bsize;
    }
  }
  CATCHALLBUF(m_Buf, m_Cursor, m_Bufsize, m_Granularity, "operator+=(char* padd, int bsize)")

  return *this;
}

/*****************************************************************************
 - FUNCTION NAME: InsAt
 -----------------------------------------------------------------------------
 - INPUT PARAMETERS:
 -----------------------------------------------------------------------------
 - RETURN VALUE:
 -----------------------------------------------------------------------------
 - ACTION DESCRIPTION:
 *****************************************************************************/
void itxBuffer::InsAt(char* padd, unsigned int bsize, int pos) // pos = 0 means insert at the head
{
  try
  {
    if (padd == NULL)
      return;

    if ((m_Bufsize - (unsigned)m_Cursor + (unsigned)m_Buf) <= bsize)
      XALLOC(m_Buf, m_Cursor, m_Bufsize, m_Granularity, m_Bufsize + bsize);

    memmove(m_Buf + pos + bsize, m_Buf + pos, PTR_DISTANCE(m_Cursor, m_Buf) - pos);
    memcpy(m_Buf + pos, padd, bsize);
    m_Cursor  += bsize;
  }
  CATCHALLBUF(m_Buf, m_Cursor, m_Bufsize, m_Granularity, "operator+=(char* padd, int bsize)")
}

