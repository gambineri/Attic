/*
 | _-.-..-._-_-._--_-._.-._     AITECSA S.r.l.   2000
 |
 |
 | FILENAME    : tannitobj.cpp
 | TAB         : 2 spaces
 |
 | DESCRIPTION : main implementation file.
 |
 |
 */



#include <time.h>
#include <stdlib.h>

#include "tannitobj.h"  
#include "templatefile.h"  
#include "commands.h"  

#include "itxtypes.h"
#include "itxlib.h"

#ifdef FCGITANNIT
#include "fcgi_config.h"
#include "fcgi_stdio.h"
#else 
#pragma message("=== Fast CGI support disabled ===")
#pragma message("Link the C run-time as (Debug) Multithreaded DLL and the following libraries:\n"\
                "      odbc32.lib itxlib.lib winmm.lib ws2_32.lib.\nGood luck.") //Release message
#include <stdio.h>
#endif  //FCGITANNIT


// This typedef is needed to call the one and only one
// function exported by user which connects any 
// external module to this instance of Tannit.
typedef int (ITXCDECL* TANNIT_HANDSHAKE)(AbstractCommand**, TNTAPI*);


Tannit::Tannit()
{
  m_ResponseMIMEtype.SetEmpty();

  m_pTNTAPIImpl  = NULL;
  m_NExtModules  = 0;

  for (int i=0; i<MAX_EXT_MODULES; i++)
    m_ExtModHandle[i] = NULL;

  m_TQRManager.SetTQRCollection(&m_TQRCollection);
  m_TQRODBCManager.SetTQRCollection(&m_TQRCollection);
}

//-----------------------------------------------------------------

Tannit::~Tannit()
{
  for (int i=0; i<MAX_EXT_MODULES; i++)
    if (m_ExtModHandle[i] != NULL)
      m_Sys.DLFreeLibrary(m_ExtModHandle[i]);

  delete m_pTNTAPIImpl;
}

//-----------------------------------------------------------------

bool Tannit::InitCurrentPRM()
{
	itxString initFileName;	//nome del file di inizializzazione
	itxString initFileExt;  //nome del file di inizializzazione con l'estensione
	itxString lanId;

  try
  {
    if (m_PRMConf.Clone(&m_PRMFile) == false)
      return false;

	  // il nome del file di inizializzazione viene ricavato dal valore di get PAR_FILE_TAG
	  if (cgiFormString(PAR_FILE_TAG, &initFileExt, GET_VAR_LEN) == cgiFormSuccess)
    {
      initFileExt += ".";
      initFileExt += DEF_PAR_FILE_EXT;

      m_PRMFile.MergePRM(&initFileExt);
    }

	  // Estrazione dalla stringa di get del valore del parametro opzionale
	  // CONC_LAN_ID_TAG (concatenated language id)
    lanId.SetEmpty();
	  cgiFormString(CONC_LAN_ID_TAG, &lanId, LANID_LEN);

    m_PRMFile.SetLanguageId(lanId.GetBuffer());
    m_PRMFile.SetPRMFileName(initFileExt.GetBuffer());
  }
  catch(...)  
  {
    DebugTrace2(IN_ERROR, "Unhandled exception in Tannit::InitCurrentPRM.");
    return false;
  }

  return true;
}

//-----------------------------------------------------------------

bool Tannit::Initialize()
{
// Set prm and conf files auxiliary directory to the one containing
// the Tannit executable. For a IIS WebServer it allows the user to
// place the inizialization files in the cgi-bin directory.
// The auxiliary directory is the place where to search for the
// prm and conf file when not found in the default tannit working
// directory.
  char buf[1024];
  m_Sys.PRGetModuleName(NULL, buf, 1024);

  char* pLastSlash = strrchr(buf, PATH_SEPARATOR_C);
  if (pLastSlash)
  {
    *pLastSlash = 0;
    m_PRMConf.SetAuxiliaryDirectory(buf);
    m_PRMFile.SetAuxiliaryDirectory(buf);
  }

  //Reads the web server variables from the environment table
  GetEnvironment(false);

  // read configuration file: Tannit.conf
  if (!m_PRMConf.ReadPRM())
  {
    DebugTrace2(IN_WARNING, "Tannit configuration file not found.");
    return false;
  }

  // if required here we start the ODBC connection and hook the 
  // TQRODBCManager to it
  //InitODBC(&m_PRMConf);  OBSOLETE

  // load external and core commands
  m_Parser.CreateBaseCommands(this);  
  LoadModules(m_PRMConf);
  return true;
}

