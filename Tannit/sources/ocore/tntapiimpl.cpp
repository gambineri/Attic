/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/*
 | _-.-..-._-_-._--_-._.-._     AITECSA S.r.l.  2000
 |
 |
 | FILENAME    : tntapiimpl.cpp
 | TAB         : 2 spaces
 |
 | DESCRIPTION : External modules interface: implementation file.
 |               
 |
 |
 */


#include "tannitobj.h"
#include "tntapiimpl.h"
#include "auxfile.h"


/////////////////////////// CONSTRUCTION - DESTRUCTION

TNTAPIImpl::TNTAPIImpl(void* tannit)
{
  Tannit* ptannit = (Tannit*)tannit;

  m_pParser         = &ptannit->m_Parser;
  m_pTQRManager     = &ptannit->m_TQRManager;
  m_pTQRODBCManager = &ptannit->m_TQRODBCManager;
  m_pCGIResolver    = (CGIResolver*)ptannit;
}

TNTAPIImpl::TNTAPIImpl()
{
}

TNTAPIImpl::~TNTAPIImpl()
{
}



//*************************** EXPORT METHODS ***************************



/////////////////////////// TNTPickInt
int TNTAPIImpl::_PickInt(char* inputstr, int par_pos, int* retval)
{
  int ret = 0;
  
  try 
  {
    ret = m_pParser->PickInt(inputstr, par_pos, retval);
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNTPickInt(%p, %d, %p);'\n", inputstr, par_pos, retval);
  }

  return ret;
}

/////////////////////////// TNTPickDouble
int TNTAPIImpl::_PickDouble(char* inputstr, int par_pos, double* retval)
{
  int ret = 0;
  
  try 
  {
    ret = m_pParser->PickDouble(inputstr, par_pos, retval);
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNTPickInt(%p, %d, %p);'\n", inputstr, par_pos, retval);
  }

  return ret;
}

/////////////////////////// TNTPickString
int TNTAPIImpl::_PickString(char* inputstr, int position, char* retval, int* bufdim)
{  
  int ret = 0;
  
  try 
  {
    ret = m_pParser->PickString(inputstr, position, retval, bufdim);
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNTPickString(%p, %d, %p, %p);'\n", inputstr, position, retval, bufdim);
  }

  return ret;
}

int TNTAPIImpl::_PICKSTRING(char* inputstr, int par_pos, void* retstr)
{
  itxString* a = (itxString*) retstr;
  if (m_pParser->PickPar(inputstr, par_pos, a) == PARAM_NOT_FOUND)
  {
    a->SetEmpty();
    return 0;
  }
  return 1;
}

/////////////////////////// TNTPRMValue
int TNTAPIImpl::_GetPRMValue(char* param_name, char* param_value, int* bufdim)
{
  int ret = 0;
  
  try 
  {
    if (m_pCGIResolver->m_PRMFile.GetPRMValue(param_name, param_value, bufdim))
      ret = 1;
    else 
    {
      DebugTrace2(IN_WARNING, "TNTPRMValue failed: can't find '%s' variable.\n", param_name);
    }
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNTPRMValue(%s, %p, %p);'\n", param_name, param_value, bufdim);
  }

  return ret;
}

/////////////////////////// TNTCreateTQR
void* TNTAPIImpl::_TQRCreate(char* queryname, int numfields) 
{ 
  void* ret = 0;
  
  try 
  {
    ret = (void*) m_pTQRManager->CreateTQR(queryname, numfields); 
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNTCreateTQR(%s, %d);'\n", queryname, numfields);
  }

  if (ret == NULL)
    DebugTrace2(IN_WARNING, "TNTCreateTQR(%s, %d) failed.\n", queryname, numfields);

  return ret;
}

/////////////////////////// TNTLoaddataBuffer
void TNTAPIImpl::_TQRLoadDataBuffer(char* tqrname, int tqrcols, char recsep, char fieldsep, char* buffer) 
{ 
  try 
  {
    m_pTQRManager->LoadDataBuffer(tqrname, tqrcols, recsep, fieldsep, buffer); 
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNTLoadDataBuffer(%s, %d, %c, %c, %s);'\n", 
                tqrname, tqrcols, recsep, fieldsep, buffer);
  }

  return;
}

/////////////////////////// TNTTQRExist
bool TNTAPIImpl::_TQRExist(char* tqrname) 
{ 
  int result = 0;
  
  try 
  {
    result = m_pTQRManager->Exist(tqrname); 
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNT_TQRExist(%s);'\n", 
                tqrname);
  }

  return result == TQR_NOT_EXIST ? false : true;
}

/////////////////////////// TNTRewind
void TNTAPIImpl::_TQRRewind(char* tqrname) 
{ 
  try 
  {
    m_pTQRManager->MoveFirst(m_pTQRManager->Get(tqrname)); 
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNTRewind(%s);'\n", 
                tqrname);
  }
}

