#pragma once
#include <glm/glm.hpp>
#include <memory/ToiStackAllocator.h>
#include "../Subsystem.h"

#define TEST_SIZE 8192 * 512
#define ALLOCATOR_SIZE sizeof(float) + sizeof(bool) + sizeof(TestData) * TEST_SIZE

struct TestData {
	glm::vec4 data1;
	glm::mat4 data2;
};

//#define USE_STACK_ALLOC
class SSStackTest : public Subsystem {
public:
	SSStackTest( int ID ) : Subsystem( Name, ID ) {
		SSStackTest::ID = ID;
	}
	~SSStackTest() = default;
	
	void 		Startup( SubsystemCollection* const subsystemCollection ) override;
	void 		Shutdown( SubsystemCollection* const subsystemCollection ) override;
	void 		UpdateUserLayer( const float deltaTime ) override;
	void 		UpdateSimulationLayer( const float timeStep ) override;

	Subsystem* 	CreateNew( ) const override;

	static int GetStaticID( );

	const static pString Name;
private:
	static int ID;
	ToiStackAllocator::Marker	m_Marker;
	ToiStackAllocator*			m_Allocator;
	int							m_CurrentTestSize;
	float*						m_Array1[TEST_SIZE];
	bool*						m_Array2[TEST_SIZE];
	TestData*					m_Array3[TEST_SIZE];
};
