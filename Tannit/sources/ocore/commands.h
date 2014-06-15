/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/*
 | _-.-..-._-_-._--_-._.-._     AITECSA S.r.l.   2000
 |
 |
 | FILENAME    : commands.h
 | TAB         : 2 spaces
 |
 | DESCRIPTION : Base commands declarations.
 |
 |
 */


#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "tnt.h"
#include "defines.h"
#include "cgiresolver.h"
#include "parser.h"
#include "tqrodbc.h"

#include "itxlib.h"




                        /******************
                           COMMAND NAMES
                         ******************/


#define ARRAY                           "array"
#define ARRAYGET                        "arrayget"
#define ARRAYSET                        "arrayset"
#define CHECKFORBIDDENCHARS             "cfc"
#define CGIPATH                         "cgipath"
#define CONSOLE                         "console"
#define COPYFILE                        "copyf"
#define CRYPT                           "crypt"
#define CURRENCY                        "currency"
#define CYCLETQR                        "cycle"
#define DECRYPT                         "decrypt"
#define DMYTIME                         "dmytime"
#define ALTERNATE_CONDITIONAL_BLOCK     "else"
#define ELSIF_CONDITIONAL_BLOCK         "elsif"
#define MAIL                            "email"
#define ENDCYCLETQR                     "endcycle"
#define ENDCYCLEFOR                     "endfor"
#define END_CONDITIONAL_BLOCK           "endif"
#define ODBC_EXECUTE_QUERY              "exeq"
#define TQREXIST                        "exist"
#define EXIT                            "exit"
#define FLUSH                           "flush"
#define FORMATCURRENCY                  "fmtcur"
#define CYCLEFOR                        "for"
#define FORINDEX                        "foridx"
#define GETVALUE                        "get"
#define GETCOOKIE                       "getcoo"
#define GETENV                          "getenv"
#define GETHIDE                         "gethide"
#define GETVALUECAST                    "getcast"
#define GETVALUEQUOTE                   "getq"
#define GETVAR                          "getvar"
#define START_CONDITIONAL_BLOCK         "if"
#define LCASE                           "lcase"
#define LEFT                            "left"
#define MAKEOP                          "makeop"
#define MID                             "mid"
#define NEWCONNECTION                   "newconn"
#define NOW                             "now"
#define NETPREX                         "netprex"
#define NETGRAB                         "netgrab"
#define NUMERICAL_COMPARISON            "numcmp"
#define PAGER                           "pager"
#define GETPOSTBODY                     "postbody"
#define PROCESS_EXTERN_FILE             "prex"
#define PRMVALUE                        "prmval"
#define PROCESS_MEMORY_BUFFER           "proc"
#define QUOTES                          "quotes"
#define RAND                            "rand"
#define REMOVECHAR                      "rchar"
#define RECSEL                          "recsel"
#define TQRRECFIELDVALUE                "recval"
#define TQRRECFIELDVALQUOT              "recvaq"
#define TQRREMOVE                       "remq"
#define RESETCONNECTION                 "resetconn"
#define RETURN                          "return"
#define RIGHT                           "right"
#define SETCONNECTION                   "setconn"
#define SETVAR                          "setvar"
#define STRLEN                          "strlen"
#define TQRFILT                         "tqrfilt"
#define TQRINSERT                       "tqrinsert"
#define TQRMOVE                         "tqrmove"
#define TQRSAMPLE                       "tqrsample"
#define TQRSTAT                         "tqrstat"
#define TQRSTORE                        "tqrstore"
#define TRACEUSER                       "traceu"
#define TRANS                           "trans"
#define TRIM                            "trim"
#define UCASE                           "ucase"
#define UTIME                           "utime"
#define VERINST                         "verinst"
#define VALID                           "valid"






                        /******************
                           SUPPORT CLASSES
                         ******************/


/*----------------------------------------------------------------------------
                              BaseCommand

  Main specialization class for base (tannit internal) commands.
  ----------------------------------------------------------------------------*/

class BaseCommand : public AbstractCommand
{
public:
//MEMBERS
  itxString     m_Name;
  itxString     m_Output;
  int           m_Type;

//METHODS
  inline char*  GetName(){return m_Name.GetBuffer();}
  void          Deallocate(){}

//CONSTRUCTION-DESTRUCTION
  BaseCommand(){m_Type = DEFAULT_BC_TYPE;};
  ~BaseCommand(){}
};





                        /******************
                            BASE COMMANDS
                         ******************/




/*----------------------------------------------------------------------------
                              StartConditionalBlock
  ----------------------------------------------------------------------------*/

class BC_StartConditionalBlock : public BaseCommand
{
public:
//MEMBERS
  Parser*       m_pParser;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_StartConditionalBlock(char* name, Parser* pParser, int type)
  {
    m_Name    = name;
    m_pParser = pParser;
    m_Type    = type;
  }
  ~BC_StartConditionalBlock(){}
};



/*----------------------------------------------------------------------------
                              EndConditionalBlock
  ----------------------------------------------------------------------------*/

class BC_EndConditionalBlock : public BaseCommand
{
public:
//MEMBERS

//METHODS
  char*         Execute(char* istr){return NULL;};

//CONSTRUCTION-DESTRUCTION
  BC_EndConditionalBlock(){m_Name = END_CONDITIONAL_BLOCK; m_Type = END_CND_BLK;}
  ~BC_EndConditionalBlock(){};
};


