#include "stdafx.h"

#include "NES/Cartridge.h"
#include "NES/Cpu.hpp"
#include "NES/StaticArray.hpp"

namespace ninmuse
{
	namespace nes
	{
		// Cycle by Address Mode
		void Cpu6502::processSingleClock() noexcept
		{
			static size_t clock = 1;

			bool fetchData = true;
			bool saveData = false;

			bool needsToDecode = false;
			bool needsToExecute = false;
			bool needsToCheckBranching = false;
			bool needsToDecrementStackPointer = false;
			bool needsToIncrementStackPointer = false;

			const CycleJob& cycleJob = mCycleJobs.front();

			// address bus
			switch ( cycleJob.AddressBusType )
			{
			case eAddressBusType::PROGRAM_COUNTER:
				mAddressBus = mRegisters.ProgramCounter;
				break;
			case eAddressBusType::ADDRESS:
				mAddressBus = mExecutionInfo.Operand.Address;
				break;
			case eAddressBusType::STACK_POINTER:
				mAddressBus = CreateAddress( mRegisters.StackPointer, STACK_PAGE_ADDRESS_HI );
				break;
			default:
				break;
			}

			// decode first
			switch ( cycleJob.InternalOperation )
			{
			case eInternalMode::DECODE:
				needsToDecode = true;
				break;
			case eInternalMode::EXECUTE:
				needsToExecute = true;
				break;
			case eInternalMode::SET_ADDRESS_BUS_ABSOLUTE_MODE:
				break;
			case eInternalMode::SET_PROGRAM_COUNTER:
				mRegisters.ProgramCounter = mExecutionInfo.Operand.Address;
				break;
			case eInternalMode::CHECK_STATUS:
				needsToCheckBranching = true;
				break;
			case eInternalMode::DECREASE_STACK_POINTER:
				--mRegisters.StackPointer;
				needsToDecrementStackPointer = true;
				break;
			case eInternalMode::INCREASE_STACK_POINTER:
				++mRegisters.StackPointer;
				needsToIncrementStackPointer = true;
				break;
			case eInternalMode::PREVIOUS:
				NM_ASSERT( false, "Unimplemented internal mode" );
				break;
			case eInternalMode::NONE:
				break;
			default:
				NM_ASSERT( false, "Invalid internal mode" );
				break;
			}

			bool skipFetch = false;
			if ( needsToDecode )
			{
				decode( skipFetch );
			}

			if ( needsToExecute )
			{
				execute();
			}

			if ( needsToCheckBranching )
			{
				bool isBranching = false;
				switch ( mExecutionInfo.InstructionInfoOrNull->Mnemonic )
				{
				case eMnemonic::BCC:
					NM_ASSERT( false, "Unimplemented mnemonic!!" );
					break;
				case eMnemonic::BCS:
					NM_ASSERT( false, "Unimplemented mnemonic!!" );
					break;
				case eMnemonic::BEQ:
					isBranching = mRegisters.Status.StatusBits.ZeroFlag;
					break;
				case eMnemonic::BIT:
					NM_ASSERT( false, "Unimplemented mnemonic!!" );
					break;
				case eMnemonic::BMI:
					NM_ASSERT( false, "Unimplemented mnemonic!!" );
					break;
				case eMnemonic::BNE:
					NM_ASSERT( false, "Unimplemented mnemonic!!" );
					break;
				case eMnemonic::BPL:
					NM_ASSERT( false, "Unimplemented mnemonic!!" );
					break;
				case eMnemonic::BVC:
					NM_ASSERT( false, "Unimplemented mnemonic!!" );
					break;
				case eMnemonic::BVS:
					NM_ASSERT( false, "Unimplemented mnemonic!!" );
					break;
				default:
					NM_ASSERT( false, "Invalid mnemonic!!" );
					break;
				}

				if ( isBranching == true )
				{
					NM_ASSERT( false, "Unimplemented case!!" );
					const data_t lowAddress = GetAddressLow( mRegisters.ProgramCounter ) + mExecutionInfo.Operand.Data.Value;
					if ( lowAddress < mExecutionInfo.Operand.Data.Value )
					{
						__debugbreak();
					}
					else
					{
						skipFetch = true;
					}
				}
			}

			// external operation
			switch ( cycleJob.ExternalOperation )
			{
			case eExternalMode::FETCH_OPCODE:
			{
				fetchData = true;
				mDataBus = ReadRom( mAddressBus );
				if ( skipFetch == false )
				{
					mDataToDecode = mDataBus;
					CycleJob nextCycleJob =
					{
						.AddressBusType = eAddressBusType::PROGRAM_COUNTER,
						.IncrementProgramCounter = true,
						.ExternalOperation = eExternalMode::FETCH_OPCODE,
						.InternalOperation = eInternalMode::DECODE
					};
					mCycleJobs.push( nextCycleJob );
				}
			}
			break;
			case eExternalMode::FETCH_DATA_FROM_ROM:
			{
				fetchData = true;
				mDataBus = ReadRom( mAddressBus );
				if ( getRequiredOperandNumBytes( mExecutionInfo.InstructionInfoOrNull->AddressMode ) > 0 )
				{
					mExecutionInfo.Operand.Data.Value = mDataBus;
				}
			}
			break;
			case eExternalMode::FETCH_DATA_FROM_RAM:
			{
				fetchData = true;
				mDataBus = Read( mAddressBus );
			}
			break;
			case eExternalMode::FETCH_LOW_ADDRESS_FROM_ROM:
			{
				fetchData = true;
				mDataBus = ReadRom( mAddressBus );
				mExecutionInfo.Operand.Bytes[0] = mDataBus;
			}
			break;
			case eExternalMode::FETCH_LOW_ADDRESS_FROM_RAM:
			{
				fetchData = true;
				mDataBus = Read( mAddressBus );
				mExecutionInfo.Operand.Bytes[0] = mDataBus;
			}
			break;
			case eExternalMode::FETCH_HIGH_ADDRESS_FROM_ROM:
			{
				fetchData = true;
				mDataBus = ReadRom( mAddressBus );
				mExecutionInfo.Operand.Bytes[1] = mDataBus;
			}
			break;
			case eExternalMode::FETCH_HIGH_ADDRESS_FROM_RAM:
			{
				fetchData = true;
				mDataBus = Read( mAddressBus );
				mExecutionInfo.Operand.Bytes[1] = mDataBus;
			}
			break;
			case eExternalMode::JRS_NONE:
				NM_ASSERT( false, "Unimplemented external mode" );
				break;
			case eExternalMode::SAVE_PROGRAM_COUNTER_HIGH_TO_RAM:
			{
				fetchData = false;
				saveData = true;
				mDataBus = GetAddressHigh( mRegisters.ProgramCounter );
				Write( mAddressBus, mDataBus );
			}
			break;
			case eExternalMode::SAVE_PROGRAM_COUNTER_LOW_TO_RAM:
			{
				fetchData = false;
				saveData = true;
				mDataBus = GetAddressLow( mRegisters.ProgramCounter );
				Write( mAddressBus, mDataBus );
			}
			break;
			case eExternalMode::RTS_NONE:
				NM_ASSERT( false, "Unimplemented external mode" );
				break;
			case eExternalMode::NEXT:
				[[fallthrough]];
			default:
				NM_ASSERT( false, "Invalid external mode" );
				break;
			}

			if ( cycleJob.IncrementProgramCounter )
			{
				++mRegisters.ProgramCounter;
			}

			std::cout << std::setw( 8 ) << std::left << clock;
			std::cout << std::setw( 16 ) << std::left << std::hex << mAddressBus;
			std::cout << std::setw( 16 ) << std::left << std::boolalpha << cycleJob.IncrementProgramCounter;
			std::cout << std::setw( 16 ) << std::left << std::hex << static_cast< uint32_t >( mDataBus );
			if ( fetchData )
			{
				std::cout << "Fetch " << std::setw( 18 ) << std::left << static_cast< uint32_t >( mDataBus );
			}
			else if ( saveData )
			{
				std::cout << "Save " << std::setw( 19 ) << std::left << static_cast< uint32_t >( mDataBus );
			}
			if ( needsToDecode )
			{
				std::cout << "Decoding " << std::hex << static_cast< uint32_t >( mDataToDecode );
			}
			else if ( needsToExecute )
			{
				std::cout << "Executing " << mExecutionInfo.InstructionInfoOrNull->Name << " " << convertAddressModeToString( mExecutionInfo.InstructionInfoOrNull->AddressMode );
			}
			else if ( needsToDecrementStackPointer )
			{
				std::cout << "Decrementing stack pointer";
			}
			else if ( needsToIncrementStackPointer )
			{
				std::cout << "Incrementing stack pointer";
			}
			std::cout << std::endl;

			mCycleJobs.pop();
#if 0
			address_t		nextAddressBus = mAddressBus;
			eInternalMode	nextInternalMode = mCurrentInternalMode;
			eExternalMode	nextExternalMode = mCurrentExternalMode;
			bool			nextReadRam = false;

			// Fetch
			mDataBus = mReadRam ? Read( mAddressBus ) : ReadRom( mAddressBus );
			bool needsToIncrementProgramCounter = false;
			bool setProgramCounterToAddressBus = true;
			bool setStackPointerToAddressBus = false;

			// should I decode something?
			bool skipFetch = false;
			const data_t decodedData = mDataToDecode;
			switch ( mCurrentExternalMode )
			{
			case eExternalMode::DECODE:
			{
				// read instruction
				const data_t opcode = mDataToDecode;
				mExecutionInfo.InstructionInfoOrNull = INSTRUCTION_TABLE[opcode];
				NM_ASSERT( mExecutionInfo.InstructionInfoOrNull != nullptr, "Invalid opcode!!" );

				switch ( mExecutionInfo.InstructionInfoOrNull->AddressMode )
				{
				case eAddressMode::ACCUMULATOR:
					skipFetch = true;
					mCurrentInternalMode = eInternalMode::NEXT;
					nextInternalMode = eInternalMode::FETCH_OPCODE;
					nextExternalMode = eExternalMode::EXECUTE;
					break;
				case eAddressMode::IMMEDIATE:
					mCurrentInternalMode = eInternalMode::FETCH_DATA_FROM_ROM;
					nextExternalMode = eExternalMode::EXECUTE;
					mExecutionInfo.Operand.Data.Value = mDataBus;
					break;
				case eAddressMode::ABSOLUTE:
					if ( mExecutionInfo.InstructionInfoOrNull != &Instruction::Jsr::ABSOLUTE )
					{
						mCurrentInternalMode = eInternalMode::FETCH_LOW_ADDRESS_FROM_ROM;
						nextInternalMode = eInternalMode::FETCH_HIGH_ADDRESS_FROM_ROM;
						nextExternalMode = eExternalMode::NONE;
					}
					else
					{
						mCurrentInternalMode = eInternalMode::FETCH_LOW_ADDRESS_FROM_ROM;
						nextInternalMode = eInternalMode::JRS_NONE;
						nextExternalMode = eExternalMode::NONE;
						setProgramCounterToAddressBus = false;
						setStackPointerToAddressBus = true;
						nextAddressBus = CreateAddress( mRegisters.StackPointer, STACK_PAGE_ADDRESS_HI );
					}
					break;
				case eAddressMode::ZERO_PAGE:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::IMPLIED:
					if ( mExecutionInfo.InstructionInfoOrNull->Mnemonic == eMnemonic::RTS )
					{
						skipFetch = true;
						needsToIncrementProgramCounter = true;
						setProgramCounterToAddressBus = false;
						setStackPointerToAddressBus = true;
						nextExternalMode = eExternalMode::INCREASE_STACK_POINTER;
					}
					else
					{
						skipFetch = true;
						nextExternalMode = eExternalMode::EXECUTE;
					}
					break;
				case eAddressMode::RELATIVE:
					mCurrentInternalMode = eInternalMode::FETCH_DATA_FROM_ROM;
					nextExternalMode = eExternalMode::EXECUTE;
					break;
				case eAddressMode::ABSOLUTE_INDIRECT:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::ABSOLUTE_INDEXED_WITH_X:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::ABSOLUTE_INDEXED_WITH_Y:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::ZERO_PAGE_INDEXED_WITH_X:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::ZERO_PAGE_INDEXED_WITH_Y:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::ZERO_PAGE_INDEXED_INDIRECT:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::COUNT:
					[[fallthrough]];
				default:
					NM_ASSERT( false, "Invalid address mode!!" );
					break;
				}
			}
			break;
			case eExternalMode::EXECUTE:
				execute();
				break;
			case eExternalMode::DECREASE_STACK_POINTER:
				--mRegisters.StackPointer;
				if ( mCurrentInternalMode == eInternalMode::JRS_SAVE_ADDRESS_HIGH_TO_STACK )
				{
					nextInternalMode = eInternalMode::JRS_SAVE_ADDRESS_LOW_TO_STACK;
					nextExternalMode = eExternalMode::DECREASE_STACK_POINTER;
				}
				else if ( mCurrentInternalMode == eInternalMode::JRS_SAVE_ADDRESS_LOW_TO_STACK )
				{
					nextInternalMode = eInternalMode::FETCH_HIGH_ADDRESS_FROM_ROM;
					nextExternalMode = eExternalMode::NONE;
				}
				break;
			case eExternalMode::INCREASE_STACK_POINTER:
				++mRegisters.StackPointer;
				if ( mCurrentInternalMode == eInternalMode::FETCH_LOW_ADDRESS_FROM_RAM )
				{
					needsToIncrementProgramCounter = false;
					setProgramCounterToAddressBus = false;
					setStackPointerToAddressBus = true;
					nextInternalMode = eInternalMode::FETCH_HIGH_ADDRESS_FROM_RAM;
					nextExternalMode = eExternalMode::NONE;
				}
				else
				{
					skipFetch = true;
					needsToIncrementProgramCounter = false;
					setProgramCounterToAddressBus = false;
					setStackPointerToAddressBus = true;
					nextInternalMode = eInternalMode::FETCH_LOW_ADDRESS_FROM_RAM;
					nextExternalMode = eExternalMode::INCREASE_STACK_POINTER;
				}
				break;
			case eExternalMode::PREVIOUS:
				break;
			case eExternalMode::NONE:
			{
				switch ( mExecutionInfo.InstructionInfoOrNull->AddressMode )
				{
				case eAddressMode::ACCUMULATOR:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::IMMEDIATE:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::ABSOLUTE:
					if ( mCurrentInternalMode == eInternalMode::FETCH_HIGH_ADDRESS_FROM_ROM )
					{
						if ( mExecutionInfo.InstructionInfoOrNull->Mnemonic == eMnemonic::JSR )
						{
							nextInternalMode = eInternalMode::FETCH_OPCODE;
							nextExternalMode = eExternalMode::EXECUTE;
						}
						else
						{
							nextInternalMode = eInternalMode::FETCH_DATA_FROM_RAM;
							nextExternalMode = eExternalMode::NONE;
							nextReadRam = true;
						}
					}
					else if ( mCurrentInternalMode == eInternalMode::FETCH_DATA_FROM_RAM )
					{
						nextInternalMode = eInternalMode::FETCH_OPCODE;
						nextExternalMode = eExternalMode::EXECUTE;
					}
					else if ( mCurrentInternalMode == eInternalMode::JRS_NONE )
					{
						nextInternalMode = eInternalMode::JRS_SAVE_ADDRESS_HIGH_TO_STACK;
						nextExternalMode = eExternalMode::DECREASE_STACK_POINTER;
					}
					break;
				case eAddressMode::ZERO_PAGE:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::IMPLIED:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::RELATIVE:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::ABSOLUTE_INDIRECT:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::ABSOLUTE_INDEXED_WITH_X:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::ABSOLUTE_INDEXED_WITH_Y:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::ZERO_PAGE_INDEXED_WITH_X:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::ZERO_PAGE_INDEXED_WITH_Y:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::ZERO_PAGE_INDEXED_INDIRECT:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::COUNT:
					[[fallthrough]];
				default:
					NM_ASSERT( false, "Invalid address mode!!" );
					break;
				}
			}
			break;
			default:
				NM_ASSERT( false, "Invalid external mode!!" );
				break;
			}

			// is this opcode? data? address lo/hi?
			if ( skipFetch == false )
			{
				needsToIncrementProgramCounter = nextInternalMode != eInternalMode::FETCH_DATA_FROM_RAM;
				switch ( mCurrentInternalMode )
				{
				case eInternalMode::FETCH_OPCODE:
					mDataToDecode = mDataBus;
					nextExternalMode = eExternalMode::DECODE;
					break;
				case eInternalMode::FETCH_DATA_FROM_ROM:
					mExecutionInfo.Operand.Data.Value = mDataBus;
					break;
				case eInternalMode::FETCH_DATA_FROM_RAM:
					if ( mExecutionInfo.InstructionInfoOrNull->Mnemonic == eMnemonic::STA
						|| mExecutionInfo.InstructionInfoOrNull->Mnemonic == eMnemonic::LDA )
					{
						nextInternalMode = eInternalMode::FETCH_OPCODE;
					}
					else
					{
						NM_ASSERT( false, "Unimplemented internal mode!!" );
					}
					break;
				case eInternalMode::FETCH_LOW_ADDRESS_FROM_ROM:
					mExecutionInfo.Operand.Bytes[0] = mDataBus;
					if ( mExecutionInfo.InstructionInfoOrNull->Mnemonic == eMnemonic::JSR )
					{
						mDataToDecode = mDataBus;
					}
					break;
				case eInternalMode::FETCH_LOW_ADDRESS_FROM_RAM:
					mExecutionInfo.Operand.Bytes[0] = mDataBus;
					if ( mExecutionInfo.InstructionInfoOrNull->Mnemonic == eMnemonic::RTS )
					{
						needsToIncrementProgramCounter = false;
					}
					break;
				case eInternalMode::FETCH_HIGH_ADDRESS_FROM_ROM:
					mExecutionInfo.Operand.Bytes[1] = mDataBus;
					if ( mExecutionInfo.InstructionInfoOrNull->Mnemonic == eMnemonic::JSR )
					{
						mRegisters.ProgramCounter = CreateAddress( mDataToDecode, mDataBus );
						needsToIncrementProgramCounter = false;
						setProgramCounterToAddressBus = true;
						setStackPointerToAddressBus = false;
					}
					break;
				case eInternalMode::FETCH_HIGH_ADDRESS_FROM_RAM:
					NM_ASSERT( false, "Unimplemented internal mode!!" );
					break;
				case eInternalMode::JRS_NONE:
					needsToIncrementProgramCounter = false;
					setProgramCounterToAddressBus = false;
					setStackPointerToAddressBus = true;
					break;
				case eInternalMode::JRS_SAVE_ADDRESS_HIGH_TO_STACK:
					needsToIncrementProgramCounter = false;
					setProgramCounterToAddressBus = false;
					setStackPointerToAddressBus = true;
					mDataBus = GetAddressHigh( mRegisters.ProgramCounter );
					Write( CreateAddress( mRegisters.StackPointer, STACK_PAGE_ADDRESS_HI ), mDataBus );
					break;
				case eInternalMode::JRS_SAVE_ADDRESS_LOW_TO_STACK:
					needsToIncrementProgramCounter = false;
					setProgramCounterToAddressBus = true;
					setStackPointerToAddressBus = false;
					mDataBus = GetAddressLow( mRegisters.ProgramCounter );
					Write( CreateAddress( mRegisters.StackPointer, STACK_PAGE_ADDRESS_HI ), mDataBus );
					break;
				case eInternalMode::RTS_NONE:
					needsToIncrementProgramCounter = false;
					setProgramCounterToAddressBus = false;
					setStackPointerToAddressBus = true;
					break;
				case eInternalMode::NEXT:
					break;
				default:
					NM_ASSERT( false, "Invalid internal mode!!" );
					break;
				}

				//switch ( mCurrentReadMode )
				//{
				//case eReadMode::FETCH_OPCODE:
				//	mDataToDecode = mDataBus;
				//	nextExternalMode = eInternalMode::DECODE;
				//	break;
				//case eReadMode::DATA:
				//	break;
				//case eReadMode::ADDRESS_LO:
				//	break;
				//case eReadMode::ADDRESS_HI:
				//	break;
				//case eReadMode::COUNT:
				//	[[fallthrough]];
				//default:
				//	NM_ASSERT( false, "Invalid read mode" );
				//	break;
				//}
			}


			if ( needsToIncrementProgramCounter )
			{
				++mRegisters.ProgramCounter;
			}
			if ( nextInternalMode == eInternalMode::FETCH_DATA_FROM_RAM )
			{
				setProgramCounterToAddressBus = false;
				setStackPointerToAddressBus = false;
				nextAddressBus = mExecutionInfo.Operand.Address;
				nextReadRam = true;
			}

			if ( setProgramCounterToAddressBus )
			{
				nextAddressBus = mRegisters.ProgramCounter;
			}
			else if ( setStackPointerToAddressBus )
			{
				nextReadRam = true;
				nextAddressBus = CreateAddress( mRegisters.StackPointer, STACK_PAGE_ADDRESS_HI );
			}

			std::cout << std::setw( 8 ) << std::left << clock;
			std::cout << std::setw( 16 ) << std::left << std::hex << mAddressBus;
			std::cout << std::setw( 16 ) << std::left << std::boolalpha << needsToIncrementProgramCounter;
			std::cout << std::setw( 16 ) << std::left << std::hex << static_cast< uint32_t >( mDataBus );
			if ( mCurrentInternalMode < eInternalMode::FETCH_HIGH_ADDRESS_FROM_RAM )
			{
				std::cout << "Fetch " << std::setw( 18 ) << std::left << static_cast< uint32_t >( mDataBus );
			}
			else if ( mCurrentInternalMode == eInternalMode::JRS_SAVE_ADDRESS_HIGH_TO_STACK ||
				mCurrentInternalMode == eInternalMode::JRS_SAVE_ADDRESS_LOW_TO_STACK )
			{
				std::cout << "Save " << std::setw( 18 ) << std::left << static_cast< uint32_t >( mDataBus );
			}
			if ( mCurrentExternalMode == eExternalMode::DECODE )
			{
				std::cout << "Decoding " << std::hex << static_cast< uint32_t >( decodedData );
			}
			else if ( mCurrentExternalMode == eExternalMode::EXECUTE )
			{
				std::cout << "Executing " << mExecutionInfo.InstructionInfoOrNull->Name;
			}
			std::cout << std::endl;

			mAddressBus = nextAddressBus;
			mCurrentInternalMode = nextInternalMode;
			mCurrentExternalMode = nextExternalMode;
			mReadRam = nextReadRam;
#endif
			/*
			bool needsExecuting = mExecutionInfo.IsReady == true;
			const bool needsDecoding = mDataToDecodeOrNull != nullptr && mCurrentReadMode == eReadMode::FETCH_OPCODE && needsExecuting == false;

			// Clock
			std::cout << std::setw( 8 ) << std::left << clock;

			// Fetch
			if ( mReadRam == false )
			{
				mAddressBus = mRegisters.ProgramCounter;
			}
			const data_t& data = mReadRam ? Read( mAddressBus ) : ReadRom( mAddressBus );
			const bool readFromRam = mReadRam;
			mReadRam = false;

			std::cout << std::setw( 16 ) << std::left << std::hex << mAddressBus;

			// Decode
			if ( needsDecoding )
			{
				// read instruction
				const data_t opcode = mDataBus;
				mExecutionInfo.InstructionInfoOrNull = INSTRUCTION_TABLE[opcode];
				NM_ASSERT( mExecutionInfo.InstructionInfoOrNull != nullptr, "Invalid opcode!!" );

				mDecodeCounter = 0;

				if ( getRequiredOperandNumBytes( mExecutionInfo.InstructionInfoOrNull->AddressMode ) > 0 )
				{
					mCurrentReadMode = eReadMode::DATA;
				}

				if ( mExecutionInfo.InstructionInfoOrNull == &Instruction::Jsr::ABSOLUTE )
				{
					mReadRam = true;
					mExecutionInfo.Operand.Address = CreateAddress( data, 0x01 );
				}

				mDataToDecodeOrNull = nullptr;
			}
			else if ( mCurrentReadMode == eReadMode::DATA )
			{
				// read operand
				const data_t operand = mDataBus;
				mExecutionInfo.Operand.Bytes[0] = operand;
				if ( getRequiredOperandNumBytes( mExecutionInfo.InstructionInfoOrNull->AddressMode ) > 1 )
				{
					mExecutionInfo.Operand.Bytes[1] = data;
				}
				++mDecodeCounter;
			}

			if ( mExecutionInfo.InstructionInfoOrNull != nullptr && mDecodeCounter == getRequiredOperandNumBytes( mExecutionInfo.InstructionInfoOrNull->AddressMode ) - 1)
			{
				switch ( mExecutionInfo.InstructionInfoOrNull->AddressMode )
				{
				case eAddressMode::ACCUMULATOR:
					break;
				case eAddressMode::IMMEDIATE:
					break;
				case eAddressMode::IMPLIED:
					break;
				case eAddressMode::ABSOLUTE:
					[[fallthrough]];
				case eAddressMode::ZERO_PAGE:
					[[fallthrough]];
				case eAddressMode::RELATIVE:
					[[fallthrough]];
				case eAddressMode::ABSOLUTE_INDIRECT:
					[[fallthrough]];
				case eAddressMode::ABSOLUTE_INDEXED_WITH_X:
					[[fallthrough]];
				case eAddressMode::ABSOLUTE_INDEXED_WITH_Y:
					[[fallthrough]];
				case eAddressMode::ZERO_PAGE_INDEXED_WITH_X:
					[[fallthrough]];
				case eAddressMode::ZERO_PAGE_INDEXED_WITH_Y:
					[[fallthrough]];
				case eAddressMode::ZERO_PAGE_INDEXED_INDIRECT:
					[[fallthrough]];
				case eAddressMode::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y:
					switch ( mExecutionInfo.InstructionInfoOrNull->Mnemonic )
					{
					case eMnemonic::JSR:
						break;
					case eMnemonic::STA:
						[[fallthrough]];
					case eMnemonic::STX:
						[[fallthrough]];
					case eMnemonic::STY:
						mReadRam = true;
						break;
					case eMnemonic::COUNT:
					default:
						NM_ASSERT( false, "Invalid mnemonic" );
						break;
					}
					break;
				case eAddressMode::COUNT:
					[[fallthrough]];
				default:
					break;
				}
				mExecutionInfo.IsReady = true;
				mCurrentReadMode = eReadMode::FETCH_OPCODE;
				needsExecuting = needsDecoding == false;
			}

			const bool increaseProgramCounter = readFromRam == false && ( needsDecoding == false || ( mExecutionInfo.InstructionInfoOrNull != nullptr && mDecodeCounter == 0 ) || needsExecuting == true );
			if ( increaseProgramCounter )
				++mRegisters.ProgramCounter;

			std::cout << std::setw( 16 ) << std::left << std::boolalpha << increaseProgramCounter;
			std::cout << std::setw( 16 ) << std::left << std::hex << static_cast<uint32_t>( data );
			std::cout << "Fetch " << std::setw( 18 ) << std::left << static_cast< uint32_t >( data );
			if ( needsDecoding )
			{
				std::cout << "Decoding " << std::hex << static_cast<uint32_t>( mDataBus );
			}
			else if ( needsExecuting )
			{
				std::cout << "Executing " << mExecutionInfo.InstructionInfoOrNull->Name;
			}
			std::cout << std::endl;

			// Execute
			if ( needsExecuting )
			{
				const bool isExecutionComplete = execute();
				if ( isExecutionComplete )
				{
					mExecutionInfo.Reset();
				}
			}

			if ( mDataToDecodeOrNull == nullptr )
			{
				mDataToDecodeOrNull = &data;
			}
			mDataBus = data;
			*/
			++clock;
		}

