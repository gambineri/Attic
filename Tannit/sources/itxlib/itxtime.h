/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxtime.h,v $
 * $Revision: 1.3 $  
 * $Author: administrator $
 * $Date: 2002-07-23 16:05:30+02 $
 *
 * Definition of the itxTime object (time_t format object)
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */

#ifndef __ITXTIME_H__
#define __ITXTIME_H__

#include <time.h>

#include "itxstring.h"
#include "itxsystem.h"


class itxTime
{
public:
  itxString    m_YyyyMmDd;
  unsigned int m_SecsFromMid;
  itxString    m_outputfmt;
  itxString    m_output;
  time_t       m_t;
  struct tm    m_tm;

  itxTime() { m_YyyyMmDd = ""; m_SecsFromMid = 0; m_outputfmt = ""; };
  itxTime(char* yyyymmdd, unsigned int secs, char* outputfmt = "yyymmdd") 
  { 
    m_YyyyMmDd = yyyymmdd; 
    m_SecsFromMid = secs; 
    TimeConversion(); 
    Format(outputfmt);
  };
  itxTime(int d, int m, int y, unsigned int secs, char* outputfmt = "yyymmdd") 
  { 
    SetFrom(d, m, y); 
    m_SecsFromMid = secs; 
    TimeConversion(); 
    Format(outputfmt);
  };
  itxTime(time_t t, char* outputfmt = "yyymmdd") { SetFrom(t); TimeConversion(); Format(outputfmt); };

  ~itxTime() {};

  void   SetFrom(time_t t);
  void   SetFrom(int d, int m, int y);
  void   Tomorrow();
  void   Next(unsigned int hourinsecs);
  void   Now();
  int    SecFromMid(itxString* hour);
  void   Format(char* outputfmt);

private:
  time_t TimeConversion();
};


#endif //__ITXTIME_H__
