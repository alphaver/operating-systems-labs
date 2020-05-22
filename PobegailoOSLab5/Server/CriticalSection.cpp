#include "CriticalSection.h"

CriticalSection::CriticalSection()
{
    InitializeCriticalSection(&section);
}

CriticalSection::~CriticalSection()
{
    DeleteCriticalSection(&section);
}

void CriticalSection::Enter()
{
    EnterCriticalSection(&section);
}

void CriticalSection::Leave()
{
    LeaveCriticalSection(&section);
}
