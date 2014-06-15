/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxthread.h,v $
 * $Revision: 1.6 $  
 * $Author: administrator $
 * $Date: 2002-07-23 16:05:30+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */

#ifndef _ITX_THREAD_H_
#define _ITX_THREAD_H_

#include "itxsystem.h"


class _Thread
{
  itxSystem       m_Sys;

protected:
  unsigned int    m_TID;
  void*           m_THandle;

  unsigned long   m_StackSize;
  unsigned short  m_Started    :1;
  unsigned short  m_Suspended  :1;
  unsigned short               :(sizeof(unsigned short) - 2);

  void SetThreadID(unsigned int t){m_TID = t;}

public:
  _Thread(unsigned long StackS = 4096)
  {
    m_StackSize = StackS;
    m_Started   = 0;
    m_Suspended = 0;
  }

  unsigned int GetThreadID(){return m_TID;}
  
  void ChangeStackSize(unsigned long S);
  void Start(void* thisThread);
  void Suspend();
  void Kill();
  void Resume();

  virtual void Run() = 0;  //The overridable function

  friend void ThreadStarter(void* a)
  {
    ((_Thread*)a)->Run();
  }
};

class itxThread : public _Thread
{
public:
  itxThread(){}
  ~itxThread(){}

  inline void Start(){ _Thread::Start(this); }
};


#endif  /* _ITX_THREAD_H_ */



