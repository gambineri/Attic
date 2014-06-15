/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxsocket.h,v $
 * $Revision: 1.36 $  
 * $Author: massimo $
 * $Date: 2002-06-25 11:50:49+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */
#ifndef __ITX_SOCKET_H__
#define __ITX_SOCKET_H__


#include <memory.h>
#include <exception>
#include <stdio.h>

#include "itxstring.h"
#include "itxexception.h"
#include "itxsystem.h"


#define ITXS_PCKLEN               4096
#define ITXS_TIMEOUTSEC           10
#define ITXS_TIMEOUTMILLISEC      0



class itxSocketException : public itxException
{
public:

  itxSocketException(char* procedure, SOCKET socket) : itxException(0, procedure)
  {
    itxSystem sys;
    m_errornumber = sys.SOGetLastError(socket);
  }
};

class itxSocket
{
private:
  itxString       m_destination;
  int             m_port;
  SOCKET	        m_socket;
  itxException    m_exception;
  itxString       m_ipv4;
  itxSystem       m_Sys;

  int             m_packetlen;
  int             m_transferredbytes;
  struct timeval  m_tm; 
  struct timeval  m_tm_cycle;

  //SSL
  bool            m_SSLActive;
	void*	          m_ctx;
	void*		        m_ssl;
//	X509*		    server_cert = 0;

public:
  //CONSTRUCTION-DESTRUCTION  
  itxSocket();

  //Create a server socket listening at the specified port 
  itxSocket(int port); 

  //Create a socket connecting to the specified destination
  itxSocket(char* destination, int port, bool ssl = false); 

  ~itxSocket();

  //METHODS
  int       Send(itxString* datatosend);
  int       Send(char* datatosend, int bytes_to_send);

  int       Receive(itxString* datatoreceive, int packetlen = ITXS_PCKLEN);
  int       BinReceive(char* datatoreceive, int maxbytes, int packetlen = ITXS_PCKLEN);
  int       BulkSend(FILE* fp, int packetlen = ITXS_PCKLEN);
  int       BulkReceive(FILE* fp, int packetlen = ITXS_PCKLEN);
  int       BlockingReceive(itxString* datatoreceive, int packetlen = ITXS_PCKLEN);
            
  char*     GetIpv4() { return m_ipv4.GetBuffer(); }
  char*     GetAddress() { return m_destination.GetBuffer(); }
  int       GetPort() { return m_port; }
  int       GetPacketLen() { return m_packetlen; }
  int       GetLastTransferredBytes() { return m_transferredbytes;}
  
  bool      CreateServerSocket(int port, int maxconn = SOMAXCONN);
  bool      Accept(itxSocket* pclientsocket);
  void      SetReceiveTimeout(long sec, long millisec);
  void      SetReceiveCycleTimeout(long sec, long millisec);
  void      Close();

  int       SSLSend(itxString* datatosend);
  int       SSLSend(char* datatosend, int bytes_to_send);
  int       SSLReceive(itxString* datatoreceive);
    
private:
  void      InitSocketLibrary();
  void      CreateTCPStreamSocket();
  void      AssignSocket(SOCKET socket);
  bool      SSLAllocate();
  void      SSLFree();
  bool      SSLConnect();
};

#endif // __ITX_SOCKET_H__