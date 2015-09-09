#pragma once

#include "../Subsystem.h"

namespace gfx {
	class Window;
}

class SSWindow : public Subsystem {
public:
	SSWindow( int ID ) : Subsystem( Name, ID ) {
		SSWindow::ID = ID;
	}
	~SSWindow() = default;
	
	void 		Startup( SubsystemCollection* const subsystemCollection ) override;
	void 		Shutdown( SubsystemCollection* const subsystemCollection ) override;
	void 		UpdateUserLayer( const float deltaTime ) override;
	void 		UpdateSimulationLayer( const float timeStep ) override;

	Subsystem* 	CreateNew( ) const override;

	gfx::Window* GetWindow() const;

	static int GetStaticID( );

	const static pString Name;
private:
	static int ID;

	SSWindow( const SSWindow& rhs );
	SSWindow& operator=( const SSWindow& rhs );

	const pString m_WindowTitle = "engineT";

	gfx::Window* m_Window = nullptr;

};
