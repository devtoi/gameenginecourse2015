#pragma once

#include "IndexBasedPoolAllocator.h"

#define DERANES_POOL_ALLOCATOR_LOCK_TYPE_MUTEX		0
#define DERANES_POOL_ALLOCATOR_LOCK_TYPE_SPINLOCK	1
#define DERANES_POOL_ALLOCATOR_LOCK_TYPE			DERANES_POOL_ALLOCATOR_LOCK_TYPE_MUTEX

#if DERANES_POOL_ALLOCATOR_LOCK_TYPE == DERANES_POOL_ALLOCATOR_LOCK_TYPE_MUTEX
	#include <mutex>
	#define DERANES_POOL_ALLOCATOR_LOCK_MEMBER		std::mutex m_Mutex;
#else
	#include <atomic>
	#define DERANES_POOL_ALLOCATOR_LOCK_MEMBER		std::atomic_flag m_Lock = ATOMIC_FLAG_INIT;
#endif

class DeranesPoolAllocatorWithLock {
public:
	MEMORY_API explicit			DeranesPoolAllocatorWithLock	( size_t blockSize, size_t nrOfBlocks, size_t alignment = sizeof(void*) );
	MEMORY_API					~DeranesPoolAllocatorWithLock	();
	MEMORY_API void*			allocate						();
	MEMORY_API void				deallocate						( void* memory );

private:
	IndexBasedPoolAllocator		m_Allocator;
	DERANES_POOL_ALLOCATOR_LOCK_MEMBER
};