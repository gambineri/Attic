/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxsmtp.h,v $
 * $Revision: 1.6 $  
 * $Author: andreal $
 * $Date: 2002-05-17 19:24:46+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */
#ifndef __ITX_SMTP_H__
#define __ITX_SMTP_H__

#include "itxsocket.h"

#define ITXM_MAXATTACHMENTS     16
#define ITXM_CRITICAL_ERROR     400
#define ITXM_SMTPPORT           25
#define EXIT_ON_CRITICAL(a)     { if(a >= ITXM_CRITICAL_ERROR) return a; }
#define MIME_VERSION            "MIME-Version: 1.0\r\n"
#define BOUNDARY                "==80SW223H3ACKAC232731CAC0473000328A1=="
#define B64_DEF_LINE_SIZE       72


class Recipient
{
public:
  itxString  m_address;
  Recipient* m_next;  

  Recipient(char* to) { m_address = to; m_next = NULL; }
};


class itxSmtp
{
private:
  itxSocket*  m_socket;
  itxString   m_sender;
  itxString   m_server;
  itxString   m_domain;
  itxString   m_subject; 
  Recipient*  m_to; 
  itxString   m_body;
  itxString*  m_attachments[ITXM_MAXATTACHMENTS];
  int         m_NumAttach;
  unsigned char* m_AttachedData;
  int         m_AttachedDataLen;

public:
  itxSmtp(char* server, char* domain, char* m_sender);
  ~itxSmtp();

  int Mail();

  void AddRcpt(char* to);
  void SetSubject(char* subject) { m_subject = subject; };
  void SetBody(char* body) { m_body = body; };

  char*     GetIpv4() { return m_socket ? m_socket->GetIpv4() : NULL; }
  char*     GetAddress() { return m_socket ? m_socket->GetAddress() : NULL; }
  int       GetPort() { return m_socket ? m_socket->GetPort() : -1; }
  itxString GetSender() { return m_sender; }
  bool      AddAttachment(char* attachment);
  bool      RemoveAttachments();

private:
  int       ExchangeMessage(char* cmd, char* arg);
  bool      SendAttachments();
  bool      EncodeBase64(int attidx);
  void      EncodeFromFile(FILE* infile);
  void      EncodeBlock(unsigned char in[3], unsigned char out[4], int len);
};



#endif