/*----------------------------------------------------------------------------
                              AlternateConditionalBlock
  ----------------------------------------------------------------------------*/

class BC_AlternateConditionalBlock : public BaseCommand
{
public:
//MEMBERS

//METHODS
  char*         Execute(char* istr){return NULL;};

//CONSTRUCTION-DESTRUCTION
  BC_AlternateConditionalBlock(){m_Name = ALTERNATE_CONDITIONAL_BLOCK; m_Type = ELSE_CND_BLK;}
  ~BC_AlternateConditionalBlock(){};
};


/*----------------------------------------------------------------------------
                              PRMValue
  ----------------------------------------------------------------------------*/

class BC_PRMValue : public BaseCommand
{
public:
//MEMBERS
  CGIResolver*  m_pCGIRes;

//METHODS
  char* Execute(char* istr)
  {
    m_Output.SetEmpty();

    if (istr != NULL && strlen(istr) > 0)
    {
      if (m_pCGIRes->m_PRMFile.GetPRMValue(istr, &m_Output) == false)
        DebugTrace2(IN_COMMAND | IN_WARNING, "Cannot find '%s' prm variable.", istr);
    }
    else
      DebugTrace2(IN_COMMAND | IN_WARNING, "Empty argument found.");

    return m_Output.GetBuffer();
  }

//CONSTRUCTION-DESTRUCTION
  BC_PRMValue(char* name, CGIResolver* pcgires)
  {
    m_Name = name; 
    m_pCGIRes = pcgires;
  }
  ~BC_PRMValue(){};
};

/*----------------------------------------------------------------------------
                              Cgipath
  ----------------------------------------------------------------------------*/

class BC_Cgipath : public BaseCommand
{
public:
//MEMBERS
  CGIResolver*  m_pCGIRes;

//METHODS
  char*         Execute(char* istr)
  {
    itxString cgidir, cginame;
    
    m_Output.SetEmpty();

    m_pCGIRes->m_PRMFile.GetPRMValue("webroot", &m_Output);
    m_pCGIRes->m_PRMFile.GetPRMValue("cgidir", &cgidir);
    m_pCGIRes->m_PRMFile.GetPRMValue("cginame", &cginame);

    m_Output += "/";
    m_Output += cgidir;
    m_Output += "/";
    m_Output += cginame;

    return m_Output.GetBuffer();
  }

//CONSTRUCTION-DESTRUCTION
  BC_Cgipath(char* name, CGIResolver* pcgires)
  {
    m_Name = name; 
    m_pCGIRes = pcgires;
  }
  ~BC_Cgipath(){};
};

/*----------------------------------------------------------------------------
                              GetValue
  ----------------------------------------------------------------------------*/

class BC_GetValue : public BaseCommand
{
public:
//MEMBERS
  CGIResolver*  m_pCGIRes;
  Parser*       m_pParser;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_GetValue(Parser* pParser, CGIResolver* pcgires)
  {
    m_Name    = GETVALUE; 
    m_pCGIRes = pcgires;
    m_pParser = pParser;
  }
  ~BC_GetValue(){};
};

/*----------------------------------------------------------------------------
                              GetHide
  ----------------------------------------------------------------------------*/

class BC_GetHide : public BaseCommand
{
public:
//MEMBERS
  Parser*       m_pParser;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_GetHide(Parser* pParser)
  {
    m_Name    = GETHIDE; 
    m_pParser = pParser;
  }
  ~BC_GetHide(){};
};

/*----------------------------------------------------------------------------
                              AdjustQuotes
  ----------------------------------------------------------------------------*/

class BC_AdjustQuotes : public BaseCommand
{
public:
//MEMBERS

//METHODS
  char* Execute(char* istr)
  {
    m_Output = istr;
    m_Output.AdjustStr();
	  m_Output.SubstituteSubString("\"", "\"\"");

    return m_Output.GetBuffer();
  }

//CONSTRUCTION-DESTRUCTION
  BC_AdjustQuotes(Parser* pParser)
  {
    m_Name = QUOTES; 
  }
  ~BC_AdjustQuotes(){};
};

/*----------------------------------------------------------------------------
                              ProcessExternFile
  ----------------------------------------------------------------------------*/

class BC_ProcessExternFile : public BaseCommand
{
public:
//MEMBERS
  Parser*       m_pParser;
  CGIResolver*  m_pCGIRes;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_ProcessExternFile(Parser* pParser, CGIResolver*  pCGIRes)
  {
    m_Name    = PROCESS_EXTERN_FILE;
    m_pParser = pParser;
    m_pCGIRes = pCGIRes;
  }
  ~BC_ProcessExternFile(){};
};

/*----------------------------------------------------------------------------
                              UCase
  ----------------------------------------------------------------------------*/

class BC_UCase : public BaseCommand
{
public:
//MEMBERS

//METHODS
  char* Execute(char* istr)
  {
    m_Output = istr;
    m_Output.UCase();
    return m_Output.GetBuffer();
  }

//CONSTRUCTION-DESTRUCTION
  BC_UCase(){m_Name = UCASE;}
  ~BC_UCase(){};
};

/*----------------------------------------------------------------------------
                              LCase
  ----------------------------------------------------------------------------*/

class BC_LCase : public BaseCommand
{
public:
//MEMBERS

//METHODS
  char* Execute(char* istr)
  {
    m_Output = istr;
    m_Output.LCase();
    return m_Output.GetBuffer();
  }

//CONSTRUCTION-DESTRUCTION
  BC_LCase(){m_Name = LCASE;}
  ~BC_LCase(){};
};