/////////////////////////// TNTMoveFirst
void TNTAPIImpl::_TQRMoveFirst(char* tqrname) 
{ 
  try 
  {
    m_pTQRManager->MoveFirst(m_pTQRManager->Get(tqrname)); 
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNTMoveFirst(%s);'\n", 
                tqrname);
  }
}

/////////////////////////// TNTMoveNext
void TNTAPIImpl::_TQRMoveNext(char* tqrname) 
{ 
  try 
  {
    m_pTQRManager->MoveNext(m_pTQRManager->Get(tqrname)); 
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNTMoveNext(%s);'\n", 
                tqrname);
  }
}

/////////////////////////// TNTMoveLast
void TNTAPIImpl::_TQRMoveLast(char* tqrname) 
{ 
  try 
  {
    m_pTQRManager->MoveLast(m_pTQRManager->Get(tqrname)); 
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNTMoveLast(%s);'\n", 
                tqrname);
  }
}

/////////////////////////// TNTMoveTo
void TNTAPIImpl::_TQRMoveTo(char* tqrname, int irow) 
{ 
  try 
  {
    m_pTQRManager->MoveTo(m_pTQRManager->Get(tqrname), irow); 
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNTMoveTo(%s);'\n", 
                tqrname);
  }
}

/////////////////////////// TNTGetField
char* TNTAPIImpl::_TQRGetField(char* tqrname, char* colname) 
{ 
  char* field = NULL;
  try 
  {
    field = m_pTQRManager->GetCurrentRecordField(m_pTQRManager->Get(tqrname), colname); 
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNTMoveTo(%s);'\n", 
                tqrname);
  }

  return field;
}

/////////////////////////// TNTDelete
void TNTAPIImpl::_TQRDelete(char* tqrname) 
{ 
  try 
  {
    m_pTQRManager->Remove(tqrname); 
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNTDelete(%s);'\n", 
                tqrname);
  }
}

/////////////////////////// TNTFilter
void TNTAPIImpl::_TQRFilter(char* source, char* field, char* value, char* destination) 
{ 
  try 
  {
    TQR* qres = m_pTQRManager->Filter(source, field, value, destination);
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNTFilter(%s, %s, %s, %s);'\n", 
                source, field, value, destination);
  }
}

/////////////////////////// TNTCount
int TNTAPIImpl::_TQRRecordCount(char* tqrname) 
{ 
  try 
  {
    return m_pTQRManager->GetRecordNumber(tqrname);
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNTCount(%s);'\n", 
                tqrname);
  }
  return -1;
}

/////////////////////////// TNTConnectODBC
void TNTAPIImpl::_ODBCConnect(char* dsn, char* uid, char* pwd)
{
  try 
  {
    m_pTQRODBCManager->Connect(dsn, uid, pwd);
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNTConnectODBC(%s, %s, %s);'\n", dsn, uid, pwd);
  }
}

/////////////////////////// TNTExecuteODBC
int TNTAPIImpl::_ODBCExecute(char* tqrname, char* query, int firstRec, int recsToStore)
{
  int ret = 0;
  
  try 
  {
    ret = m_pTQRODBCManager->Execute(tqrname, query, firstRec, recsToStore);
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNTExecuteODBC(%s, %s, %d, %d);'\n", tqrname, query, firstRec, recsToStore);
  }

  return ret;
}

/////////////////////////// TNTGetCurrentConnection
void* TNTAPIImpl::_ODBCGetCurrentConnection()
{
  try 
  {
    return (void*)m_pTQRODBCManager->GetConnection();
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNTGetCurrentConnection();'\n");
  }

  return NULL;
}

/////////////////////////// TNTGetQueryStringVal
int TNTAPIImpl::_GetQueryStringVal(char* var_name, char* var_value, int* bufdim)
{
  int ret = 0;
  int needed;

  try 
  {
    if (m_pCGIResolver->cgiFormStringSpaceNeeded(var_name, &needed) == cgiFormSuccess)
    {
      if (var_value == NULL)
      {
        ret = 1;
        *bufdim = needed;
      }
      else if (*bufdim >= needed)
      {
	      if (m_pCGIResolver->cgiFormString(var_name, var_value, 0) == cgiFormSuccess)
          ret = 1;
        else
          DebugTrace2(IN_WARNING, "TNTGetQueryStringVal failed: supplied buffer too small.\n");
      }
    }
    else
      DebugTrace2(IN_WARNING, "TNTGetQueryStringVal failed: can't find requested variable.\n");
  }
  catch(...)
  {
    DebugTrace2(IN_TNTAPI, "TNTAPI EXCEPTION - actual call is 'TNTGetQueryStringVal(%s, %p, %p);'\n", var_name, var_value, bufdim);
  }

  return ret;

}





