/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

//#include "dbInterface.h"
#include "tannitds.h"
#include "tannit.h"
#include "itannitc.h"
#include "itxtypes.h"
#include "extVars.h"

/*************************************************************************************
NOME			:storedData
Categoria	:interfaccia TQR
attivita'	:restituisce il puntatore alla stringa della query identificata da queryName
					 alla colonna colName alla riga rowNum; errCode accetta in output un
					 eventuale codice di errore;
*************************************************************************************/
char* storedData(int rowNum, char* colName, char* queryName, int* errCode)
{
	int i, colNum = ERR_COL_NOT_FOUND;
	int totalCols = ERR_QUERY_NOT_FOUND;
	struct Record * probingRec;
	struct ColummnHeader * probingCol;

//**/if(usedbg){fprintf(debug, "FUNCTION storedData; STARTED requesting:%s.%s[%d];\n",queryName, colName, rowNum);fflush(debug);}
//**/if(usedbg){fprintf(debug, "queryName:%s\n", queryName);fflush(debug);}
//**/if(usedbg){fprintf(debug, "colName:%s\n", colName);fflush(debug);}
//**/if(usedbg){fprintf(debug, "rowNum:%d\n", rowNum);fflush(debug);}
//**/if(usedbg){fprintf(debug, "QueryCounter:%d\n", QueryCounter);fflush(debug);}

	//  ricerca della query corrispondente al nome in input
	for (i = 0; i < QueryCounter; i++)
	{
		// nome non 
		if ( QueryResultSet[i]->id == 0)
		{
			*errCode = ERR_VOID_QUERY_NAME;
			return (DATA_VALUE_ON_ERROR);
		}

		if (strcmp(QueryResultSet[i]->id, queryName) == 0)
		{
			probingRec = QueryResultSet[i]->recPtr;
			probingCol = QueryResultSet[i]->queryHeader;
			totalCols = QueryResultSet[i]->colsNumber;
		}
	}

	if ( totalCols == ERR_QUERY_NOT_FOUND )
	{
		*errCode = ERR_QUERY_NOT_FOUND;
		return (DATA_VALUE_ON_ERROR);
	}

	for (i = 0; i < totalCols; i++)
	{
		if (strcmp(probingCol[i].name, colName)==0)
		{
			colNum=i;
		}
	}
	if (colNum == ERR_COL_NOT_FOUND)
	{
		*errCode = ERR_COL_NOT_FOUND;
		return (DATA_VALUE_ON_ERROR);
	}

	if (!(probingRec->row))
		return(0);

	for ( i = 1; i < rowNum; i++)
	{
		if (!(probingRec = probingRec->next)) return(0);
	}

	if (rowNum == 0) return(REC_VAL_ZERO);

//**/if(usedbg){fprintf(debug, "FUNCTION storedData; returning:%s;\n\n", probingRec->row[colNum]);fflush(debug);}
	return (probingRec->row[colNum]);
}

int dbInterface(char* queryName, char* query, int firstRec, int recsToStore)
{
  int result = 0;
  
  ITannit_Create(QueryResultSet, &QueryCounter);
  result = ITannit_ExecuteSQLQuery(Odbcdsn, Odbcuid, Odbcpwd, queryName, query, firstRec, recsToStore);
  ITannit_Destroy();

  if (result == ITXFAILED)
  {
    /**/if (usedbg) ITannit_ErrorSQL(debug);
    /**/if (usedbg) ITannit_ErrorSQL(cgiOut);
  }

	return result;
}

