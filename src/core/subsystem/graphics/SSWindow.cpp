#include "SSWindow.h"
#include "SDL.h"
#include <chrono>
#include <thread>
#include <assert.h>
#include <GL/glew.h>
#include <resourcing/loader/DDSLoader.h>
#include <resourcing/ResourceManager.h>
#include <gfx/GraphicsEngine.h>

const pString SSWindow::Name = "Window";
int SSWindow::ID = -1;

void SSWindow::Startup( SubsystemCollection* const ) {
	m_Window = pNew( gfx::Window );
	gfx::WindowSettings ws;
	ws.Title = m_WindowTitle;
	ws.OpenGL = true;
	ws.Width = 1600;
	ws.Height = 900;
	ws.Vsync = true;
	m_Window->Initialize( ws );

	glewExperimental = GL_TRUE;
	glewInit();
	g_ResourceManager.StartWorkerThread( m_Window->GetWindow(), m_Window->GetContext() );

//	DDSLoader ddsloader;
//	std::chrono::steady_clock::time_point start, end;
//	start = std::chrono::high_resolution_clock::now();
//	//ddsloader.LoadCompleteDDS("../../../asset/texture/color.dds");
//	std::thread t = std::thread(LoadTexture, &ddsloader);
//	t.detach();
//	end = std::chrono::high_resolution_clock::now();
//	long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
//	std::cout << "loading textures took " << duration << "ms\n";
//
	

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

gfx::Window* SSWindow::GetWindow() const {
	return m_Window;
}
