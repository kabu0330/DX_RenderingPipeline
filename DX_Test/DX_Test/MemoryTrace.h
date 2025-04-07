#pragma once

#pragma once
#include <cstddef>

void* operator new(size_t Size, const char* File, int Line);
void operator delete(void* Ptr) noexcept;

#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW

