/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxhttp.h,v $
 * $Revision: 1.12 $  
 * $Author: massimo $
 * $Date: 2002-04-10 16:17:24+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */
#ifndef __ITX_HTTP_H__
#define __ITX_HTTP_H__

#include "itxsocket.h"

#define ITXH_USERAGENT          "IKE"
#define ITXH_DEFAULTPORT        80
#define ITXH_DEFAULTPORT_SSL    443

class itxHttp
{
private:
  itxString  m_useragent;
  itxSocket* m_socket;
  itxString  m_querystring;
  itxString  m_datareceived;
  itxString  m_header;
  long       m_RecvTimeoutSecs;

public:
  itxHttp(char* useragent = ITXH_USERAGENT);
  ~itxHttp() { if (m_socket != NULL) delete m_socket; };

  int       GET(char* querystring, int packetlen = ITXS_PCKLEN, bool ssl = false);
  int       GETBin(char* querystring, char** datareceived, int maxbytes, int packetlen = ITXS_PCKLEN, long timeoutsec = ITXS_TIMEOUTSEC, long timeoutmilli = ITXS_TIMEOUTMILLISEC);
  int       POST(char* querystring, char* data, char* contentType = "application/x-www-form-urlencoded", int packetlen = ITXS_PCKLEN, bool ssl = false);

  int       ReceiveData();

  char*     GetQueryString() { return m_querystring.GetBuffer(); };
  char*     GetDataReceived() { return m_datareceived.GetBuffer(); };
  char*     GetHeader() {return m_header.GetBuffer(); };

  char*     GetIpv4() { return m_socket ? m_socket->GetIpv4() : NULL; };
  char*     GetAddress() { return m_socket ? m_socket->GetAddress() : NULL; };
  int       GetPort() { return m_socket ? m_socket->GetPort() : -1; };
  itxString GetURL();
  void      SetTimeout(long secs) {m_RecvTimeoutSecs = secs;}

private:
  void GetHttpParameter(itxString* destination, int* port, itxString* url, bool ssl = false);
};

#endif // __ITX_HTTP_H__
