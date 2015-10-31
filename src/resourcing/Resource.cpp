#include "Resource.h"
#include "ResourceManager.h"

Resource::Resource( ) {

}

Resource::~Resource( ) {
	for ( const auto& identifier : m_Dependencies ) {
		g_ResourceManager.ReleaseResource( identifier );
	}
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

void Resource::AddDependency( const ResourceIdentifier identifier ) {
	m_Dependencies.push_back( identifier );
}
