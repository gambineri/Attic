/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/*
 | _-.-..-._-_-._--_-._.-._     AITECSA S.r.l.  2000
 |
 |
 | FILENAME    : cgiresolver.h
 | TAB         : 2 spaces
 |
 | DESCRIPTION : CGIResolver object declaration header file.
 |
 |
 */


#ifndef __CGIRESOLVER_H__
#define __CGIRESOLVER_H__

#include <fcntl.h>  //This for stream modes

#include "defines.h"
#include "auxfile.h"

#include "itxlib.h"


/* Possible return codes from the cgiForm family of functions (see below). */
typedef enum 
{
	cgiFormSuccess,
	cgiFormTruncated,
	cgiFormBadType,
	cgiFormEmpty,
	cgiFormNotFound,
	cgiFormConstrained,
	cgiFormNoSuchChoice,
	cgiFormMemory
} FormResultType_t;

typedef enum 
{
	cgiEscapeRest,
	cgiEscapeFirst,
	cgiEscapeSecond
} EscapeState_t;

typedef enum 
{
	cgiUnescapeSuccess,
	cgiUnescapeMemory
} UnescapeResultType_t;

typedef enum 
{
	cgiParseSuccess,
	cgiParseMemory,
	cgiParseIO
} cgiParseResultType;

/* One form entry, consisting of an attribute-value pair. */
typedef struct cgiFormEntryStruct
{
  char* attr;
  char* value;
  struct cgiFormEntryStruct* next;
} cgiFormEntry;


/*****************************************************************************
 ----------------------------  PRMFile object  -----------------------------
 *****************************************************************************/

class PRMFile
{
public:
//MEMBERS
  itxString   m_PrmNames[MAX_PRM_PARS];
  itxString   m_PrmValues[MAX_PRM_PARS];

private:
	itxString	  m_LanId;        //Lingua utente (tipicamente da una stringa di get)
	itxString	  m_FileName;     //Nome del file di inizializzazione con l'estensione.
  int         m_NumParams;
	itxString	  m_FileDir;      //Nome della directory del file di inizializzazione
                              //ultimo slash incluso
	itxString	  m_FilePath;      //Path completo del file di inizializzazione

public:
//METHODS
  bool        ReadPRM();
  bool        Clone(PRMFile*);
  bool        MergePRM(itxString*);

  void        SetLanguageId(char* lid);
  void        GetLanguageId(itxString* plid);

  void        SetPRMFileName(char* fname);
  void        GetPRMFileName(itxString* pfname);

  bool        GetPRMValue(itxString* param_name, itxString* param_value);
  bool        GetPRMValue(char* param_name, itxString* param_value);
  bool        GetPRMValue(char* param_name, char* param_value, int* bufdim);
  bool        GetPRMValue(int index, itxString* param_value);
  bool        GetPRMName(int index, itxString* param_value);
  int         GetIndexByName(char* param_name);
  void        ClearNamesAndValues();
  void        SetAuxiliaryDirectory(char* dir_path);
  itxString   GetAuxiliaryDirectory();
public:
//CONSTRUCTION-DESTRUCTION
  PRMFile();
  PRMFile(char* prmfile);

  ~PRMFile(){}
};


/*****************************************************************************
 ---------------------------- CGIResolver object -----------------------------
 *****************************************************************************/

class CGIResolver
{
//MEMBERS
public:
/*The various CGI environment variables. Instead of using getenv(),
	the programmer should refer to these, which are always
	valid null-terminated strings (they may be empty, but they 
	will never be null). If these variables are used instead
	of calling getenv(), then it will be possible to save
	and restore CGI environments, which is highly convenient
	for debugging. */
  char*                     cgiServerSoftware;
  char*                     cgiServerName;
  char*                     cgiGatewayInterface;
  char*                     cgiServerProtocol;
  char*                     cgiServerPort;
  char*                     cgiRequestMethod;
  char*                     cgiPathInfo;
  char*                     cgiPathTranslated;
  char*                     cgiScriptName;
  char*                     cgiQueryString;
  char*                     cgiRemoteHost;
  char*                     cgiHttpHost;
  char*                     cgiRemoteAddr;
  char*                     cgiAuthType;
  char*                     cgiRemoteUser;
  char*                     cgiRemoteIdent;
  char*                     cgiContentType;
  char*                     cgiAccept;
  char*                     cgiUserAgent;
  char*                     cgiCookie;
  char*                     cgiReferrer;

/* The number of bytes of data received.
	 Note that if the submission is a form submission
	 the library will read and parse all the information
	 directly from cgiIn; the programmer need not do so. */
  int                       cgiContentLength;

/* Pointer to CGI output. The cgiHeader functions should be used
	first to output the mime headers; the output HTML
	page, GIF image or other web document should then be written
	to cgiOut by the programmer. */
  FILE*                     cgiOut;

/* Pointer to CGI input. In 99% of cases, the programmer will NOT
	need this. However, in some applications, things other than 
	forms are posted to the server, in which case this file may
	be read from in order to retrieve the contents. */
  FILE*                     cgiIn;

