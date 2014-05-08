#include "xRing.h"

void xRing::create(int32 RingSize)
{
  xAssert(RingSize>0);

  m_Ring = (void**)calloc(1, RingSize*sizeof(void*));
  m_RingSize = RingSize;
  m_DataCnt = 0;
  m_WriteId = 0;
  m_ReadId  = 0;
  m_TailId  = 0;
  m_ReadSemaphore  = CreateSemaphore(NULL, 0,        RingSize, NULL);
  m_WriteSemaphore = CreateSemaphore(NULL, RingSize, RingSize, NULL);
  InitializeCriticalSection(&m_CriticalSection);
}

void xRing::destroy()
{
  free(m_Ring); 
  CloseHandle(m_ReadSemaphore);  m_ReadSemaphore = NULL;
  CloseHandle(m_WriteSemaphore); m_WriteSemaphore = NULL;
  DeleteCriticalSection(&m_CriticalSection);
}

void xRing::put(void* NodeData)
{
  DWORD Hr = WaitForSingleObject(m_WriteSemaphore, INFINITE);
  EnterCriticalSection(&m_CriticalSection);
  m_Ring[m_WriteId] = NodeData;
  m_TailId = m_WriteId;
  m_WriteId = (m_WriteId+1)%m_RingSize;
  m_DataCnt++;
  LeaveCriticalSection(&m_CriticalSection);
  ReleaseSemaphore(m_ReadSemaphore, 1, NULL);
}

void* xRing::get()
{
  void *NodeData;
  DWORD hr = WaitForSingleObject(m_ReadSemaphore, INFINITE);
  EnterCriticalSection(&m_CriticalSection);
  NodeData = m_Ring[m_ReadId];
  m_ReadId = (m_ReadId+1)%m_RingSize;
  m_DataCnt--;
  LeaveCriticalSection(&m_CriticalSection);
  ReleaseSemaphore(m_WriteSemaphore, 1, NULL);
  return NodeData;
}

void* xRing::peekLast()
{
  void *NodeData;
  EnterCriticalSection(&m_CriticalSection);
  xAssert(m_DataCnt > 0);
  NodeData = m_Ring[m_TailId];
  LeaveCriticalSection(&m_CriticalSection);
  return NodeData;
}

bool xRing::isEmpty()
{
  bool isEmpty;
  EnterCriticalSection(&m_CriticalSection);
  isEmpty = m_DataCnt==0;
  LeaveCriticalSection(&m_CriticalSection);
  return isEmpty;
}

bool xRing::isFull()
{
  bool isFull;
  EnterCriticalSection(&m_CriticalSection);
  isFull = m_DataCnt==m_RingSize;
  LeaveCriticalSection(&m_CriticalSection);
  return isFull;
}

int32 xRing::getSize()
{
  int32 Size;
  EnterCriticalSection(&m_CriticalSection);
  Size = m_RingSize;
  LeaveCriticalSection(&m_CriticalSection);
  return Size;
}

//===================================================================================

void xPriorityRing::create(int32 RingSize)
{
  xRing::create(RingSize);
  m_Priority = (int32*)calloc(RingSize, sizeof(int32*));
}

void xPriorityRing::destroy()
{
  xRing::destroy();
  free(m_Priority);
}

void xPriorityRing::put(void* NodeData, int32 NodePriority)
{
  DWORD Hr = WaitForSingleObject(m_WriteSemaphore, INFINITE);
  EnterCriticalSection(&m_CriticalSection);

  int32 Id=m_WriteId;
  while(1)
  {
    int32 PrevId = Id==0 ? m_RingSize-1 : Id-1;

    if(m_Priority[PrevId] <= NodePriority || Id==m_WriteId)
    {
      m_Ring[Id] = NodeData;
      m_Priority[Id] = NodePriority;
      if(Id==m_WriteId) m_TailId = m_WriteId;
      m_WriteId = (m_WriteId+1)%m_RingSize;
      m_DataCnt++;
      break;
    }
    else
    {
      m_Ring[Id] = m_Ring[PrevId];
      m_Priority[Id] = m_Priority[PrevId];
      Id = PrevId;
    }  
  }
 
  LeaveCriticalSection(&m_CriticalSection);
  ReleaseSemaphore(m_ReadSemaphore, 1, NULL);
}

