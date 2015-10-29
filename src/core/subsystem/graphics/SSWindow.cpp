#include "SSWindow.h"
#include <gfx/Window.h>
#include <SDL2/SDL.h>
#include <resourcing/loader/DDSLoader.h>
#include <resourcing/ResourceManager.h>
#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <assert.h>
#include <utility/PlatformDefinitions.h>
#ifdef PLATFORM == PLATFORM_WINDOWS
#include <SDL2/SDL_syswm.h>
#include <GL/wglew.h>
HGLRC LoadingContext;
HGLRC MainContext;
HDC Device;
#elif PLATFORM == PLATFORM_LINUX
#include <GL/glxew.h
typedef GLXContext(*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
GLXContext MainContext;
GLXContext LoadingContext;
#endif

const pString SSWindow::Name = "Window";
int SSWindow::ID = -1;

void LoadTexture(DDSLoader* loader) {
#ifdef PLATFORM == PLATFORM_WINDOWS
	wglMakeCurrent(Device, LoadingContext);
#elif PLATFORM == PLATFORM_LINUX
	//TODO:Fill out with GLX code
#endif
	//g_ResourceManager.AquireResource(HashResourceName("Texture.Color"));

	GLsync fenceId = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	GLenum result;
	while (true)
	{
		result = glClientWaitSync(fenceId, GL_SYNC_FLUSH_COMMANDS_BIT, GLuint64(5000000000)); //5 Second timeout 
		if (result != GL_TIMEOUT_EXPIRED) break; //we ignore timeouts and wait until all OpenGL commands are processed! 
	}
	//mark for completion here
#ifdef PLATFORM == PLATFORM_WINDOWS
	wglMakeCurrent(nullptr, nullptr);

#elif PLATFORM == PLATFORM_LINUX
	//TODO:Fill out with GLX code
#endif
	
}

void SSWindow::Startup( SubsystemCollection* const ) {
	m_Window = pNew( Window );
	WindowSettings ws;
	ws.Title = m_WindowTitle;
	ws.OpenGL = true;
	ws.Width = 1600;
	ws.Height = 900;
	ws.Vsync = true;
	m_Window->Initialize( ws );

	glewExperimental = GL_TRUE;
	glewInit();
	g_ResourceManager.StartWorkerThread(m_Window->GetWindow());
//
//#ifdef PLATFORM == PLATFORM_WINDOWS
//	SDL_SysWMinfo info;
//	SDL_VERSION(&info.version);
//	if (SDL_GetWindowWMInfo(m_Window->GetWindow(), &info) < 0) {
//		assert(false);
//	}
//	HWND hWnd = info.info.win.window;
//	Device = GetDC(hWnd);
//	MainContext = wglGetCurrentContext();
//	LoadingContext = wglCreateContext(Device);
//	wglShareLists(MainContext, LoadingContext);
//#elif PLATFORM == PLATFORM_LINUX
//	//TODO:Fill out with GLX code
//	return;
//#endif
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

Window* SSWindow::GetWindow() const {
	return m_Window;
}
