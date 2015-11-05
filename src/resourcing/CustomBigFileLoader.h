#pragma once

#include "Package.h"
#include <stdint.h>
#include "ResourcingLibraryDefine.h"

class CustomBigFileLoader : public Package {
public:
	RESOURCING_API bool				Initialize( const pString& path ) override;
	RESOURCING_API FileContent		GetFileContent( ResourceIdentifier identifier ) override;

private:
	void							FileReadInt( std::fstream& fileStream, uint64_t& outValue );
	void							FileReadString( std::fstream& fileStream, std::string& outString );

	struct PacketAsset {
		uint64_t	Offset;
		uint64_t	Size;
		std::string	Suffix;
		std::string	OriginalFilePath;	// Debug info.
		std::string	GUI;				// Debug info.
	};

	std::string											m_PacketPath	= "";
	std::unordered_map<ResourceIdentifier, PacketAsset>	m_PackageIndex;
};