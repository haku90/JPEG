#pragma once
#ifndef _xThreadPoolH_
#define _xThreadPoolH_

#include "xLicense.h"
#include "xCommon.h"
#include "xRing.h"

class xThreadPool
{
protected:
  THREAD_HANDLE* m_ThreadHandle;
  uint32* m_ThreadId;
  int32   m_NumThreads;
  xRing   m_WaitingTasks;
  xRing   m_CompletedTasks;  
  void*   (*m_WorkingFunction) (void*);
  CRITICAL_SECTION m_CriticalSection;  //critical section

  static uint32 xTthreadFunc(xThreadPool* threadpool);

public:
  void   create               (int32 NumThreads, int32 RingSize, void* (*WorkingFunction)(void*));
  void   destroy              ();
  void   setWorkingFunction   (void* (*WorkingFunction)(void*));
  void   addWaitingTask       (void* Task);
  void*  receiveCompletedTask ();
};

#endif //_xThreadPoolH_