//-----------------------------------------------------------------

bool Tannit::InitODBC(PRMFile* prmfile)
{
  bool retval = false;

  if (m_odbcconnection.IsConnected())
    return true;

  prmfile->GetPRMValue("odbcdsn", &(m_odbcconnection.m_dsn));

  if (!m_odbcconnection.m_dsn.IsEmpty())
  {
    prmfile->GetPRMValue("odbcuid", &(m_odbcconnection.m_usr));
    prmfile->GetPRMValue("odbcpwd", &(m_odbcconnection.m_pwd));
    
    if (m_odbcconnection.Create())
    {
      m_connections.Put(&m_odbcconnection);
      DebugTrace2(DEFAULT, "Connection successfully created.");
      m_TQRODBCManager.SetConnect(&m_odbcconnection);
      retval = true;
    }
    else
      SQLTrace(&m_odbcconnection);
  }

  return retval;
}

//-----------------------------------------------------------------
void Tannit::InquiryCurWorkingDir(itxString* ret)
{
  if (ret == NULL)
    return;

  char buf[1024];
  m_Sys.FSGetCurrentDirectory(buf, 1024);
  *ret = buf;
}

//-----------------------------------------------------------------
bool Tannit::InquiryDebugFile()
{
  if (g_DebugFile.m_DebugPath.Len() == 0)
    return false;

  FILE* fp; 
 	if ((fp = fopen(g_DebugFile.m_DebugPath.GetBuffer(), "w")) == NULL)
    return false;
  else
  {
    fclose(fp);
    remove(g_DebugFile.m_DebugPath.GetBuffer());
  }

  return true;
}

//-----------------------------------------------------------------
void Tannit::InquiryConfigFiles(itxString* ret)
{
  if (ret == NULL)
    return;

//  MakeConfigurationFileList("", "*.conf", ret);
//  MakeConfigurationFileList("", "*.prm", ret);

  MakeConfigurationFileList(m_PRMConf.GetAuxiliaryDirectory(), "*.conf", ret);
  MakeConfigurationFileList(m_PRMFile.GetAuxiliaryDirectory(), "*.prm", ret);
}

//-----------------------------------------------------------------
void Tannit::InquiryConnections(itxString prmlist, itxString* ret)
{
  if (ret == NULL)
    return;

  int         i = 0;
  PRMFile     prmf;
  itxString   prmfile;
  itxString   appo;

  prmlist.GetToken(INQ_TOK_SEP, i++, &prmfile);
  
  while (prmfile.Len() > 0)
  {
    prmf.SetPRMFileName(prmfile.GetBuffer());
    prmf.ClearNamesAndValues();
    prmf.ReadPRM();
    prmf.GetPRMValue("odbcdsn", &(m_odbcconnection.m_dsn));

    if (!m_odbcconnection.m_dsn.IsEmpty())
    {
      *ret += m_odbcconnection.m_dsn;
      *ret += INQ_TOK_SEP;
      prmf.GetPRMFileName(&appo);
      *ret += appo;
      *ret += INQ_TOK_SEP;

      prmf.GetPRMValue("odbcuid", &(m_odbcconnection.m_usr));
      prmf.GetPRMValue("odbcpwd", &(m_odbcconnection.m_pwd));
  
      if (m_odbcconnection.Create())
      {
        *ret += INQ_CONN_OK;
        m_odbcconnection.Destroy();
      }
      else
        *ret += INQ_CONN_NOK;

      *ret += INQ_TOK_SEP;
    }

    prmlist.GetToken(INQ_TOK_SEP, i++, &prmfile);
  }
}

//-----------------------------------------------------------------
void Tannit::MakeConfigurationFileList(itxString auxdir, char* filter, itxString* conflist)
{
  itxString auxpath;

  if (filter == NULL || conflist == NULL)
    return;
  
  m_Sys.FSDirToTokenString(auxdir.GetBuffer(), filter, INQ_TOK_SEP, conflist);
}

