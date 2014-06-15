/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxfileini.h,v $
 * $Revision: 1.4 $  
 * $Author: massimo $
 * $Date: 2002-06-07 11:05:03+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */

#ifndef _ITXFILEINI_H_
#define _ITXFILEINI_H_


#include "itxstring.h"


//defines
#define MAX_PARS          1000    //Max num of params in a ini file
#define INI_LINE_LEN      512     //Max len of ini file line     


class itxFileINI
{
public:
//MEMBERS
  itxString   m_Names[MAX_PARS];
  itxString   m_Values[MAX_PARS];

private:
	itxString	  m_FileName;     //Nome del file di inizializzazione con l'estensione.
  int         m_NumParams;

public:
//METHODS
  bool        Read(char* inifile);
  bool        GetINIValue(char* param_name, itxString* param_value);
  int         GetCount(){ return m_NumParams;}


public:
//CONSTRUCTION-DESTRUCTION
  itxFileINI() 
  {
    m_FileName = "";
    m_NumParams = 0;
  };
  ~itxFileINI(){}
};


#endif  /* _ITXFILEINI_H_ */

