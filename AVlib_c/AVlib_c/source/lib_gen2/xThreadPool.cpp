#include "xThreadPool.h"

void xThreadPool::create(int32 NumThreads, int32 RingSize, void* (*WorkingFunction)(void*))
{
  xAssert(NumThreads>0);

  m_ThreadHandle = (THREAD_HANDLE*)calloc(1, NumThreads*sizeof(THREAD_HANDLE));
  m_ThreadId = (uint32*)calloc(1, NumThreads*sizeof(uint32));
  m_NumThreads = NumThreads;
  m_WaitingTasks.create(RingSize);
  m_CompletedTasks.create(RingSize);
  m_WorkingFunction = WorkingFunction;
  InitializeCriticalSection(&m_CriticalSection);

  for(int32 i=0; i<m_NumThreads; i++)
  {
    m_ThreadHandle[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)xTthreadFunc, this, 0, (LPDWORD)&m_ThreadId);
  } 

}

void xThreadPool::destroy()
{
  uint32 ExitCode;
  bool AnyActive = false;

  for(int32 i=0; i<m_NumThreads; i++)
  {
    m_CompletedTasks.put((void*)nullptr);
  }

  Sleep(10);
  
  for(int32 i=0; i<m_NumThreads; i++)
  {   
    GetExitCodeThread(m_ThreadHandle[i], (LPDWORD)&ExitCode);
    if(ExitCode == STILL_ACTIVE){ AnyActive = true; }
  }

  if(AnyActive)
  {
    Sleep(500);
    for(int32 i=0; i<m_NumThreads; i++)
    {    
    GetExitCodeThread(m_ThreadHandle[i], (LPDWORD)&ExitCode);
      if(ExitCode == STILL_ACTIVE) { TerminateThread(m_ThreadHandle[i], EXIT_SUCCESS); }
    }
  }

  free(m_ThreadHandle);
  free(m_ThreadId);
  m_WaitingTasks.destroy();
  m_CompletedTasks.destroy();
  DeleteCriticalSection(&m_CriticalSection);
}

uint32 xThreadPool::xTthreadFunc(xThreadPool* ThreadPool) 
{
  while(1)
  {    
    void* Task = ThreadPool->m_WaitingTasks.get();
    if(Task==NULL) break;

    EnterCriticalSection(&ThreadPool->m_CriticalSection);
    void* (*Worker) (void*) = ThreadPool->m_WorkingFunction;
    LeaveCriticalSection(&ThreadPool->m_CriticalSection);

    void* Result = Worker(Task);
    ThreadPool->m_CompletedTasks.put(Result);
  }
  return EXIT_SUCCESS;
}

void xThreadPool::setWorkingFunction(void* (*WorkingFunction)(void*))
{
  EnterCriticalSection(&m_CriticalSection);
  m_WorkingFunction = WorkingFunction;
  LeaveCriticalSection(&m_CriticalSection);
}

void xThreadPool::addWaitingTask(void* Task)
{
  xAssert(Task!=nullptr); //x_threadpool_add_waiting_task: NULL task is prohibited
  m_WaitingTasks.put(Task);
}

void* xThreadPool::receiveCompletedTask()
{
  return m_CompletedTasks.get();
}