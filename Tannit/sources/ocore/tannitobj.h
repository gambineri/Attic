/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#include "defines.h"
#include "cgiresolver.h"
#include "parser.h"  
#include "tntapiimpl.h"  
#include "tqrodbc.h"  

#include "itxlib.h"  


class Tannit : public CGIResolver
{
//MEMBERS
private:
  itxString             m_ResponseMIMEtype;
public:
  Parser                m_Parser;
  TQRCollection         m_TQRCollection;
  TQRManager            m_TQRManager;
  TQRODBCManager        m_TQRODBCManager;
  itxSQLConnection      m_odbcconnection;
  itxSQLConnCollection  m_connections;
  TNTAPIImpl*           m_pTNTAPIImpl;
  void*                 m_ExtModHandle[MAX_EXT_MODULES]; //Addon DLL's handles
  int                   m_NExtModules;

private:
//METHODS
  RunType_e         ManageContext(itxString* tplName, itxString* tplDir, itxString* preproc_outtpl, itxString* preproc_outdir);
  void              ManageOutput(itxString preproc_outdir, itxString preproc_outtpl);

public:
  void              OnStart();
  void              OnPostRequest();
  void              CommandLineMode(int argc, char** argv);
  void              OnCommandLine(char* modulename, char cmdsep, char* inputfile, char*outputfile, char* ext);
  void              Usage(char* argv);
  bool              IsWebRequest();
  void              SendResponseMimeType(char* mimetype = NULL);

  void              Inquiry();
  void              InquiryCurWorkingDir(itxString* ret);
  bool              InquiryDebugFile();
  void              InquiryConfigFiles(itxString* ret);
  void              InquiryConnections(itxString prmlist, itxString* ret);
  void              MakeConfigurationFileList(itxString auxdir, char* filter, itxString* conflist);

  bool              InitCurrentPRM();
  bool              Initialize();
  bool              InitODBC(PRMFile* prmfile);
  void              LoadModules(PRMFile& paramfile);
  void              LoadUserCommands(itxString* ext_mod_pathname, TNTAPIImpl* pTNTAPIImpl);

//CONSTRUCTORS-DESTRUCTORS
public:
  Tannit();
  ~Tannit();
};











