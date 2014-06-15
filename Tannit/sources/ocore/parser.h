/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/*
 | _-.-..-._-_-._--_-._.-._     AITECSA S.r.l.   2000
 |
 |
 | FILENAME    : parser.h
 | TAB         : 2 spaces
 |
 | DESCRIPTION : Parser object declaration.
 |
 |
 */



#ifndef _PARSER_H_
#define _PARSER_H_

#include "defines.h"
#include "tnt.h"
#include "templatefile.h"

#include "itxlib.h"

                        /****************
                              Parser
                         ****************/

typedef enum 
{
	Preprocrun,
	Previewrun,
  Forbiddenrun,
  Normalrun,
  Diagnostic
} RunType_e;


typedef struct TplVarVect
{
	int			    idx;
	itxString   values[TPL_VARS_NUM];
	itxString   names[TPL_VARS_NUM];
}	TplVarVect_t;

//The command interpreter
class Parser
{
public:
//MEMBERS
  AbstractCommand*    m_Command[MAX_CMDS];
  int                 m_TotalCount;
  int                 m_BaseCount;
  int                 m_ForceExit;
  int                 m_ForceReturn;
  TplVarVect_t        m_TplVars;
  itxStack            m_TplStack;
  TemplateFile*       m_CurTpl;
  char                m_StartCommandTag;
  bool                m_PreprocRun; // becoming obsolete
  RunType_e           m_RunType; // replacing m_PreprocRun
  itxString           m_ActualTplDir;

//METHODS
  void                CreateBaseCommands(void* tannit);
  void                DestroyBaseCommands();
  void                UpdateTotalCount();
  int                 PickPar(char* inputStr, int position, itxString* returnStr);
  int                 PickInt(char* inputstr, int position, int* retval);
  int                 PickDouble(char* inputstr, int position, double* retval);
  int                 PickString(char* inputstr, int position, char* retval, int* bufdim);
  int                 PickListString(char* inputstr, itxString** params);
  int                 VerifyCmdSynt(char** tplString, int* cmdOrd, itxString* argStr, char** cmdStart);
  int                 ProcTplData(char* tplString, itxString* outputString);
  TemplateFile*       LoadTemplate(itxString* tpldir, itxString* pname, char* ext);
  void                Run(itxString* tpldir, itxString* pname, char* ext = TPL_EXT);
  void                Run(char* content);
  void                SetValidBlock(int val);
  void                SetCycleBlock(int val);
  int                 GetCycleBlock(int offset = 0);

  void                AddVar(char* varName, char* varValue);

//CONSTRUCTION-DESTRUCTION
  Parser();
  ~Parser();
};


#endif  /* _PARSER_H_ */










