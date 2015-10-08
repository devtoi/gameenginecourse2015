#pragma once

//#define SHARED_POOL_ALLOCATOR_SET_ALLOCATED_MEMORY
#define SHARED_POOL_ALLOCATOR_SET_ALLOCATED_MEMORY_VALUE 0

//#define SHARED_POOL_ALLOCATOR_SET_FREED_MEMORY
#define SHARED_POOL_ALLOCATOR_SET_FREED_MEMORY_VALUE 0

//#define SHARED_POOL_ALLOCATOR_COUT_INFO

//#define SHARED_POOL_ALLOCATOR_MUTEX_LOCK
#define SHARED_POOL_ALLOCATOR_SPIN_LOCK
#define SHARED_POOL_ALLOCATOR_SPIN_LOCK_YIELD_CODE std::this_thread::yield()
//#define SHARED_POOL_ALLOCATOR_SPIN_LOCK_YIELD_CODE 

#include "MemoryLibraryDefine.h"
#include <cstddef>
#include <stdint.h>
#include <cassert>
#include <iostream>
#include <iomanip>
#ifdef SHARED_POOL_ALLOCATOR_MUTEX_LOCK
	#include <mutex>
#endif
#ifdef SHARED_POOL_ALLOCATOR_SPIN_LOCK
	#include <atomic>
#endif
#include <thread>

//#define SHARED_POOL_ALLOCATOR_NR_OF_BLOCKS 65536
#define SHARED_POOL_ALLOCATOR_NR_OF_BLOCKS 1048576
#define SHARED_POOL_ALLOCATOR_MAX_BLOCK_SIZE 4096
//#define SHARED_POOL_ALLOCATOR_MAX_BLOCK_SIZE 512

//#define TOI_TEMPLATED_LOCKABLE_POOL_USE_STANDARD_ALLOCATOR

#if defined(TOI_TEMPLATED_LOCKABLE_POOL_USE_STANDARD_ALLOCATOR)
	#define poolAlloc(Size) malloc(Size)
	#define poolFree(Size, Ptr) free(Ptr)
	#define poolNew(Type, ...) new Type(__VA_ARGS__)
	#define poolDelete(Ptr) delete Ptr;
#else
	#define poolAlloc(Size) GetSharedPoolAllocator<Size>().allocate()
	#define poolFree(Size, Ptr) GetSharedPoolAllocator<Size>().deallocate(Ptr)
	#define poolNew(Type, ...) GetSharedPoolAllocator<sizeof(Type)>().construct<Type>(__VA_ARGS__)
	#define poolDelete(Ptr) GetSharedPoolAllocator<sizeof(*Ptr)>().destroy(Ptr)
#endif

template <size_t BlockSize, size_t NrOfBlocks>
class SharedPoolAllocator {
public:
	SharedPoolAllocator( ) {
		assert( BlockSize >= sizeof(void*) );
        assert( BlockSize <= SHARED_POOL_ALLOCATOR_MAX_BLOCK_SIZE );
#ifdef SHARED_POOL_ALLOCATOR_COUT_INFO
		std::cout << "Creating templated pool allocator for blocksize " << BlockSize << std::endl;
#endif

		m_ActualAllocation = static_cast<uint8_t*>( operator new( BlockSize * NrOfBlocks + BlockSize ) );

		// Align allocation to block size
		size_t mask = reinterpret_cast<size_t>( m_ActualAllocation ) & ( BlockSize - 1 );
		m_Memory = reinterpret_cast<uint8_t*>( reinterpret_cast<size_t>( m_ActualAllocation ) + ( mask > 0 ? BlockSize - mask : 0 ) );

		for ( size_t i = 0; i < NrOfBlocks * BlockSize; i+= BlockSize ) {
			size_t* nextFree = reinterpret_cast<size_t*>( m_Memory + i );
			*nextFree = reinterpret_cast<size_t>( m_Memory + i + BlockSize );
		}
		
		size_t* last = reinterpret_cast<size_t*>( m_Memory + ( NrOfBlocks - 1) * BlockSize );
		*last = reinterpret_cast<size_t>( nullptr );

		m_FirstFree = reinterpret_cast<size_t*>( m_Memory );
	}

	~SharedPoolAllocator() {
#ifdef SHARED_POOL_ALLOCATOR_COUT_INFO
		std::cout << "Destroying templated pool allocator for blocksize " << BlockSize << std::endl;
#endif
		free( m_ActualAllocation );
	}

