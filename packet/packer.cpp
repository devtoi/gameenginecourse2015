#include <iostream>
#include <string>
#include <vector>
#include <fstream>
	
const std::string gPacketHeaderVersionInfo = "CustomBigFile v0.1";

enum class InstructionType {
	None,
	OpenPacket,
	NameNextAsset,
	AppendAsset,
	GeneratePacket,
	TrimPacket,
	ListAssets
};

struct Instruction {
	InstructionType		Type;
	std::string			Argument;
	Instruction( InstructionType type, const std::string& argument ) : Type(type), Argument(argument) {};
};

struct Asset {
	std::string		GUI;
	std::string		FilePath;
	Asset( const std::string& gui, const std::string& filePath ) : GUI(gui), FilePath(filePath) {};
};

struct Index {
	struct Entry {
		std::string		GUI;
		std::string		FilePath;
		size_t			Offset;
		std::streampos	OffsetFilePosition;
		size_t			Size;
		std::streampos	SizeFilePosition;

		Entry() {};
		Entry( const std::string& gui, const std::string& filePath ) : GUI(gui), FilePath(filePath) {};
	};

	std::streampos		AssetCountFilePosition;
	std::streampos		EndOfIndexFilePosition;
	std::vector<Entry>	Entries;
};

void		InterpretProgramArguments		( const std::vector<std::string>& programArguments );
bool		ValidateAndOrderInstructions	( std::vector<Instruction>& instructions );
bool		ExecuteInstructions				( std::vector<Instruction>& instructions );
std::string	MakeGUIFromFilePath				( const std::string& filePath );
bool		AppendAssetsToPacket			( const std::string& packetPath, const std::vector<Asset>& assets );
bool		GeneratePacket					( const std::string& packetPath );
bool		TrimPacket						( const std::string& packetPath );
bool		ListAssets						( const std::string& packetPath );
void		FileWriteInt					( std::fstream& fileStream, size_t value );
void		FileWriteString					( std::fstream& fileStream, const std::string& string );
void		FileReadInt						( std::fstream& fileStream, size_t& outValue );
void		FileReadString					( std::fstream& fileStream, std::string& outString );
bool		BuildIndex						( std::fstream& fileStream, Index& index );

int main(int argc, char* argv[]) {
	std::vector<std::string> programArguments;
	programArguments.reserve( argc - 1 );

	for ( int i = 1; i < argc; ++i ) {
		programArguments.push_back( argv[i] );
	}

	InterpretProgramArguments( programArguments );

	return 0;
};

void InterpretProgramArguments( const std::vector<std::string>& programArguments ) {
	if ( programArguments.empty() ) {
		std::cout << "Usage: TODO write usage information." << std::endl;
		return;
	}

	std::vector<Instruction> instructions;
	InstructionType currentInstructionType = InstructionType::None;

	size_t currentArgumentIndex = 0;
	while ( currentArgumentIndex < programArguments.size() ) {
		if ( programArguments[currentArgumentIndex] == "-o" ) {
			currentInstructionType = InstructionType::OpenPacket;
		} else if ( programArguments[currentArgumentIndex] == "-a" ) {
			currentInstructionType = InstructionType::AppendAsset;
		} else if ( programArguments[currentArgumentIndex] == "-g" ) {
			currentInstructionType = InstructionType::GeneratePacket;
		} else if ( programArguments[currentArgumentIndex] == "-t" ) {
			currentInstructionType = InstructionType::TrimPacket;
		} else if ( programArguments[currentArgumentIndex] == "-l" ) {
			currentInstructionType = InstructionType::ListAssets;
		}  else if ( programArguments[currentArgumentIndex] == "-n" ) {
			if ( currentInstructionType == InstructionType::AppendAsset ) {
				currentInstructionType = InstructionType::NameNextAsset;
			} else {
				std::cout << "Error: Naming of an asset can only be done while appending assets." << std::endl;
				return;
			}
		} else {
			if ( currentInstructionType == InstructionType::None ) {
				std::cout << "Unknown command \"" << programArguments[currentArgumentIndex] << "\"" << std::endl;
				return;
			} else {
				instructions.push_back( Instruction( currentInstructionType, programArguments[currentArgumentIndex] ) );
				if ( currentInstructionType == InstructionType::NameNextAsset || currentInstructionType == InstructionType::AppendAsset ) {
					currentInstructionType = InstructionType::AppendAsset;
				} else {
					currentInstructionType = InstructionType::None;
				}
			}
		}
		if ( currentInstructionType == InstructionType::GeneratePacket || currentInstructionType == InstructionType::TrimPacket || currentInstructionType == InstructionType::ListAssets ) {
			if ( currentArgumentIndex == programArguments.size() - 1 || programArguments[currentArgumentIndex + 1][0] == '-' ) {
				instructions.push_back( Instruction( currentInstructionType, "" ) );
			}
		}
		++currentArgumentIndex;
	}

	if ( !ValidateAndOrderInstructions( instructions ) ) {
		return;
	}
	if ( !ExecuteInstructions( instructions ) ) {
		return;
	}
}

