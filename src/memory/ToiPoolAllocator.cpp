#include "ToiPoolAllocator.h"
#include <cassert>
#include <cstdlib>
#include <cstring>

ToiPoolAllocator::ToiPoolAllocator( size_t blockSize, size_t nrOfBlocks )
: m_BlockSize( blockSize ), m_NrOfBlocks( nrOfBlocks ) {
	assert( blockSize >= sizeof(void*) );

	m_Memory = static_cast<uint8_t*>( operator new( blockSize * nrOfBlocks ) );

	for ( size_t i = 0; i < nrOfBlocks * blockSize; i+= blockSize ) {
		size_t* nextFree = reinterpret_cast<size_t*>( m_Memory + i );
		*nextFree = reinterpret_cast<size_t>( m_Memory + i + blockSize );
	}
	
	size_t* last = reinterpret_cast<size_t*>( m_Memory + ( nrOfBlocks - 1) * blockSize );
	*last = reinterpret_cast<size_t>( nullptr );

	m_FirstFree = reinterpret_cast<size_t*>( m_Memory );
}

ToiPoolAllocator::~ToiPoolAllocator() {
	free( m_Memory );
}

void* ToiPoolAllocator::allocate( ) {
#ifdef TOI_POOL_ALLOCATOR_SPIN_LOCK
	while (m_Lock.test_and_set(std::memory_order_acquire)){}
#endif

#ifdef TOI_POOL_ALLOCATOR_MUTEX_LOCK
	std::lock_guard<std::mutex> lock( m_Mutex );
#endif

	assert( m_FirstFree != nullptr );
	size_t* nextnextFree = reinterpret_cast<size_t*>( *reinterpret_cast<size_t*>( m_FirstFree ) );
	size_t* toAllocate = m_FirstFree;

#ifdef TOI_POOL_ALLOCATOR_SET_ALLOCATED_MEMORY
	memset( toAllocate, TOI_POOL_ALLOCATOR_SET_ALLOCATED_MEMORY_VALUE, m_BlockSize );
#endif

	m_FirstFree = nextnextFree;

#ifdef TOI_POOL_ALLOCATOR_SPIN_LOCK
	m_Lock.clear(std::memory_order_release); 
#endif
	return toAllocate;
}

void ToiPoolAllocator::deallocate( void* memory ) {
#ifdef TOI_POOL_ALLOCATOR_SPIN_LOCK
	while (m_Lock.test_and_set(std::memory_order_acquire)){}
#endif

#ifdef TOI_POOL_ALLOCATOR_MUTEX_LOCK
	std::lock_guard<std::mutex> lock( m_Mutex );
#endif

	size_t* firstFreeTemp = m_FirstFree;

#ifdef TOI_POOL_ALLOCATOR_SET_FREED_MEMORY
	memset( memory, TOI_POOL_ALLOCATOR_SET_FREED_MEMORY_VALUE, m_BlockSize );
#endif

	m_FirstFree = reinterpret_cast<size_t*>( memory );
	size_t* toSet = reinterpret_cast<size_t*>( memory );
	*toSet = reinterpret_cast<size_t>( firstFreeTemp );

#ifdef TOI_POOL_ALLOCATOR_SPIN_LOCK
	m_Lock.clear(std::memory_order_release); 
#endif
}

