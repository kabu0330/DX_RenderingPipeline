#include "PreCompile.h"
#include "MemoryTrace.h"

// 매크로 무효화
#undef new

static size_t g_AllocCount = 0;

void* operator new(size_t Size, const char* File, int Line)
{
	void* Ptr = malloc(Size);
	++g_AllocCount;

	printf("[NEW #%zu] %p (%zu bytes) at %s:%d\n", g_AllocCount, Ptr, Size, File, Line);
	return Ptr;
}

void operator delete(void* Ptr) noexcept
{
	printf("[DELETE] %p\n", Ptr);
	free(Ptr);
}
