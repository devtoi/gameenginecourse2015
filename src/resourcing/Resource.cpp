#include "Resource.h"

Resource::Resource( size_t size )
	: m_Size( size ) {

}

size_t Resource::GetSize() const {
	return m_Size;
}
