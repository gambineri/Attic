/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxsmtp.cpp,v $
 * $Revision: 1.6 $  
 * $Author: andreal $
 * $Date: 2002-05-17 19:24:46+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */

#include "itxsmtp.h"


itxSmtp::itxSmtp(char* server, char* domain, char* sender)
{
  m_socket = NULL;
  m_sender = sender;
  m_server = server;
  m_domain = domain;
  m_to = NULL;
  m_subject.SetEmpty();
  m_body.SetEmpty();
  m_attachments[0] = NULL;
  m_NumAttach = 0;
  m_AttachedData = NULL;
  m_AttachedDataLen = 0;
}

itxSmtp::~itxSmtp()
{ 
  if (m_socket != NULL) 
    delete m_socket;
  RemoveAttachments();
}

void itxSmtp::AddRcpt(char* to)
{
  Recipient*  newrcp = NULL;
  Recipient** cursor = &m_to;

  try
  {
    newrcp = (Recipient*) new Recipient(to);
    while (*cursor != NULL) 
      cursor = &((*cursor)->m_next);
    *cursor = newrcp;
  }
  catch(...)
  {
    throw new itxException(0, "itxSmtp::AddRcpt");
  }
}

int itxSmtp::ExchangeMessage(char* cmd, char* arg)
{
  itxString datatosend;
  itxString datareceived;

  datatosend = cmd;
  datatosend += " ";
  datatosend += arg;
  datatosend += "\r\n";

  m_socket->Send(&datatosend);
  m_socket->Receive(&datareceived);
  return atoi(datareceived.GetBuffer());
}

bool  itxSmtp::AddAttachment(char* attachment)
{
  itxString* pAtt = new itxString;

  if (pAtt != NULL)
  {
    *pAtt = attachment;
    m_attachments[m_NumAttach++] = pAtt;
    m_attachments[m_NumAttach] = NULL;
    return true;
  }
  
  return false;
}

bool itxSmtp::RemoveAttachments()
{
  for (int i=0; i<m_NumAttach; i++)
  {
    delete m_attachments[i];
    m_attachments[i] = NULL;
  }

  m_NumAttach = 0;

  return true;
}

int itxSmtp::Mail()
{
  int         retcode = 0;
  int         bytesreceived;
  itxString   datareceived;
  itxString   data;
  Recipient** cursor = &m_to;
  
  try
  {
    m_socket = new itxSocket(m_server.GetBuffer(), ITXM_SMTPPORT);
    m_socket->SetReceiveTimeout(20, 0);
    m_socket->SetReceiveCycleTimeout(0, 0);

    // receive welcome
    bytesreceived = m_socket->Receive(&datareceived);

    // presentation
    EXIT_ON_CRITICAL(ExchangeMessage("HELO", m_domain.GetBuffer()));

    // header and message body
    EXIT_ON_CRITICAL(ExchangeMessage("MAIL FROM:", m_sender.GetBuffer()));
    
    cursor = &m_to;
    while (*cursor != NULL)
    { 
      EXIT_ON_CRITICAL(ExchangeMessage("RCPT TO:", (*cursor)->m_address.GetBuffer()));
      cursor = &((*cursor)->m_next);      
    }

    data += "From: ";
    data += m_sender;
    data += "\n";

    data += "To: ";
    cursor = &m_to;
    while (*cursor != NULL)
    { 
      data += (*cursor)->m_address;
      cursor = &((*cursor)->m_next);      
      if (*cursor != NULL) data += ", ";
    }

    data += "\n";
    data += "Subject: ";
    data += m_subject;
    data += "\n";
    data += MIME_VERSION;

    if(m_attachments[0] != NULL)
    {
      data += "Content-Type: multipart/mixed; boundary=\"";
      data += BOUNDARY;
      data += "\"\n";
    }

    data += "\r\n";

    // body
    EXIT_ON_CRITICAL(ExchangeMessage("DATA", ""));
    
    if(m_attachments[0] != NULL)
    {
      data += "--";
      data += BOUNDARY;
      data += '\n';
      data += "Content-Type: text/plain";// charset=\"iso-8859-1\"";
      data += '\n';
      data += "Content-Transfer-Encoding: quoted-printable";
      data += '\n';
      data += "\r\n";
    }

    data += m_body;
    data += '\n';
    data += "\r\n";
    
    m_socket->Send(data.GetBuffer(), data.Len());

    //Invio di eventuali attachments
    if(m_attachments[0] != NULL)
      SendAttachments();

    EXIT_ON_CRITICAL(ExchangeMessage("", "\r\n."));
    
    // salutation
    EXIT_ON_CRITICAL(ExchangeMessage("QUIT", ""));
  }
  catch(itxException* e)
  {
    throw e;
  }

  return retcode;
}

