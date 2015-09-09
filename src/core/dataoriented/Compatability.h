#pragma once

#include "ComponentCollection.h"

////////////////////////////////////
/// Sparse conveniency functions ///
////////////////////////////////////

template <typename Component>
static Component* GetSparseComponent( Entity entity ) {
	return &GetComponentCollection<Component>()->GetComponent( entity );
}

template <typename Component>
static pUnorderedMap<Entity, Component>* GetSparseComponentUnorderedMap( ) {
	return &GetComponentCollection<Component>()->GetComponents( );
}

template <typename Component>
static EntityMask GetSparseComponentFlag( ) {
	return GetComponentCollection<Component>()->GetComponentTypeFlag( );
}

template <typename Component>
static ComponentTypeIndex GetSparseComponentTypeIndex( ) {
	return GetComponentCollection<Component>()->GetComponentTypeIndex( );
}

template <typename Component>
static const ComponentMask& GetSparseEntitiesWithComponentMask() {
	return GetComponentCollection<Component>()->GetEntitiesWithComponentMask();
}

template <typename Component>
static std::shared_lock<std::shared_timed_mutex>& GetSparseReadLock() {
	return GetComponentCollection<Component>()->GetReadLock();
}

template <typename Component>
static std::unique_lock<std::shared_timed_mutex>& GetSparseWriteLock() {
	return GetComponentCollection<Component>()->GetWriteLock();
}

////////////////////////////////////
/// Dense conveniency functions ////
////////////////////////////////////

template <typename Component>
static Component* GetDenseComponent( Entity entity ) {
	return &GetComponentCollection<Component>()->GetComponent( entity );
}

template <typename Component>
static rVector<Component>* GetDenseComponentArray( ) {
	return &GetComponentCollection<Component>()->GetComponents();
}

template <typename Component>
static EntityMask GetDenseComponentFlag( ) {
	return GetComponentCollection<Component>()->GetComponentTypeFlag( );
}

template <typename Component>
static ComponentTypeIndex GetDenseComponentTypeIndex( ) {
	return GetComponentCollection<Component>()->GetComponentTypeIndex( );
}

template <typename Component>
static const ComponentMask& GetDenseEntitiesWithComponentMask( ) {
	return GetComponentCollection<Component>()->GetEntitiesWithComponentMask( );
}

template <typename Component>
static std::shared_lock<std::shared_timed_mutex>& GetDenseReadLock() {
	return GetComponentCollection<Component>()->GetReadLock();
}

template <typename Component>
static std::unique_lock<std::shared_timed_mutex>& GetDenseWriteLock() {
	return GetComponentCollection<Component>()->GetWriteLock();
}
