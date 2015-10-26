#include <iostream>
#include <string>
#include <vector>

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

void		InterpretProgramArguments		( const std::vector<std::string>& programArguments );
bool		ValidateAndOrderInstructions	( std::vector<Instruction>& instructions );
bool		ExecuteInstructions				( std::vector<Instruction>& instructions );
std::string	MakeGUIFromFilePath				( const std::string& filePath );
bool		AppendAssetsToPacket			( const std::string& packetPath, const std::vector<Asset>& assets );
bool		GeneratePacket					( const std::string& packetPath );
bool		TrimPacket						( const std::string& packetPath );
bool		ListAssets						( const std::string& packetPath );

int main(int argc, char* argv[]) {
	std::vector<std::string> programArguments;
	programArguments.reserve( argc - 1 );

	for ( int i = 1; i < argc; ++i ) {
		programArguments.push_back( argv[i] );
	}

	//for ( int i = 0; i < programArguments.size(); ++i ) {
	//	std::cout << "Argument " << i << ": \"" << programArguments[i] << "\"" << std::endl;
	//}

	InterpretProgramArguments( programArguments );

	//system("pause");	// TODOOE: Remove when starting to debug from commandline.

	return 0;
};

void InterpretProgramArguments( const std::vector<std::string>& programArguments ) {
	if ( programArguments.empty() ) {
		std::cout << "Usage: TODO write usage information." << std::endl;
		return;
	}

	std::vector<Instruction> instructions;
	InstructionType currentInstructionType = InstructionType::None;

	int currentArgumentIndex = 0;
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
	for ( int i = 0; i < instructions.size(); ++i ) {
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
		for ( int i = 0; i < instructions.size(); ++i ) {
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

	for ( int i = 0; i < instructions.size(); ++i ) {
		if ( instructions[i].Type == InstructionType::NameNextAsset || instructions[i].Type == InstructionType::AppendAsset ) {
			orderedInstructions.push_back( instructions[i] );	// TODO: Check for duplicates.
		}
	}

	auto addInstructionsOfType = [&instructions, &orderedInstructions]( InstructionType type ) {
		for ( int i = 0; i < instructions.size(); ++i ) {
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
		for ( int i = 1; i < instructions.size(); ++i ) {
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

	for ( int i = 0; i < instructions.size(); ++i ) {
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
	for ( int i = 0; i < gui.size(); ++i ) {
		if ( gui[i] == '/' || gui[i] == '\\' ) {
			gui[i] == '.';
		}
	}
	return gui;
}

bool AppendAssetsToPacket( const std::string& packetPath, const std::vector<Asset>& assets ) {
	std::cout << "Appending assets to packet \"" << packetPath << "\"..." << std::endl;
	for ( int i = 0; i < assets.size(); ++i ) {
		std::cout << "+ Asset GUI=\"" << assets[i].GUI << "\", Path=\"" << assets[i].FilePath << "\"" << std::endl;
	}

	return true;
}

bool GeneratePacket ( const std::string& packetPath ) {
	std::cout << "Generating packet \"" << packetPath << "\"..." << std::endl;

	return true;
}

bool TrimPacket ( const std::string& packetPath ) {
	std::cout << "Triming packet \"" << packetPath << "\"..." << std::endl;

	return true;
}

bool ListAssets ( const std::string& packetPath ) {
	std::cout << "Listing assets in packet \"" << packetPath << "\"..." << std::endl;

	return true;
}