	void* allocate( ) {
#ifdef SHARED_POOL_ALLOCATOR_SPIN_LOCK
		while (m_Lock.test_and_set(std::memory_order_acquire)){ SHARED_POOL_ALLOCATOR_SPIN_LOCK_YIELD_CODE; }
#endif

#ifdef SHARED_POOL_ALLOCATOR_MUTEX_LOCK
		std::lock_guard<std::mutex> lock( m_Mutex );
#endif

		assert( m_FirstFree != nullptr );
		size_t* toAllocate = m_FirstFree;
		m_FirstFree = reinterpret_cast<size_t*>( *reinterpret_cast<size_t*>( m_FirstFree ) );

#ifdef SHARED_POOL_ALLOCATOR_SET_ALLOCATED_MEMORY
		memset( toAllocate, SHARED_POOL_ALLOCATOR_SET_ALLOCATED_MEMORY_VALUE, BlockSize );
#endif

#ifdef SHARED_POOL_ALLOCATOR_SPIN_LOCK
		m_Lock.clear(std::memory_order_release); 
#endif
		return toAllocate;
	}

	void deallocate( void* memory ) {
#ifdef SHARED_POOL_ALLOCATOR_SPIN_LOCK
		while (m_Lock.test_and_set(std::memory_order_acquire)){SHARED_POOL_ALLOCATOR_SPIN_LOCK_YIELD_CODE;}
#endif

#ifdef SHARED_POOL_ALLOCATOR_MUTEX_LOCK
		std::lock_guard<std::mutex> lock( m_Mutex );
#endif

		//size_t* firstFreeTemp = m_FirstFree;

#ifdef SHARED_POOL_ALLOCATOR_SET_FREED_MEMORY
		memset( memory, SHARED_POOL_ALLOCATOR_SET_FREED_MEMORY_VALUE, BlockSize );
#endif

		size_t* toSet = reinterpret_cast<size_t*>( memory );
		*toSet = reinterpret_cast<size_t>( m_FirstFree );
		m_FirstFree = reinterpret_cast<size_t*>( memory );

#ifdef SHARED_POOL_ALLOCATOR_SPIN_LOCK
		m_Lock.clear(std::memory_order_release); 
#endif
	}


	template<typename Type>
	void PrintMemory() {
#ifdef SHARED_POOL_ALLOCATOR_SPIN_LOCK
		while (m_Lock.test_and_set(std::memory_order_acquire)){ SHARED_POOL_ALLOCATOR_SPIN_LOCK_YIELD_CODE; }
#endif

#ifdef SHARED_POOL_ALLOCATOR_MUTEX_LOCK
		std::lock_guard<std::mutex> lock( m_Mutex );
#endif

			assert( sizeof(Type ) <= BlockSize );

			for ( size_t i = 0; i < NrOfBlocks * BlockSize; i+= BlockSize ) {
				Type* block = reinterpret_cast<Type*>( m_Memory + i );
				std::cout << std::hex << *block << std::endl;
			}
		std::cout << std::resetiosflags( std::ios::hex ) << std::endl;;
#ifdef SHARED_POOL_ALLOCATOR_SPIN_LOCK
		m_Lock.clear(std::memory_order_release); 
#endif
	}

	template<typename Type, typename... Args>
	Type* construct( Args&&... args ) {
		assert( sizeof( Type ) <= BlockSize );

		Type* data = static_cast<Type*>( allocate() );
		new(data) Type(std::forward<Args>( args )...);
		return data;
	}

	template<typename Type>
	void destroy( Type* memory ) {
		memory->~Type();
		assert( sizeof( Type ) <= BlockSize );
		deallocate( memory );
	}

private:
	uint8_t* m_Memory = nullptr;
	uint8_t* m_ActualAllocation = nullptr;
	size_t* m_FirstFree = nullptr;

#ifdef SHARED_POOL_ALLOCATOR_MUTEX_LOCK
	std::mutex m_Mutex;
#endif
#ifdef SHARED_POOL_ALLOCATOR_SPIN_LOCK
	std::atomic_flag m_Lock = ATOMIC_FLAG_INIT;
#endif
};

template<size_t BlockSize>
static SharedPoolAllocator<BlockSize, SHARED_POOL_ALLOCATOR_NR_OF_BLOCKS>& GetSharedPoolAllocator() {
	static SharedPoolAllocator<BlockSize, SHARED_POOL_ALLOCATOR_NR_OF_BLOCKS> poolAllocator;
	return poolAllocator;
}
