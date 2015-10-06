#include "DeranesPoolAllocatorWithLock.h"

#if DERANES_POOL_ALLOCATOR_LOCK_TYPE == DERANES_POOL_ALLOCATOR_LOCK_TYPE_MUTEX
	#define DERANES_POOL_ALLOCATOR_LOCK		std::lock_guard<std::mutex> lock( m_Mutex );
	#define DERANES_POOL_ALLOCATOR_UNLOCK	
#else
	#include <thread>
	#define DERANES_POOL_ALLOCATOR_LOCK		while ( m_Lock.test_and_set( std::memory_order_acquire ) ){ std::this_thread::yield(); }
	#define DERANES_POOL_ALLOCATOR_UNLOCK	m_Lock.clear( std::memory_order_release );
#endif

DeranesPoolAllocatorWithLock::DeranesPoolAllocatorWithLock( size_t blockSize, size_t nrOfBlocks, size_t alignment ) : m_Allocator( blockSize, nrOfBlocks, alignment ) {

}

DeranesPoolAllocatorWithLock::~DeranesPoolAllocatorWithLock() {
	// Handle object being destructed while allocate or deallocate is called?
}

void* DeranesPoolAllocatorWithLock::allocate() {
	DERANES_POOL_ALLOCATOR_LOCK
	return m_Allocator.allocate();
	DERANES_POOL_ALLOCATOR_UNLOCK
}

void DeranesPoolAllocatorWithLock::deallocate( void* memory ) {
	DERANES_POOL_ALLOCATOR_LOCK
	m_Allocator.deallocate( memory );
	DERANES_POOL_ALLOCATOR_UNLOCK
}