		data_t Cpu6502::ReadRom( const address_t& address ) const noexcept
		{
			const DynamicArray<data_t>& programRomData = mRomOrNull->GetProgramRom().Data;
			NM_ASSERT( address < programRomData.GetSize(), "Invalid address!!");
			const data_t data = programRomData[address];
			return data;
		}

		void Cpu6502::Run() noexcept
		{
			const Cartridge::ProgramRom& programRom = mRomOrNull->GetProgramRom();
			const data_t addressLow = programRom.Data[0xFFFC];
			const data_t addressHigh = programRom.Data[0xFFFD];

			mRegisters.ProgramCounter = CreateAddress( addressLow, addressHigh );
			mAddressBus = mRegisters.ProgramCounter;

			char buffer[64] = { 0, };
			const data_t* mem = &mRomOrNull->GetProgramRom().Data.GetData()[mAddressBus];
			//const size_t disassembleCount = 256;
			const size_t disassembleCount = 0;
			address_t address = mAddressBus;
			for ( size_t i = 0; i < disassembleCount; ++i )
			{
				const data_t* prevMem = mem;
				mem = disassemble( buffer, mem );
				const size_t diff = mem - prevMem;
				std::cout << std::hex << address << " " << buffer << '\n';
				address += static_cast<address_t>( diff );
			}

			mCycleJobs.push( CycleJob{ .AddressBusType = eAddressBusType::PROGRAM_COUNTER, .IncrementProgramCounter = true, .ExternalOperation = eExternalMode::FETCH_OPCODE } );

			std::cout << std::setw( 8 ) << std::left << "Clocks";
			std::cout << std::setw( 16 ) << std::left << "Program Counter";
			std::cout << std::setw( 16 ) << std::left << "Increase PC?";
			std::cout << std::setw( 16 ) << std::left << "Data Bus";
			std::cout << std::setw( 24 ) << std::left << "External Operation";
			std::cout << std::setw( 24 ) << std::left << "Internal Operation";
			std::cout << std::endl;
			while ( true )
			{
				processSingleClock();
			}
		}

