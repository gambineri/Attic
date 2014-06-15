/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/*
 | _-.-..-._-_-._--_-._.-._     AITECSA S.r.l.  2000
 |
 |
 | FILENAME    : tntapiimpl.h
 | TAB         : 2 spaces
 |
 | DESCRIPTION : External modules interface object declaration: 
 |               the manager of all exposed Tannit resources.
 |
 |
 */

#ifndef _TNTAPIIMPL_H_
#define _TNTAPIIMPL_H_

#include "tnt.h"
#include "parser.h"
#include "tqr.h"
#include "tqrodbc.h"
#include "cgiresolver.h"




/*****************************************************************************
                              TNTAPIImpl
 -----------------------------------------------------------------------------
  This is the object used to export ALL the resources toward every 
  Tannit addon module.
 *****************************************************************************/
class TNTAPIImpl : public TNTAPI
{
public:
  Parser*         m_pParser;
  TQRManager*     m_pTQRManager;
  TQRODBCManager* m_pTQRODBCManager;
  CGIResolver*    m_pCGIResolver;

//CGIResolver Interface
  int   _GetPRMValue(char* param_name, char* param_value, int* bufdim);
  int   _GetQueryStringVal(char* var_name, char* var_value, int* bufdim);

// Parser Interface
  int   _PickInt(char* inputstr, int par_pos, int* retval);
  int   _PickDouble(char* inputstr, int par_pos, double* retval);
  int   _PickString(char* inputstr, int position, char* retval, int* bufdim);
  int   _PICKSTRING(char* inputstr, int par_pos, void* retstr);

// TQRManager Interface
  void* _TQRCreate(char* queryname, int numfields);
  void  _TQRLoadDataBuffer(char* tqrname, int tqrcols, char recsep, char fieldsep, char* buffer);
  bool  _TQRExist(char* tqrname);
  void  _TQRRewind(char* tqrname);
  void  _TQRMoveFirst(char* tqrname);
  void  _TQRMoveNext(char* tqrname);
  void  _TQRMoveLast(char* tqrname);
  void  _TQRMoveTo(char* tqrname, int irow);
  char* _TQRGetField(char* tqrname, char* colname);
  void  _TQRDelete(char* tqrname);
  void  _TQRFilter(char* source, char* field, char* value, char* destination);
  int   _TQRRecordCount(char* source);

// TQRODBCManager Interface
  void  _ODBCConnect(char* dsn, char* uid, char* pwd);
  int   _ODBCExecute(char* tqrname, char* query, int firstRec = STARTING_ROW, int recsToStore = ROWS_TO_STORE);
  void* _ODBCGetCurrentConnection();

  TNTAPIImpl(void* tannit);
  TNTAPIImpl();
  ~TNTAPIImpl();
};


#endif  /* _TNTAPIIMPL_H_ */




