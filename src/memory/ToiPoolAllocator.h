#pragma once

#include "MemoryLibraryDefine.h"
#include <cstddef>
#include <stdint.h>
#include <cassert>
#include <iostream>

#define TOI_POOL_ALLOCATOR_SET_ALLOCATED_MEMORY
#define TOI_POOL_ALLOCATOR_SET_ALLOCATED_MEMORY_VALUE 0

#define TOI_POOL_ALLOCATOR_SET_FREED_MEMORY
#define TOI_POOL_ALLOCATOR_SET_FREED_MEMORY_VALUE 0

class ToiPoolAllocator {
public:
	MEMORY_API explicit ToiPoolAllocator( size_t blockSize, size_t nrOfBlocks );
	MEMORY_API ~ToiPoolAllocator();

	MEMORY_API void* Allocate( );
	MEMORY_API void Deallocate( void* memory );

	template<typename Type>
	void PrintMemory() {
		assert( sizeof(Type ) <= m_BlockSize );

		for ( size_t i = 0; i < m_NrOfBlocks * m_BlockSize; i+= m_BlockSize ) {
			Type* block = reinterpret_cast<Type*>( m_Memory + i );
			std::cout << *block << std::endl;
		}
	}

private:
	uint8_t* m_Memory = nullptr;
	size_t m_BlockSize = 0;
	size_t* m_FirstFree = nullptr;
	size_t m_NrOfBlocks = 0;
};
