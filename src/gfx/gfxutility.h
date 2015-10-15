#include <string>
static std::string GetDirectoryFromFilePath( const std::string& filePath ){
	int lastSlashPos = static_cast<int>( filePath.rfind( '/' ) );
	return filePath.substr( 0, lastSlashPos );
}

#define SAFE_DELETE(x) if(x) delete x