		constexpr const char* Cpu6502::convertAddressModeToString( const eAddressMode addressMode ) noexcept
		{
			switch ( addressMode )
			{
			case eAddressMode::ACCUMULATOR:
				return "A";
			case eAddressMode::IMMEDIATE:
				return "#";
			case eAddressMode::ABSOLUTE:
				return "a";
			case eAddressMode::ZERO_PAGE:
				return "zp";
			case eAddressMode::IMPLIED:
				return "i";
			case eAddressMode::RELATIVE:
				return "r";
			case eAddressMode::ABSOLUTE_INDIRECT:
				return "(a)";
			case eAddressMode::ABSOLUTE_INDEXED_WITH_X:
				return "a,x";
			case eAddressMode::ABSOLUTE_INDEXED_WITH_Y:
				return "a,y";
			case eAddressMode::ZERO_PAGE_INDEXED_WITH_X:
				return "zp,x";
			case eAddressMode::ZERO_PAGE_INDEXED_WITH_Y:
				return "zp,y";
			case eAddressMode::ZERO_PAGE_INDEXED_INDIRECT:
				return "(zp,x)";
			case eAddressMode::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y:
				return "(zp),y";
			case eAddressMode::COUNT:
				[[fallthrough]];
			default:
				assert( false );
				break;
			}

			return nullptr;
		}

