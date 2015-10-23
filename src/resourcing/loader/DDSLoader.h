#pragma once
#include "../ResourceLoader.h"
#include "../ResourcingLibraryDefine.h"
#include <SDL2/SDL.h>
#include <memory>
class DDSLoader : public ResourceLoader {
public:
	RESOURCING_API DDSLoader();
	RESOURCING_API ~DDSLoader();
	RESOURCING_API std::unique_ptr<Resource> LoadResource( const FileContent& fileContent ) override;
	//RESOURCING_API void LoadCompleteDDS(const char* filename);
private:
	SDL_Window* m_Window;
};
