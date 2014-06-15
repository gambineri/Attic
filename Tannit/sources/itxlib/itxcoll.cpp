/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxcoll.cpp,v $
 * $Revision: 1.9 $  
 * $Author: massimo $
 * $Date: 2002-06-10 15:04:20+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */

#include "itxcoll.h"
#include "itxdefines.h"



itxStack::itxStack()
{
  m_Stack   = NULL;
  m_Bottom  = NULL;
	m_Cursor  = NULL;
}

itxStack::~itxStack()
{
  try
  {
    void* p;
    Pop(&p);
    while (p != NULL)
      Pop(&p);

    m_Stack = NULL;
  }
  GENERIC_CATCHALL
}

int itxStack::Push(void* item)
{
	PtrStack_t*   new_element;
  
  try
  {
	  if ((new_element = (PtrStack_t*) malloc(sizeof(PtrStack_t))) == NULL)
		  return 0;

	  new_element->item   = item;
	  new_element->under  = m_Stack;
    new_element->over   = NULL;

    if (m_Stack == NULL)
      m_Bottom = new_element;
    else
      m_Stack->over = new_element;

	  m_Stack = new_element;
  }
  GENERIC_CATCHALL

	return 1;
}

int itxStack::Pop(void** item)
{
	PtrStack_t* appo;

	if (m_Stack == NULL)
	{
    if (item != NULL)
		  *item = NULL;
		return 0;
	}
  
  try
  {
	  appo = m_Stack->under;
    if (item != NULL)
  	  *item = m_Stack->item;
    free(m_Stack);
	  m_Stack = appo;
    if (m_Stack != NULL)
      m_Stack->over = NULL;
  }
  GENERIC_CATCHALL

	return 1;
}



//////////////////
//class itxListPtr


itxListPtr::itxListPtr()
{
  m_Head    = NULL;
  m_Tail    = NULL;
  m_Count   = 0;
}

itxListPtr::~itxListPtr()
{
  try
  {
    Empty();
  }
  GENERIC_CATCHALL
}

PtrElem_t* itxListPtr::GetAt(int pos)
{
  if (pos < 0 || pos >= m_Count)
    return NULL;
  
  int curpos = 0;
  PtrElem_t* current = m_Head;
  while (curpos < pos)
  {
    current = current->next;
    curpos++;
  }

  return current;
}

bool  itxListPtr::Append(void* item)
{
  PtrElem_t* new_element;
  PtrElem_t* currel;

  try
  {
	  if ((new_element = (PtrElem_t*)malloc(sizeof(PtrElem_t))) == NULL)
		  return false;

    if (currel = GetAt(m_Count - 1))
      currel->next = new_element;

	  new_element->item = item;
    new_element->next = NULL;
	  new_element->prev = currel;
    
    if (m_Head == NULL)
	    m_Head = new_element;

    m_Tail = new_element;
    m_Count++;
  }
  GENERIC_CATCHALL

	return true;
}

bool itxListPtr::Remove(int pos)
{
  PtrElem_t* prevel;
  PtrElem_t* currel;
  PtrElem_t* nextel;
  int        i = 0;

  try
  {
    if (pos == 0)
    {
      nextel = m_Head->next;
      free(m_Head);
      m_Head = nextel;
      m_Head->prev = NULL;
      m_Count--;
    }
    else if(pos == m_Count - 1)
    {
      prevel = m_Tail->prev;
      free(m_Tail);
      m_Tail = prevel;
      m_Tail->next = NULL;
      m_Count--;
    }
    else if (currel = GetAt(pos))
    {      
      if (prevel = currel->prev)
        prevel->next = currel->next;
      
      if (nextel = currel->next)
        nextel->prev = prevel;

      free(currel);
      m_Count--;
    }
    else
      return false;
  }
  GENERIC_CATCHALL

  return true;
}

bool itxListPtr::Insert(void* item, int pos)
{
  PtrElem_t* prevel;
  PtrElem_t* currel;
  PtrElem_t* new_element;
  int        i = 0;

  try
  {
    if (currel = GetAt(pos))
    {
	    if ((new_element = (PtrElem_t*)malloc(sizeof(PtrElem_t))) == NULL)
		    return false;

	    new_element->item = item;
      new_element->next = currel;
	    new_element->prev = currel->prev;
      
      prevel = currel->prev;
      currel->prev = new_element;

      if (prevel == NULL) //pos == 0
        m_Head = new_element;
      else
        prevel->next = new_element;

      m_Count++;
    }    
    else
      return false;
  }
  GENERIC_CATCHALL

  return true;
}

bool itxListPtr::Empty()
{
/* DUMP POINTERS FOR DEBUG
  PtrElem_t* el = (PtrElem_t*)m_Head;

  while (el)
  {
    printf("%p - %p - %p\n", el->prev, el, el->next);
    el = el->next;
  }
//*/

  PtrElem_t* currel = m_Tail;
  PtrElem_t* tobedel;

  try
  {
    while (currel)
    {
      tobedel = currel;
      currel = currel->prev;
      free(tobedel);
    }

    m_Head    = NULL;
    m_Tail    = NULL;
    m_Count   = 0;
  }
  catch(...)
  {
    return false;
  }

  return true;
}

void* itxListPtr::GetElement(int pos)
{
  PtrElem_t* pelem = GetAt(pos);
  return (pelem ? pelem->item : NULL);
}