		constexpr size_t Cpu6502::getRequiredOperandNumBytes( const eAddressMode addressMode ) noexcept
		{
			switch ( addressMode )
			{
			case eAddressMode::ACCUMULATOR:
				return 0;
			case eAddressMode::IMMEDIATE:
				return 1;
			case eAddressMode::ABSOLUTE:
				return 2;
			case eAddressMode::ZERO_PAGE:
				return 1;
			case eAddressMode::IMPLIED:
				return 0;
			case eAddressMode::RELATIVE:
				return 1;
			case eAddressMode::ABSOLUTE_INDIRECT:
				return 2;
			case eAddressMode::ABSOLUTE_INDEXED_WITH_X:
				return 2;
			case eAddressMode::ABSOLUTE_INDEXED_WITH_Y:
				return 2;
			case eAddressMode::ZERO_PAGE_INDEXED_WITH_X:
				return 1;
			case eAddressMode::ZERO_PAGE_INDEXED_WITH_Y:
				return 1;
			case eAddressMode::ZERO_PAGE_INDEXED_INDIRECT:
				return 1;
			case eAddressMode::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y:
				return 1;
			case eAddressMode::COUNT:
				[[fallthrough]];
			default:
				assert( false );
				break;
			}

			return 0;
		}

		void Cpu6502::decode( bool& inoutSkipFetch ) noexcept
		{
			const data_t opcode = mDataToDecode;
			mExecutionInfo.InstructionInfoOrNull = INSTRUCTION_TABLE[opcode];
			NM_ASSERT( mExecutionInfo.InstructionInfoOrNull != nullptr, "Invalid opcode!!" );

			switch ( mExecutionInfo.InstructionInfoOrNull->AddressMode )
			{
			case eAddressMode::ACCUMULATOR:
			{
				inoutSkipFetch = true;

				CycleJob& currentCycleJob = mCycleJobs.front();
				currentCycleJob.IncrementProgramCounter = false;

				mCycleJobs.push( CycleJob{ .AddressBusType = eAddressBusType::PROGRAM_COUNTER,
												.IncrementProgramCounter = true,
												.ExternalOperation = eExternalMode::FETCH_OPCODE,
												.InternalOperation = eInternalMode::EXECUTE } );
			}
			break;
			case eAddressMode::IMMEDIATE:
			{
				CycleJob& currentCycleJob = mCycleJobs.front();
				currentCycleJob.IncrementProgramCounter = true;
				currentCycleJob.ExternalOperation = eExternalMode::FETCH_DATA_FROM_ROM;

				mCycleJobs.push( CycleJob{ .AddressBusType = eAddressBusType::PROGRAM_COUNTER,
												.IncrementProgramCounter = true,
												.ExternalOperation = eExternalMode::FETCH_OPCODE,
												.InternalOperation = eInternalMode::EXECUTE } );
			}
			break;
			case eAddressMode::ABSOLUTE:
			{
				CycleJob& currentCycleJob = mCycleJobs.front();
				currentCycleJob.IncrementProgramCounter = true;
				currentCycleJob.ExternalOperation = eExternalMode::FETCH_LOW_ADDRESS_FROM_ROM;

				if ( mExecutionInfo.InstructionInfoOrNull->Mnemonic == eMnemonic::JSR )
				{
					mCycleJobs.push( CycleJob{ .AddressBusType = eAddressBusType::STACK_POINTER,
													.IncrementProgramCounter = false,
													.ExternalOperation = eExternalMode::FETCH_DATA_FROM_RAM,
													.InternalOperation = eInternalMode::SET_ADDRESS_BUS_ABSOLUTE_MODE } );
					mCycleJobs.push( CycleJob{ .AddressBusType = eAddressBusType::STACK_POINTER,
													.IncrementProgramCounter = false,
													.ExternalOperation = eExternalMode::SAVE_PROGRAM_COUNTER_HIGH_TO_RAM,
													.InternalOperation = eInternalMode::DECREASE_STACK_POINTER } );
					mCycleJobs.push( CycleJob{ .AddressBusType = eAddressBusType::STACK_POINTER,
													.IncrementProgramCounter = false,
													.ExternalOperation = eExternalMode::SAVE_PROGRAM_COUNTER_LOW_TO_RAM,
													.InternalOperation = eInternalMode::DECREASE_STACK_POINTER } );
					mCycleJobs.push( CycleJob{ .AddressBusType = eAddressBusType::PROGRAM_COUNTER,
													.IncrementProgramCounter = false,
													.ExternalOperation = eExternalMode::FETCH_HIGH_ADDRESS_FROM_ROM,
													.InternalOperation = eInternalMode::SET_ADDRESS_BUS_ABSOLUTE_MODE } );
					mCycleJobs.push( CycleJob{ .AddressBusType = eAddressBusType::ADDRESS,
													.IncrementProgramCounter = true,
													.ExternalOperation = eExternalMode::FETCH_OPCODE,
													.InternalOperation = eInternalMode::EXECUTE } );
				}
				else
				{
					mCycleJobs.push( CycleJob{ .AddressBusType = eAddressBusType::PROGRAM_COUNTER,
													.IncrementProgramCounter = true,
													.ExternalOperation = eExternalMode::FETCH_HIGH_ADDRESS_FROM_ROM,
													.InternalOperation = eInternalMode::SET_ADDRESS_BUS_ABSOLUTE_MODE } );
					mCycleJobs.push( CycleJob{ .AddressBusType = eAddressBusType::ADDRESS,
													.IncrementProgramCounter = false,
													.ExternalOperation = eExternalMode::FETCH_DATA_FROM_RAM,
													.InternalOperation = eInternalMode::NONE } );
					mCycleJobs.push( CycleJob{ .AddressBusType = eAddressBusType::PROGRAM_COUNTER,
													.IncrementProgramCounter = true,
													.ExternalOperation = eExternalMode::FETCH_OPCODE,
													.InternalOperation = eInternalMode::EXECUTE } );
				}
			}
			break;
			case eAddressMode::ZERO_PAGE:
				NM_ASSERT( false, "Unimplemented address mode!!" );
				break;
			case eAddressMode::IMPLIED:
			{
				inoutSkipFetch = true;
				CycleJob& currentCycleJob = mCycleJobs.front();
				currentCycleJob.IncrementProgramCounter = false;

				if ( mExecutionInfo.InstructionInfoOrNull->Mnemonic == eMnemonic::RTS )
				{
					mCycleJobs.push( CycleJob{ .AddressBusType = eAddressBusType::STACK_POINTER,
													.IncrementProgramCounter = false,
													.ExternalOperation = eExternalMode::FETCH_DATA_FROM_RAM,
													.InternalOperation = eInternalMode::INCREASE_STACK_POINTER } );
					mCycleJobs.push( CycleJob{ .AddressBusType = eAddressBusType::STACK_POINTER,
													.IncrementProgramCounter = false,
													.ExternalOperation = eExternalMode::FETCH_LOW_ADDRESS_FROM_RAM,
													.InternalOperation = eInternalMode::INCREASE_STACK_POINTER } );
					mCycleJobs.push( CycleJob{ .AddressBusType = eAddressBusType::STACK_POINTER,
													.IncrementProgramCounter = false,
													.ExternalOperation = eExternalMode::FETCH_HIGH_ADDRESS_FROM_RAM,
													.InternalOperation = eInternalMode::SET_ADDRESS_BUS_ABSOLUTE_MODE } );
					mCycleJobs.push( CycleJob{ .AddressBusType = eAddressBusType::ADDRESS,
													.IncrementProgramCounter = true,
													.ExternalOperation = eExternalMode::FETCH_DATA_FROM_ROM,
													.InternalOperation = eInternalMode::SET_PROGRAM_COUNTER } );
					mCycleJobs.push( CycleJob{ .AddressBusType = eAddressBusType::PROGRAM_COUNTER,
													.IncrementProgramCounter = true,
													.ExternalOperation = eExternalMode::FETCH_OPCODE,
													.InternalOperation = eInternalMode::EXECUTE } );
				}
				else
				{
					mCycleJobs.push( CycleJob{ .AddressBusType = eAddressBusType::PROGRAM_COUNTER,
													.IncrementProgramCounter = true,
													.ExternalOperation = eExternalMode::FETCH_OPCODE,
													.InternalOperation = eInternalMode::EXECUTE } );
				}
			}
			break;
			case eAddressMode::RELATIVE:
			{
				CycleJob& currentCycleJob = mCycleJobs.front();
				currentCycleJob.IncrementProgramCounter = true;
				currentCycleJob.ExternalOperation = eExternalMode::FETCH_LOW_ADDRESS_FROM_ROM;

				mCycleJobs.push( CycleJob{ .AddressBusType = eAddressBusType::PROGRAM_COUNTER,
												.IncrementProgramCounter = true,
												.ExternalOperation = eExternalMode::FETCH_OPCODE,
												.InternalOperation = eInternalMode::CHECK_STATUS } );
			}
			break;
			case eAddressMode::ABSOLUTE_INDIRECT:
				NM_ASSERT( false, "Unimplemented address mode!!" );
				break;
			case eAddressMode::ABSOLUTE_INDEXED_WITH_X:
				NM_ASSERT( false, "Unimplemented address mode!!" );
				break;
			case eAddressMode::ABSOLUTE_INDEXED_WITH_Y:
				NM_ASSERT( false, "Unimplemented address mode!!" );
				break;
			case eAddressMode::ZERO_PAGE_INDEXED_WITH_X:
				NM_ASSERT( false, "Unimplemented address mode!!" );
				break;
			case eAddressMode::ZERO_PAGE_INDEXED_WITH_Y:
				NM_ASSERT( false, "Unimplemented address mode!!" );
				break;
			case eAddressMode::ZERO_PAGE_INDEXED_INDIRECT:
				NM_ASSERT( false, "Unimplemented address mode!!" );
				break;
			case eAddressMode::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y:
				NM_ASSERT( false, "Unimplemented address mode!!" );
				break;
			case eAddressMode::COUNT:
				[[fallthrough]];
			default:
				NM_ASSERT( false, "Invalid address mode!!" );
				break;
			}
		}