bool itxSmtp::EncodeBase64(int attidx)
{
  FILE* fp;

  if( (fp = fopen((*m_attachments[attidx]).GetBuffer(), "rb" )) == NULL )
    return false;
    
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 0, SEEK_END);
	int len = ftell(fp);
  
  if ((m_AttachedData = (unsigned char*)malloc(len*2)) == NULL)
    return false;

	fseek(fp, 0, SEEK_SET);
  EncodeFromFile(fp);
	fclose(fp);

  return true;
}


/*
** base64 encode a stream adding padding and line breaks as per spec.
*/
//--------------------------------------------------------
void itxSmtp::EncodeBlock(unsigned char in[3], unsigned char out[4], int len)
{
  static const char cb64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  out[0] = cb64[ in[0] >> 2 ];
  out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
  out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
  out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}

//--------------------------------------------------------
void itxSmtp::EncodeFromFile(FILE* infile)
{
  unsigned char in[3];
  int i, len, blocksout = 0;

  m_AttachedDataLen = 0;  //paranoic

  while (!feof(infile)) 
  {
    len = 0;
    for (i = 0; i < 3; i++) 
    {
      in[i] = (unsigned char) getc(infile);
      if (!feof(infile)) 
        len++;
      else 
        in[i] = 0;
    }
    
    if (len) 
    {
      EncodeBlock(in, &m_AttachedData[m_AttachedDataLen], B64_DEF_LINE_SIZE);
      m_AttachedDataLen += 4;

      blocksout++;
    }
    
    if (blocksout >= (B64_DEF_LINE_SIZE/4) || feof(infile)) 
    {
      if (blocksout) 
      {
        m_AttachedData[m_AttachedDataLen++] = (unsigned char)'\r';
        m_AttachedData[m_AttachedDataLen++] = (unsigned char)'\n';
      }
      
      blocksout = 0;
    }
  }
}


bool itxSmtp::SendAttachments()
{
  itxString data;
  itxString fname;

  int i=0;
  while(m_attachments[i] != NULL)
  {
    if (!EncodeBase64(i))
      continue;

    fname = *m_attachments[i];
    fname.Right(fname.Len() - fname.RightInstr("\\"));

    data  = "--";
    data += BOUNDARY;
    data += '\n';
//    data += "Content-Type: text/plain";           //Forse bisognera' differenziarli...
    data += "Content-Type: ; name=\"";     //Forse bisognera' differenziarli...
    data += fname;
    data += "\"";
    data += '\n';
    data += "Content-Transfer-Encoding: Base64";
    data += '\n';
    data += "Content-Disposition: attachment; filename=\"";
    data += fname;
    data += "\"\r\n\r\n";

    m_socket->Send(data.GetBuffer(), data.Len());
    m_socket->Send((char*)m_AttachedData, m_AttachedDataLen);
    m_socket->Send("\r\n\r\n", 4);
    
    data.SetEmpty();
    i++;

    if (m_AttachedData != NULL)
    {
      free(m_AttachedData);
      m_AttachedDataLen = 0;
    }
  }

  //Chiusura blocchi multipart
  data += "--";
  data += BOUNDARY;
  data += "--\n\r\n";

  m_socket->Send(data.GetBuffer(), data.Len());

  return true;
}