bool ValidateAndOrderInstructions( std::vector<Instruction>& instructions ) {
	std::vector<Instruction> orderedInstructions;

	bool foundOpenPacket = false;
	for ( size_t i = 0; i < instructions.size(); ++i ) {
		if ( instructions[i].Type != InstructionType::OpenPacket ) {
			continue;
		}
		if ( foundOpenPacket ) {
			std::cout << "Error: Not allowed to open multiple packets." << std::endl;
			return false;
		} else {
			orderedInstructions.push_back( instructions[i] );
			foundOpenPacket = true;
		}
	}

	if ( !foundOpenPacket ) {
		for ( size_t i = 0; i < instructions.size(); ++i ) {
			if ( instructions[i].Type != InstructionType::GeneratePacket && instructions[i].Type != InstructionType::TrimPacket && instructions[i].Type != InstructionType::ListAssets ) {
				continue;
			}
			if ( instructions[i].Argument != "" ) {
				orderedInstructions.push_back( Instruction( InstructionType::OpenPacket, instructions[i].Argument ) );
				foundOpenPacket = true;
				break;
			}
		}
	}

	if ( !foundOpenPacket ) {
		std::cout << "Error: Found no name for the package." << std::endl;
		return false;
	}

	for ( size_t i = 0; i < instructions.size(); ++i ) {
		if ( instructions[i].Type == InstructionType::NameNextAsset || instructions[i].Type == InstructionType::AppendAsset ) {
			orderedInstructions.push_back( instructions[i] );	// TODO: Check for duplicates.
		}
	}

	auto addInstructionsOfType = [&instructions, &orderedInstructions]( InstructionType type ) {
		for ( size_t i = 0; i < instructions.size(); ++i ) {
			if ( instructions[i].Type != type ) {
				continue;
			}
			if ( instructions[i].Argument != "" ) {	// TODO: Check for duplicates.
				orderedInstructions.push_back( instructions[i] );
			} else {
				orderedInstructions.push_back( Instruction( instructions[i].Type, orderedInstructions[0].Argument ) );
			}
		}
	};

	addInstructionsOfType( InstructionType::GeneratePacket );
	addInstructionsOfType( InstructionType::TrimPacket );
	addInstructionsOfType( InstructionType::ListAssets );

	instructions.clear();
	instructions = orderedInstructions;

	return true;
}

bool ExecuteInstructions( std::vector<Instruction>& instructions ) {
	{
		std::vector<Asset> assets;
		for ( size_t i = 1; i < instructions.size(); ++i ) {
			if ( instructions[i].Type == InstructionType::NameNextAsset ) {
				assets.push_back( Asset( instructions[i].Argument, instructions[i + 1].Argument ) );
				++i;
			} else if ( instructions[i].Type == InstructionType::AppendAsset ) {
				assets.push_back( Asset( MakeGUIFromFilePath( instructions[i].Argument ), instructions[i].Argument ) );
			} else {
				break;
			}
		}

		if ( !assets.empty() ) {
			if ( !AppendAssetsToPacket( instructions[0].Argument, assets ) ) {
				return false;
			}
		}
	}

	for ( size_t i = 0; i < instructions.size(); ++i ) {
		if ( instructions[i].Type == InstructionType::GeneratePacket ) {
			GeneratePacket( instructions[0].Argument );
		} else if ( instructions[i].Type == InstructionType::TrimPacket ) {
			TrimPacket( instructions[0].Argument );
		} else if ( instructions[i].Type == InstructionType::ListAssets ) {
			ListAssets( instructions[0].Argument );
		}
	}

	return true;
}

