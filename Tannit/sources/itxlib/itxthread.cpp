/*
 * Copyright (c) AITECSA S.r.l.
 * See the file LICENSE for copying permission.
 * */

/* $RCSfile: itxthread.cpp,v $
 * $Revision: 1.5 $  
 * $Author: administrator $
 * $Date: 2002-07-23 16:05:30+02 $
 *
 * aitecsa
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 *               � aitecsa s.r.l. via baglivi 3 00161 roma italy 
 *               aitecsa@aitecsa.it
 */


#include "itxthread.h"


//--------------------------------------------------------------
void _Thread::ChangeStackSize(unsigned long S)
{
  if (!m_Started) 
    m_StackSize = S;
}

//--------------------------------------------------------------
void _Thread::Start(void* thisThread)
{
  if ((m_THandle = m_Sys.THCreateThread((void*)ThreadStarter, (void*)thisThread, (unsigned int*)&m_TID)) == NULL)
    return;

  m_Started = 1;
}

//--------------------------------------------------------------
void _Thread::Suspend()
{
  if (m_Started)
  {
    m_Sys.THPauseThread(m_THandle);
    m_Suspended = 1;
  }
}

//--------------------------------------------------------------
void _Thread::Kill()
{
  m_Sys.THTerminateThread(m_THandle);
}

//--------------------------------------------------------------
void _Thread::Resume()
{
  if (m_Suspended)
  {
    m_Sys.THResumeThread(m_THandle);
    m_Suspended = 0;
  }
}






