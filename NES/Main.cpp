#include "stdafx.h"

#include <iostream>
#include <string>

#include "NES/Cartridge.h"
#include "NES/Nes.h"

using namespace ninmuse;
using namespace ninmuse::nes;

static constexpr const char* CARTRIDGE_FILE_NAME_KEY = "CartidgeFileName=";

int main(int argc, char* argv[])
{
	std::filesystem::path romFileName;
	for (int argumentIndex = 0; argumentIndex < argc; ++argumentIndex)
	{
		const std::string argument = argv[argumentIndex];
		if (argument.starts_with(CARTRIDGE_FILE_NAME_KEY) == true)
		{
			const size_t fileNameIndex = argument.find_first_of('=');
			romFileName = argument.substr(fileNameIndex + 1);
			break;
		}
	}

	const std::filesystem::path workingDirectory = std::filesystem::current_path();
	const std::filesystem::path romFilePath = workingDirectory / romFileName;
	std::unique_ptr<Cartridge> cartridge = std::make_unique<Cartridge>( romFilePath );

	Nes nes;
	nes.InsertCartridge( std::move( cartridge ) );
	nes.TurnOn();
	nes.TurnOff();

	return 0;
}