/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxcoll.h,v $
 * $Revision: 1.11 $  
 * $Author: massimo $
 * $Date: 2002-06-07 11:04:25+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */


#ifndef __ITXCOLL_H__
#define __ITXCOLL_H__

#include <stdio.h>
#include <stdlib.h>

typedef struct PtrStack
{
	void*             item;
	struct PtrStack*  under;
	struct PtrStack*  over;
} PtrStack_t;


class itxStack
{
private:
	PtrStack_t*   m_Stack;
	PtrStack_t*   m_Bottom;
	PtrStack_t*   m_Cursor;

public:
  int           Push(void* item);
  int           Pop(void** item);
  void*         GetCurrent(){return (m_Cursor != NULL ? m_Cursor->item : NULL);}
  inline void*  Top(){return (m_Stack != NULL ? m_Stack->item : NULL);}
  inline void*  Bottom(){return (m_Bottom != NULL ? m_Bottom->item : NULL);}
  inline void*  NextUp(){m_Cursor = m_Cursor->over; return (m_Cursor != NULL ? m_Cursor->item : NULL);}
  inline void*  NextDown(){m_Cursor = m_Cursor->under; return (m_Cursor != NULL ? m_Cursor->item : NULL);}
  inline void   SetCursorTop(){m_Cursor = m_Stack;}
  inline void   SetCursorBottom(){m_Cursor = m_Bottom;}

  itxStack();
  ~itxStack();
};


//////////////////
//class itxListPtr

typedef struct PtrElem
{
	void*            item;
	struct PtrElem*  next;
	struct PtrElem*  prev;
} PtrElem_t;

class itxListPtr
{
//MEMBERS
private:
  PtrElem_t*  m_Head;
  PtrElem_t*  m_Tail;
  PtrElem_t*  m_Cursor;
  int         m_Count;
  
//METHODS
private:
  PtrElem_t*  GetAt(int pos);

public:
  bool        Append(void* item);
  bool        Remove(int pos);
  bool        Insert(void* item, int pos);
  bool        Empty();
  void*       GetElement(int pos);
  int         GetCount(){return m_Count;}

  void*       GetHead(){m_Cursor = m_Head; return (m_Cursor ? m_Cursor->item : NULL);}
  void*       GetTail(){m_Cursor = m_Tail; return (m_Cursor ? m_Cursor->item : NULL);}
  void*       GetNext(){if (m_Cursor) m_Cursor = m_Cursor->next; return (m_Cursor ? m_Cursor->item : NULL);}
  void*       GetPrev(){if (m_Cursor) m_Cursor = m_Cursor->prev; return (m_Cursor ? m_Cursor->item : NULL);}

  itxListPtr();
  ~itxListPtr();
};



#endif  //__ITXCOLL_H__