#ifdef NDEBUG
#	error Tests can be run only in debug mode
#endif

#include <Windows.h>
#include <wchar.h>
#include <assert.h>

#include "..\Deque\deque.h"

#define SECTOMS(s)	 ((s) * 1000)

#define DEQUE_SIZE	 128

#define THREAD_COUNT 64 // <= MAXIMUM_WAIT_OBJECTS (64).
#define WAIT_TIME	 SECTOMS(15)

typedef struct
{
	DEQUE_ENTRY Entry;
	SIZE_T		Number;
} TEST_ENTRY, *PTEST_ENTRY;

static PDEQUE_ENTRY g_pDeque	  = NULL;
static HANDLE		g_hStartEvent = NULL;

static volatile LONG64 g_llElementsInDeque;

static VOID CheckDequeChain(PDEQUE_ENTRY pHead)
{
	PDEQUE_ENTRY pTmp = NULL;
	INT			 i	  = 0;

	EnterCriticalSection(pHead->pCS);

	// We move forward and backward through the chain to make sure it is consistent.
	for (pTmp = pHead; pTmp->Next; ++i, pTmp = pTmp->Next);
	for (; pTmp != pHead; --i, pTmp = pTmp->Prev);

	LeaveCriticalSection(pHead->pCS);

	assert(i == 0);
}

static DWORD WINAPI TestThread(PVOID pvParam)
{
	PTEST_ENTRY pEntry = NULL;
	DWORD		dwRes,
				i;

	dwRes = WaitForSingleObject(g_hStartEvent, INFINITE);
	assert(dwRes == WAIT_OBJECT_0);

	for (i = 0; i < DEQUE_SIZE; ++i)
	{
		pEntry = (PTEST_ENTRY)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(TEST_ENTRY));
		assert(pEntry != NULL);
		pEntry->Number = (SIZE_T)pvParam;

		if (pEntry->Number % 2)
		{
			DequePushFront(g_pDeque, &pEntry->Entry);
		}
		else
			DequePushBack(g_pDeque, &pEntry->Entry);

		InterlockedIncrement64(&g_llElementsInDeque);
	}

	return 0;
}