//-----------------------------------------------------------------
void Tannit::Inquiry()
{
  fprintf(cgiOut,   "<HTML><HEAD>\n"
                    "<TITLE>Tannit� diagnostic information panel</TITLE>\n");

  fprintf(cgiOut,   "<style>                           \n"
                    ".item_name                        \n"
                    "{                                 \n"
	                  "  color: black;                   \n"
	                  "  background: #AABBCC;            \n"
	                  "  font-family : Verdana;          \n"
	                  "  font-size: 15px;                \n"
                    "}                                 \n"
                    ".item_val                         \n"
                    "{                                 \n"
	                  "  color: black;                   \n"
	                  "  background: #ABCABC;            \n"
	                  "  font-family : Verdana;          \n"
	                  "  font-size: 13px;                \n"
                    "}                                 \n"
                    ".odbc_header                      \n"
                    "{                                 \n"
	                  "  color: black;                   \n"
	                  "  background: #FFFFFF;            \n"
	                  "  font-family : Verdana;          \n"
	                  "  font-size: 11px;                \n"
                    "}                                 \n"
                    ".odbc_dsn                         \n"
                    "{                                 \n"
	                  "  color: black;                   \n"
	                  "  font-family : Verdana;          \n"
	                  "  font-size: 13px;                \n"
                    "}                                 \n");

  fprintf(cgiOut,   ".odbc_test_ok                     \n"
                    "{                                 \n"
	                  "  color: green;                   \n"
	                  "  font-family : Verdana;          \n"
	                  "  font-size: 13px;                \n"
                    "}                                 \n"
                    ".odbc_test_nok                    \n"
                    "{                                 \n"
	                  "  color: red;                     \n"
	                  "  font-family : Verdana;          \n"
	                  "  font-size: 13px;                \n"
                    "}                                 \n"
                    "A                                 \n"
                    "{                                 \n"
                    "  text-decoration: none;          \n"
                    "  font-family: verdana;           \n"
                    "  font-size: 13pt                 \n"
                    "}                                 \n"
                    "</style>                          \n");
                    
  fprintf(cgiOut, "</HEAD>\n"
                  "<BODY bgcolor=#FFFFFF>\n");

  //If tannit executable is a read-only file, no inquiry is possible
  char  buf[1024];
  m_Sys.PRGetModuleName(NULL, buf, 1024);

  if (m_Sys.FSFileIsReadOnly(buf))
  {
    fprintf(cgiOut, "<CENTER><FONT FACE=Verdana SIZE = 10>Tannit is alive and kicking.</FONT></CENTER>\n"
                    "<CENTER><A HREF=http://www.aitecsa.it><FONT FACE=Verdana SIZE = 3>www.aitecsa.it</FONT></A></CENTER>\n");
  }
  else
  {
    fprintf(cgiOut, "<CENTER><FONT FACE=Arial SIZE = 5>Tannit� diagnostic information panel</FONT></CENTER><P></P>\n"
                    "<FONT FACE=Verdana SIZE=3>\n"
                    "<TABLE bgcolor=#999999 width=100%% cellspacing=3 cellpadding=5>\n");

    char* tablerow = "<TR>\n"
                     "  <TD class=item_name>%s</TD>\n"
                     "  <TD class=item_val>%s</TD>\n"
                     "</TR>\n";

    //Inquiry on current working directory
    itxString curdir;
    InquiryCurWorkingDir(&curdir);
    curdir.Trim();
    fprintf(cgiOut, tablerow, "current working directory", curdir.GetBuffer());

    //Inquiry on debug file //TBD aggiungere tentativo scrittura file di debug
    if (InquiryDebugFile() == false)
      fprintf(cgiOut, tablerow, "Debug file", 
        "NOT GENERATED<BR>" 
        "Possible causes:<BR>"
        ".) the file 'dbgname.par' was not found in the current working directory;<BR>"
        ".) the parameter inside this file has not been assigned;<BR>"
        ".) the parameter inside this file has been assigned with an incorrect value;<BR>"
        ".) for some reason, it's not allowed to write in the specified directory.");
    else
    {
      itxString appo;
      appo  = curdir;
      if (curdir.Right(1).Compare(PATH_SEPARATOR) != 0)
        appo += PATH_SEPARATOR;
      appo += g_DebugFile.m_DebugPath.GetBuffer();
      fprintf(cgiOut, tablerow, "Debug file", appo.GetBuffer());
    }

    //Inquiry on configuration files
    itxString       prmlist;
    itxString       out;
    
    InquiryConfigFiles(&prmlist);
    out = prmlist;
    char sep[2] = {INQ_TOK_SEP, 0};
    out.SubstituteSubString(sep, "<BR>");
    fprintf(cgiOut, tablerow, "Configuration files", out.GetBuffer());
    
    //Inquiry on database connections
    itxString conntest;
    InquiryConnections(prmlist, &conntest);

    out.SetEmpty();
    out = "<table width=100%>\n"
          "<tr>\n"
          "  <td class=odbc_header>DSN</td>\n"
          "  <td class=odbc_header>found on</td>\n"
          "  <td class=odbc_header>test</td>\n"
          "</tr>\n";

    int i = 0;
    itxString tok;
    conntest.GetToken(INQ_TOK_SEP, i++, &tok);  //take dsn
  
    while (tok.Len() > 0)
    {
      out += "<tr>\n"
             "  <td class=odbc_dsn>\n";
      out += tok; //dsn name
      out += "\n"
             "</td>\n"
             "<td class=item_val>\n";

      tok.SetEmpty();
      conntest.GetToken(INQ_TOK_SEP, i++, &tok); //take prm
      out += tok; //prm/conf file 
      out += "\n"
             "</td>\n";

      tok.SetEmpty();
      conntest.GetToken(INQ_TOK_SEP, i++, &tok); //take test
      if (tok.Compare(INQ_CONN_OK) == 0)
        out += "<td class=odbc_test_ok>\n"
               "    OK\n";
      else
        out += "<td class=odbc_test_nok>\n"
               "    NOT OK\n";

      out += "  </td>\n"
             "</tr>\n";
      
      tok.SetEmpty();
      conntest.GetToken(INQ_TOK_SEP, i++, &tok);  //take next dsn
    }
    out += "</table>\n";
    fprintf(cgiOut, tablerow, "ODBC connections", out.GetBuffer());

    //Inquiry Extension Module DLLs successfully loaded
    out.SetEmpty();
    char modulefile[1024];
    for (int dll=0; dll<m_NExtModules; dll++)
    {
      m_Sys.PRGetModuleName(m_ExtModHandle[dll], modulefile, 1024);

      out += modulefile;
      out += "<br>";
    }

    fprintf(cgiOut, tablerow, "Extension modules<br>successfully loaded", out.GetBuffer());

    //Close table
    fprintf(cgiOut, "</TABLE>\n");
    fprintf(cgiOut, "</FONT>\n");
  }

  fprintf(cgiOut, "</BODY></HTML>\n");
}

