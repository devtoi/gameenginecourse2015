#pragma once
#include <stdint.h>
#include <assert.h>
#include <mutex>
#include "MemoryLibraryDefine.h"

#define DRINQ_STACK_ALLOCATOR_SIZE 64
#define DRINQ_STACK_ALLOCATOR_ALIGNMENT 8

#define DRINQ_STACK_ALLOCATOR_INITIAL 128
#define DRINQ_STACK_ALLOCATOR_PADDED 127
#define DRINQ_STACK_ALLOCATOR_RESET 126

//#define RESET_MEMORY
//#define THREAD_SAFE

class DrinQStackAllocator {
public:

	MEMORY_API DrinQStackAllocator( );

	MEMORY_API DrinQStackAllocator( size_t size );
	MEMORY_API ~DrinQStackAllocator();
	MEMORY_API size_t GetMarker();

	MEMORY_API uint8_t* GetBuffer();

	MEMORY_API size_t GetSize();

	MEMORY_API void Unwind( size_t marker );
	
	MEMORY_API void* Allocate(size_t size, size_t alignment = DRINQ_STACK_ALLOCATOR_ALIGNMENT);

	template<typename Type, typename... Args>
	Type* construct( size_t count, Args&&... args ) {
		Type* data = static_cast<Type*>( Allocate( count * sizeof( Type ), alignof( Type ) ) );
		for( size_t i = 0; i < count; ++i ) {
			new (&data[i]) Type(std::forward<Args>( args )...);
		}
		return data;
	}

	template< typename T>
	void Destroy( T*& ptr ) {
		ptr[0].~T();
	}

private:

	std::mutex m_Mutex;

	uint8_t* m_Memory = nullptr;
	size_t m_Size;

	size_t m_Marker;
};
