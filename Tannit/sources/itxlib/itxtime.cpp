/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxtime.cpp,v $
 * $Revision: 1.6 $  
 * $Author: administrator $
 * $Date: 2002-07-23 16:05:31+02 $
 *
 * Implementation of the itxTime object (time_t format object)
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */
 
#ifndef __ITXTIME_CPP__
#define __ITXTIME_CPP__
#endif

#include <time.h>
#include <memory.h>

#include "itxtime.h" 

//conversion from ("yyyymmdd", secs from midnight) to time_t
time_t itxTime::TimeConversion()
{
  char buffer[10];

  char* dateref = m_YyyyMmDd.GetBuffer();
  int secs = m_SecsFromMid;
  
  m_tm.tm_hour  = secs / 3600; 
  m_tm.tm_min   = (secs - m_tm.tm_hour * 3600) / 60;
  m_tm.tm_sec   = secs - m_tm.tm_hour * 3600 - m_tm.tm_min * 60;

  memcpy(buffer, &dateref[0], 4);
  buffer[4] = '\0';
  m_tm.tm_year  = atoi(buffer) - 1900; 

  memcpy(buffer, &dateref[4], 2);
  buffer[2] = '\0';
  m_tm.tm_mon  = atoi(buffer) - 1; 

  memcpy(buffer, &dateref[6], 2);
  buffer[2] = '\0';
  m_tm.tm_mday  = atoi(buffer); 

  m_tm.tm_wday  = 0; 
  m_tm.tm_yday  = 0; 
  m_tm.tm_isdst = -1;
  
  m_t = mktime(&m_tm);

	return m_t;
}

//conversion from time_t to ("yyyymmdd", secs from midnight)
void itxTime::SetFrom(time_t t)
{
  char buffer[10];

  m_tm = *localtime(&t);
  m_t = t;
      
  m_SecsFromMid = m_tm.tm_hour * 3600; 
  m_SecsFromMid += m_tm.tm_min * 60;
  m_SecsFromMid += m_tm.tm_sec;

  sprintf(buffer, "%d", 1900 + m_tm.tm_year); 
  m_YyyyMmDd = buffer;
  sprintf(buffer, "%02d", 1 + m_tm.tm_mon); 
  m_YyyyMmDd += buffer;
  sprintf(buffer, "%02d", m_tm.tm_mday); 
  m_YyyyMmDd += buffer;
}

void itxTime::SetFrom(int d, int m, int y)
{
  char buffer[10];

  sprintf(buffer, "%d", y); 
  m_YyyyMmDd = buffer;
  sprintf(buffer, "%02d", m); 
  m_YyyyMmDd += buffer;
  sprintf(buffer, "%02d", d); 
  m_YyyyMmDd += buffer;
}

int itxTime::SecFromMid(itxString* hour)
{
  hour->Trim();

  itxString l(*hour);
  itxString r(*hour);
  l.Left(2);
  r.Right(2);

  return atoi(l.GetBuffer()) * 3600 + atoi(r.GetBuffer()) * 60;
}

void itxTime::Tomorrow()
{
  char buffer[10];

  m_tm.tm_mday++;
  m_t = mktime(&m_tm);

  m_SecsFromMid = m_tm.tm_hour * 3600; 
  m_SecsFromMid += m_tm.tm_min * 60;
  m_SecsFromMid += m_tm.tm_sec;

  sprintf(buffer, "%d", 1900 + m_tm.tm_year); 
  m_YyyyMmDd = buffer;
  sprintf(buffer, "%02d", 1 + m_tm.tm_mon); 
  m_YyyyMmDd += buffer;
  sprintf(buffer, "%02d", m_tm.tm_mday); 
  m_YyyyMmDd += buffer;
}

void itxTime::Next(unsigned int hourinsecs)
{
  if (hourinsecs <= m_SecsFromMid)
    Tomorrow();
  m_SecsFromMid = hourinsecs;     
  TimeConversion();
}

void itxTime::Now()
{
  time(&m_t);
  SetFrom(m_t);
}

void itxTime::Format(char* outputfmt)
{
  m_outputfmt = outputfmt;
  m_output.SetEmpty();

  if (m_outputfmt.Compare("yyyymmdd") == 0)
    m_output = m_YyyyMmDd;

  if (m_outputfmt.Compare("dd/mm/yyyy") == 0)
  {
    char buffer[12];

    sprintf(buffer, "%02d/", m_tm.tm_mday); 
    m_output += buffer;
    sprintf(buffer, "%02d/", 1 + m_tm.tm_mon); 
    m_output += buffer;
    sprintf(buffer, "%d", 1900 + m_tm.tm_year); 
    m_output += buffer;
  }

  if (m_outputfmt.Compare("mm/dd/yyyy") == 0)
  {
    char buffer[12];

    sprintf(buffer, "%02d/", 1 + m_tm.tm_mon); 
    m_output += buffer;
    sprintf(buffer, "%02d/", m_tm.tm_mday); 
    m_output += buffer;
    sprintf(buffer, "%d", 1900 + m_tm.tm_year); 
    m_output += buffer;
  }

  if (m_outputfmt.Compare("dd/mm/yy") == 0)
  {
    char buffer[12];

    sprintf(buffer, "%02d/", m_tm.tm_mday); 
    m_output += buffer;
    sprintf(buffer, "%02d/", 1 + m_tm.tm_mon); 
    m_output += buffer;
    sprintf(buffer, "%02d", (m_tm.tm_year < 100 ? m_tm.tm_year : m_tm.tm_year - 100)); 
    m_output += buffer;
  }

  if (m_outputfmt.Compare("Dd/mm/yy") == 0)
  {
    char buffer[12];
  
    switch (m_tm.tm_wday)
    {
      case 0: sprintf(buffer, "%s", "Dom"); break;
      case 1: sprintf(buffer, "%s", "Lun"); break;
      case 2: sprintf(buffer, "%s", "Mar"); break;
      case 3: sprintf(buffer, "%s", "Mer"); break;
      case 4: sprintf(buffer, "%s", "Gio"); break;
      case 5: sprintf(buffer, "%s", "Ven"); break;
      case 6: sprintf(buffer, "%s", "Sab"); break;
    }
    m_output += buffer;
    sprintf(buffer, " %02d/", m_tm.tm_mday); 
    m_output += buffer;
    sprintf(buffer, "%02d/", 1 + m_tm.tm_mon); 
    m_output += buffer;
    sprintf(buffer, "%02d", (m_tm.tm_year < 100 ? m_tm.tm_year : m_tm.tm_year - 100)); 
    m_output += buffer;
  }

  if (m_outputfmt.Compare("yymmdd:hhmm") == 0)
  {
    char buffer[12];

    sprintf(buffer, "%02d", (m_tm.tm_year < 100 ? m_tm.tm_year : m_tm.tm_year - 100)); 
    m_output += buffer;
    sprintf(buffer, "%02d", 1 + m_tm.tm_mon); 
    m_output += buffer;
    sprintf(buffer, "%02d", m_tm.tm_mday); 
    m_output += buffer;
    sprintf(buffer, ":%02d", m_tm.tm_hour); 
    m_output += buffer;
    sprintf(buffer, "%02d", m_tm.tm_min); 
    m_output += buffer;
  }
}