/*----------------------------------------------------------------------------
                              Left
  ----------------------------------------------------------------------------*/

class BC_Left : public BaseCommand
{
public:
//MEMBERS
  Parser* m_pParser;

//METHODS
  char* Execute(char* istr)
  {
    int len;

    m_Output.SetEmpty();

	  if(m_pParser->PickPar(istr, 1, &m_Output) == PARAM_NOT_FOUND)
      return PARAM_NOT_FOUND_MSG;
	  if(m_pParser->PickInt(istr, 2, &len) == PARAM_NOT_FOUND)
      return PARAM_NOT_FOUND_MSG;    

    m_Output.Left(len);

    return m_Output.GetBuffer();
  }

//CONSTRUCTION-DESTRUCTION
  BC_Left(Parser* pParser){m_Name = LEFT; m_pParser = pParser;}
  ~BC_Left(){};
};

/*----------------------------------------------------------------------------
                              Right
  ----------------------------------------------------------------------------*/

class BC_Right : public BaseCommand
{
public:
//MEMBERS
  Parser* m_pParser;

//METHODS
  char* Execute(char* istr)
  {
    int len;

    m_Output.SetEmpty();

	  if(m_pParser->PickPar(istr, 1, &m_Output) == PARAM_NOT_FOUND)
      return PARAM_NOT_FOUND_MSG;
	  if(m_pParser->PickInt(istr, 2, &len) == PARAM_NOT_FOUND)
      return PARAM_NOT_FOUND_MSG;    

    m_Output.Right(len);

    return m_Output.GetBuffer();
  }

//CONSTRUCTION-DESTRUCTION
  BC_Right(Parser* pParser){m_Name = RIGHT; m_pParser = pParser;}
  ~BC_Right(){};
};

/*----------------------------------------------------------------------------
                              Strlen
  ----------------------------------------------------------------------------*/

class BC_Strlen : public BaseCommand
{
public:
//MEMBERS

//METHODS
  char* Execute(char* istr)
  {
    m_Output = istr;
    m_Output.Trim();
    int l = m_Output.Len();
    m_Output.SetInt(l);
    return m_Output.GetBuffer();
  }

//CONSTRUCTION-DESTRUCTION
  BC_Strlen(){m_Name = STRLEN;}
  ~BC_Strlen(){};
};

/*----------------------------------------------------------------------------
                              Trim
  ----------------------------------------------------------------------------*/

class BC_Trim : public BaseCommand
{
public:
//MEMBERS

//METHODS
  char* Execute(char* istr)
  {
    m_Output = istr;
    m_Output.Trim();
    return m_Output.GetBuffer();
  }

//CONSTRUCTION-DESTRUCTION
  BC_Trim(){m_Name = TRIM;}
  ~BC_Trim(){};
};

/*----------------------------------------------------------------------------
                              SetVar
  ----------------------------------------------------------------------------*/

class BC_SetVar : public BaseCommand
{
public:
//MEMBERS
  Parser* m_pParser;

//METHODS
  char* Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_SetVar(Parser* pParser){m_Name = SETVAR; m_pParser = pParser;}
  ~BC_SetVar(){};
};

/*----------------------------------------------------------------------------
                              GetVar
  ----------------------------------------------------------------------------*/

class BC_GetVar : public BaseCommand
{
public:
//MEMBERS
  Parser* m_pParser;

//METHODS
  char* Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_GetVar(Parser* pParser){m_Name = GETVAR; m_pParser = pParser;}
  ~BC_GetVar(){};
};

/*----------------------------------------------------------------------------
                              GetCookie
  ----------------------------------------------------------------------------*/

class BC_GetCookie : public BaseCommand
{
public:
//MEMBERS
  Parser*       m_pParser;
  CGIResolver*  m_pCGIRes;

//METHODS
  char* Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_GetCookie(Parser* pParser, CGIResolver*  pCGIRes)
  {
    m_Name = GETCOOKIE; 
    m_pParser = pParser; 
    m_pCGIRes = pCGIRes;
  }
  ~BC_GetCookie(){};
};

/*----------------------------------------------------------------------------
                              UTime
  ----------------------------------------------------------------------------*/

class BC_UTime : public BaseCommand
{
public:
//MEMBERS
  Parser*       m_pParser;

//METHODS
  char* Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_UTime(Parser* pParser)
  {
    m_Name    = UTIME;
    m_pParser = pParser; 
  }
  ~BC_UTime(){};
};

/*----------------------------------------------------------------------------
                              DMYTime
  ----------------------------------------------------------------------------*/

class BC_DMYTime : public BaseCommand
{
public:
//MEMBERS
  Parser*       m_pParser;

//METHODS
  char* Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_DMYTime(Parser* pParser)
  {
    m_Name    = DMYTIME;
    m_pParser = pParser; 
  }
  ~BC_DMYTime(){};
};

/*----------------------------------------------------------------------------
                              Now
  ----------------------------------------------------------------------------*/

class BC_Now : public BaseCommand
{
public:
//MEMBERS
  Parser*       m_pParser;

//METHODS
  char* Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_Now(Parser* pParser)
  {
    m_Name    = NOW;
    m_pParser = pParser; 
  }
  ~BC_Now(){};
};

/*----------------------------------------------------------------------------
                              FormatCurrency
  ----------------------------------------------------------------------------*/

