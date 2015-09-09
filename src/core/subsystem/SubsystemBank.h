#pragma once
#include <memory/Alloc.h>

class Subsystem;

#define g_SubsystemBank SubsystemBank::GetInstance()

class SubsystemBank {
public:
	static SubsystemBank& GetInstance();
	void Initialize();
	void Deinitialize();
	Subsystem* CreateSubsystem( int subsystemID );

private:
	SubsystemBank() = default;
	~SubsystemBank() = default;

	template<typename ConcreteSubsystem>
	void CreateSubsystemTemplate() {
		Subsystem* subsystem = pNew( ConcreteSubsystem, static_cast<int>( m_SubsystemTemplates.size() ) );
		m_SubsystemTemplates.push_back( subsystem );
		m_SubsystemTemplateNameToSubsystem.emplace( ConcreteSubsystem::Name, subsystem );
	}

	pVector<Subsystem*> m_SubsystemTemplates;
	pMap<pString, Subsystem*> m_SubsystemTemplateNameToSubsystem;
};
