#pragma once

#include <type_traits>

namespace arch {

template<typename...>
constexpr bool dependent_false = false;

enum class endian {
    little = __ORDER_LITTLE_ENDIAN__,
    big    = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
};

inline constexpr endian little_endian = endian::little;
inline constexpr endian big_endian = endian::big;
inline constexpr endian native_endian = endian::native;

static_assert(endian::native == endian::little || endian::native == endian::big,
		"only little and big endian are supported");

template<typename T>
inline T bswap(T val) {
	static_assert(std::is_integral_v<T>, "T must be an integral type");
	if constexpr (sizeof(T) == 1) {
		return val;
	} else if constexpr (sizeof(T) == 2) {
		return __builtin_bswap16(val);
	} else if constexpr (sizeof(T) == 4) {
		return __builtin_bswap32(val);
	} else if constexpr (sizeof(T) == 8) {
		return __builtin_bswap64(val);
	} else {
		static_assert(dependent_false<T>, "unsupported swap size");
	}
}

template<endian NewEndian, endian OldEndian = endian::native, typename T>
inline T convert_endian(T native) {
	static_assert(std::is_integral_v<T>, "T must be an integral type");
	if constexpr (NewEndian != OldEndian) {
		return bswap(native);
	} else {
		return native;
	}
}

// X forces users to specify T.
template<endian To, typename T, typename X = T>
inline T to_endian(X v) {
	return convert_endian<To, endian::native, T>(v);
}

// X forces users to specify T.
template<endian From, typename T, typename X = T>
inline T from_endian(X v) {
	return convert_endian<endian::native, From, T>(v);
}

} // namespace arch
