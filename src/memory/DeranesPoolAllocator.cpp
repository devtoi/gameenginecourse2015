#include "DeranesPoolAllocator.h"

#include <cassert>
#include <iostream>	// TODOOE: Remove after HelloWorld test is removed.

DeranesPoolAllocator::DeranesPoolAllocator( size_t blockSize, size_t nrOfBlocks ) {
	size_t granularity	= sizeof( muint );
	m_BlockSizeInMuints	= ( blockSize / granularity ) + ( blockSize % granularity != 0 );
	m_BlockSizeInBytes	= granularity * m_BlockSizeInMuints;										// TODOOE: Make sure memory is aligned and suits the platform.
	m_NrOfBlocks		= nrOfBlocks;																// TODOOE: Make sure block size is big enough to point to all blocks.

	m_MemoryChunk		= reinterpret_cast<muint*>( malloc( m_BlockSizeInBytes * m_NrOfBlocks ) );	// TODOOE: Make sure allocation succeds?
	m_NrOfFreeBlocks	= m_NrOfBlocks;
	m_FreeBlockIndex	= 0;

	for ( size_t i = 0; i < m_NrOfBlocks; ) {
		muint*	block	= m_MemoryChunk + m_BlockSizeInMuints * i;
		*block			= static_cast<muint>( ++i );
	}
}

DeranesPoolAllocator::~DeranesPoolAllocator() {
	if ( m_MemoryChunk ) {
		free( m_MemoryChunk );
		m_MemoryChunk = nullptr;
	}
}

void * DeranesPoolAllocator::allocate() {
	assert( m_NrOfFreeBlocks > 0 );

	if ( m_NrOfFreeBlocks <= 0 ) {
		return nullptr;	// TODOOE: Allocate more memory?
	}
	--m_NrOfFreeBlocks;

	muint* freeBlock = m_MemoryChunk + m_BlockSizeInMuints * m_FreeBlockIndex;
	m_FreeBlockIndex = *freeBlock;

	return freeBlock;
}

void DeranesPoolAllocator::deallocate( void* memory ) {
	assert( memory >= m_MemoryChunk && memory <= m_MemoryChunk + m_BlockSizeInMuints * m_NrOfBlocks );
	assert( m_NrOfFreeBlocks < m_NrOfBlocks );
	// TODOOE: Assert address is aligned.

	++m_NrOfFreeBlocks;

	muint* memoryAsMuint = reinterpret_cast<muint*>( memory );
	*memoryAsMuint = m_FreeBlockIndex;
	m_FreeBlockIndex = static_cast<muint>( (memoryAsMuint - m_MemoryChunk) / m_BlockSizeInMuints );	// TODOOE: Do multiplication instead of division.
}