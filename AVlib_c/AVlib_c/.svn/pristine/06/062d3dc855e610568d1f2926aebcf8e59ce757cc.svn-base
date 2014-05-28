#pragma once
#ifndef _xRingH_
#define _xRingH_

#include "xCommon.h"

//thread safe FIFO with ring buffer
class xRing
{
protected:
  void** m_Ring;   
  int32  m_RingSize; //ring size
  int32  m_DataCnt;  //number of units in ring
  int32  m_WriteId;  //writting address 
  int32  m_ReadId;   //reading address
  int32  m_TailId;   //tail address   
  
  SEMAPHORE_HANDLE m_ReadSemaphore;    //read semaphore
  SEMAPHORE_HANDLE m_WriteSemaphore;   //write semaphore
  CRITICAL_SECTION m_CriticalSection;  //critical section

public:
  void   create    (int32 RingSize);
  void   destroy   ();
  void   put       (void *NodeData);
  void*  get       ();
  void*  peekLast  ();
  bool   isEmpty   ();
  bool   isFull    ();
  int32  getSize   ();
};  

//thread safe priority queue with ring buffer
class xPriorityRing : public xRing
{
protected:
  int32* m_Priority;

public:
  void   create    (int32 RingSize);
  void   destroy   ();
  void   put       (void* NodeData, int32 NodePriority);
};

#endif //_xRingH_