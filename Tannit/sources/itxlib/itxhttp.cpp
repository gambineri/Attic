/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxhttp.cpp,v $
 * $Revision: 1.13 $  
 * $Author: massimo $
 * $Date: 2002-05-02 16:28:48+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */

#include "itxhttp.h"

itxHttp::itxHttp(char* useragent)
{
  m_socket = NULL;
  m_useragent = useragent;
  m_querystring.SetEmpty();
  m_datareceived.SetEmpty();
  m_header.SetEmpty();
  m_RecvTimeoutSecs = 300;
}

itxString itxHttp::GetURL()
{
  itxString appo = m_querystring;
  appo.PurgeSubString("http://");
  appo.Right(appo.Len() - appo.FindSubString("/"));

  return appo;
}

void itxHttp::GetHttpParameter(itxString* destination, int* port, itxString* url, bool ssl)
{
  //from rfc2616 - HTTP/1.1
  //http_URL = "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]]

  itxString appo = m_querystring;
  appo.PurgeSubString("http://");
  appo.PurgeSubString("https://");

  int firstslash = appo.FindSubString("/");
  int pos_colon = appo.FindSubString(":");
  int len = appo.Len();

  //Check if the colon is not the port delimiter, but something else
  if (pos_colon > firstslash)
    pos_colon = len;

  *url = appo;
  url->Right(len - firstslash); 

  *destination = appo;
  if (pos_colon == len)
  {
    destination->Left(firstslash);
    *port = (ssl ? ITXH_DEFAULTPORT_SSL : ITXH_DEFAULTPORT);
  }
  else
  {
    destination->Left(pos_colon);
    appo.Left(firstslash);
    appo.Right(appo.Len() - (pos_colon + 1));
    *port = atoi(appo.GetBuffer());
  }
}

int itxHttp::GET(char* querystring, int packetlen, bool ssl)
{
  itxString datatosend;
  int bytesreceived;

  m_querystring = querystring;
  m_querystring.Trim();

  itxString destination;
  int port;
  itxString url; 

  GetHttpParameter(&destination, &port, &url, ssl);
  url.SubstituteSubString(" ", "%20"); //Spaces must be escaped

  try
  {
    m_socket = new itxSocket(destination.GetBuffer(), port, ssl);
    m_socket->SetReceiveTimeout(m_RecvTimeoutSecs, 0);
    m_socket->SetReceiveCycleTimeout(m_RecvTimeoutSecs, 0);

    // HTTP GET Method string preparation
    datatosend = "GET ";
    datatosend += url;
    datatosend += " HTTP/1.0\r\nHost: ";
    datatosend += destination;
    datatosend += "\r\nUser-Agent: ";
    datatosend += m_useragent;
    datatosend += "\r\n\r\n";

    if (ssl)
    {
      m_socket->SSLSend(&datatosend);
      bytesreceived = m_socket->SSLReceive(&m_datareceived);
    }
    else
    {
      m_socket->Send(&datatosend);
      bytesreceived = m_socket->Receive(&m_datareceived);
    }

    m_header = m_datareceived;
    int end_header = m_header.FindSubString("\r\n\r\n") + 4;

    //Chunked transfer-encoding management (?TO VERIFY?)
//    if (m_header.FindSubString("chunked") < m_header.Len())
//      end_header += (m_header.FindSubString("\r\n", end_header) + 2);

    m_header.Left(end_header);
    m_datareceived.Right(bytesreceived - end_header);
  }
  catch(itxException* e)
  {
    throw (itxException*) e;
  }

  return bytesreceived;
}

int itxHttp::GETBin(char* querystring, char** datareceived, int maxbytes, int packetlen, long timeoutsec, long timeoutmilli)
{
  itxString datatosend;
  int bytesreceived;

  m_querystring = querystring;
  m_querystring.Trim();

  itxString destination;
  int port;
  itxString url; 

  GetHttpParameter(&destination, &port, &url);

  url.SubstituteSubString(" ", "%20"); //Spaces must be escaped

  try
  {
    m_socket = new itxSocket(destination.GetBuffer(), port);

    // HTTP GET Method string preparation
    datatosend = "GET ";
    datatosend += url;
    datatosend += " HTTP/1.0\r\nHost: ";
    datatosend += destination;
    datatosend += "\r\nUser-Agent: ";
    datatosend += m_useragent;
    datatosend += "\r\n\r\n";

    m_socket->Send(&datatosend);
    m_socket->SetReceiveTimeout(timeoutsec, timeoutmilli);
    bytesreceived = m_socket->BinReceive(*datareceived, maxbytes, packetlen);

    m_header = "" ;

    char* rawdata = strstr(*datareceived, "\r\n\r\n");
    m_header.Strncpy(*datareceived, (unsigned int) rawdata - (unsigned int) *datareceived);
    *datareceived = rawdata + 4;
    bytesreceived -= m_header.Len();
  }
  catch(itxException* e)
  {
    throw (itxException*) e;
  }

  return bytesreceived;
}

int itxHttp::POST(char* querystring, char* data, char* contentType, int packetlen, bool ssl)
{
  itxString destination;
  itxString datatosend;
  itxString url; 
  itxString appo; 
  int       bytesreceived;
  int       port;

  m_querystring = querystring;
  m_querystring.Trim();

  GetHttpParameter(&destination, &port, &url, ssl);
  url.SubstituteSubString(" ", "%20"); //Spaces must be escaped
  
  try
  {
    m_socket = new itxSocket(destination.GetBuffer(), port, ssl);
    m_socket->SetReceiveTimeout(m_RecvTimeoutSecs, 0);
    m_socket->SetReceiveCycleTimeout(m_RecvTimeoutSecs, 0);

    // HTTP POST Method string preparation
    datatosend = "POST ";

    datatosend += url;
    datatosend += " HTTP/1.0\r\nHost: ";
    datatosend += destination;
    datatosend += "\r\nUser-Agent: ";
    datatosend += m_useragent;
    datatosend += "\r\nContent-Type: ";
    datatosend += contentType;
    datatosend += "\r\n";

    datatosend += "Content-Length: ";
    datatosend += (int)(data != NULL ? strlen(data) : 0);
    datatosend += "\r\n\r\n";
    datatosend += data;

    if (ssl)
    {
      m_socket->SSLSend(&datatosend);
      bytesreceived = m_socket->SSLReceive(&m_datareceived);
    }
    else
    {
      m_socket->Send(&datatosend);
      bytesreceived = m_socket->Receive(&m_datareceived);
    }

    m_header = m_datareceived;
    int end_header = m_header.FindSubString("\r\n\r\n") + 4;

    m_header.Left(end_header);
    m_datareceived.Right(bytesreceived - end_header);
  }
  catch(itxException* e)
  {
    throw (itxException*) e;
  }

  return bytesreceived;
}

