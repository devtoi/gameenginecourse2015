#include "CustomBigFileLoader.h"
#include <memory/DrinQStackAllocator.h>

#include <fstream>

const std::string gPacketHeaderVersionInfo = "CustomBigFile v0.1";

bool CustomBigFileLoader::Initialize( const pString& path, DrinQStackAllocator* allocator ) {
	m_PacketPath = path;

	std::fstream fileStream;
	fileStream.open( path, std::ios::in | std::ios::binary );

	if ( !fileStream ) {
		return false;
	}

	size_t marker = allocator->GetMarker();
	char* buffer = (char*)allocator->Allocate( gPacketHeaderVersionInfo.size() );
	//char* buffer = new char[gPacketHeaderVersionInfo.size()];
	fileStream.read( buffer, gPacketHeaderVersionInfo.size() );
	for ( size_t i = 0; i < gPacketHeaderVersionInfo.size(); ++i ) {
		if ( buffer[i] != gPacketHeaderVersionInfo[i] ) {
			fileStream.close();
			return false;
		}
	}
	//delete [] buffer;
	allocator->Unwind( marker );

	uint64_t assetCount = 0;
	FileReadInt( fileStream, assetCount );

	if ( assetCount == 0 ) {
		fileStream.close();
		return true;
	}
	
	std::string	gui;
	PacketAsset	packetAsset;
	for ( size_t i = 0; i < assetCount; ++i ) {
		FileReadString( fileStream, gui );
		FileReadString( fileStream, packetAsset.OriginalFilePath );
		FileReadInt( fileStream, packetAsset.Offset );
		FileReadInt( fileStream, packetAsset.Size );
		packetAsset.GUI = gui;

		size_t dotPos = packetAsset.OriginalFilePath.find_last_of( "." );
		if ( dotPos != std::string::npos ) {
			packetAsset.Suffix = packetAsset.OriginalFilePath.substr( dotPos + 1 );
		}

		m_PackageIndex.emplace( HashResourceName( gui ), packetAsset );
	}

	fileStream.close();
	return true;
}

FileContent CustomBigFileLoader::GetFileContent( ResourceIdentifier identifier, DrinQStackAllocator* allocator ) {
	auto indexEntry = m_PackageIndex.find( identifier );
	if ( indexEntry == m_PackageIndex.end() ) {
		return INVALID_FILE_CONTENT;
	}
	const PacketAsset& assetInfo = indexEntry->second;

	std::fstream fileStream;
	fileStream.open( m_PacketPath, std::ios::in | std::ios::binary );

	if ( !fileStream ) {
		return INVALID_FILE_CONTENT;
	}

	//char* buffer = new char[assetInfo.Size];
	char* buffer = (char*)allocator->Allocate( assetInfo.Size );
	fileStream.seekg( assetInfo.Offset );
	fileStream.read( buffer, assetInfo.Size );
	
	FileContent fileContent;
	fileContent.Content	= buffer;
	fileContent.Size	= assetInfo.Size;
	fileContent.Suffix	= assetInfo.Suffix;
	fileContent.Loaded	= true;

	fileStream.close();
	return fileContent;
}

void CustomBigFileLoader::FileReadInt( std::fstream& fileStream, uint64_t& outValue ) {
	fileStream.read( reinterpret_cast<char*>(&outValue), sizeof(outValue) );
}

void CustomBigFileLoader::FileReadString( std::fstream& fileStream, std::string& outString ) {
	uint64_t stringLength = 0;
	FileReadInt( fileStream, stringLength );

	if ( stringLength == 0 ) {
		outString = "";
		return;
	}

	char* buffer = new char[stringLength];
	fileStream.read( buffer, stringLength );

	outString = std::string( buffer, stringLength );

	delete [] buffer;
}