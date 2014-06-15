/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#ifndef _T_N_T_H_
#define _T_N_T_H_


/////////////////////////// AbstractCommand

class AbstractCommand
{
public:
  virtual char* GetName()        = 0;
  virtual char* Execute(char*)   = 0;
  virtual void  Deallocate()     = 0;

  AbstractCommand(){};
  ~AbstractCommand(){};
};


/////////////////////////// TNTAPI 

class TNTAPI
{
public: 
  // CGIResolver
  virtual int   _GetPRMValue(char* param_name, char* param_value, int* bufdim) = 0;
  virtual int   _GetQueryStringVal(char* var_name, char* var_value, int* bufdim) = 0;

  // Parser
  virtual int   _PickInt(char* inputstr, int par_pos, int* retval) = 0;
  virtual int   _PickDouble(char* inputstr, int par_pos, double* retval) = 0;
  virtual int   _PickString(char* inputstr, int position, char* retval, int* bufdim) = 0;
  virtual int   _PICKSTRING(char* inputstr, int par_pos, void* retstr) = 0;
  
  // TQRManager
  virtual void* _TQRCreate(char* queryname, int numfields) = 0;
  virtual void  _TQRLoadDataBuffer(char* tqrname, int tqrcols, char recsep, char fieldsep, char* buffer) = 0;
  virtual bool  _TQRExist(char* tqrname) = 0;
  virtual void  _TQRRewind(char* tqrname) = 0;
  virtual void  _TQRMoveFirst(char* tqrname) = 0;
  virtual void  _TQRMoveNext(char* tqrname) = 0;
  virtual void  _TQRMoveLast(char* tqrname) = 0;
  virtual void  _TQRMoveTo(char* tqrname, int irow) = 0;
  virtual char* _TQRGetField(char* tqrname, char* colname) = 0;
  virtual void  _TQRDelete(char* tqrname) = 0;
  virtual void  _TQRFilter(char* source, char* field, char* value, char* destination) = 0;
  virtual int   _TQRRecordCount(char* source) = 0;


  // TQRODBCManager
  virtual void  _ODBCConnect(char* dsn, char* uid, char* pwd) = 0;
  virtual int   _ODBCExecute(char* tqrname, char* query, int firstRec, int recsToStore) = 0;
  virtual void* _ODBCGetCurrentConnection() = 0;

  TNTAPI(){};
  ~TNTAPI(){};
};


extern TNTAPI*  g_pTNTAPI; // Tannit hook


//------------------------------------------------------------------
//------------ Tannit Extension Module Facilities ------------------
//------------------------------------------------------------------

#ifdef WIN32
  #define TNTEXPORT __declspec(dllexport)
#endif

// ******************************
// **** BEGIN_TANNIT_COMMAND_LIST 
// ******************************

#define BEGIN_TANNIT_COMMAND_LIST \
TNTAPI*    g_pTNTAPI; \
void TNTEXPORT TannitHandshake(AbstractCommand** ppCommands, TNTAPI* pTNTAPI)  \
{int i = 0;

// ******************************
// **** END_TANNIT_COMMAND_LIST 
// ******************************

#define END_TANNIT_COMMAND_LIST   g_pTNTAPI = pTNTAPI;}

// ******************************
// **** TANNIT_COMMAND
// ******************************

#define TANNIT_COMMAND(cmd)       ppCommands[i++] = (AbstractCommand*)&(cmd);



//------------------------------------------------------------------
//------------ Tannit Application Programming Interfaces -----------
//------------------------------------------------------------------

#define TNTPickInt(inputstr, par_pos, pint)               \
        g_pTNTAPI->_PickInt((inputstr), (par_pos), (pint))

#define TNTPickDouble(inputstr, par_pos, pdouble)               \
        g_pTNTAPI->_PickDouble((inputstr), (par_pos), (pdouble))

#define TNTPickString(inputstr, par_pos, retstr, bufdim)  \
        g_pTNTAPI->_PickString((inputstr), (par_pos), (retstr), (bufdim))

#define TNTPRMValue(param_name, param_value, bufdim)  \
        g_pTNTAPI->_GetPRMValue((param_name), (param_value), (bufdim))

#define TNTCreateTQR(queryname, numfields) \
        g_pTNTAPI->_TQRCreate((queryname), (numfields))

#define TNTLoadDataBuffer(tqrname, tqrcols, recsep, fieldsep, buffer) \
        g_pTNTAPI->_TQRLoadDataBuffer(tqrname, tqrcols, recsep, fieldsep, buffer)

#define TNT_TQRExist(tqrname) \
        g_pTNTAPI->_TQRExist(tqrname)

#define TNTRewind(tqrname) \
        g_pTNTAPI->_TQRRewind(tqrname)

#define TNTMoveFirst(tqrname) \
        g_pTNTAPI->_TQRMoveFirst(tqrname)

#define TNTMoveNext(tqrname) \
        g_pTNTAPI->_TQRMoveNext(tqrname)

#define TNTMoveLast(tqrname) \
        g_pTNTAPI->_TQRMoveLast(tqrname)

#define TNTMoveTo(tqrname, irow) \
        g_pTNTAPI->_TQRMoveTo(tqrname, irow)

#define TNTGetField(tqrname, colname) \
        g_pTNTAPI->_TQRGetField(tqrname, colname)

#define TNTDelete(tqrname) \
        g_pTNTAPI->_TQRDelete(tqrname)

#define TNTFilter(source, field, value, destination) \
        g_pTNTAPI->_TQRFilter(source, field, value, destination)

#define TNTCount(tqrname) \
        g_pTNTAPI->_TQRRecordCount(tqrname)

#define TNTConnectODBC(dsn, uid, pwd)  \
        g_pTNTAPI->_ODBCConnect((dsn), (uid), (pwd))

#define TNTExecuteODBC(tqrname, query, firstRec, recsToStore)  \
        g_pTNTAPI->_ODBCExecute((tqrname), (query), (firstRec), (recsToStore))

#define TNTGetCurrentConnection()  \
        g_pTNTAPI->_ODBCGetCurrentConnection()

#define TNTGetQueryStringVal(var_name, var_value, bufdim)  \
        g_pTNTAPI->_GetQueryStringVal((var_name), (var_value), (bufdim))

#define TNTPICKSTRING(inpustr, par_pos, retstr)          \
        g_pTNTAPI->_PICKSTRING((inpustr), (par_pos), (void*)(&retstr))


#endif /* _T_N_T_H_ */

