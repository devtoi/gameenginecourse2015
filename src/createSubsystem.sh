#!/bin/bash
HEADERFILE="#pragma once

#include \"../Subsystem.h\"

class SS$1 : public Subsystem {
public:
	SS$1( int ID ) : Subsystem( Name, ID ) {
		SS$1::ID = ID;
	}
	~SS$1() = default;
	
	void 		Startup( SubsystemCollection* const subsystemCollection ) override;
	void 		Shutdown( SubsystemCollection* const subsystemCollection ) override;
	void 		UpdateUserLayer( const float deltaTime ) override;
	void 		UpdateSimulationLayer( const float timeStep ) override;

	Subsystem* 	CreateNew( ) const override;

	static int GetStaticID( );

	const static pString Name;
private:
	static int ID;
};
"

CPPFILE="#include \"SS$1.h\"

const pString SS$1::Name = \"$1\";
int SS$1::ID = -1;

void SS$1::Startup( SubsystemCollection* const subsystemCollection ) {
	// Perform Initialization here (Don't forget to set startup order priority!)
}

void SS$1::Shutdown( SubsystemCollection* const subsystemCollection ) {
	// Perform Cleanup here (Don't forget to set shutdown order priority!)
}

void SS$1::UpdateUserLayer( const float deltaTime ) {
	// Perform non-simulation update logic here (Don't forget to set update order priority!)
}

void SS$1::UpdateSimulationLayer( const float timeStep ) {
	// Perform simulation update logic here (Don't forget to set update order priority!)
}

Subsystem* SS$1::CreateNew( ) const {
	return pNew( SS$1, SS$1::ID );
}

int SS$1::GetStaticID() {
	return SS$1::ID;
}
"

if [ $2 = "" ]; then
	SUBSYSTEMDIR=""
else
	SUBSYSTEMDIR="$2/"
	mkdir -p "${SUBSYSTEMDIR}"
fi

printf '%s' "${HEADERFILE}" > "${SUBSYSTEMDIR}SS$1.h"
printf '%s' "${CPPFILE}" > "${SUBSYSTEMDIR}SS$1.cpp"
