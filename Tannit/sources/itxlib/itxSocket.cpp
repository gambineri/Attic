/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxSocket.cpp,v $
 * $Revision: 1.26 $  
 * $Author: administrator $
 * $Date: 2002-07-23 16:05:32+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */

#include "itxsocket.h"

//====================================================================
// The OPENSSL name enables openssl library support:
// to compile itxlib with this name defined you need to add:
//    1) additional include path to openssl header files;
// to use itxlib and the ssl library from your program you need to add:
//    2) additional library path to libssl32.lib;
//    3) a way to find (at runtime) libeay32.dll and libssl32.dll.
//
// N.B. DO NOT TRY to move next #ifdef block into the .h because
// this way makes the itxlib client DEPENDENT from the name above,
// which is intended ONLY to compile itxlib with additional SSL support.
//====================================================================
#ifdef OPENSSL
  #pragma message("=== Compiling with OPENSSL support ===")
  #include "openssl/err.h"
  #include "openssl/ssl.h"
#endif  //OPENSSL


#pragma pack(4) 

//----------------------------------------------------------------
//--------------- CONSTRUCTION-DESTRUCTION -----------------------

itxSocket::itxSocket()
{
  InitSocketLibrary(); // MUST be alway called here.
}


itxSocket::itxSocket(int port)
{
  InitSocketLibrary(); // MUST be alway called here.
  CreateServerSocket(port);
}

