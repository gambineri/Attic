/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/*****************************************************************************
                                AITECSA S.R.L. 
 -----------------------------------------------------------------------------
 - FILENAME    : templatefile.h
 - TAB         : 2 spaces
 -----------------------------------------------------------------------------
 - DESCRIPTION :  
    Template file wrapper object. 
 *****************************************************************************/

#ifndef _TEMPLATEFILE_H_
#define _TEMPLATEFILE_H_


#include "itxlib.h"

class Parser;

class TemplateFile
{
friend Parser;

private:
//Members
  itxString   m_Tpl;
  itxString   m_PathName;
  long        m_ContentLen;  //Comprese le linee di commento gasp
  int         m_TplFound;

public:
  int 	      m_ReadCycle[CYC_NESTING_DEPTH];
  char	      m_ValidBlock[CYC_NESTING_DEPTH];
  int		      m_CndLevel;
  int         m_CycLevel;
  itxStack    m_CycleStk;
  int         m_RecursionZero; //Counts the recursion nestings of Parser::ProcTplData on this template
  itxString   m_Output;

private:
//Methods
  void             InitMembers();
  int              Read(char* ext);
  void             PurgeComments(char*);
  int              CheckStartBlock(int type, char* cmdStart);
  int              CheckEndBlock(int type, char** tplString);
  inline int       MustExec(){return (m_ValidBlock[m_CndLevel] == EXEC_BLK && m_ReadCycle[m_CycLevel]);}
                        
public:                 
  int              GetContent(char* tplstr);
  itxString*       GetContent(itxString* tplstr);
  
  // Returns the pointer to the internal content buffer
  inline char*     GetContentBuffer(){return m_Tpl.GetBuffer();}

  inline int       Exist(){return m_TplFound;}
  inline char*     GetPathName(){return m_PathName.GetBuffer();}
  inline long      ContentLen(){return m_ContentLen;}


//Construction-Destruction
  TemplateFile(char* tpl_path_name, char* ext = TPL_EXT);
  TemplateFile(itxString* tpl_path_name, char* ext = TPL_EXT);
  TemplateFile(char* tpl_content, int finto);
  ~TemplateFile(){};
};


#endif /* _TEMPLATEFILE_H_ */












