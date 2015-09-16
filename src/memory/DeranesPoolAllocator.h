#pragma once

#include <cstdint>
#include "MemoryLibraryDefine.h"

typedef uint16_t muint;

class DeranesPoolAllocator {
public:
	MEMORY_API explicit			DeranesPoolAllocator		( size_t blockSize, size_t nrOfBlocks );
	MEMORY_API					~DeranesPoolAllocator		();
	MEMORY_API void*			allocate					();
	MEMORY_API void				deallocate					( void* memory );

private:
	size_t						m_BlockSizeInBytes			= 0;
	size_t						m_BlockSizeInMuints			= 0;
	size_t						m_NrOfBlocks				= 0;
	size_t						m_NrOfFreeBlocks			= 0;
	muint*						m_MemoryChunk				= nullptr;
	muint						m_FreeBlockIndex			= 0;
};