  /* The first form entry. */
  cgiFormEntry*             cgiFormEntryFirst;
                            
  int                       cgiHexValue[256];
  char*                     cgiFindTarget;
  cgiFormEntry*             cgiFindPos;
  PRMFile                   m_PRMConf;
  PRMFile                   m_PRMFile;
  itxString                 m_POSTbody;
  itxSystem                 m_Sys;

//METHODS
  void                      GetEnvironment(bool must_trace = false);
  void                      cgiGetenv(char **s, char *var);
  void                      cgiFreeResources();
  int                       cgiFirstNonspaceChar(char *s);
  int                       cgiWriteString(FILE *out, char *s);
  int                       cgiWriteInt(FILE *out, int i);
  cgiParseResultType        cgiParseGetFormInput();
  bool                      ReadPostContent();
  cgiParseResultType        cgiParseFormInput(char *data, int length);
  UnescapeResultType_t      cgiUnescapeChars(char **sp, char *cp, int len);
  FormResultType_t          cgiFormEntryString(cgiFormEntry *e, char *result, int max, int newlines);
  cgiFormEntry*             cgiFormEntryFindFirst(char *name);
  cgiFormEntry*             cgiFormEntryFindNext();
  void                      ManageRequestMethod();
  void                      Flush(char* outbuf);
  void                      Flush(char* outbuf, int outbufdim);
  bool                      FlushOnFile(char* path, char* fname, char* outbuf, char* ext = TPL_EXT, char mode = 'b');
  void                      SendResponseMimeType();

  //Overridables
  virtual void              OnStart();          //Tannit entry point

/* These functions are used to retrieve form data. */
  FormResultType_t          cgiFormString(char *name, char *result, int max);
  FormResultType_t          cgiFormString(char *name, itxString* istrresult, int max); //itxString version
  FormResultType_t          cgiFormStringSpaceNeeded(char *name, int *length);
  
  //TBD da levare da qui in poi, una per una pero`...
  FormResultType_t          cgiFormStringNoNewlines(char *name, char *result, int max);
  FormResultType_t          cgiFormStringMultiple(char *name, char ***ptrToStringArray);
  FormResultType_t          cgiFormInteger(char *name, int *result, int defaultV);
  FormResultType_t          cgiFormIntegerBounded(char *name, int *result, int min, int max, int defaultV);
  FormResultType_t          cgiFormDouble(char *name, double *result, double defaultV);
  FormResultType_t          cgiFormDoubleBounded(char *name, double *result, double min, double max, double defaultV);
  FormResultType_t          cgiFormSelectSingle(char *name, char **choicesText, int choicesTotal, int *result, int defaultV);
  FormResultType_t          cgiFormSelectMultiple(char *name, char **choicesText, int choicesTotal, int *result, int *invalid);
  FormResultType_t          cgiFormCheckboxSingle(char *name);
  FormResultType_t          cgiFormCheckboxMultiple(char *name, char **valuesText, int valuesTotal, int *result, int *invalid);
  FormResultType_t          cgiFormRadio(char *name, char **valuesText, int valuesTotal, int *result, int defaultV);	
  void                      cgiStringArrayFree(char **stringArray);
  void                      cgiHeaderLocation(char *redirectUrl);
  void                      cgiHeaderStatus(int status, char *statusMessage);
                            
//CONSTRUCTION-DESTRUCTION
  CGIResolver();
  ~CGIResolver();
};


#endif  /* __CGIRESOLVER_H__ */



