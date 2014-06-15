/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#ifndef __ITX_TANNITDS_H__
#define __ITX_TANNITDS_H__

#define	COL_NAME_LEN			128
#define	QUERY_NAME_LEN		64
#define	QUERY_NUMBER			10000

#define	STARTING_ROW					1
#define	ROWS_TO_STORE					512

typedef struct Record
{
	char ** row;
	struct Record * next;
} TannitRecord;

typedef struct ColummnHeader
{
	char  name[COL_NAME_LEN];
	short sqlDataType;
	long int colummnSize;
} TannitColumnHeader;

typedef struct QueryResult
{
	char id[QUERY_NAME_LEN];
	int startingRow, rowsToStore;
	int actualRow, totalRows;
	int colsNumber;
	TannitRecord* current_record;
	TannitRecord* recPtr;
	TannitColumnHeader* queryHeader;
} TannitQueryResult, *PTannitQueryResult;

#endif //__ITX_TANNITDS_H__