//-----------------------------------------------------------------
void Tannit::SendResponseMimeType(char* mimetype)
{
  if (mimetype == NULL)
  {
    // Check if the client requests a specific MIME Content Type and send it
    if (!m_PRMFile.GetPRMValue(MIME_TAG, &m_ResponseMIMEtype))
    {
	    if (cgiFormString(MIME_TAG, &m_ResponseMIMEtype, TPL_NAME_LEN) != cgiFormSuccess)
        m_ResponseMIMEtype= DEFAULT_MIME_TYPE;
    }
  }
  else
    m_ResponseMIMEtype = mimetype;

	fprintf(cgiOut, "Content-type: %s\r\n\r\n", m_ResponseMIMEtype.GetBuffer());
}

//-----------------------------------------------------------------
RunType_e Tannit::ManageContext(itxString* tplName, itxString* tpldir, itxString* preproc_outtpl, itxString* preproc_outdir)
{
  itxString prmPPkey;
  itxString prmPVkey;
  itxString getPPkey;
  itxString inquiryTag;
  bool keyfound = false;
  RunType_e runtype = Normalrun;

  //Send appropriate response MIME type
  SendResponseMimeType();

  //If the case, put the disclaimer before anything...
  if(m_ResponseMIMEtype.CompareNoCase(DEFAULT_MIME_TYPE))
    fprintf(cgiOut, "%s", AITECSA_DISCLAIMER);
  
  //Diagnostic run?
	if (cgiFormString(INQUIRY_TAG, &inquiryTag, TPL_NAME_LEN) == cgiFormSuccess) 
    return Diagnostic;

  tplName->SetEmpty();

  keyfound = m_PRMFile.GetPRMValue("ppkey", &prmPPkey);
  keyfound = (m_PRMFile.GetPRMValue("pvkey", &prmPVkey) || keyfound);

  //Gets preprocessing key from param file
  if(keyfound)
  {
    // preprocessing key found in get string
	  if (cgiFormString("ppkey", &getPPkey, TPL_NAME_LEN) == cgiFormSuccess)
    {
      // set source template name and directory
      m_PRMFile.GetPRMValue(TPLDIR, tpldir);
      if (cgiFormString(PREPROC_TPL_TAG, tplName, TPL_NAME_LEN) != cgiFormSuccess)
		    return Forbiddenrun;

      if(getPPkey.Compare(prmPPkey.GetBuffer()) == 0)
      {
        // set output file name
        if (cgiFormString(TPL_TAG, preproc_outtpl, TPL_NAME_LEN) != cgiFormSuccess)
		      return Forbiddenrun;

        Flush(WWW_PP_STARTED);
        Flush(tplName->GetBuffer());
        // set output file directory
        // if input and output directory are the same the execution is blocked
        runtype = Preprocrun;
        m_PRMFile.GetPRMValue(PREPROC_DIR_PRM_NAME, preproc_outdir);
        m_Parser.m_StartCommandTag = PREPROC_COMMAND_TAG;
      }
      else if(getPPkey.Compare(prmPVkey.GetBuffer()) == 0)
      {
        runtype = Previewrun;
        m_Parser.m_StartCommandTag = PREPROC_COMMAND_TAG;
      }
      else
        runtype = Forbiddenrun; // the key is not valid
    }
    else  // preprocessing key not found in get string
    {
      m_PRMFile.GetPRMValue(PREPROC_DIR_PRM_NAME, tpldir);
      runtype = Normalrun;
      if (cgiFormString(TPL_TAG, tplName, TPL_NAME_LEN) != cgiFormSuccess)
		    return Forbiddenrun;
    }
  }
  else
  {
    //Gets template directory from param file
    m_PRMFile.GetPRMValue(TPLDIR, tpldir);

    //Dedicated template (servicetpl): gets the template file name from param file
    if (!m_PRMFile.GetPRMValue(TPL_SERVICE_TAG, tplName))
    {
      if (cgiFormString(PREPROC_TPL_TAG, tplName, TPL_NAME_LEN) != cgiFormSuccess)
      {
        if (cgiFormString(TPL_TAG, tplName, TPL_NAME_LEN) != cgiFormSuccess)
		      return Forbiddenrun;
      }
      else
      {
        Flush(PP_STARTED);
        runtype = Preprocrun;
        m_Parser.m_StartCommandTag = PREPROC_COMMAND_TAG;
        m_PRMFile.GetPRMValue(PREPROC_DIR_PRM_NAME, preproc_outdir);

        if (cgiFormString(TGT_TAG, preproc_outtpl, TPL_NAME_LEN) != cgiFormSuccess)
		      preproc_outtpl = tplName;
      }
    }  
  }

  if(runtype == Preprocrun)
  {
    if ((*preproc_outdir).CompareNoCase(tpldir))
    {
      runtype = Forbiddenrun;
      Flush(PP_IN_OUT_DIRS_EQ);
      Flush(PP_FINISHED);
    }
  }

  return runtype;
}

