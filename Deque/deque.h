#ifndef _DEQUE_H_
#define _DEQUE_H_

#include <Windows.h>

typedef struct _DEQUE_ENTRY
{
	struct _DEQUE_ENTRY* Next;
	struct _DEQUE_ENTRY* Prev;
	PCRITICAL_SECTION	 pCS;
} DEQUE_ENTRY, *PDEQUE_ENTRY;

PDEQUE_ENTRY DequeInit(BOOL ThreadSafe);
VOID DequeCleanup(PDEQUE_ENTRY pHead);

VOID DequePushFront(PDEQUE_ENTRY pHead, PDEQUE_ENTRY pEntry);
VOID DequePushBack(PDEQUE_ENTRY pHead, PDEQUE_ENTRY pEntry);

PDEQUE_ENTRY DequePopFront(PDEQUE_ENTRY pHead);
PDEQUE_ENTRY DequePopBack(PDEQUE_ENTRY pHead);

BOOL DequeIsEmpty(PDEQUE_ENTRY pHead);
SIZE_T DequeSize(PDEQUE_ENTRY pHead);

#endif // _DEQUE_H_
