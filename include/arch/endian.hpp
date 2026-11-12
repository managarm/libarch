#pragma once

#include <arch/bit.hpp>
#include <type_traits>

namespace arch {

// An integer stored in a fixed byte order, with host-order arithmetic.
// Size and alignment match T. This type does not support unaligned accesses
// or MMIO; use it for naturally aligned fields in disk and wire structures.
template<typename T, endian E>
struct endian_scalar {
	static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>
			&& !std::is_same_v<T, bool>, "T must be an unsigned integer");
	static_assert(E == endian::little || E == endian::big);

	using value_type = T;

	endian_scalar() = default;

	constexpr endian_scalar(T value)
	: _value{to_endian<E, T>(value)} { }

	constexpr operator T() const {
		return from_endian<E, T>(_value);
	}

	constexpr endian_scalar &operator=(T value) {
		_value = to_endian<E, T>(value);
		return *this;
	}

	template<typename U>
	constexpr endian_scalar &operator+=(U value) {
		return *this = static_cast<T>(static_cast<T>(*this) + value);
	}

	template<typename U>
	constexpr endian_scalar &operator-=(U value) {
		return *this = static_cast<T>(static_cast<T>(*this) - value);
	}

	template<typename U>
	constexpr endian_scalar &operator*=(U value) {
		return *this = static_cast<T>(static_cast<T>(*this) * value);
	}

	template<typename U>
	constexpr endian_scalar &operator/=(U value) {
		return *this = static_cast<T>(static_cast<T>(*this) / value);
	}

	template<typename U>
	constexpr endian_scalar &operator%=(U value) {
		return *this = static_cast<T>(static_cast<T>(*this) % value);
	}

	template<typename U>
	constexpr endian_scalar &operator&=(U value) {
		return *this = static_cast<T>(static_cast<T>(*this) & value);
	}

	template<typename U>
	constexpr endian_scalar &operator|=(U value) {
		return *this = static_cast<T>(static_cast<T>(*this) | value);
	}

	template<typename U>
	constexpr endian_scalar &operator^=(U value) {
		return *this = static_cast<T>(static_cast<T>(*this) ^ value);
	}

	template<typename U>
	constexpr endian_scalar &operator<<=(U value) {
		return *this = static_cast<T>(static_cast<T>(*this) << value);
	}

	template<typename U>
	constexpr endian_scalar &operator>>=(U value) {
		return *this = static_cast<T>(static_cast<T>(*this) >> value);
	}

	constexpr endian_scalar &operator++() {
		return *this += 1;
	}

	constexpr T operator++(int) {
		T old = *this;
		++*this;
		return old;
	}

	constexpr endian_scalar &operator--() {
		return *this -= 1;
	}

	constexpr T operator--(int) {
		T old = *this;
		--*this;
		return old;
	}

private:
	T _value;
};

} // namespace arch