class BC_FormatCurrency : public BaseCommand
{
public:
//MEMBERS
  Parser*       m_pParser;

//METHODS
  char* Execute(char* istr);

private:
  char* Format(char* num, int mant_len, int want_sep);

public:
//CONSTRUCTION-DESTRUCTION
  BC_FormatCurrency(Parser* pParser)
  {
    m_Name    = FORMATCURRENCY;
    m_pParser = pParser; 
  }
  ~BC_FormatCurrency(){};
};

/*----------------------------------------------------------------------------
                              CheckForbiddenChars
  ----------------------------------------------------------------------------*/

class BC_CheckForbiddenChars : public BaseCommand
{
public:
//MEMBERS
  Parser*       m_pParser;

//METHODS
  char* Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_CheckForbiddenChars(Parser* pParser)
  {
    m_pParser = pParser; 
    m_Name    = CHECKFORBIDDENCHARS;
  }
  ~BC_CheckForbiddenChars(){};
};

/*----------------------------------------------------------------------------
                              TraceUser
  ----------------------------------------------------------------------------*/

class BC_TraceUser : public BaseCommand
{
public:
//MEMBERS
  Parser*       m_pParser;
  CGIResolver*  m_pCGIRes;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_TraceUser(Parser* pParser, CGIResolver*  pCGIRes)
  {
    m_Name    = TRACEUSER;
    m_pParser = pParser;
    m_pCGIRes = pCGIRes;
  }
  ~BC_TraceUser(){};
};

/*----------------------------------------------------------------------------
                              ODBCExecuteQuery
  ----------------------------------------------------------------------------*/

class BC_ODBCExecuteQuery : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;
  CGIResolver*    m_pCGIRes;
  TQRODBCManager* m_pTQRODBCManager;
  itxString       m_ODBCdsn;
  itxString       m_ODBCpwd;
  itxString       m_ODBCuid;


//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_ODBCExecuteQuery(Parser* pParser, CGIResolver*  pCGIRes, TQRODBCManager* pTQRODBCManager)
  {
    m_Name            = ODBC_EXECUTE_QUERY;
    m_pParser         = pParser;
    m_pCGIRes         = pCGIRes;
    m_pTQRODBCManager = pTQRODBCManager;
  }
  ~BC_ODBCExecuteQuery(){};
};

/*----------------------------------------------------------------------------
                              TQRRecordFieldValue
  ----------------------------------------------------------------------------*/

class BC_TQRRecordFieldValue : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;
  TQRCollection*  m_pTQRCollection;
//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_TQRRecordFieldValue(Parser* pParser, TQRCollection* pTQRCollection)
  {
    m_Name            = TQRRECFIELDVALUE;
    m_pParser         = pParser;
    m_pTQRCollection  = pTQRCollection;
  }
  ~BC_TQRRecordFieldValue(){};
};

/*----------------------------------------------------------------------------
                              TQRRecordFieldValQuot
  ----------------------------------------------------------------------------*/

class BC_TQRRecordFieldValQuot : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;
  TQRCollection*  m_pTQRCollection;
//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_TQRRecordFieldValQuot(Parser* pParser, TQRCollection* pTQRCollection)
  {
    m_Name            = TQRRECFIELDVALQUOT;
    m_pParser         = pParser;
    m_pTQRCollection  = pTQRCollection;
  }
  ~BC_TQRRecordFieldValQuot(){};
};


/*----------------------------------------------------------------------------
                              CycleTQR
  ----------------------------------------------------------------------------*/

class BC_CycleTQR : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;
  CGIResolver*    m_pCGIRes;
  TQRCollection*  m_pTQRCollection;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_CycleTQR(Parser* pParser, CGIResolver* pCGIRes, TQRCollection* pTQRCollection)
  {
    m_Name            = CYCLETQR;
    m_pParser         = pParser;
    m_pCGIRes         = pCGIRes;
    m_pTQRCollection  = pTQRCollection;
    m_Type            = START_CYCLE;
  }
  ~BC_CycleTQR(){};
};

/*----------------------------------------------------------------------------
                              EndCycleTQR
  ----------------------------------------------------------------------------*/

class BC_EndCycleTQR : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;
  CGIResolver*    m_pCGIRes;
  TQRCollection*  m_pTQRCollection;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_EndCycleTQR(Parser* pParser, TQRCollection* pTQRCollection)
  {
    m_Name            = ENDCYCLETQR;
    m_pParser         = pParser;
    m_pTQRCollection  = pTQRCollection;
    m_Type            = END_CYCLE;
  }
  ~BC_EndCycleTQR(){};
};

/*----------------------------------------------------------------------------
                              CycleFor
  ----------------------------------------------------------------------------*/

class BC_CycleFor : public BaseCommand
{
public:
//MEMBERS
  TQRManager*     m_pTQRManager;
  Parser*         m_pParser;
  CGIResolver*    m_pCGIRes;
  TQRCollection*  m_pTQRCollection;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_CycleFor(Parser* pParser, CGIResolver* pCGIRes, TQRCollection* pTQRCollection,
           TQRManager* pTQRManager)
  {
    m_Name            = CYCLEFOR;
    m_pTQRManager     = pTQRManager;
    m_pParser         = pParser;
    m_pCGIRes         = pCGIRes;
    m_pTQRCollection  = pTQRCollection;
    m_Type            = START_CYCLE;
  }
  ~BC_CycleFor(){};
};

