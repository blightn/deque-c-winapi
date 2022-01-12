#include <Windows.h>
#include <wchar.h>

#include "../Deque/deque.h"

#define DEQUE_SIZE 16

typedef struct
{
	DEQUE_ENTRY Entry;
	DWORD		dwNumber;
} SIMPLE_ENTRY, *PSIMPLE_ENTRY;

INT wmain(INT Argc, WCHAR* pArgv[], WCHAR* pEnv[])
{
	PDEQUE_ENTRY  pDeque = NULL;
	DWORD		  i;
	PSIMPLE_ENTRY pEntry = NULL;

	if (pDeque = DequeInit(FALSE))
	{
		wprintf(L"Deque size before push: %zu\n", DequeSize(pDeque)); // 0

		for (i = 0; i < DEQUE_SIZE; ++i)
		{
			if (pEntry = (PSIMPLE_ENTRY)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(SIMPLE_ENTRY)))
			{
				pEntry->dwNumber = i;
				DequePushBack(pDeque, &pEntry->Entry);
			}
		}

		wprintf(L"Deque size after push: %zu\n", DequeSize(pDeque)); // DEQUE_SIZE
		wprintf(L"Numbers from deque:");

		while (pEntry = (PSIMPLE_ENTRY)DequePopFront(pDeque))
		{
			wprintf(L" %u", pEntry->dwNumber);

			HeapFree(GetProcessHeap(), 0, (PVOID)pEntry);
			pEntry = NULL;
		}

		wprintf(L"\nDeque size after pop: %zu\n", DequeSize(pDeque)); // 0
		wprintf(L"Deque is %s\n", DequeIsEmpty(pDeque) ? L"empty" : L"not empty"); // "empty"

		DequeCleanup(pDeque);
		pDeque = NULL;
	}

	wprintf(L"\n");
	system("pause");

	return 0;
}
