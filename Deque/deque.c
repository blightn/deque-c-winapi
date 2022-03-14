#include "deque.h"

// 1. "pHead->Next == NULL && pHead->Prev == NULL" => N = 0 elements.
// 2. "pHead->Next == pHead->Prev" => N = 1 element.
// 3. "pHead->Next != pHead->Prev" => N > 1 elements.

PDEQUE_ENTRY DequeInit(BOOL ThreadSafe)
{
	PDEQUE_ENTRY pHead = NULL;

	if (pHead = (PDEQUE_ENTRY)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(DEQUE_ENTRY)))
	{
		if (ThreadSafe)
		{
			if (pHead->pCS = (PCRITICAL_SECTION)HeapAlloc(GetProcessHeap(), 0, sizeof(CRITICAL_SECTION)))
			{
				InitializeCriticalSection(pHead->pCS);
			}
			else
			{
				HeapFree(GetProcessHeap(), 0, (PVOID)pHead);
				pHead = NULL;
			}
		}
	}

	return pHead;
}

VOID DequeCleanup(PDEQUE_ENTRY pHead)
{
	if (pHead)
	{
		pHead->Next = pHead->Prev = NULL;

		if (pHead->pCS)
		{
			DeleteCriticalSection(pHead->pCS);
			
			HeapFree(GetProcessHeap(), 0, (PVOID)pHead->pCS);
			pHead->pCS = NULL;
		}

		HeapFree(GetProcessHeap(), 0, (PVOID)pHead);
		pHead = NULL;
	}
}

VOID DequePushFront(PDEQUE_ENTRY pHead, PDEQUE_ENTRY pEntry)
{
	if (pHead && pEntry)
	{
		if (pHead->pCS)
		{
			EnterCriticalSection(pHead->pCS);
		}

		*pEntry      = *pHead;
		pEntry->Prev = pHead;
		pHead->Next  = pEntry;

		if (pEntry->Next)
		{
			pEntry->Next->Prev = pEntry;
		}
		else
			pHead->Prev = pEntry;

		if (pHead->pCS)
		{
			LeaveCriticalSection(pHead->pCS);
		}
	}
}

VOID DequePushBack(PDEQUE_ENTRY pHead, PDEQUE_ENTRY pEntry)
{
	if (pHead && pEntry)
	{
		if (pHead->pCS)
		{
			EnterCriticalSection(pHead->pCS);
		}

		*pEntry      = *pHead;
		pHead->Prev  = pEntry;
		pEntry->Next = NULL;

		if (!pHead->Next)
		{
			pEntry->Prev = pHead;
			pHead->Next  = pEntry;
		}
		else
			pEntry->Prev->Next = pEntry;

		if (pHead->pCS)
		{
			LeaveCriticalSection(pHead->pCS);
		}
	}
}

PDEQUE_ENTRY DequePopFront(PDEQUE_ENTRY pHead)
{
	PDEQUE_ENTRY pEntry = NULL;

	if (pHead)
	{
		if (pHead->pCS)
		{
			EnterCriticalSection(pHead->pCS);
		}

		if (pHead->Next)
		{
			pEntry      = pHead->Next;
			pHead->Next = pEntry->Next;

			if (pHead->Next)
			{
				pHead->Next->Prev = pHead;
			}
			else
				pHead->Prev = NULL;
		}

		if (pHead->pCS)
		{
			LeaveCriticalSection(pHead->pCS);
		}
	}

	return pEntry;
}

PDEQUE_ENTRY DequePopBack(PDEQUE_ENTRY pHead)
{
	PDEQUE_ENTRY pEntry = NULL;

	if (pHead)
	{
		if (pHead->pCS)
		{
			EnterCriticalSection(pHead->pCS);
		}

		if (pHead->Prev)
		{
			pEntry = pHead->Prev;

			if (pEntry->Prev == pHead)
			{
				pHead->Next = pHead->Prev = NULL;
			}
			else
			{
				pHead->Prev       = pEntry->Prev;
				pHead->Prev->Next = NULL;
			}
		}

		if (pHead->pCS)
		{
			LeaveCriticalSection(pHead->pCS);
		}
	}

	return pEntry;
}

BOOL DequeIsEmpty(PDEQUE_ENTRY pHead)
{
	BOOL Empty;

	if (pHead->pCS)
	{
		EnterCriticalSection(pHead->pCS);
	}

	Empty = pHead->Next == NULL;

	if (pHead->pCS)
	{
		LeaveCriticalSection(pHead->pCS);
	}

	return Empty;
}

SIZE_T DequeSize(PDEQUE_ENTRY pHead)
{
	SIZE_T Count = 0;

	if (pHead)
	{
		if (pHead->pCS)
		{
			EnterCriticalSection(pHead->pCS);
		}

		for (; pHead->Next; ++Count, pHead = pHead->Next);

		if (pHead->pCS)
		{
			LeaveCriticalSection(pHead->pCS);
		}
	}

	return Count;
}