/*----------------------------------------------------------------------------
                              EndCycleFor
  ----------------------------------------------------------------------------*/

class BC_EndCycleFor : public BC_EndCycleTQR
{
public:
//MEMBERS

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_EndCycleFor(Parser* pParser, TQRCollection* pTQRCollection) :
    BC_EndCycleTQR(pParser, pTQRCollection)
  {
    m_Name            = ENDCYCLEFOR;
  }
  ~BC_EndCycleFor(){};
};

/*----------------------------------------------------------------------------
                              EndCycleFor
  ----------------------------------------------------------------------------*/

class BC_ForIndex : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;
  TQRCollection*  m_pTQRCollection;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_ForIndex(Parser* pParser, TQRCollection* pTQRCollection)
  {
    m_Name            = FORINDEX;
    m_pParser         = pParser;
    m_pTQRCollection  = pTQRCollection;
  }
  ~BC_ForIndex(){};
};

/*----------------------------------------------------------------------------
                              Exit
  ----------------------------------------------------------------------------*/

class BC_Exit : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_Exit(Parser* pParser)
  {
    m_Name            = EXIT;
    m_pParser         = pParser;
  }
  ~BC_Exit(){};
};

/*----------------------------------------------------------------------------
                              TQRExist
  ----------------------------------------------------------------------------*/

class BC_TQRExist : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;
  TQRCollection*  m_pTQRCollection;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_TQRExist(Parser* pParser, TQRCollection*  pTQRCollection)
  {
    m_Name            = TQREXIST;
    m_pParser         = pParser;
    m_pTQRCollection  = pTQRCollection;
  }
  ~BC_TQRExist(){};
};

/*----------------------------------------------------------------------------
                              Crypt
  ----------------------------------------------------------------------------*/

class BC_Crypt : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;

//METHODS
  char*         Execute(char* istr);

private:
  unsigned short itxEncrypt(unsigned char* DESKey, unsigned char* Whitenings,
	                          unsigned char* source, unsigned char* destination);

public:
//CONSTRUCTION-DESTRUCTION
  BC_Crypt(Parser* pParser)
  {
    m_Name            = CRYPT;
    m_pParser         = pParser;
  }
  ~BC_Crypt(){};
};

/*----------------------------------------------------------------------------
                              Decrypt
  ----------------------------------------------------------------------------*/

class BC_Decrypt : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;

//METHODS
  char*         Execute(char* istr);

private:
  unsigned short itxDecrypt(unsigned char* DESKey, unsigned char* Whitenings,
	                          unsigned char* source, unsigned char* destination);

public:

//CONSTRUCTION-DESTRUCTION
  BC_Decrypt(Parser* pParser)
  {
    m_Name            = DECRYPT;
    m_pParser         = pParser;
  }
  ~BC_Decrypt(){}
};


/*----------------------------------------------------------------------------
                              Valid
  ----------------------------------------------------------------------------*/

class BC_Valid : public BC_ODBCExecuteQuery
{
public:
//MEMBERS
  TQRCollection*  m_pTQRCollection;

//METHODS
  char*         Execute(char* istr);
  int           CheckDbPwd(itxString* login, itxString* pwd, 
                           itxString* extraField, itxString* extraVal, 
                           itxString* retMsg);
  bool          LoginWantsQuotes(itxString* logintable, itxString* loginfield);

//CONSTRUCTION-DESTRUCTION
  BC_Valid(Parser* pParser, CGIResolver* pCGIRes, TQRODBCManager* pTQRODBCManager, TQRCollection* pTQRCollection) : 
    BC_ODBCExecuteQuery(pParser, pCGIRes, pTQRODBCManager)
  {
    m_Name = VALID;
    m_pTQRCollection  = pTQRCollection;
  }
  ~BC_Valid(){};
};


/*----------------------------------------------------------------------------
                              RemoveTQR
  ----------------------------------------------------------------------------*/

class BC_RemoveTQR : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;
  TQRCollection*  m_pTQRCollection;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_RemoveTQR(Parser* pParser, TQRCollection* pTQRCollection)
  {
    m_Name            = TQRREMOVE;
    m_pParser         = pParser;
    m_pTQRCollection  = pTQRCollection;
  }
  ~BC_RemoveTQR(){};
};


/*----------------------------------------------------------------------------
                              TQRStat
  ----------------------------------------------------------------------------*/
class BC_TQRStat : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;
  TQRCollection*  m_pTQRCollection;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_TQRStat(Parser* pParser, TQRCollection* pTQRCollection)
  {
    m_Name            = TQRSTAT;
    m_pParser         = pParser;
    m_pTQRCollection  = pTQRCollection;
  }
  ~BC_TQRStat(){};
};


/*----------------------------------------------------------------------------
                              Return
  ----------------------------------------------------------------------------*/
class BC_Return : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_Return(Parser* pParser)
  {
    m_Name            = RETURN;
    m_pParser         = pParser;
  }
  ~BC_Return(){};
};

/*----------------------------------------------------------------------------
                              Flush
  ----------------------------------------------------------------------------*/

class BC_Flush : public BaseCommand
{
public:
//MEMBERS
  CGIResolver*    m_pCGIRes;
  Parser*         m_pParser;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_Flush(Parser* pParser, CGIResolver* pCGIRes)
  {
    m_Name            = FLUSH;
    m_pCGIRes         = pCGIRes;
    m_pParser         = pParser;
  }
  ~BC_Flush(){};
};

