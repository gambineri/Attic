/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: teg.h,v $
 * $Revision: 1.0 $  
 * $Author: fabio $
 * $Date: 2001-01-09 19:11:09+01 $
 *
 * Tannit Extension Manager Command Definition
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */

#include "tnt.h"
#include "itxlib.h"

class teg_header : public AbstractCommand
{
public:
  itxString  m_Name;
  itxString  m_Output;

  teg_header(char* name) { m_Name = name; }
  ~teg_header() {}

  char* Execute(char* inputstr);
  char* GetName(){ return m_Name.GetBuffer(); }

  void  Deallocate(){}
};

class teg_load : public AbstractCommand
{
public:
  itxString  m_Name;

  teg_load(char* name) { m_Name = name; }
  ~teg_load() {}

  char* Execute(char* inputstr);
  char* GetName(){ return m_Name.GetBuffer(); }

  void  Deallocate(){}
};