		constexpr const data_t* Cpu6502::disassemble( char* out_buffer64, const data_t* mem ) noexcept
		{
			// read instruction
			const data_t opcode = mem[0];
			const InstructionInfo* const p_instruction = INSTRUCTION_TABLE[opcode];
			if ( p_instruction == NULL ) {
				return mem;
			}

			const InstructionInfo instruction = *p_instruction;
			const char* const mnemonic = instruction.Name;
			const char* const AddressMode = convertAddressModeToString( instruction.AddressMode );
			const size_t operand_bytes = getRequiredOperandNumBytes( instruction.AddressMode );
			const data_t operand_hi_byte = operand_bytes > 1 ? mem[2] : 0;
			const data_t operand_lo_byte = operand_bytes > 0 ? mem[1] : 0;

			const char operand_hi_byte_string[3] = {
				operand_bytes > 1 ? HEX_CHAR_TABLE[( operand_hi_byte >> 4 ) & 0xF] : '.',
				operand_bytes > 1 ? HEX_CHAR_TABLE[operand_hi_byte & 0xF] : '.',
				'\0'
			};

			const char operand_lo_byte_string[3] = {
				operand_bytes > 0 ? HEX_CHAR_TABLE[( operand_lo_byte >> 4 ) & 0xF] : '.',
				operand_bytes > 0 ? HEX_CHAR_TABLE[operand_lo_byte & 0xF] : '.',
				'\0'
			};

			memset( out_buffer64, 0, BUFFER_SIZE );
			sprintf_s( out_buffer64, BUFFER_SIZE, "OPCODE=%02X[%s %s] OPERAND=%s %s", opcode, mnemonic, AddressMode, operand_hi_byte_string, operand_lo_byte_string );

			return mem + 1 + operand_bytes;
		}