/*----------------------------------------------------------------------------
                              TQRMove
  ----------------------------------------------------------------------------*/
class BC_TQRMove : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;
  TQRCollection*  m_pTQRCollection;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_TQRMove(Parser* pParser, TQRCollection* pTQRCollection)
  {
    m_Name            = TQRMOVE;
    m_pParser         = pParser;
    m_pTQRCollection  = pTQRCollection;
  }
  ~BC_TQRMove(){};
};


/*----------------------------------------------------------------------------
                              TQRFilt
  ----------------------------------------------------------------------------*/
class BC_TQRFilt : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;
  TQRManager*     m_pTQRManager;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_TQRFilt(Parser* pParser, TQRManager* pTQRManager)
  {
    m_Name            = TQRFILT;
    m_pParser         = pParser;
    m_pTQRManager     = pTQRManager;
  }
  ~BC_TQRFilt(){};
};

/*----------------------------------------------------------------------------
                              TQRSample
  ----------------------------------------------------------------------------*/
class BC_TQRSample : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;
  TQRManager*     m_pTQRManager;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_TQRSample(Parser* pParser, TQRManager* pTQRManager)
  {
    m_Name            = TQRSAMPLE;
    m_pParser         = pParser;
    m_pTQRManager     = pTQRManager;
  }
  ~BC_TQRSample(){};
};

/*----------------------------------------------------------------------------
                              Recsel
  ----------------------------------------------------------------------------*/
class BC_Recsel : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;
  TQRCollection*  m_pTQRCollection;
  CGIResolver*    m_pCGIRes;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_Recsel(Parser* pParser, TQRCollection*  pTQRCollection, CGIResolver* pCGIRes)
  {
    m_Name            = RECSEL;
    m_pParser         = pParser;
    m_pTQRCollection  = pTQRCollection;
    m_pCGIRes         = pCGIRes;
  }
  ~BC_Recsel(){};
};


/*----------------------------------------------------------------------------
                              Trans
  ----------------------------------------------------------------------------*/

class BC_Trans : public BC_ODBCExecuteQuery
{
public:
//MEMBERS
  TQRCollection*  m_pTQRCollection;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_Trans(Parser* pParser, CGIResolver* pCGIRes, TQRODBCManager* pTQRODBCManager, TQRCollection* pTQRCollection) : 
    BC_ODBCExecuteQuery(pParser, pCGIRes, pTQRODBCManager)
  {
    m_Name = TRANS;
    m_pTQRCollection  = pTQRCollection;
  }
  ~BC_Trans(){};
};


/*----------------------------------------------------------------------------
                              TQRStore
  ----------------------------------------------------------------------------*/

class BC_TQRStore : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;
  TQRManager*     m_pTQRManager;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_TQRStore(Parser* pParser, TQRManager*  pTQRManager)
  {
    m_Name            = TQRSTORE;
    m_pParser         = pParser;
    m_pTQRManager     = pTQRManager;
  }
  ~BC_TQRStore(){};
};

/*----------------------------------------------------------------------------
                              TQRInsert
  ----------------------------------------------------------------------------*/

class BC_TQRInsert : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;
  TQRODBCManager* m_pTQRODBCManager;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_TQRInsert(Parser* pParser, TQRODBCManager*  pTQRODBCManager)
  {
    m_Name            = TQRINSERT;
    m_pParser         = pParser;
    m_pTQRODBCManager = pTQRODBCManager;
  }
  ~BC_TQRInsert(){};
};


/*----------------------------------------------------------------------------
                              Pager
  ----------------------------------------------------------------------------*/

class BC_Pager : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;
  TQRManager*     m_pTQRManager;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_Pager(Parser* pParser, TQRManager*  pTQRManager)
  {
    m_Name            = PAGER;
    m_pParser         = pParser;
    m_pTQRManager     = pTQRManager;
  }
  ~BC_Pager(){};
};

/*----------------------------------------------------------------------------
                              Netprex
  ----------------------------------------------------------------------------*/

class BC_Netprex : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_Netprex(Parser* pParser)
  {
    m_Name            = NETPREX;
    m_pParser         = pParser;
  }
  ~BC_Netprex(){};
};

/*----------------------------------------------------------------------------
                              Netgrab
  ----------------------------------------------------------------------------*/

class BC_Netgrab : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_Netgrab(Parser* pParser)
  {
    m_Name            = NETGRAB;
    m_pParser         = pParser;
  }
  ~BC_Netgrab(){};
};

/*----------------------------------------------------------------------------
                              Proc
  ----------------------------------------------------------------------------*/

class BC_Proc : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;
  CGIResolver*  m_pCGIRes;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_Proc(Parser* pParser, CGIResolver*  pCGIRes)
  {
    m_Name            = PROCESS_MEMORY_BUFFER;
    m_pParser         = pParser;
    m_pCGIRes = pCGIRes;
  }
  ~BC_Proc(){};
};


/*----------------------------------------------------------------------------
                              Rand
  ----------------------------------------------------------------------------*/

class BC_Rand : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_Rand(Parser* pParser)
  {
    m_Name            = RAND;
    m_pParser         = pParser;
  }
  ~BC_Rand(){};
};

/*----------------------------------------------------------------------------
                              MakeOp
  ----------------------------------------------------------------------------*/

