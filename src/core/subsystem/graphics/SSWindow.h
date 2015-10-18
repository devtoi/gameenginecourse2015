#pragma once

#include "../Subsystem.h"
class Window;
typedef void* SDL_GL_Context;
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

	Window* GetWindow() const;

	static int GetStaticID( );

	const static pString Name;
private:
	static int ID;

	SSWindow( const SSWindow& rhs );
	SSWindow& operator=( const SSWindow& rhs );

	const pString m_WindowTitle = "engineT";

	Window* m_Window = nullptr;
	SDL_GL_Context  m_LoadingContext = nullptr;

};
