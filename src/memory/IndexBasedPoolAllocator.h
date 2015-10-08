#pragma once

#include <cstdint>
#include <cstddef>
#include <functional>
#include "MemoryLibraryDefine.h"

class IndexBasedPoolAllocator {
public:
	MEMORY_API explicit			IndexBasedPoolAllocator		( size_t blockSize, size_t nrOfBlocks, size_t alignment = sizeof(void*) );
	MEMORY_API					~IndexBasedPoolAllocator		();
	MEMORY_API void*			allocate					();
	MEMORY_API void				deallocate					( void* memory );

private:
	size_t						m_BlockSizeInBytes			= 0;
	size_t						m_NrOfBlocks				= 0;
	size_t						m_NrOfFreeBlocks			= 0;
	size_t						m_FreeBlockIndex			= 0;
	uint8_t*					m_MemoryChunk				= nullptr;

	std::function<void( void*, size_t )>	m_WriteUintFunction;
	std::function<size_t( void* )>			m_ReadUintFunction;
};