class BC_MakeOp : public BaseCommand
{
public:
//MEMBERS
  Parser*       m_pParser;

//METHODS
  char*         Execute(char* istr);
  bool          MakeMulDivs(itxListPtr* ops, itxListPtr* terms, char op);

//CONSTRUCTION-DESTRUCTION
  BC_MakeOp(Parser* pParser)
  {
    m_Name      = MAKEOP;
    m_pParser   = pParser;
  }
  ~BC_MakeOp(){};
};

/*----------------------------------------------------------------------------
                              GetValueQuote
  ----------------------------------------------------------------------------*/

class BC_GetValueQuote : public BaseCommand
{
public:
//MEMBERS
  CGIResolver*  m_pCGIRes;
  Parser*       m_pParser;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_GetValueQuote(Parser* pParser, CGIResolver* pcgires)
  {
    m_Name    = GETVALUEQUOTE; 
    m_pCGIRes = pcgires;
    m_pParser = pParser;
  }
  ~BC_GetValueQuote(){};
};

/*----------------------------------------------------------------------------
                              BC_GetValueCast
  ----------------------------------------------------------------------------*/

class BC_GetValueCast : public BaseCommand
{
public:
//MEMBERS
  CGIResolver*  m_pCGIRes;
  Parser*       m_pParser;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_GetValueCast(Parser* pParser, CGIResolver* pcgires)
  {
    m_Name    = GETVALUECAST;
    m_pCGIRes = pcgires;
    m_pParser = pParser;
  }
  ~BC_GetValueCast(){};
};


/*----------------------------------------------------------------------------
                              NewConnection
  ----------------------------------------------------------------------------*/

class BC_NewConnection : public BaseCommand
{
public:
//MEMBERS
  Parser*               m_pParser;
  itxSQLConnCollection* m_pconnections;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_NewConnection(Parser* pParser, itxSQLConnCollection* pconnections)
  {
    m_Name    = NEWCONNECTION; 
    m_pParser = pParser;
    m_pconnections = pconnections;
  }
  ~BC_NewConnection(){};
};

/*----------------------------------------------------------------------------
                              SetConnection
  ----------------------------------------------------------------------------*/

class BC_SetConnection : public BaseCommand
{
public:
//MEMBERS
  Parser*               m_pParser;
  TQRODBCManager*       m_pTQRODBCManager;
  itxSQLConnCollection* m_pconnections;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_SetConnection(Parser* pParser, TQRODBCManager* ptqrodbcmng, itxSQLConnCollection* pconnections)
  {
    m_Name    = SETCONNECTION; 
    m_pTQRODBCManager = ptqrodbcmng;
    m_pParser = pParser;
    m_pconnections = pconnections;
  }
  ~BC_SetConnection(){};
};

/*----------------------------------------------------------------------------
                              ResetConnection
  ----------------------------------------------------------------------------*/

class BC_ResetConnection : public BaseCommand
{
public:
//MEMBERS
  Parser*               m_pParser;
  TQRODBCManager*       m_pTQRODBCManager;
  itxSQLConnection*     m_podbcconnection;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_ResetConnection(Parser* pParser, TQRODBCManager* ptqrodbcmng, itxSQLConnection* podbcconnection)
  {
    m_Name    = RESETCONNECTION; 
    m_pParser = pParser;
    m_pTQRODBCManager = ptqrodbcmng;
    m_podbcconnection = podbcconnection;
  }
  ~BC_ResetConnection(){};
};

/*----------------------------------------------------------------------------
                              Array
  ----------------------------------------------------------------------------*/

class BC_Array : public BaseCommand
{
public:
//MEMBERS
  Parser*               m_pParser;
  TQRManager*           m_pTQRManager;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_Array(Parser* pParser, TQRManager* pTQRManager)
  {
    m_Name    = ARRAY; 
    m_pParser = pParser;
    m_pTQRManager = pTQRManager;
  }
  ~BC_Array(){};
};

/*----------------------------------------------------------------------------
                              ArraySet
  ----------------------------------------------------------------------------*/

class BC_ArraySet : public BaseCommand
{
public:
//MEMBERS
  Parser*               m_pParser;
  TQRManager*           m_pTQRManager;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_ArraySet(Parser* pParser, TQRManager* pTQRManager)
  {
    m_Name    = ARRAYSET; 
    m_pParser = pParser;
    m_pTQRManager = pTQRManager;
  }
  ~BC_ArraySet(){};
};

/*----------------------------------------------------------------------------
                              ArrayGet
  ----------------------------------------------------------------------------*/

class BC_ArrayGet : public BaseCommand
{
public:
//MEMBERS
  Parser*               m_pParser;
  TQRManager*           m_pTQRManager;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_ArrayGet(Parser* pParser, TQRManager* pTQRManager)
  {
    m_Name    = ARRAYGET; 
    m_pParser = pParser;
    m_pTQRManager = pTQRManager;
  }
  ~BC_ArrayGet(){};
};

/*----------------------------------------------------------------------------
                              RemoveChar
  ----------------------------------------------------------------------------*/

class BC_RemoveChar : public BaseCommand
{
public:
//MEMBERS
  Parser*               m_pParser;

//METHODS
  char*         Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_RemoveChar(Parser* pParser)
  {
    m_Name    = REMOVECHAR; 
    m_pParser = pParser;
  }
  ~BC_RemoveChar(){};
};

/*----------------------------------------------------------------------------
                              Mid
  ----------------------------------------------------------------------------*/