itxSocket::itxSocket(char* destination, int port, bool ssl)
{
  InitSocketLibrary(); // MUST be alway called here.

  try
  {
    CreateTCPStreamSocket();
  }
  catch(itxSocketException e)
  {
//    throw e;
    return;
  }

  struct hostent* hp; 
	SOCKADDR_IN     dest;
  unsigned int    addr=0;

  m_destination = destination;

  memset(&dest, 0, sizeof(SOCKADDR_IN));

  if ((hp = gethostbyname(destination)) == NULL)
  {
	  if ((addr = inet_addr(destination)) == INADDR_NONE)
      throw new itxException(INADDR_NONE, "itxSocket constructor - inet_addr");
    dest.sin_addr.s_addr = addr; 
  }
  else
    memcpy((char*)&(dest.sin_addr),hp->h_addr,hp->h_length); 

  dest.sin_family = AF_INET;
	dest.sin_port = htons(port);
  memset(dest.sin_zero, '\0', 8);

  // Connect the socket
  if (connect(m_socket, (LPSOCKADDR)&dest, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
    throw new itxSocketException("itxSocket constructor - connect", this->m_socket);

  if (ssl)
  {
    if (SSLAllocate())
    {
	    if (!SSLConnect())
        throw new itxSocketException("itxSocket constructor - SSLConnect", this->m_socket);
    }
  }

  m_ipv4 = inet_ntoa(dest.sin_addr); 
  m_port = port;
  m_packetlen = ITXS_PCKLEN;
  m_transferredbytes = 0;
}

itxSocket::~itxSocket() 
{ 
  Close();
  itxSystem sys;
  sys.SOCloseLibrary();
}


//----------------------------------------------------------------
//--------------------------- METHODS ----------------------------

// WARNING!!!
// THIS FUNCTION MUST BE CALLED BEFORE ANY OTHER CALL
// BECAUSE IT INITIALIZES THE PROPER SOCKET LIBRARY.
void itxSocket::InitSocketLibrary()
{
  m_destination.SetEmpty();
  m_port = 0;
  m_socket = INVALID_SOCKET;
  m_ipv4.SetEmpty();
  m_packetlen = 0;
  m_transferredbytes = 0;
  m_tm.tv_sec = ITXS_TIMEOUTSEC;
  m_tm.tv_usec = ITXS_TIMEOUTMILLISEC;
  m_tm_cycle.tv_sec = ITXS_TIMEOUTSEC;
  m_tm_cycle.tv_usec = ITXS_TIMEOUTMILLISEC;  

#ifdef OPENSSL
  m_SSLActive = false;
#endif

  if (!m_Sys.SOInitLibrary())
    throw new itxSocketException("SOInitLibrary", this->m_socket);
}

void itxSocket::SetReceiveTimeout(long sec, long millisec)
{
  m_tm.tv_sec  = sec;
  m_tm.tv_usec = millisec;
}

void itxSocket::SetReceiveCycleTimeout(long sec, long millisec)
{
  m_tm_cycle.tv_sec  = sec;
  m_tm_cycle.tv_usec = millisec;
}

void itxSocket::CreateTCPStreamSocket()
{
  char buf[1024] = {0};

  // Create a TCP/IP stream socket
  m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (m_socket == INVALID_SOCKET)
  {
    itxSystem sys;
    sprintf(buf, "CreateTCPStreamSocket - socket() fails and WSAGetLastError returns %d.", sys.SOGetLastError(m_socket));
    throw new itxSocketException(buf, this->m_socket);
  }
}

bool itxSocket::CreateServerSocket(int port, int maxconn)
{
  SOCKADDR_IN sockaddr;
  bool        sockcreated = true;
  itxSystem   sys;

  m_port = port;

  try
  {
    CreateTCPStreamSocket();
  }
  catch(itxSocketException e)
  {
//    throw e;
    return false;
  }

  memset(&sockaddr, 0, sizeof(SOCKADDR_IN));

  sockaddr.sin_family      = AF_INET;
  sockaddr.sin_port        = htons(m_port);
  sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if ((bind(m_socket, (LPSOCKADDR) &sockaddr, sizeof(sockaddr))) == SOCKET_ERROR)
  {
    throw new itxSocketException("itxSocket::CreateServerSocket() - bind()", this->m_socket);
    sys.SOClose(m_socket);
    sockcreated = false;
  }

	if ((listen(m_socket, maxconn)) == SOCKET_ERROR)
  {
    throw new itxSocketException("itxSocket::CreateServerSocket() - listen()", this->m_socket);
    sys.SOClose(m_socket);
    sockcreated = false;
  }

  return sockcreated;
}


bool itxSocket::Accept(itxSocket* pclientsocket)
{
  SOCKET sockaccept;

  sockaccept = accept(m_socket,NULL, NULL); 
  if (sockaccept == INVALID_SOCKET) 
  {
    throw new itxSocketException("itxSocket::Accept() - accept()", this->m_socket);
    return false;
  }
  
  pclientsocket->AssignSocket(sockaccept);

  return true;
} 

void itxSocket::AssignSocket(SOCKET socket)
{
  if (socket != INVALID_SOCKET)
    m_socket = socket;
  else 
    m_socket = INVALID_SOCKET;
}

int itxSocket::Send(char* datatosend, int bytes_to_send)
{
  if ((m_transferredbytes = send(m_socket, datatosend, bytes_to_send, NULL)) == SOCKET_ERROR)
    throw new itxSocketException("itxSocket::Send", this->m_socket);

  return m_transferredbytes;
}

int itxSocket::Send(itxString* datatosend)
{
  return Send(datatosend->GetBuffer(), datatosend->Len());
}

// non si occupa della apertura o chiusura del file
int itxSocket::BulkSend(FILE* fp, int packetlen)
{
  char* buffer = NULL;
  int datatosend;
  int bytesent = 0;

  m_transferredbytes = 0;

  if (fp == NULL) return 0;

  m_packetlen = packetlen;

  if ((buffer = (char*) malloc((packetlen + 1) * sizeof(char))) == NULL)
    throw new itxSocketException("itxSocket::BulkSend - malloc", this->m_socket);
  
  while(!feof(fp))
  {
    datatosend = fread(buffer, 1, packetlen, fp);
    if (datatosend < packetlen && ferror(fp) != 0)
      throw new itxSocketException("itxSocket::BulkSend - fread", this->m_socket);

    if ((bytesent = send(m_socket, buffer, datatosend, NULL)) == SOCKET_ERROR)
      throw new itxSocketException("itxSocket::BulkSend - send", this->m_socket);

    m_transferredbytes += bytesent;
  }    

  return m_transferredbytes;
}

int itxSocket::Receive(itxString* datatoreceive, int packetlen)
{
  int     nfds = 0, result, dataread;
  fd_set  datatoread;
  char*   buffer = NULL;

  m_Sys.SOInitDescriptors(&datatoread, m_socket);

  m_packetlen         = packetlen;
  m_transferredbytes  = 0;
  
  if ((buffer = (char*) malloc((packetlen + 1) * sizeof(char))) == NULL)
    throw new itxSocketException("itxSocket::Receive - malloc", this->m_socket);
  
  dataread       = packetlen;
  *datatoreceive = "";

  // select() function returns:
  // - the total number of socket handles that are ready 
  // - 0 if the time limit expired
  // - SOCKET_ERROR if an error occurred
  // timeout controls how long the select() can take to complete:
  // - null pointer: select() will block
  // - {0, 0}: select() will return immediately (nonblocking select) 
  result = select(nfds, &datatoread, NULL, NULL, &m_tm);

  while (!(result == 0 || result == SOCKET_ERROR)) //socket has been checked for readability 
  {
    dataread = recv(m_socket, buffer, m_packetlen, NULL);

    if (dataread == 0)  //Connection was gracefully closed
      break; 
    else if (dataread != SOCKET_ERROR)
    {
      buffer[dataread] = 0;
      *datatoreceive += buffer;
      m_transferredbytes += dataread;
    }
    else
      throw new itxException(dataread, "itxSocket::Receive - recv");
    
    //checking for socket readability
      result = select(nfds, &datatoread, NULL, NULL, &m_tm_cycle);
  }

  free(buffer); //not needed anymore

  if (result == SOCKET_ERROR)
    throw new itxSocketException("itxSocket::Receive - select", this->m_socket);
  
  return m_transferredbytes;
}

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *  NON USARE la BlockingReceive !!! OBSOLETA !!!
 *  (Lasciata solo per retro-compatibilita`)
 *  Equivale alle due chiamate:
 *    SetReceiveCycleTimeout(0, 0);
 *    Receive(...);
 */
int itxSocket::BlockingReceive(itxString* datatoreceive, int packetlen)
{
  int             nfds = 0, result, dataread;
  fd_set          datatoread;
  char*           buffer = NULL;
  struct timeval  tm;

  tm.tv_sec  = 0;
  tm.tv_usec = 0;

  m_Sys.SOInitDescriptors(&datatoread, m_socket);

  m_packetlen        = packetlen;
  m_transferredbytes = 0;
  
  if ((buffer = (char*) malloc((packetlen + 1) * sizeof(char))) == NULL)
    throw new itxSocketException("itxSocket::Receive - malloc", this->m_socket);
  
  dataread       = packetlen;
  *datatoreceive = "";

  // select() function returns:
  // - the total number of socket handles that are ready 
  // - 0 if the time limit expired
  // - SOCKET_ERROR if an error occurred
  // timeout controls how long the select() can take to complete:
  // - null pointer: select() will block
  // - {0, 0}: select() will return immediately (nonblocking select) 
 
   //Receive without timeout
//  result = select(nfds, &datatoread, NULL, NULL, NULL);

  result = select(nfds, &datatoread, NULL, NULL, &m_tm);
  
  while (!(result == 0 || result == SOCKET_ERROR)) //socket has been checked for readability 
  {
    dataread = recv(m_socket, buffer, m_packetlen, NULL);

    if (dataread == 0)  //Connection was gracefully closed
      break;
    else if (dataread != SOCKET_ERROR)
    {
      buffer[dataread] = 0;
      *datatoreceive += buffer;
      m_transferredbytes += dataread;
    }
    else
      throw new itxException(dataread, "itxSocket::Receive - recv");
    
    //checking for socket readability
    result = select(nfds, &datatoread, NULL, NULL, &tm);
  }

  free(buffer); //not needed anymore

  if (result == SOCKET_ERROR)
    throw new itxSocketException("itxSocket::Receive - select", this->m_socket);
  
  return m_transferredbytes;
}


// non si occupa della apertura o chiusura del file
//TBD DA METTERE A POSTO COME LA Receive
int itxSocket::BulkReceive(FILE* fp, int packetlen)
{
  int nfds = 0, result, dataread, totalbyteread = 0;
  fd_set datatoread;
  char* buffer = NULL;

  m_Sys.SOInitDescriptors(&datatoread, m_socket);

  m_packetlen = packetlen;
  m_transferredbytes = 0;

  if ((buffer = (char*) malloc((packetlen + 1) * sizeof(char))) == NULL)
    throw new itxSocketException("itxSocket::BulkReceive - malloc", this->m_socket);
  dataread = packetlen;

  result = select(nfds, &datatoread, NULL, NULL, &m_tm);

  while (!(result == 0 || result == SOCKET_ERROR))
  {
    dataread = recv(m_socket, buffer, m_packetlen, NULL);
    if (!(dataread == 0 || dataread == SOCKET_ERROR))
    {
      buffer[dataread] = 0;
      if (fwrite(buffer, 1, dataread, fp) < dataread)
        throw new itxSocketException("itxSocket::BulkReceive - fwrite", this->m_socket);
      m_transferredbytes += dataread;
    }
    else
      throw new itxException(dataread, "itxSocket::Receive - recv");
    
    if (dataread == m_packetlen)
      result = select(nfds, &datatoread, NULL, NULL, &m_tm);
    else
      break;
  }

  if (result == 0 || result == SOCKET_ERROR)
      throw new itxSocketException("itxSocket::Receive - select", this->m_socket);
  
  return m_transferredbytes;
}

void  itxSocket::Close()
{
  SSLFree();

  if (m_socket != INVALID_SOCKET) 
  {
    m_Sys.SOClose(m_socket); 
    m_socket = INVALID_SOCKET;
  }
}

int itxSocket::BinReceive(char* datatoreceive, int maxbytes, int packetlen)
{
  int     nfds = 0, result, dataread;
  fd_set  datatoread;
  char*   buffer = NULL;
  char*   startdata = datatoreceive;

  m_Sys.SOInitDescriptors(&datatoread, m_socket);

  m_packetlen = packetlen;
  m_transferredbytes = 0;
  
  if ((buffer = (char*) malloc((packetlen + 1) * sizeof(char))) == NULL)
    throw new itxSocketException("itxSocket::Receive - malloc", this->m_socket);
  
  dataread       = packetlen;
  *datatoreceive = 0;

  // select() function returns:
  // - the total number of socket handles that are ready 
  // - 0 if the time limit expired
  // - SOCKET_ERROR if an error occurred
  // timeout controls how long the select() can take to complete:
  // - null pointer: select() will block
  // - {0, 0}: select() will return immediately (nonblocking select) 
  result = select(nfds, &datatoread, NULL, NULL, &m_tm);

  while (!(result == 0 || result == SOCKET_ERROR)) //socket has been checked for readability 
  {
    dataread = recv(m_socket, buffer, m_packetlen, NULL);

    if (dataread == 0)  //Connection was gracefully closed
      break; 
    else if (dataread != SOCKET_ERROR)
    {
      m_transferredbytes += dataread;
      if (maxbytes <= m_transferredbytes)
      {
        m_transferredbytes -= dataread;
        break;
      }
      buffer[dataread] = 0;
      memcpy(startdata, buffer, dataread);
      startdata += dataread;
    }
    else
      throw new itxException(dataread, "itxSocket::Receive - recv");
    
    //checking for socket readability
      result = select(nfds, &datatoread, NULL, NULL, &m_tm);
  }

  free(buffer); //not needed anymore

  if (result == SOCKET_ERROR)
    throw new itxSocketException("itxSocket::Receive - select", this->m_socket);
  
  return m_transferredbytes;
}

//SSL support  ----------------------------
bool itxSocket::SSLConnect()
{
#ifdef OPENSSL
  if (!m_SSLActive)
    return false;

	SSL_set_fd((SSL*)m_ssl, m_socket);
	return (SSL_connect((SSL*)m_ssl) == -1 ? false : true);
#else
	return false;
#endif
}

bool itxSocket::SSLAllocate()
{
#ifdef OPENSSL
	SSL_load_error_strings();
	SSLeay_add_ssl_algorithms();

	SSL_METHOD* meth = SSLv2_client_method();

	if ((m_ctx = SSL_CTX_new(meth)) == NULL)
		return false;

	if ((m_ssl = SSL_new((SSL_CTX*)m_ctx)) == false) 
		return false;

  m_SSLActive = true;
  return true;
#else
	return false;
#endif
}

void itxSocket::SSLFree()
{
#ifdef OPENSSL

  if (!m_SSLActive)
    return;

  SSL_shutdown((SSL*)m_ssl);
  SSL_free((SSL*)m_ssl);
  SSL_CTX_free((SSL_CTX*)m_ctx);
#endif
}

int itxSocket::SSLSend(char* datatosend, int bytes_to_send)
{
#ifdef OPENSSL
  if (!m_SSLActive)
    return 0;

  if ((m_transferredbytes = SSL_write((SSL*)m_ssl, datatosend, bytes_to_send)) == SOCKET_ERROR)
    throw new itxSocketException("itxSocket::SSLSend", this->m_socket);

  return m_transferredbytes;
#else
  return 0;
#endif
}

int itxSocket::SSLSend(itxString* datatosend)
{
  return SSLSend(datatosend->GetBuffer(), datatosend->Len());
}

int itxSocket::SSLReceive(itxString* datatoreceive)
{
#ifdef OPENSSL
  
  if (!m_SSLActive)
    return 0;

  int   err = SOCKET_ERROR - 1;
	char  szBuff[4096];

  datatoreceive->SetEmpty();

	while (err != SOCKET_ERROR && err != 0)
	{
		if ((err = SSL_read((SSL*)m_ssl, szBuff, 4096)) == SOCKET_ERROR)
			break;
		else
		{
			szBuff[err] = 0;
			*datatoreceive += szBuff;
		}
	}

  return datatoreceive->Len();
#else
  return 0;
#endif
}
