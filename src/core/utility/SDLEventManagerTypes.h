#pragma once

#include <functional>
#include <SDL2/SDL_events.h>
#include <utility/Handle.h>

struct SDLEventCallback_tag { };

typedef Handle<SDLEventCallback_tag, int, -1> SDLEventCallbackHandle;
typedef std::function<void ( const SDL_Event& )>	  SDLEventCallback;
