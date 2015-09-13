#pragma once

#include "../Subsystem.h"

class SSDeranesPoolTest : public Subsystem {
public:
							SSDeranesPoolTest( int ID ) : Subsystem( Name, ID ) {
								SSDeranesPoolTest::ID = ID;
							}

							~SSDeranesPoolTest		() = default;
	
	void 					Startup					( SubsystemCollection* const subsystemCollection ) override;
	void 					Shutdown				( SubsystemCollection* const subsystemCollection ) override;
	void 					UpdateUserLayer			( const float deltaTime ) override;
	void 					UpdateSimulationLayer	( const float timeStep ) override;

	Subsystem*				CreateNew				() const override;

	static int				GetStaticID				();
	const static pString	Name;
private:
	static int				ID;
};
