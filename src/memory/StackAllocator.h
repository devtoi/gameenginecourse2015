#pragma once
#include <cstddef>
#include <stdint.h>
#include <atomic>
#include "MemoryLibraryDefine.h"

#define STACK_ALLOCATOR_SIZE 64 * 1024 * 1024

#define STACK_ALLOCATOR_SET_MEMORY
#define STACK_ALLOCATOR_MEMORY_INITIALIZE_VALUE 254
#define STACK_ALLOCATOR_MEMORY_ALLOCATION_VALUE 253
#define STACK_ALLOCATOR_MEMORY_FREE_VALUE 252

#define STACK_ALLOCATOR_ASSERT_ON_OVERFLOW
#define STACK_ALLOCATOR_TRACK_NR_OF
//#define STACK_ALLOCATOR_COUT_CONSTRUCTION
//#define STACK_ALLOCATOR_COUT_DESTRUCTION

#define g_ThreadStack StackAllocator::GetThreadStack()

class StackAllocator {
public:
	MEMORY_API static StackAllocator& GetThreadStack();
	typedef size_t Marker;
	
	MEMORY_API explicit StackAllocator( size_t stackSize );
	MEMORY_API ~StackAllocator();
	MEMORY_API bool operator==( StackAllocator& ) = delete;
	MEMORY_API bool operator=( StackAllocator& ) = delete;

	MEMORY_API void* Allocate( size_t allocationSize, size_t alignment = 8 );
	MEMORY_API void Deallocate();

	MEMORY_API void Unwind( Marker marker );

	MEMORY_API Marker GetMarker() const;

	MEMORY_API uint8_t* GetBuffer();
	MEMORY_API size_t GetSize() const;

	MEMORY_API static int GetNrOfStacks();

private:
#ifdef STACK_ALLOCATOR_TRACK_NR_OF
	static std::atomic_int m_NrOfCreatedStacks;
#endif
	uint8_t* m_Memory = nullptr;
	Marker m_Marker = 0;
	size_t m_StackSize = 0;
};
