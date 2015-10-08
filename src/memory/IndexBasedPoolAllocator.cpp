#include "IndexBasedPoolAllocator.h"

#include <cassert>
#include <stdlib.h>

void WriteUint8( void* writeLocation, size_t number ) {
	*static_cast<uint8_t*>( writeLocation ) = static_cast<uint8_t>( number );
}

size_t ReadUint8( void* readLocation ) {
	return static_cast<size_t>( *static_cast<uint8_t*>( readLocation ) );
}

void WriteUint16( void* writeLocation, size_t number ) {
	*static_cast<uint16_t*>( writeLocation ) = static_cast<uint16_t>( number );
}

size_t ReadUint16( void* readLocation ) {
	return static_cast<size_t>( *static_cast<uint16_t*>( readLocation ) );
}

void WriteUint32( void* writeLocation, size_t number ) {
	*static_cast<uint32_t*>( writeLocation ) = static_cast<uint32_t>( number );
}

size_t ReadUint32( void* readLocation ) {
	return static_cast<size_t>( *static_cast<uint32_t*>( readLocation ) );
}

void WriteUint64( void* writeLocation, size_t number ) {
	*static_cast<uint64_t*>( writeLocation ) = static_cast<uint64_t>( number );
}

size_t ReadUint64( void* readLocation ) {
	return static_cast<size_t>( *static_cast<uint64_t*>( readLocation ) );
}

IndexBasedPoolAllocator::IndexBasedPoolAllocator( size_t blockSize, size_t nrOfBlocks, size_t alignment ) {
	assert( nrOfBlocks > 0 );
	assert( alignment > 0 );

	m_BlockSizeInBytes			= alignment * ( ( blockSize / alignment ) + ( blockSize % alignment != 0 ) );
	m_NrOfBlocks				= nrOfBlocks;
	m_MemoryChunk				= reinterpret_cast<uint8_t*>( malloc( m_BlockSizeInBytes * m_NrOfBlocks ) );	// TODOOE: Make sure allocation succeds?
	m_NrOfFreeBlocks			= m_NrOfBlocks;
	m_FreeBlockIndex			= 0;

	if ( m_BlockSizeInBytes >= 8 ) {
		assert( m_NrOfBlocks - 1 <= UINT64_MAX );
		m_WriteUintFunction	= std::bind( &WriteUint64, std::placeholders::_1, std::placeholders::_2 );
		m_ReadUintFunction	= std::bind( &ReadUint64, std::placeholders::_1 );
	} else if ( m_BlockSizeInBytes >= 4 ) {
		assert( m_NrOfBlocks - 1 <= UINT32_MAX );
		m_WriteUintFunction	= std::bind( &WriteUint32, std::placeholders::_1, std::placeholders::_2 );
		m_ReadUintFunction	= std::bind( &ReadUint32, std::placeholders::_1 );
	} else if ( m_BlockSizeInBytes >= 2 ) {
		assert( m_NrOfBlocks - 1 <= UINT16_MAX );
		m_WriteUintFunction	= std::bind( &WriteUint16, std::placeholders::_1, std::placeholders::_2 );
		m_ReadUintFunction	= std::bind( &ReadUint16, std::placeholders::_1 );
	} else {
		assert( m_NrOfBlocks - 1 <= UINT8_MAX );
		m_WriteUintFunction	= std::bind( &WriteUint8, std::placeholders::_1, std::placeholders::_2 );
		m_ReadUintFunction	= std::bind( &ReadUint8, std::placeholders::_1 );
	}

	for ( size_t i = 0; i < m_NrOfBlocks; ) {
		void* block = m_MemoryChunk + m_BlockSizeInBytes * i;
		m_WriteUintFunction( block, ++i );
	}
}

IndexBasedPoolAllocator::~IndexBasedPoolAllocator() {
	if ( m_MemoryChunk ) {
		free( m_MemoryChunk );
		m_MemoryChunk = nullptr;
	}
}

void* IndexBasedPoolAllocator::allocate() {
	assert( m_NrOfFreeBlocks > 0 );

	if ( m_NrOfFreeBlocks <= 0 ) {
		return nullptr;	// TODOOE: Allocate more memory?
	}
	--m_NrOfFreeBlocks;

	void* freeBlock = m_MemoryChunk + m_BlockSizeInBytes * m_FreeBlockIndex;
	m_FreeBlockIndex = m_ReadUintFunction( freeBlock );

	return freeBlock;
}

void IndexBasedPoolAllocator::deallocate( void* memory ) {
	assert( memory >= m_MemoryChunk && memory <= m_MemoryChunk + m_BlockSizeInBytes * m_NrOfBlocks );
	assert( m_NrOfFreeBlocks < m_NrOfBlocks );
	// TODOOE: Assert address is aligned.

	++m_NrOfFreeBlocks;

	m_WriteUintFunction( memory, m_FreeBlockIndex );
	m_FreeBlockIndex = ( static_cast<uint8_t*>( memory ) - m_MemoryChunk ) / m_BlockSizeInBytes;
}