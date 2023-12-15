#include "stdafx.h"

#include <bitset>

#include "Cartridge.h"

namespace ninmuse
{
	namespace nes
	{
		Cartridge::Cartridge(const std::filesystem::path& romFilePath) noexcept
			: mRomFilePath(romFilePath)
			, mRomFile()
			, mHeader()
		{
			mRomFile.open(mRomFilePath.c_str(), std::ios_base::binary | std::ios_base::in);
			assert(mRomFile.is_open());
		}

		Cartridge::~Cartridge()
		{
			mRomFile.close();
		}

		const Cartridge::Header& Cartridge::ReadHeader() noexcept
		{
			static constexpr const size_t			HEADER_DATA_KEY_WIDTH = 32;
#define	HEADER_DATA_KEY_ALIGNMENT (std::right)
			// Read Header (16 bytes)
			{
				char headerData[Header::SIZE] = { 0, };
				mRomFile.read(headerData, Header::SIZE);
				memcpy(mHeader.Identification, &headerData[Header::ID_INDEX], Header::ID_SIZE);
				memcpy(&mHeader.PrgRomSize, &headerData[Header::PRG_ROM_SIZE_INDEX], Header::PRG_ROM_SIZE_SIZE);
				memcpy(&mHeader.ChrRomSize, &headerData[Header::CHR_ROM_SIZE_INDEX], Header::CHR_ROM_SIZE_SIZE);
				memcpy(&mHeader.Flags6.Value, &headerData[Header::FLAGS6_INDEX], Header::FLAGS6_SIZE);
				memcpy(&mHeader.Flags7.Value, &headerData[Header::FLAGS7_INDEX], Header::FLAGS7_SIZE);
				memcpy(&mHeader.Flags8.Value, &headerData[Header::FLAGS8_INDEX], Header::FLAGS8_SIZE);
				memcpy(&mHeader.Flags9.Value, &headerData[Header::FLAGS9_INDEX], Header::FLAGS9_SIZE);
				memcpy(&mHeader.Flags10.Value, &headerData[Header::FLAGS10_INDEX], Header::FLAGS10_SIZE);

				const bool isNes2_0Format = (mHeader.Flags7.Bits.NES2_0Id == Header::NES_2_0_ID);;
				// Read Identification String (4 bytes)
				{
					std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "Identification: ";
					std::cout << mHeader.Identification << std::endl;
				}
				// PRG-ROM Size
				{
					std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT;
					if (isNes2_0Format == false)
					{
						std::cout << "PRG-ROM Size: ";
					}
					else
					{
						std::cout << "PRG-ROM Size Low: ";
					}
					std::cout << static_cast<uint32_t>(mHeader.PrgRomSize.Value) << std::endl;
				}
				// CHR-ROM Size
				{
					std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT;
					if (isNes2_0Format == false)
					{
						std::cout << "CHR-ROM Size: ";
					}
					else
					{
						std::cout << "CHR-ROM Size Low: ";
					}
					std::cout << static_cast<uint32_t>(mHeader.ChrRomSize.Value) << std::endl;
				}
				// Flags6
				{
					// Mapper
					std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "Mapper D03...D00: ";
					std::cout << "0b" 
							  << static_cast<uint32_t>(mHeader.Flags6.Bits.MapperD03)
							  << static_cast<uint32_t>(mHeader.Flags6.Bits.MapperD02) 
							  << static_cast<uint32_t>(mHeader.Flags6.Bits.MapperD01) 
							  << static_cast<uint32_t>(mHeader.Flags6.Bits.MapperD00) << std::endl;
					// Four Screen Mode
					std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "Hard-Wired Four Screen Mode: ";
					std::cout << std::boolalpha << mHeader.Flags6.Bits.MirroringMode << std::endl;
					// Trainer
					std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "512-byte Trainer: ";
					std::cout << (mHeader.Flags6.Bits.Trainer ? "Present" : "Not Present") << std::endl;
					// Battery
					std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "Battery: ";
					std::cout << (mHeader.Flags6.Bits.Battery ? "Present" : "Not Present") << std::endl;
					// Mirroring Type
					std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "Mirroring Type: ";
					std::cout << (mHeader.Flags6.Bits.MirroringType ? "Vertical" : "Horizontal") << std::endl;
				}
				// Flags7
				{
					// Mapper
					std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "Mapper D07...D04: ";
					std::cout << "0b"
						<< mHeader.Flags7.Bits.MapperD07
						<< mHeader.Flags7.Bits.MapperD06
						<< mHeader.Flags7.Bits.MapperD05
						<< mHeader.Flags7.Bits.MapperD04 << std::endl;
					// NES 2.0 Identifier
					std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "Is NES 2.0 Format?: ";
					std::cout << std::boolalpha << isNes2_0Format << std::endl;
					// Console Type
					std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "Console Type: ";
					switch (mHeader.Flags7.Bits.ConsoleType)
					{
					case 0b00:
						std::cout << "Nintendo Entertainment System/Family Computer";
						break;
					case 0b01:
						std::cout << "Nintendo Vs. System";
						break;
					case 0b10:
						std::cout << "Nintendo Playchoice 10";
						break;
					case 0b11:
						std::cout << "Extended Console Type";
						break;
					default:
						break;
					}
					std::cout << std::endl;
				}
				// Flags8
				{
					// NES
					if (isNes2_0Format == false)
					{
						std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "PRG RAM Size: ";
						std::cout << static_cast<uint32_t>(mHeader.Flags8.NESBits.PrgRamSize) << std::endl;
					}
					// NES 2.0
					else
					{
						// Submapper number
						std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "Mapper D11...D08: ";
						std::cout << "0b"
							<< std::bitset<4>(mHeader.Flags8.NES2_0Bits.SubmapperNumber) << std::endl;

						// Mapper number
						std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "Mapper D11...D08: ";
						std::cout << "0b"
							<< mHeader.Flags8.NES2_0Bits.MapperD11
							<< mHeader.Flags8.NES2_0Bits.MapperD10
							<< mHeader.Flags8.NES2_0Bits.MapperD09
							<< mHeader.Flags8.NES2_0Bits.MapperD08 << std::endl;
					}
				}
				// Flags9
				{
					// NES
					if (isNes2_0Format == false)
					{
						std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "TV System: ";
						switch (mHeader.Flags9.NESBits.TvSystem)
						{
						case eTvSystemType::NTSC:
							std::cout << "NTSC";
							break;
						case eTvSystemType::PAL:
							std::cout << "PAL";
							break;
						default:
							assert(false);
							break;
						}
						std::cout << std::endl;
					}
					// NES 2.0
					else
					{
						// CHR-Rom Size MSB
						std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "CHR-ROM Size High: ";
						std::cout << mHeader.Flags9.NES2_0Bits.ChrRomSizeHigh << std::endl;

						// PRG-ROM Size MSB
						std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "PRG-ROM Size High: ";
						std::cout << mHeader.Flags9.NES2_0Bits.PrgRomSizeHigh << std::endl;
					}
				}
				// Flags10
				{
					// NES
					if (isNes2_0Format == false)
					{
						std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "Board has confict: ";
						std::cout << std::boolalpha << mHeader.Flags10.NESBits.HasBusConflict << std::endl;
						std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "Is PRG-RAM present: ";
						std::cout << std::boolalpha << mHeader.Flags10.NESBits.PrgRam << std::endl;
						std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "TV System: ";
						if (mHeader.Flags10.NESBits.TvSystem | static_cast<uint8_t>(eTvSystemType::NTSC))
						{
							std::cout << "NTSC ";
						}
						if (mHeader.Flags10.NESBits.TvSystem | static_cast<uint8_t>(eTvSystemType::PAL))
						{
							std::cout << "PAL";
						}
					}
					// NES 2.0
					else
					{
						std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "PRG-NVRAM/EEPROM (non-volatile) shift count: ";
						std::cout << mHeader.Flags10.NES2_0Bits.PrgNvramEepromShiftCount << std::endl;
						std::cout << std::setw(HEADER_DATA_KEY_WIDTH) << HEADER_DATA_KEY_ALIGNMENT << "PRG-RAM (volatile) shift count: ";
						std::cout << mHeader.Flags10.NES2_0Bits.PrgRamShiftCount << std::endl;
					}
				}
			}

			return mHeader;
		}
	}
}