void Tannit::ManageOutput(itxString preproc_outdir, itxString preproc_outtpl)
{
  //Output management
  if (m_Parser.m_RunType == Preprocrun)
  {
    if (m_Parser.m_CurTpl != NULL)
    {
      if (!FlushOnFile(preproc_outdir.GetBuffer(), 
                       preproc_outtpl.GetBuffer(), 
                       m_Parser.m_CurTpl->m_Output.GetBuffer()))
        Flush(WWW_PP_CANT_WRITEOUTPUT);
    }

    Flush(WWW_PP_FINISHED);
  }
  else if (m_Parser.m_RunType == Previewrun)
  {
    if (!g_DebugFile.m_Errors)
    {
      if (m_Parser.m_CurTpl != NULL)
      {
        m_Parser.m_RunType = Normalrun;
        m_Parser.m_StartCommandTag = START_COMMAND_TAG;
        m_Parser.Run((m_Parser.m_CurTpl->m_Output).GetBuffer());
      }
      else
        Flush(ERR_OPEN_TPL_FILE);
    }
  }

  if (m_Parser.m_RunType == Normalrun)
  {
    if (!g_DebugFile.m_Errors)
    {
      if (m_Parser.m_CurTpl != NULL)
        Flush(m_Parser.m_CurTpl->m_Output.GetBuffer());
      else
        Flush(ERR_OPEN_TPL_FILE);
    }
  }
}