std::string	MakeGUIFromFilePath( const std::string& filePath ) {
	std::string gui = filePath;
	for ( size_t i = 0; i < gui.size(); ++i ) {
		if ( gui[i] == '/' || gui[i] == '\\' ) {
			gui[i] = '.';
		}
	}
	return gui;
}

bool AppendAssetsToPacket( const std::string& packetPath, const std::vector<Asset>& assets ) {
	std::fstream fileStream;
	fileStream.open( packetPath, std::ios::in | std::ios::out | std::ios::binary );

	Index index;
	if ( !fileStream || !BuildIndex( fileStream, index ) ) {
		fileStream.close();
		fileStream.open( packetPath, std::ios::out | std::ios::binary );
		if ( !fileStream ) {
			std::cout << "Error! Could not open nor create packet using path \"" << packetPath << "\"" << std::endl;
			fileStream.close();
			return false;
		}
		std::cout << "Creating new packet \"" << packetPath << "\"" << std::endl;
		fileStream.write( gPacketHeaderVersionInfo.c_str(), gPacketHeaderVersionInfo.length() );
		index.AssetCountFilePosition = fileStream.tellp();
		FileWriteInt( fileStream, 0 );
		index.EndOfIndexFilePosition = fileStream.tellp();
	}

	size_t oldIndexSize = index.Entries.size();

	for ( auto it = assets.cbegin(); it != assets.cend(); ++it ) {
		bool guiAlreadyAdded = false;
		for ( auto indexIt = index.Entries.cbegin(); indexIt != index.Entries.cend(); ++indexIt ) {
			if ( it->GUI == indexIt->GUI ) {
				guiAlreadyAdded = true;
				continue;
			}
		}
		if ( guiAlreadyAdded ) {
			std::cout << "Warning! GUI \"" << it->GUI << "\" already exists. Did not append asset to \"" << packetPath << "\"" << std::endl;
			continue;
		}
		index.Entries.push_back( Index::Entry( it->GUI, it->FilePath ) );
	}

	size_t newIndexSize = index.Entries.size();
	if ( oldIndexSize >= newIndexSize ) {
		fileStream.close();
		return true;
	}

	fileStream.seekp( index.AssetCountFilePosition );
	FileWriteInt( fileStream, newIndexSize );

	std::cout << "Creating asset index in packet \"" << packetPath << "\"" << std::endl;
	fileStream.seekp( index.EndOfIndexFilePosition );
	for ( auto it = index.Entries.cbegin() + oldIndexSize; it != index.Entries.cend(); ++it ) {
		std::cout << "+ Asset GUI=\"" << it->GUI << "\", Path=\"" << it->FilePath << "\"" << std::endl;
		FileWriteString( fileStream, it->GUI );
		FileWriteString( fileStream, it->FilePath );
		FileWriteInt( fileStream, 0 );	// Offset
		FileWriteInt( fileStream, 0 );	// Size
	}

	fileStream.close();

	return true;
}