INT wmain(INT Argc, WCHAR* pArgv[], WCHAR* pEnv[])
{
	PTEST_ENTRY pEntry = NULL;
	SIZE_T		i,
				Sum1,
				Sum2;
	HANDLE		hThreads[THREAD_COUNT];
	DWORD		dwRes;

	// 1 - DequeInit()
	g_pDeque = DequeInit(TRUE);

	assert(g_pDeque != NULL);
	CheckDequeChain(g_pDeque);
	assert(g_pDeque->Next == NULL);
	assert(g_pDeque->Prev == NULL);
	assert(DequeSize(g_pDeque) == 0);

	// 2 - DequePushBack()
	pEntry = (PTEST_ENTRY)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(TEST_ENTRY));
	assert(pEntry != NULL);
	pEntry->Number = 1;

	DequePushBack(g_pDeque, &pEntry->Entry);

	CheckDequeChain(g_pDeque);
	assert(g_pDeque->Next == &pEntry->Entry);
	assert(g_pDeque->Prev == &pEntry->Entry);
	assert(pEntry->Entry.Next == NULL);
	assert(pEntry->Entry.Prev == g_pDeque);
	assert(DequeSize(g_pDeque) == 1);

	// 3 - DequePopFront()
	pEntry = (PTEST_ENTRY)DequePopFront(g_pDeque);

	CheckDequeChain(g_pDeque);
	assert(pEntry != NULL);
	assert(pEntry->Entry.Next == NULL);
	assert(pEntry->Entry.Prev == g_pDeque);
	assert(pEntry->Number == 1);
	assert(DequeSize(g_pDeque) == 0);

	HeapFree(GetProcessHeap(), 0, (PVOID)pEntry);
	pEntry = NULL;

	// 4 - DequePushFront() + cycle
	for (i = 1; i <= DEQUE_SIZE; ++i)
	{
		pEntry = (PTEST_ENTRY)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(TEST_ENTRY));
		assert(pEntry != NULL);
		pEntry->Number = i;

		DequePushFront(g_pDeque, &pEntry->Entry);

		CheckDequeChain(g_pDeque);
		assert(g_pDeque->Next == &pEntry->Entry);
		assert(pEntry->Entry.Prev == g_pDeque);
		assert(DequeSize(g_pDeque) == i);

		if (i == 1)
		{
			assert(g_pDeque->Prev == &pEntry->Entry);
			assert(pEntry->Entry.Next == NULL);
		}
		else
		{
			assert(pEntry->Entry.Next != NULL);
			assert(pEntry->Entry.Next != g_pDeque);
		}
	}

	// 5 - DequePopBack() + cycle
	for (i = 1; pEntry = (PTEST_ENTRY)DequePopBack(g_pDeque); ++i)
	{
		CheckDequeChain(g_pDeque);
		assert(pEntry->Entry.Next == NULL);

		if (i < DEQUE_SIZE)
		{
			assert(g_pDeque->Prev == pEntry->Entry.Prev);
			assert(pEntry->Entry.Prev->Next == NULL);
			assert(pEntry->Entry.Prev->Prev != NULL);

			if (i < DEQUE_SIZE - 1)
			{
				assert(pEntry->Entry.Prev->Prev != g_pDeque);
			}
			else // i == DEQUE_SIZE - 1
				assert(pEntry->Entry.Prev->Prev == g_pDeque);
		}
		else
			assert(pEntry->Entry.Prev == g_pDeque);

		assert(pEntry->Number == i);
		assert(DequeSize(g_pDeque) == DEQUE_SIZE - i);

		HeapFree(GetProcessHeap(), 0, (PVOID)pEntry);
		pEntry = NULL;
	}

	CheckDequeChain(g_pDeque);
	assert(i - 1 == DEQUE_SIZE);
	assert(DequeSize(g_pDeque) == 0);

	// 6 - DequePushBack() + cycle
	for (i = 1; i <= DEQUE_SIZE; ++i)
	{
		pEntry = (PTEST_ENTRY)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(TEST_ENTRY));
		assert(pEntry != NULL);
		pEntry->Number = i;

		DequePushBack(g_pDeque, &pEntry->Entry);

		CheckDequeChain(g_pDeque);
		assert(g_pDeque->Prev == &pEntry->Entry);
		assert(pEntry->Entry.Next == NULL);
		assert(DequeSize(g_pDeque) == i);

		if (i == 1)
		{
			assert(g_pDeque->Next == &pEntry->Entry);
			assert(pEntry->Entry.Prev == g_pDeque);
		}
		else
		{
			assert(g_pDeque->Next != &pEntry->Entry);
			assert(pEntry->Entry.Prev != g_pDeque);
		}
	}

	// 7 - DequePopFront() + cycle
	for (i = 1; pEntry = (PTEST_ENTRY)DequePopFront(g_pDeque); ++i)
	{
		CheckDequeChain(g_pDeque);
		assert(pEntry->Number == i);
		assert(DequeSize(g_pDeque) == DEQUE_SIZE - i);

		if (i < DEQUE_SIZE)
		{
			assert(g_pDeque->Next == pEntry->Entry.Next);
			assert(pEntry->Entry.Prev == g_pDeque);

			if (i < DEQUE_SIZE - 1)
			{
				assert(pEntry->Entry.Next->Next != g_pDeque);
				assert(pEntry->Entry.Next->Prev == g_pDeque);
			}
			else // i == DEQUE_SIZE - 1
			{
				assert(pEntry->Entry.Next->Next == NULL);
				assert(pEntry->Entry.Next->Prev == g_pDeque);
			}
		}
		else
		{
			assert(pEntry->Entry.Next == NULL);
			assert(pEntry->Entry.Prev == g_pDeque);
		}

		HeapFree(GetProcessHeap(), 0, (PVOID)pEntry);
		pEntry = NULL;
	}

	CheckDequeChain(g_pDeque);
	assert(i - 1 == DEQUE_SIZE);
	assert(DequeSize(g_pDeque) == 0);

	// 8 - Thread-safety
	g_hStartEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
	assert(g_hStartEvent != NULL);

	for (Sum1 = i = 0; i < ARRAYSIZE(hThreads); Sum1 += (i + 1) * DEQUE_SIZE, ++i)
	{
		hThreads[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TestThread, (PVOID)(i + 1), 0, NULL);
		assert(hThreads[i] != NULL);
	}

	SetEvent(g_hStartEvent);

	dwRes = WaitForMultipleObjects(ARRAYSIZE(hThreads), hThreads, TRUE, INFINITE);
	assert(dwRes == WAIT_OBJECT_0);

	for (i = 0; i < ARRAYSIZE(hThreads); ++i)
	{
		CloseHandle(hThreads[i]);
		hThreads[i] = NULL;
	}

	CheckDequeChain(g_pDeque);
	assert(InterlockedAdd64(&g_llElementsInDeque, 0) == THREAD_COUNT * DEQUE_SIZE);
	assert(DequeSize(g_pDeque)						 == THREAD_COUNT * DEQUE_SIZE);

	Sum2 = 0;

	while (pEntry = (PTEST_ENTRY)DequePopFront(g_pDeque))
	{
		CheckDequeChain(g_pDeque);

		Sum2 += pEntry->Number;

		HeapFree(GetProcessHeap(), 0, (PVOID)pEntry);
		pEntry = NULL;
	}

	assert(Sum1 == Sum2);
	assert(DequeSize(g_pDeque) == 0);

	CloseHandle(g_hStartEvent);
	g_hStartEvent = NULL;

	// 9 - DequeCleanup()
	DequeCleanup(g_pDeque);
	g_pDeque = NULL;

	wprintf(L"All tests passed\n\n");
	system("pause");

	return 0;
}