//-----------------------------------------------------------------
void Tannit::OnStart()
{
  itxString preproc_outdir;
  itxString preproc_enableext;
  itxString preproc_outtpl;
  itxString tpldir;
  itxString tplName;
  itxString forcedebug;
  bool      goon = true;

  TimeTrace("Tannit Program Starting");
  StartMilliCount();
  
  //Reads the web server variables from the environment table
  GetEnvironment(true);

  ManageRequestMethod();

  //Looks for the tag indicating that the output must be the debug file, 
  //also if the request will complete successfully.
	if (cgiFormString(DEBUG_2_VIDEO_TAG, &forcedebug, TPL_NAME_LEN) == cgiFormSuccess)
  {
    g_DebugFile.m_ReportLevel = atoi(forcedebug.GetBuffer());
    g_DebugFile.m_Errors = true;
  }

  //Reads the prm file from the get string, if anyone
  if (InitCurrentPRM() == false)
  {
    Flush(ERR_OPEN_INI_FILE);
	  DebugTrace2(IN_ERROR, "%s\n", ERR_OPEN_INI_FILE);
    return;
  }

  LoadModules(m_PRMFile);

  // if required here we start the ODBC connection and hook the
  // TQRODBCManager to it
  InitODBC(&m_PRMFile);

  // define the running enviroment (normalrun, preprocrun, previewrun, forbiddenrun)
  // and set the correct input/output values
  m_Parser.m_RunType = ManageContext(&tplName, &tpldir, &preproc_outtpl, &preproc_outdir);
  m_Parser.m_ActualTplDir = tpldir;

  if (m_Parser.m_RunType != Forbiddenrun && m_Parser.m_RunType != Diagnostic)
  {
    m_PRMFile.GetPRMValue("enableext", &preproc_enableext);

    //~~~~~~~~~~~~~~~~~~~~~ BEGIN CORE JOB
 	  if (preproc_enableext.IsEmpty() || preproc_enableext.IsNull())
      m_Parser.Run(&tpldir, &tplName);
    else
      m_Parser.Run(&tpldir, &tplName, "");
    //~~~~~~~~~~~~~~~~~~~~~ END CORE JOB

    ManageOutput(preproc_outdir, preproc_outtpl);
  }
  else if (m_Parser.m_RunType == Diagnostic)
    Inquiry();

  StopMilliCount();
  TraceMilliDiff("OnStart execution time:");
  DebugTrace2(DEFAULT, "At %s OnStart ends.", CurrentDateTimeStr());

/* TEST MASSIMO NON TOCCARE PLEASE
  itxString img;
	if (cgiFormString("img", &img, TPL_NAME_LEN) == cgiFormSuccess)
  {
    FILE* fp = fopen(img.GetBuffer(), "rb");    
    char img[300000];
    int read = fread(img, 1, 300000, fp);
    fclose(fp);
    int written = fwrite(img, 1, read, cgiOut);
  }
//*/
}

//-----------------------------------------------------------------
bool Tannit::IsWebRequest()
{
  return !(cgiServerSoftware == NULL || *cgiServerSoftware == 0);
}

//-----------------------------------------------------------------
void Tannit::LoadModules(PRMFile& paramfile)
{
  itxString addon;
  itxString addonLval;
  char      c[10];

  addonLval = ADDON;

  for (int i = m_NExtModules; i < MAX_EXT_MODULES; i++)
  {
    sprintf(c, "%d", i + 1);
    addonLval += c;

    addon = "";
//    m_PRMFile.GetPRMValue(&addonLval, &addon);
    paramfile.GetPRMValue(&addonLval, &addon);
  
    if (addon.IsNull() || addon.IsEmpty())
      return; //no more external modules

    if (m_pTNTAPIImpl == NULL)
      m_pTNTAPIImpl = new TNTAPIImpl(this);

    LoadUserCommands(&addon, m_pTNTAPIImpl);
    addonLval = ADDON;
  }
}

//-----------------------------------------------------------------

void Tannit::LoadUserCommands(itxString* ext_mod_pathname, TNTAPIImpl* pTNTAPIImpl)
{
  try // Try to make the connection with the extern module
  {
    if ((m_ExtModHandle[m_NExtModules] = m_Sys.DLLoadLibrary(ext_mod_pathname->GetBuffer())) == NULL)
    {
      DebugTrace("Extern module '%s': LoadLibrary failed. last error: %d\n", 
                 ext_mod_pathname->GetBuffer(), m_Sys.DLGetLastError());
      return;
    }

    TANNIT_HANDSHAKE pf;

    //  The way to call an export function is quite different between platforms:
    //  we try every method and fall in error only if no one applies.
    if ((pf = (TANNIT_HANDSHAKE)m_Sys.DLGetFunction(m_ExtModHandle[m_NExtModules], (char*)1)) == NULL)  //win32
      if ((pf = (TANNIT_HANDSHAKE)m_Sys.DLGetFunction(m_ExtModHandle[m_NExtModules], "TannitHandshake")) == NULL)  //linux
      {
        DebugTrace("Extern module '%s': Unable to get TannitHandshake.", ext_mod_pathname->GetBuffer());
        return;
      }
    
    //  Now make the hanshake.
    (*pf)((AbstractCommand**)&m_Parser.m_Command[m_Parser.m_TotalCount], (TNTAPI*)pTNTAPIImpl);
  }
  catch(...)
  {
    DebugTrace("Extern module '%s': Handshake failed.", ext_mod_pathname->GetBuffer());
    return;
  }

  DebugTrace2(DEFAULT, "Extern module '%s' successfully loaded.", ext_mod_pathname->GetBuffer());
  
  m_NExtModules++;
  m_Parser.UpdateTotalCount();
}

