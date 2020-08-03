#ifndef LIBARCH_VARIABLE_HPP
#define LIBARCH_VARIABLE_HPP

#include <arch/bit.hpp>
#include <arch/bits.hpp>
#include <arch/mem_space.hpp>

namespace arch {

template<typename R, typename B>
struct basic_variable {
	using rep_type = R;
	using bits_type = B;

	basic_variable() = default;

	explicit constexpr basic_variable(R r)
	: _embedded{static_cast<B>(r)} { }

	R load() {
		return static_cast<R>(_detail::mem_ops<B>::load(&_embedded));
	}

	void store(R r) {
		_detail::mem_ops<B>::store(&_embedded, static_cast<B>(r));
	}

	R atomic_exchange(R r) {
		return static_cast<R>(_detail::mem_ops<B>::atomic_exchange(&_embedded, static_cast<B>(r)));
	}

private:
	B _embedded;
};

template<typename T>
using scalar_variable = basic_variable<T, T>;

template<typename B>
using bit_variable = basic_variable<bit_value<B>, B>;

template<typename R, typename B, endian E>
struct basic_storage {
	using rep_type = R;
	using bits_type = B;

	basic_storage() = default;

	constexpr basic_storage(R r)
	: _embedded{convert_endian<E, endian::native, B>(static_cast<B>(r))} { }

	R load() {
		return static_cast<R>(convert_endian<endian::native, E, B>(_embedded));
	}

	void store(R r) {
		_embedded = convert_endian<E, endian::native, B>(static_cast<B>(r));
	}

private:
	B _embedded;
};

template<typename T, endian E>
using scalar_storage = basic_storage<T, T, E>;

template<typename B, endian E>
using bit_storage = basic_storage<bit_value<B>, B, E>;

} // namespace arch

#endif // LIBARCH_VARIABLE_HPP
