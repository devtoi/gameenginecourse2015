#include "SSWindow.h"
#include <gfx/Window.h>

const pString SSWindow::Name = "Window";
int SSWindow::ID = -1;

void SSWindow::Startup( SubsystemCollection* const ) {
	m_Window = pNew( Window );
	WindowSettings ws;
	ws.Title = m_WindowTitle;
	ws.OpenGL = true;
	ws.Width = 1600;
	ws.Height = 900;
	ws.Vsync = true;
	m_Window->Initialize( ws );
}

void SSWindow::Shutdown( SubsystemCollection* const ) {
	pDelete( m_Window );
}

void SSWindow::UpdateUserLayer( const float ) {
	// Perform non-simulation update logic here (Don't forget to set update order priority!)
}

void SSWindow::UpdateSimulationLayer( const float ) {
	// Perform simulation update logic here (Don't forget to set update order priority!)
}

Subsystem* SSWindow::CreateNew( ) const {
	return pNew( SSWindow, SSWindow::ID );
}

int SSWindow::GetStaticID() {
	return SSWindow::ID;
}

Window* SSWindow::GetWindow() const {
	return m_Window;
}
