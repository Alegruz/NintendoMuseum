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

	namespace nes
	{
		using address_t = uint16_t;
		using data_t = uint8_t;

		constexpr address_t CreateAddress( const data_t low, const data_t high ) noexcept;
		constexpr address_t SetAddressLow( address_t& inoutAddress, const data_t low ) noexcept;
		constexpr address_t SetAddressHigh( address_t& inoutAddress, const data_t high ) noexcept;
	}

	template <typename T>
	concept IsData = ( std::same_as<T, nes::data_t> );
}