void Tannit::Usage(char* name)
{
  fprintf(stdout, "usage:\n"); 
  fprintf(stdout, "        %s [option ... [option]]\n\n", name); 
  fprintf(stdout, "option:\n"); 
  fprintf(stdout, "        -h: this help\n"); 
  fprintf(stdout, "        -t<modulename>: TEG module's command description file\n");
  fprintf(stdout, "        -s<cmdsep>: processor command separator (i.e.: $, * or #)\n");
  fprintf(stdout, "        -i<filename>: input file for processing\n");
  fprintf(stdout, "        -o<filename>: output file\n");
  fprintf(stdout, "        -p<filename>: tannit parameter file\n");
  fprintf(stdout, "        -e<filename>: output file extension (i.e.: .htm)\n");
  fprintf(stdout, "        -g<query_string>: GET input string\n");

  fprintf(stdout, "\n");
}

void Tannit::OnCommandLine(char* modulename, char cmdsep, char* inputfile, char* outputfile, char* ext)
{
  itxString preproc_outdir;
  itxString preproc_outtpl;
  itxString tpldir;
  itxString tplName;
  itxString module;
  itxString extension;

  TimeTrace("Tannit Program Starting");
  StartMilliCount();

  // if required here we start the ODBC connection and hook the
  // TQRODBCManager to it
  InitODBC(&m_PRMFile);
  
  //Gets template directory from param file
  m_PRMFile.GetPRMValue(TPLDIR, &tpldir);
  m_PRMFile.GetPRMValue(PREPROC_DIR_PRM_NAME, &preproc_outdir);

  Flush(PP_STARTED);
  m_Parser.m_PreprocRun = true;
  m_Parser.m_StartCommandTag = cmdsep;

  module = modulename;

  if (module.IsEmpty())
  {
	  tplName = inputfile;
    preproc_outtpl = outputfile;

	  m_Parser.Run(&tpldir, &tplName);
  
    //Output management
    if (m_Parser.m_CurTpl != NULL)
    {
      if (!FlushOnFile(preproc_outdir.GetBuffer(), preproc_outtpl.GetBuffer(), m_Parser.m_CurTpl->m_Output.GetBuffer(), ext))
        Flush(PP_CANT_WRITEOUTPUT);
    }

    Flush(PP_FINISHED);
  }
  else
  {
    m_Parser.AddVar("TEG_module_name", modulename);

    preproc_outtpl = modulename;

    tplName = "teg_def";
	 
    m_Parser.Run(&tpldir, &tplName, TEG_EXT);
  
    //Output management
    extension = ".h";
    if (m_Parser.m_CurTpl != NULL)
    {
      if (!FlushOnFile(preproc_outdir.GetBuffer(), preproc_outtpl.GetBuffer(), m_Parser.m_CurTpl->m_Output.GetBuffer(), extension.GetBuffer()))
        Flush(PP_CANT_WRITEOUTPUT);
    }
    Flush(PP_FINISHED);
    
    tplName = "teg_imp";
	 
    m_Parser.Run(&tpldir, &tplName, TEG_EXT);
  
    //Output management
    extension = ".cpp";
    if (m_Parser.m_CurTpl != NULL)
    {
      if (!FlushOnFile(preproc_outdir.GetBuffer(), preproc_outtpl.GetBuffer(), m_Parser.m_CurTpl->m_Output.GetBuffer(), extension.GetBuffer()))
        Flush(PP_CANT_WRITEOUTPUT);
    }
    Flush(PP_FINISHED);
  }

  StopMilliCount();
  TraceMilliDiff("OnStart execution time:");
  DebugTrace2(DEFAULT, "At %s OnStart ends.", CurrentDateTimeStr());
}

