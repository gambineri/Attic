/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#ifndef __ITX_TP_COMM_CPP__
#define __ITX_TP_COMM_CPP__
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#include "itxtypes.h"
#include "itannitdef.h"
#include "tpcomm.h"
#include "iappcdim.h"

char* ITannit_TPReceiveFromFile(FILE* fp, PACKET_TYPE msgwaited)
{
	char* appc_packet[ITX_APPC_MAX_NUM_OF_PACKETS];
	char appc_header[ITX_APPC_HEADER_LENGTH + 1];
  char as400_end_info[ITX_APPC_AS400_TX_END_LEN + 1];
	char pn[ITX_APPC_PN_LEN + 1];
	PACKET_TYPE msgtype;
	bool TP_completed = FALSE;
	int packet_number = 0;
	int last_packet = ITX_APPC_LAST_PACKET;
	char* appc_serial;

	memset(appc_packet, '\0', ITX_APPC_MAX_NUM_OF_PACKETS);
	memset(appc_header, '\0', ITX_APPC_HEADER_LENGTH + 1);
	memset(as400_end_info, '\0', ITX_APPC_AS400_TX_END_LEN + 1);
	memset(pn, '\0', ITX_APPC_PN_LEN + 1);

	while (!TP_completed)
	{
		if((appc_packet[packet_number] = TPReceiveFromFile(fp)) == NULL)
			break;

		strncpy(appc_header, appc_packet[packet_number], ITX_APPC_HEADER_LENGTH);
		strncpy(pn, appc_header + ITX_APPC_PN_LOCATOR, ITX_APPC_PN_LEN);
		msgtype = appc_header[ITX_APPC_MSG_TYPE_LOCATOR];
    strncpy(as400_end_info, appc_header, 4);

    if (strcmp(as400_end_info, ITX_APPC_AS400_TX_END) == 0)
      TP_completed = TRUE;

		switch (msgwaited)
		{
		case ITX_APPC_ANY_MSG:
			last_packet = atoi(pn);
			if (last_packet == 0)
			{
				// Il pacchetto che non presenta un numero progressivo viene scartato
				FREE(appc_packet[packet_number]);
	//			appc_packet[packet_number] = NULL;
			}
			else
			{
				// pacchetto valido
				packet_number++;
				if (last_packet == ITX_APPC_LAST_PACKET)
					TP_completed = TRUE;
			}
			break;

		default:
			break;
		}
	}

	appc_serial = TPAssembly(appc_packet, packet_number);

	for (last_packet = 0; last_packet < packet_number; last_packet++)
		FREE(appc_packet[last_packet]);

	return appc_serial;
}


char* TPReceiveFromFile(FILE* fp)
{
	char* packet;
	char trash[ITX_APPC_MAX_INFO_LENGTH];
	int size;

	packet = (char*) calloc(1, ITX_APPC_MAX_INFO_LENGTH + 1);
	if (packet != NULL)
	{
		fgets(packet, ITX_APPC_MAX_INFO_LENGTH, fp);
		size = strlen(packet);
		if(size == ITX_APPC_MAX_INFO_LENGTH)
			fgets(trash, ITX_APPC_MAX_INFO_LENGTH, fp);
    if(size == 0)
      FREE(packet);
	}
	return packet;
}


// Rimuove l'header, assembla i pacchetti e rimuove 
// l'ultimo carattere di fine informazione.
char* TPAssembly(char** pks, int pknum)
{
	int i;
	unsigned long size = 0;
	char* tpstr = NULL; 

	for (i=0; i< pknum; i++)
		size += strlen((pks[i] + ITX_APPC_HEADER_LENGTH)) - 1;

	if ((tpstr = (char*) calloc(1, size + 1)) != NULL)
	{
		size = 0;
		for (i=0; i < pknum; i++)
		{
			strncpy(tpstr + size, pks[i] + ITX_APPC_HEADER_LENGTH, strlen((pks[i] + ITX_APPC_HEADER_LENGTH)) - 1);
			size += strlen((pks[i] + ITX_APPC_HEADER_LENGTH)) - 1;
		}

		tpstr[size - 1] = 0;
	}

	return tpstr;
}


// tp deve essere gi� preparato con spazi bianchi in coda (pad con ' ')
char* TPSendAndReceive(char* tp, FILE* log, PACKET_TYPE msgwaited, int MAX_NUMBER_OF_RETRAY)
{
	char* appc_packet[ITX_APPC_MAX_NUM_OF_PACKETS];
	char appc_header[ITX_APPC_HEADER_LENGTH + 1];
	char pn[ITX_APPC_PN_LEN + 1];
	PACKET_TYPE msgtype;
	bool TP_completed = FALSE;
	int packet_number = 0, retray = 0;
	int last_packet = ITX_APPC_LAST_PACKET;
	char* appc_serial = NULL;

	memset(appc_packet, '\0', ITX_APPC_MAX_NUM_OF_PACKETS);
	memset(appc_header, '\0', ITX_APPC_HEADER_LENGTH + 1);
	memset(pn, '\0', ITX_APPC_PN_LEN + 1);

  if (!IConversation_OpenTransaction(log)) return NULL;

  if (!IConversation_SendData(tp, log)) return NULL;

	while (!TP_completed)
	{
    if (retray == MAX_NUMBER_OF_RETRAY) return NULL;

    if (!IConversation_ReceiveData(&(appc_packet[packet_number]), ITX_APPC_MAX_INFO_LENGTH, log))
      retray++;
    else
    {
      retray = 0;
		  strncpy(appc_header, appc_packet[packet_number], ITX_APPC_HEADER_LENGTH);
		  strncpy(pn, appc_header + ITX_APPC_PN_LOCATOR, ITX_APPC_PN_LEN);
		  msgtype = appc_header[ITX_APPC_MSG_TYPE_LOCATOR];

		  switch (msgwaited)
		  {
		  case ITX_APPC_ANY_MSG:
		  case ITX_APPC_PC_REQ:
			  last_packet = atoi(pn);
			  if (last_packet == 0)
			  {
				  // Il pacchetto che non presenta un numero progressivo � il pacchetto FINE
				  FREE(appc_packet[packet_number]);
				  TP_completed = TRUE;
			  }
			  else
			  {
				  // pacchetto terminatore
          if (appc_packet[packet_number][ITX_APPC_HEADER_LENGTH] == ITX_APPC_ENDINFO_SEP)
          {
  				  FREE(appc_packet[packet_number]);
          }
          else // pacchetto valido
  				  packet_number++;
//				  if (last_packet == ITX_APPC_LAST_PACKET)
//					  TP_completed = TRUE;
			  }
			  break;

		  default:
			  break;
		  }
    }
  }

  IConversation_CloseTransaction(log);

	appc_serial = TPAssembly(appc_packet, packet_number);

	for (last_packet = 0; last_packet < packet_number; last_packet++)
		FREE(appc_packet[last_packet]);

	return appc_serial;
}