		constexpr bool Cpu6502::execute() noexcept
		{
			const InstructionInfo& instruction = *mExecutionInfo.InstructionInfoOrNull;

			const size_t numOperands = getRequiredOperandNumBytes( instruction.AddressMode );
			data_t operand = 0;
			address_t address = 0;
			if ( numOperands > 0 )
			{
				switch ( instruction.AddressMode )
				{
				case eAddressMode::ACCUMULATOR:
					[[fallthrough]];
				case eAddressMode::IMPLIED:
					NM_ASSERT( false, "Logic error!!" );
					break;
				case eAddressMode::IMMEDIATE:
					operand = mExecutionInfo.Operand.Data.Value;
					break;
				case eAddressMode::ABSOLUTE:
					address = mExecutionInfo.Operand.Address;
					break;
				case eAddressMode::ZERO_PAGE:
					NM_ASSERT( false, "Unimplemented mnemonic!!" );
					break;
				case eAddressMode::RELATIVE:
					operand = mExecutionInfo.Operand.Data.Value;
					break;
				case eAddressMode::ABSOLUTE_INDIRECT:
					NM_ASSERT( false, "Unimplemented mnemonic!!" );
					break;
				case eAddressMode::ABSOLUTE_INDEXED_WITH_X:
					NM_ASSERT( false, "Unimplemented mnemonic!!" );
					break;
				case eAddressMode::ABSOLUTE_INDEXED_WITH_Y:
					NM_ASSERT( false, "Unimplemented mnemonic!!" );
					break;
				case eAddressMode::ZERO_PAGE_INDEXED_WITH_X:
					NM_ASSERT( false, "Unimplemented mnemonic!!" );
					break;
				case eAddressMode::ZERO_PAGE_INDEXED_WITH_Y:
					NM_ASSERT( false, "Unimplemented mnemonic!!" );
					break;
				case eAddressMode::ZERO_PAGE_INDEXED_INDIRECT:
					NM_ASSERT( false, "Unimplemented mnemonic!!" );
					break;
				case eAddressMode::ZERO_PAGE_INDIRECT_INDEXED_WITH_Y:
					NM_ASSERT( false, "Unimplemented address mode!!" );
					break;
				case eAddressMode::COUNT:
					[[fallthrough]];
				default:
					NM_ASSERT( false, "Invalid address mode!!" );
					break;
				}
			}

			bool result = true;
			switch ( instruction.Mnemonic )
			{
			case eMnemonic::ADC:
			{
				const data_t prevAccumulator = mRegisters.Accumulator;
				mRegisters.Accumulator += operand;
				mRegisters.Status.StatusBits.CarryFlag = prevAccumulator > mRegisters.Accumulator;
				mRegisters.Status.StatusBits.ZeroFlag = mRegisters.Accumulator;
				mRegisters.Status.StatusBits.OverflowFlag = ( prevAccumulator ^ mRegisters.Accumulator ) | 0b1000'0000;
				mRegisters.Status.StatusBits.NegativeFlag = mRegisters.Accumulator | 0b1000'0000;
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
			}
				break;
			case eMnemonic::AND:
				mRegisters.Accumulator &= operand;
				mRegisters.Status.StatusBits.ZeroFlag = mRegisters.Accumulator;
				mRegisters.Status.StatusBits.NegativeFlag = mRegisters.Accumulator | 0b1000'0000;
				break;
			case eMnemonic::ASL:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::BCC:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::BCS:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::BEQ:
				mRegisters.ProgramCounter += operand * mRegisters.Status.StatusBits.ZeroFlag;
				break;
			case eMnemonic::BIT:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::BMI:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::BNE:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::BPL:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::BRK:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::BVC:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::BVS:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::CLC:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::CLD:
				mRegisters.Status.StatusBits.DecimalModeFlag = false;
				break;
			case eMnemonic::CLI:
				mRegisters.Status.StatusBits.InterruptDisableFlag = false;
				break;
			case eMnemonic::CLV:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::CMP:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::CPX:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::CPY:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::DEC:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::DEX:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::DEY:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::EOR:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::INC:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::INX:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::INY:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::JMP:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::JSR:
				mRegisters.ProgramCounter = mExecutionInfo.Operand.Address;
				break;
			case eMnemonic::LDA:
				mRegisters.Accumulator = operand;
				mRegisters.Status.StatusBits.ZeroFlag = mRegisters.Accumulator;
				mRegisters.Status.StatusBits.NegativeFlag = mRegisters.Accumulator | 0b1000'0000;
				break;
			case eMnemonic::LDX:
				mRegisters.IndexX = operand;
				mRegisters.Status.StatusBits.ZeroFlag = mRegisters.IndexX;
				mRegisters.Status.StatusBits.NegativeFlag = mRegisters.IndexX | 0b1000'0000;
				break;
			case eMnemonic::LDY:
				mRegisters.IndexY = operand;
				mRegisters.Status.StatusBits.ZeroFlag = mRegisters.IndexY;
				mRegisters.Status.StatusBits.NegativeFlag = mRegisters.IndexY | 0b1000'0000;
				break;
			case eMnemonic::LSR:
				if ( instruction.AddressMode == eAddressMode::ACCUMULATOR )
				{
					mRegisters.Accumulator >>= 1;
				}
				else
				{
					NM_ASSERT( false, "Unimplemented mnemonic!!" );
				}
				break;
			case eMnemonic::NOP:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::ORA:
				mRegisters.Accumulator |= operand;
				mRegisters.Status.StatusBits.ZeroFlag = mRegisters.Accumulator;
				mRegisters.Status.StatusBits.NegativeFlag = mRegisters.Accumulator | 0b1000'0000;
				break;
			case eMnemonic::PHA:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::PHP:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::PLA:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::PLP:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::ROL:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::ROR:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::RTI:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::RTS:
				break;
			case eMnemonic::SBC:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::SEC:
				NM_ASSERT( false, "Unimplemented mnemonic!!" );
				break;
			case eMnemonic::SED:
				mRegisters.Status.StatusBits.DecimalModeFlag = true;
				break;
			case eMnemonic::SEI:
				mRegisters.Status.StatusBits.InterruptDisableFlag = true;
				break;
			case eMnemonic::STA:
				Write( address, mRegisters.Accumulator );
				break;
			case eMnemonic::STX:
				Write( address, mRegisters.IndexX );
				break;
			case eMnemonic::STY:
				Write( address, mRegisters.IndexY );
				break;
			case eMnemonic::TAX:
				mRegisters.IndexX = mRegisters.Accumulator;
				mRegisters.Status.StatusBits.ZeroFlag = mRegisters.IndexX;
				mRegisters.Status.StatusBits.NegativeFlag = mRegisters.IndexX | 0b1000'0000;
				break;
			case eMnemonic::TAY:
				mRegisters.IndexY = mRegisters.Accumulator;
				mRegisters.Status.StatusBits.ZeroFlag = mRegisters.IndexY;
				mRegisters.Status.StatusBits.NegativeFlag = mRegisters.IndexY | 0b1000'0000;
				break;
			case eMnemonic::TSX:
				mRegisters.IndexX = mRegisters.StackPointer;
				mRegisters.Status.StatusBits.ZeroFlag = mRegisters.IndexX;
				mRegisters.Status.StatusBits.NegativeFlag = mRegisters.IndexX | 0b1000'0000;
				break;
			case eMnemonic::TXA:
				mRegisters.StackPointer = mRegisters.Accumulator;
				mRegisters.Status.StatusBits.ZeroFlag = mRegisters.Accumulator;
				mRegisters.Status.StatusBits.NegativeFlag = mRegisters.Accumulator | 0b1000'0000;
				break;
			case eMnemonic::TXS:
				mRegisters.StackPointer = mRegisters.IndexX;
				break;
			case eMnemonic::TYA:
				mRegisters.Accumulator = mRegisters.IndexY;
				mRegisters.Status.StatusBits.ZeroFlag = mRegisters.Accumulator;
				mRegisters.Status.StatusBits.NegativeFlag = mRegisters.Accumulator | 0b1000'0000;
				break;
			case eMnemonic::COUNT:
				[[fallthrough]];
			default:
				NM_ASSERT( false, "Invalid mnemonic!!" );
				break;
			}

			return result;
		}
	}
}
