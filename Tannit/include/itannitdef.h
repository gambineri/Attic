/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

#ifndef __ITANNITDEF_H__
#define __ITANNITDEF_H__

#define ITX_APPC_ANY_MSG						'*'
#define ITX_APPC_PC_REQ							'P'
#define ITX_APPC_HS_RPY							'H'
#define ITX_APPC_PC_FTX						  'C'
#define ITX_APPC_HS_FTX						  'T'
#define ITX_APPC_HS_RST						  'I'
#define ITX_APPC_HS_PRT						  'S'
#define ITX_APPC_ERROR						  'E'

#define ITXOPLC_TEST_ENV             1
#define ITXOPLC_PROD_ENV             2
#define ITXUSER_TEST_ENV             3
#define ITXUSER_PROD_ENV             4


// SQL INTERFACE CONSTANTS
#define  ITX_QRS_MAX_NAME_LEN     63
#define  ITX_QRS_MAX_FIELD_LEN    8191
#define  ITX_QRS_MAX_QUERY_LEN    32767
//

// DIMENSIONE APPC COMMUNICATION CONSTANTS
#define ITX_APPC_MAX_INFO_LENGTH		1920
#define ITX_APPC_HEADER_LENGTH			44
#define ITX_APPC_DATA_LEN           ITX_APPC_MAX_INFO_LENGTH - ITX_APPC_HEADER_LENGTH
#define ITX_APPC_MAX_NUM_OF_PACKETS	512
#define ITX_APPC_PN_LEN							5
#define ITX_APPC_PN_LOCATOR					19
#define ITX_APPC_MSG_TYPE_LOCATOR		1

#define ITX_APPC_LAST_PACKET				99999

#define ITX_APPC_PC_FILLER					'-'
#define ITX_APPC_AS400_TX_END       "FINE\0"
#define ITX_APPC_AS400_TX_END_LEN   4
//

// DIMENSIONE PARSING CONSTANTS
#define ITX_APPC_ENDINFO_SEP        '�' //'\xAB'
#define ITX_APPC_RECORD_SEP         '�' //'\xBA'
#define ITX_APPC_FIELD_SEP          '�' //'\xAB'

#define ITX_ASCII_ENDINFO_SEP       '\xAB'
#define ITX_ASCII_RECORD_SEP        '\xBA'
#define ITX_ASCII_FIELD_SEP         '\xAB'

#define ITX_APPC_MAX_RECORD_LENGTH  8192
#define ITX_APPC_MAX_FIELD_LENGTH   256
//

typedef char PACKET_TYPE;

#endif //__ITANNITDEF_H__
