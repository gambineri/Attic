/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxexception.h,v $
 * $Revision: 1.2 $  
 * $Author: massimo $
 * $Date: 2001-04-20 19:03:11+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */
#ifndef __ITX_EXCEPTION_H__
#define __ITX_EXCEPTION_H__

#include "itxstring.h"

#define ITXE_ERRLEN             20
#define ITXE_PROCLEN            128


class itxException : public exception
{
public:
  int       m_errornumber;
  itxString m_procedure;

  itxException(){m_procedure.SetEmpty();}
  itxException(int errornumber, char* procedure)
  {
    m_errornumber = errornumber;
    m_procedure = procedure;
  }
};

#endif // __ITX_EXCEPTION_H__