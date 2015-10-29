#include "Resource.h"

Resource::Resource( ) {

}

size_t Resource::GetSize() const {
	return m_Size;
}

void Resource::SetSize(size_t size) {
	m_Size = size;
}

void Resource::SetReady() {
	m_Ready = true;
}
bool Resource::IsReady() const {
	return m_Ready;
}