void Tannit::CommandLineMode(int argc, char* argv[])
{
  char cmdsep;
  itxString inputfile;
  itxString outputfile;
  itxString extension;
  itxString modulename;

  fprintf(stdout, "\nTannit� 4.0 (copyright 2001)\n"); 
  fprintf(stdout, "aitecsa\n");
  fprintf(stdout, "._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.\n");
  fprintf(stdout, "                 �aitecsa s.r.l. via baglivi 3 00161 roma italy\n"); 
  fprintf(stdout, "                 aitecsa@aitecsa.it\n");
  fprintf(stdout, "                 all rights reserved\n\n");

  if (argc <= 1 || ISEQUAL(argv[1], "-h"))
  {
    Usage(argv[0]);
    return;
  }

  cmdsep = '*';
  inputfile = "tannitin";
  outputfile = "tannitout";
  extension = TPL_EXT;
  modulename.SetEmpty();
  
  bool prmfile = m_PRMConf.Clone(&m_PRMFile);

  int i = 1;
  while(argv[i] != NULL)
  {
    char optflag = argv[i][0];
    char option = argv[i][1];
    char* optvalue = &argv[i][2];

    if (optflag != '-')
    {
      Usage(argv[0]);
      return;
    }
    
    switch(option)
    {
      case 't':
        modulename = optvalue;
      break;

      case 's':
        cmdsep = optvalue[0];
      break;

      case 'i':
        inputfile = optvalue;
      break;

      case 'o':
        outputfile = optvalue;        
      break;

      case 'g':
      {
        if (m_Sys.BSSetenv("QUERY_STRING", optvalue) != 0)
        {
          fprintf(stdout, "ERROR: unable set QUERY_STRING\n");
          return;
        }
        GetEnvironment(true);
    		cgiContentLength = strlen(cgiQueryString);
		    if (cgiParseGetFormInput() != cgiParseSuccess)
        {
          fprintf(stdout, "ERROR: unable to parse get input\n");
          return;
        }
      }
      break;

      case 'p':
        try
        {
          itxString initFileExt;

          initFileExt = optvalue;
          if (prmfile != false)
          {
            m_PRMFile.MergePRM(&initFileExt);
            m_PRMFile.SetPRMFileName(optvalue);
            fprintf(stdout, "parameter file %s read...\n", optvalue);
          }      
        }
        catch(...)
        {
          fprintf(stdout, "ERROR: unable to read %s paramenter file\n", optvalue);
          return;
        }
      break;

      case 'e':
        extension = optvalue;        
      break;

      default:
        Usage(argv[0]);
        return;
    }

    i++;
  }

  OnCommandLine(modulename.GetBuffer(), cmdsep, inputfile.GetBuffer(), outputfile.GetBuffer(), extension.GetBuffer());
}

//-----------------------------------------------------------------
//-----------------------------------------------------------------
//                MAIN APPLICATION ENTRY POINT
//-----------------------------------------------------------------
//-----------------------------------------------------------------

int main(int argc, char* argv[])
{
#ifdef DOWNLOAD
	time_t      tm;
	struct tm*  today; 
	time(&tm);
	today = localtime(&tm);
  if ((today->tm_year + 1900) != 2002)
    return 0;
#endif

  Tannit  t; // THE object

// DEBUG FACILITY - BEGIN
#ifdef _DEBUG
//*
int p=1;
while (p)
  Sleep(1);
//*/
#endif
// DEBUG FACILITY - END

  bool    result;

  g_DebugFile.Open();
  result = t.Initialize();

  //test if tannit is invoked from command line
  if (!t.IsWebRequest())
  {
    t.CommandLineMode(argc, argv);
    g_DebugFile.Close();
    return 0;
  }
  //tannit was invoked by the WebServer

  //TBD gestione debug tracing prima dell'inizio del ciclo
  if (result)
  {
#ifdef FCGITANNIT
    while (FCGI_Accept() >= 0)
    {
      //Close initialization instance of debug file
      //and re-open it for current request
      g_DebugFile.Close();
      g_DebugFile.Open();
#endif

      try
      {
          t.OnStart();  //User code
      }
      catch(...)
      {
        DebugTrace("%s\n", "Exception during OnStart (overridden by user)");
      }
     
      if (g_DebugFile.m_Errors)
      {
        if (!result) // we must send the header
          t.SendResponseMimeType(DEFAULT_MIME_TYPE);

        itxString appodbg;
        if (g_DebugFile.Bufferize(&appodbg))
          t.Flush(appodbg.GetBuffer());
      }

      g_DebugFile.Close();

#ifdef FCGITANNIT
    } //FCGI loop
#endif
  }
  else
  {
    if (!result) // we must send the header
      t.SendResponseMimeType(DEFAULT_MIME_TYPE);

    itxString appodbg;
    if (g_DebugFile.Bufferize(&appodbg))
      t.Flush(appodbg.GetBuffer());
  }

  g_DebugFile.Close();

  return 0;
}
