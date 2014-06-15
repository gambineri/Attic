/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

// AITECSA s.r.l.   1999
//    filename:     tpcomm.h
//    description:  itannit AS400 interface library
//                  transaction program main communication procedure
//    project:      ASWEB

#ifndef __ITX_TP_COMM_H__
#define __ITX_TP_COMM_H__

extern "C"
{
	char* TP_Simul(FILE* fp);
	char* TPReceiveFromFile(FILE* fp);
  char* TPExecute(char* tp, FILE* log);
	char* TPAssembly(char** pks, int pknum);
  char* TPSendAndReceive(char* tp, FILE* log, PACKET_TYPE msgwaited, int maxretray);
}

#endif //__ITX_TP_COMM_H__