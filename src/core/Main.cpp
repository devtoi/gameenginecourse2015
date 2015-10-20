#include <memory/Alloc.h>
#ifndef GLM_FORCE_RADIANS
	#define GLM_FORCE_RADIANS // Needs to be included before any GLM stuff is loaded.
#endif
#ifdef WINDOWS_DEBUG
	#include <crtdbg.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <gfx/Window.h>
#include <utility/ConfigManager.h>
#include <utility/Logger.h>
#include "Engine.h"

void RegisterLogger ();

int main( int argc, char* argv[] ) {
	#ifdef WINDOWS_DEBUG
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
		// _CrtSetBreakAlloc( 2704 ); //Insert allocation numbers here to invoke a break at that point
	#endif

	SDL_Init( 0 );

	RegisterLogger();

	Engine engine;
	engine.Initialize();
	engine.Run();
	engine.Cleanup();

	g_ConfigManager.SaveDirty();

	Logger::Cleanup();
	SDL_Quit();
	return 0;
}

void RegisterLogger() {
	struct {
		rString name;
		rString parent;
	} loggerRegisters[] =
	{
		// Name					Parent
		{ "Graphics",		  ""							},
		{ "GraphicsEngine",	  "Graphics"					},
		{ "Window",			  "Graphics"					},
		{ "Shader",			  "Graphics"					},
		{ "ShaderProgram",	  "Graphics"					},
		{ "ShaderBank",		  "Graphics"					},

		{ "SubsystemManager", ""							},

		{ "Subsystem",		  ""							},
		{ "SSWindow",		  "Subsystem"					},
		{ "SSGraphics",		  "Subsystem"					},
		{ "ProfileInOut",	  "Subsystem"					},

		{ "SDLEventManager",  ""							},

		{ "Engine",			  ""							},

		{ "Input",			  ""							},
		{ "KeyBindings",	  "Input"						},
		{ "GamepadState",	  "Input"						},

		{ "Utility",		  ""							},
		{ "Config",			  "Utility"						},
		{ "CallbackConfig",	  "Utility"						},
		{ "ConfigManager",	  "Utility"						},

		{ "Dataoriented",	  ""							},
		{ "CollectionHelper", "Dataoriented"				},

		{ "Resourcing", 	  ""							},
		{ "ResourceManager",  "Resourcing"					},
		{ "PackageManager",   "Resourcing"					}
	};

	for ( auto& entry : loggerRegisters ) {
		Logger::RegisterType( entry.name, entry.parent );
	}

	Logger::Initialize();

	Logger::RegisterInterestFromFile();
}