class BC_Mid : public BaseCommand
{
public:
//MEMBERS
  Parser* m_pParser;

//METHODS
  char* Execute(char* istr)
  {
    int len;
    int start;

    m_Output.SetEmpty();

	  if(m_pParser->PickPar(istr, 1, &m_Output) == PARAM_NOT_FOUND)
      return PARAM_NOT_FOUND_MSG;
	  if(m_pParser->PickInt(istr, 2, &start) == PARAM_NOT_FOUND)
      return PARAM_NOT_FOUND_MSG;    
	  if(m_pParser->PickInt(istr, 3, &len) == PARAM_NOT_FOUND)
      return PARAM_NOT_FOUND_MSG;    

    m_Output.Mid(start, len);

    return m_Output.GetBuffer();
  }

//CONSTRUCTION-DESTRUCTION
  BC_Mid(Parser* pParser){m_Name = MID; m_pParser = pParser;}
  ~BC_Mid(){};
};

/*----------------------------------------------------------------------------
                              NumericalCmp
  ----------------------------------------------------------------------------*/

class BC_NumericalCmp : public BaseCommand
{
public:
//MEMBERS
  Parser* m_pParser;

//METHODS
  char* Execute(char* istr)
  {
    double a, b;

    m_Output.SetEmpty();

	  if(m_pParser->PickDouble(istr, 1, &a) == PARAM_NOT_FOUND)
      return PARAM_NOT_FOUND_MSG;
	  if(m_pParser->PickDouble(istr, 2, &b) == PARAM_NOT_FOUND)
      return PARAM_NOT_FOUND_MSG;

    if (a == b) 
      m_Output = "0";
    else if (a < b) 
      m_Output = "-1";
    else if (a > b) 
      m_Output = "1";

    return m_Output.GetBuffer();
  }

//CONSTRUCTION-DESTRUCTION
  BC_NumericalCmp(Parser* pParser){m_Name = NUMERICAL_COMPARISON; m_pParser = pParser;}
  ~BC_NumericalCmp(){};
};


/*----------------------------------------------------------------------------
                              Verinst
  ----------------------------------------------------------------------------*/

class BC_Verinst : public BaseCommand
{
public:
//MEMBERS
  Parser*         m_pParser;

//METHODS
  char*         Execute(char* istr);

public:

//CONSTRUCTION-DESTRUCTION
  BC_Verinst(Parser* pParser)
  {
    m_Name            = VERINST;
    m_pParser         = pParser;
  }
  ~BC_Verinst(){}
};



/*----------------------------------------------------------------------------
                              CopyFile
  ----------------------------------------------------------------------------*/

class BC_CopyFile : public BaseCommand
{
public:
//MEMBERS
  Parser* m_pParser;

//METHODS
  char* Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_CopyFile(Parser* pParser)
  {
    m_Name            = COPYFILE;
    m_pParser         = pParser;
  }
  ~BC_CopyFile(){}
};

/*----------------------------------------------------------------------------
                              Currency
  ----------------------------------------------------------------------------*/

class BC_Currency : public BaseCommand
{
public:
//MEMBERS
  Parser* m_pParser;

//METHODS
  char* Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_Currency(Parser* pParser)
  {
    m_Name            = CURRENCY;
    m_pParser         = pParser;
  }
  ~BC_Currency(){}
};

/*----------------------------------------------------------------------------
                              Console
  ----------------------------------------------------------------------------*/

class BC_Console : public BaseCommand
{
public:
//MEMBERS
  CGIResolver*  m_pCGIRes;
  Parser*       m_pParser;

//METHODS
  char* Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_Console(Parser* pParser, CGIResolver* pCGIRes)
  {
    m_Name            = CONSOLE;
    m_pParser         = pParser;
    m_pCGIRes         = pCGIRes;
  }
  ~BC_Console(){}
};

/*----------------------------------------------------------------------------
                              GetPOSTbody
  ----------------------------------------------------------------------------*/

class BC_GetPOSTbody : public BaseCommand
{
public:
//MEMBERS
  CGIResolver*  m_pCGIRes;

//METHODS
  char* Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_GetPOSTbody(CGIResolver* pCGIRes)
  {
    m_Name    = GETPOSTBODY;
    m_pCGIRes = pCGIRes;
  }
  ~BC_GetPOSTbody(){}
};

/*----------------------------------------------------------------------------
                              Mail
  ----------------------------------------------------------------------------*/

class BC_Mail : public BaseCommand
{
public:
//MEMBERS
  Parser*       m_pParser;

//METHODS
  char* Execute(char* istr);

//CONSTRUCTION-DESTRUCTION
  BC_Mail(Parser* pParser)
  {
    m_Name = MAIL;
    m_pParser = pParser;
  }
  ~BC_Mail(){}
};

/*----------------------------------------------------------------------------
                              Getenv
  ----------------------------------------------------------------------------*/

class BC_Getenv : public BaseCommand
{
public:
//MEMBERS
  Parser*       m_pParser;

//METHODS
  char* Execute(char* istr)
  {
    itxString varname;

	  if (m_pParser->PickPar(istr, 1, &varname) == PARAM_FOUND)
      m_Output = getenv(varname.GetBuffer());

    return m_Output.GetBuffer();
  }

//CONSTRUCTION-DESTRUCTION
  BC_Getenv(Parser* pParser)
  {
    m_Name = GETENV;
    m_pParser = pParser;
  }
  ~BC_Getenv(){}
};


#endif  /* _COMMANDS_H_ */






