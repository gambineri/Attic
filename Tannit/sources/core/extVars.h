/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

extern char BgMain[PAR_VALUE_LEN], BgMenu[PAR_VALUE_LEN], BgTop[PAR_VALUE_LEN], BgLeft[PAR_VALUE_LEN];
extern char	BgRight[PAR_VALUE_LEN], BgBott[PAR_VALUE_LEN], TplDir[PAR_VALUE_LEN], CrudeTplDir[PAR_VALUE_LEN], WebHome[PAR_VALUE_LEN];
extern char	CgiDir[PAR_VALUE_LEN] ,CgiName[PAR_VALUE_LEN], PrepropKey[PAR_VALUE_LEN], PreviewKey[PAR_VALUE_LEN], NormviewKey[PAR_VALUE_LEN], PrepKeyTag[PAR_VALUE_LEN];
extern char	ImgDir[PAR_VALUE_LEN], FileDir[PAR_VALUE_LEN], Odbcdsn[PAR_VALUE_LEN], Odbcuid[PAR_VALUE_LEN];
extern char	Odbcpwd[PAR_VALUE_LEN], WebUrl[PAR_VALUE_LEN], LoginTable[PAR_VALUE_LEN], PwdField[PAR_VALUE_LEN];
extern char	LoginField[PAR_VALUE_LEN], ExtrTable[PAR_VALUE_LEN], IdField[PAR_VALUE_LEN], ExtrField[PAR_VALUE_LEN];
extern char	LangTagGet[PAR_VALUE_LEN], LangTable[PAR_VALUE_LEN], LangNameField[PAR_VALUE_LEN], LangCodeField[PAR_VALUE_LEN];
extern char	TransTable[PAR_VALUE_LEN], TransTagField[PAR_VALUE_LEN], DefaultLanguageId[PAR_VALUE_LEN];
extern char UploadDir[PAR_VALUE_LEN],AllowUpload[PAR_VALUE_LEN];
extern char CorrTable[PAR_VALUE_LEN], CorrUserField[PAR_VALUE_LEN], CorrCodeField[PAR_VALUE_LEN];
extern char CorrAppDir[PAR_VALUE_LEN], CorrFileName[PAR_VALUE_LEN], CorrDndStatus[PAR_VALUE_LEN];
extern char	CorrDnDir[PAR_VALUE_LEN], Llu[PAR_VALUE_LEN], Rtp[PAR_VALUE_LEN];
extern char UserAgentSpy[256];
extern char FFace1[PAR_VALUE_LEN], FFace2[PAR_VALUE_LEN], FSize[PAR_VALUE_LEN], FStyle[PAR_VALUE_LEN], FColor[PAR_VALUE_LEN], FDecor[PAR_VALUE_LEN], Lheight[PAR_VALUE_LEN];
extern char DbgPath[PAR_VALUE_LEN], CooTimeDelay[PAR_VALUE_LEN], CooURLEscape[PAR_VALUE_LEN];
extern char TargetTplField[PAR_VALUE_LEN],TplTable[PAR_VALUE_LEN],TplTableId[PAR_VALUE_LEN],	TplTableName[PAR_VALUE_LEN];
extern char CurrentTpl[MAX_TPL_NESTS][TPL_NAME_LEN], ContextField[PAR_VALUE_LEN],ContextTag[PAR_VALUE_LEN];
extern char ForbiddenChars[PAR_VALUE_LEN];
extern int	TntRequestType, TplNest;
// dopo il porting, da riaggiornare
extern char  SSDir[PAR_VALUE_LEN];

extern TannitQueryResult* QQres;

extern int		Rumble;
extern char		QueryLabel[128];
extern bool		usedbg, alloctrc;

extern char* cgiCookie;

extern struct	QueryResult * QueryResultSet[QUERY_NUMBER];
extern StrStack *   cycleStk;
extern StrStack *   queryStk;
extern TplVarsStrct	TplVars;
extern int		QueryCounter;
extern int		ReadCycle[CYC_NESTING_DEPTH];
extern int		ValidBlock[CYC_NESTING_DEPTH];
extern int		CndLevel, CycLevel;
extern int		LockLoop;
extern FILE*  debug;