bool GeneratePacket ( const std::string& packetPath ) {
	std::cout << "Generating packet \"" << packetPath << "\"" << std::endl;

	std::fstream fileStream;
	fileStream.open( packetPath, std::ios::in | std::ios::out | std::ios::binary );

	if ( !fileStream ) {
		std::cout << "Error: Could not open packet \"" << packetPath << "\"" << std::endl;
		return false;
	}

	Index index;
	if ( !BuildIndex( fileStream, index ) ) {
		std::cout << "Error: File is not an accepted packet \"" << packetPath << "\"" << std::endl;
	}

	fileStream.seekp( index.EndOfIndexFilePosition );
	for ( auto it = index.Entries.begin(); it != index.Entries.end(); ++it ) {
		std::fstream assetStream( it->FilePath, std::ios::in | std::ios::binary );

		if ( !assetStream ) {
			std::cout << "Warning! Could not find asset \"" << it->FilePath << "\"" << std::endl;
			it->Offset = 0;
			it->Size = 0;
			continue;
		}

		std::streampos assetFilePosition = assetStream.tellg();
		assetStream.seekg( 0, std::ios::end );
		it->Size = static_cast<size_t>( assetStream.tellg() - assetFilePosition );
		it->Offset = static_cast<size_t>( fileStream.tellp() );

		char* assetBuffer = new char[it->Size];
		assetStream.seekg( 0, std::ios::beg );
		assetStream.read( assetBuffer, it->Size );
		fileStream.write( assetBuffer, it->Size );
		delete [] assetBuffer;
		assetStream.close();
	}

	for ( auto it = index.Entries.begin(); it != index.Entries.end(); ++it ) {
		fileStream.seekp( it->OffsetFilePosition );
		FileWriteInt( fileStream, it->Offset );
		fileStream.seekp( it->SizeFilePosition );
		FileWriteInt( fileStream, it->Size );
	}

	fileStream.close();

	return true;
}

bool TrimPacket ( const std::string& packetPath ) {
	std::cout << "Triming packet \"" << packetPath << "\"" << std::endl;

	return true;
}

bool ListAssets ( const std::string& packetPath ) {
	std::cout << "Listing assets in packet \"" << packetPath << "\"" << std::endl;

	std::fstream fileStream;
	fileStream.open( packetPath, std::ios::in | std::ios::binary );

	if ( !fileStream ) {
		std::cout << "Error: Could not open packet \"" << packetPath << "\"" << std::endl;
		return false;
	}

	Index index;
	if ( !BuildIndex( fileStream, index ) ) {
		std::cout << "Error: File is not an accepted packet \"" << packetPath << "\"" << std::endl;
	}

	if ( index.Entries.empty() ) {
		std::cout << "Packet is empty." << std::endl;
		fileStream.close();
		return false;
	}

	for ( auto it = index.Entries.begin(); it != index.Entries.end(); ++it ) {
		if ( it != index.Entries.begin() ) {
			std::cout << " ";
		}
		std::cout << it->GUI << "(" << it->FilePath << ")";
	}
	std::cout << std::endl;

	fileStream.close();

	return true;
}

void FileWriteInt( std::fstream& fileStream, size_t value ) {
	fileStream.write( reinterpret_cast<char*>(&value), sizeof(value) );
}

void FileWriteString( std::fstream& fileStream, const std::string& string ) {
	FileWriteInt( fileStream, string.length() );
	fileStream.write( string.c_str(), string.length() );
}

void FileReadInt( std::fstream& fileStream, size_t& outValue ) {
	fileStream.read( reinterpret_cast<char*>(&outValue), sizeof(outValue) );
}

void FileReadString( std::fstream& fileStream, std::string& outString ) {
	size_t stringLength = 0;
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

bool BuildIndex( std::fstream& fileStream, Index& index ) {
	char* buffer;

	buffer = new char[gPacketHeaderVersionInfo.size()];
	fileStream.read( buffer, gPacketHeaderVersionInfo.size() );
	for ( size_t i = 0; i < gPacketHeaderVersionInfo.size(); ++i ) {
		if ( buffer[i] != gPacketHeaderVersionInfo[i] ) {
			return false;
		}
	}
	delete [] buffer;

	size_t assetCount = 0;
	index.AssetCountFilePosition = fileStream.tellg();
	FileReadInt( fileStream, assetCount );

	if ( assetCount == 0 ) {
		index.EndOfIndexFilePosition = fileStream.tellg();
		return true;
	}

	index.Entries.resize( assetCount );
	for ( auto it = index.Entries.begin(); it != index.Entries.end(); ++it ) {
		FileReadString( fileStream, it->GUI );
		FileReadString( fileStream, it->FilePath );
		it->OffsetFilePosition = fileStream.tellg();
		FileReadInt( fileStream, it->Offset );
		it->SizeFilePosition = fileStream.tellg();
		FileReadInt( fileStream, it->Size );
	}

	index.EndOfIndexFilePosition = fileStream.tellg();

	return true;
};