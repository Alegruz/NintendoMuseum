#pragma once

#include "Common.h"

namespace ninmuse
{
	namespace nes
	{
		inline constexpr address_t CreateAddress( const uint8_t low, const uint8_t high ) noexcept
		{
			return static_cast< address_t >( low ) | ( ( static_cast< address_t >( high ) ) << 8 );
		}

		inline constexpr address_t SetAddressLow( address_t& inoutAddress, const uint8_t low ) noexcept
		{
			inoutAddress |= static_cast< address_t >( low );
			return inoutAddress;
		}

		inline constexpr address_t SetAddressHigh( address_t& inoutAddress, const uint8_t high ) noexcept
		{
			inoutAddress |= ( static_cast< address_t >( high ) ) << 8;
			return inoutAddress;
		}
	}
}