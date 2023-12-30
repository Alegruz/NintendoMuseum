#pragma once

// CRT
#include <cassert>
#include <cstring>

// C++
#include <iostream>

// C++14
#include <memory>

// C++20
#include <filesystem>

// Macros
#if !defined(ARRAYSIZE)
#define ARRAYSIZE(arr)    (sizeof(arr)/sizeof(arr[0]))
#endif

#define STRINGIFY(token)            (#token)
#define CONCATENATE_STR(lhs, rhs)   STRINGIFY(lhs##rhs)

// Types
namespace ninmuse
{
	constexpr const size_t NUM_BITS_IN_BYTE = 8;
	constexpr const size_t KILO_BYTE = 1 << 10;

	namespace nes
	{
		using address_t = uint16_t;
		using data_t = uint8_t;

		constexpr address_t CreateAddress( const data_t low, const data_t high ) noexcept;
		constexpr address_t SetAddressLow( address_t& inoutAddress, const data_t low ) noexcept;
		constexpr address_t SetAddressHigh( address_t& inoutAddress, const data_t high ) noexcept;

		inline constexpr data_t GetAddressLow( const address_t address ) noexcept { return static_cast<data_t>( address & 0xFF ); }
		inline constexpr data_t GetAddressHigh( const address_t address ) noexcept { return static_cast<data_t>( address >> NUM_BITS_IN_BYTE ); }
	}

	template <typename T>
	concept Data = ( std::same_as<T, nes::data_t> );

	template <typename T>
	concept Address = ( std::same_as<T, nes::address_t> );

	template <typename T, typename U>
	concept Derived = std::is_base_of<U, T>::value;

	// [REF]: Squeezing More Out of Assert. Steve Rabin. Game Programming Gems 1.
#if defined(_DEBUG)
	bool Assert( const bool expression, const char* description, const int32_t lineNumber, const char* filename ) noexcept;

#define NM_ASSERT(expression, description)	\
		if ( Assert( expression, description, __LINE__, __FILE__ ) ) \
		{ __debugbreak(); }
#else	// NOT defined(_DEBUG)
#define ASSERT(expression, description)
#endif	// defined(_DEBUG)

#if !defined(ARRAYSIZE)
#define ARRAYSIZE(arr)	( sizeof( arr ) / sizeof( arr[0] ) )
#endif	// !defined(ARRAYSIZE)
}