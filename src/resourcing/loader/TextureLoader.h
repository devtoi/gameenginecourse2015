#include "../ResourceLoader.h"
#include "../ResourcingLibraryDefine.h"
class TextureLoader : public ResourceLoader {
public:
	RESOURCING_API TextureLoader();
	RESOURCING_API ~TextureLoader();
	RESOURCING_API Resource* LoadResource(const FileContent& fileContent) override;
private:
};