#pragma once

#define TOI_POOL_ALLOCATOR_SET_ALLOCATED_MEMORY
#define TOI_POOL_ALLOCATOR_SET_ALLOCATED_MEMORY_VALUE 0

#define TOI_POOL_ALLOCATOR_SET_FREED_MEMORY
#define TOI_POOL_ALLOCATOR_SET_FREED_MEMORY_VALUE 0

#define TOI_POOL_ALLOCATOR_MUTEX_LOCK
#define TOI_POOL_ALLOCATOR_SPIN_LOCK

#include "MemoryLibraryDefine.h"
#include <cstddef>
#include <stdint.h>
#include <cassert>
#include <iostream>
#ifdef TOI_POOL_ALLOCATOR_MUTEX_LOCK
	#include <mutex>
#endif
#ifdef TOI_POOL_ALLOCATOR_SPIN_LOCK
	#include <atomic>
#endif

class ToiPoolAllocator {
public:
	MEMORY_API explicit ToiPoolAllocator( size_t blockSize, size_t nrOfBlocks );
	MEMORY_API ~ToiPoolAllocator();

	MEMORY_API void* allocate( );
	MEMORY_API void deallocate( void* memory );

	template<typename Type>
	void PrintMemory() {
#ifdef TOI_POOL_ALLOCATOR_SPIN_LOCK
	while (m_Lock.test_and_set(std::memory_order_acquire)){}
#endif

#ifdef TOI_POOL_ALLOCATOR_MUTEX_LOCK
	std::lock_guard<std::mutex> lock( m_Mutex );
#endif

		assert( sizeof(Type ) <= m_BlockSize );

		for ( size_t i = 0; i < m_NrOfBlocks * m_BlockSize; i+= m_BlockSize ) {
			Type* block = reinterpret_cast<Type*>( m_Memory + i );
			std::cout << std::hex << *block << std::endl;
		}
#ifdef TOI_POOL_ALLOCATOR_SPIN_LOCK
	m_Lock.clear(std::memory_order_release); 
#endif
	}

	template<typename Type, typename... Args>
	Type* construct( Args&&... args ) {
		assert( sizeof( Type ) <= m_BlockSize );

		Type* data = static_cast<Type*>( allocate() );
		new(data) Type(std::forward<Args>( args )...);
		return data;
	}

	template<typename Type>
	void destroy( Type* memory ) {
		memory->~Type();
		assert( sizeof( Type ) <= m_BlockSize );
		deallocate( memory );
	}

private:
	uint8_t* m_Memory = nullptr;
	size_t m_BlockSize = 0;
	size_t* m_FirstFree = nullptr;
	size_t m_NrOfBlocks = 0;

#ifdef TOI_POOL_ALLOCATOR_MUTEX_LOCK
	std::mutex m_Mutex;
#endif
#ifdef TOI_POOL_ALLOCATOR_SPIN_LOCK
	std::atomic_flag m_Lock = ATOMIC_FLAG_INIT;
#endif
};
