#pragma once

#if defined(__i386__) || defined(__x86_64__)
#	include<arch/x86/io_space.hpp>
#else
	#include <stdint.h>

	// TODO: If we support MMIO-redirected PIO in the future,
	//	   we may want to provide a proper implementation here.
	template<typename T>
	struct io_ops {
		static void store(uint16_t, T) {
			__builtin_trap();
		}
		static T load(uint16_t) {
			__builtin_trap();
		}

		static void store_iterative(uint16_t, const T *, size_t) {
			__builtin_trap();
		}
		static void load_iterative(uint16_t, T *, size_t) {
			__builtin_trap();
		}
	};
#endif

namespace arch {

struct io_space {
	constexpr io_space()
	: _base(0) { }

	constexpr io_space(uint16_t base)
	: _base(base) { }

	io_space subspace(ptrdiff_t offset) const {
		return io_space(_base + offset);
	}

	template<typename RT>
	void store(RT r, typename RT::rep_type value) const {
		auto v = static_cast<typename RT::bits_type>(value);
		io_ops<typename RT::bits_type>::store(_base + r.offset(), v);
	}

	template<typename RT>
	typename RT::rep_type load(RT r) const {
		auto b = io_ops<typename RT::bits_type>::load(_base + r.offset());
		return static_cast<typename RT::rep_type>(b);
	}

	template<typename RT>
	void store_iterative(RT r, const typename RT::rep_type *p, size_t n) const {
		io_ops<typename RT::bits_type>::store_iterative(_base + r.offset(), p, n);
	}

	template<typename RT>
	void load_iterative(RT r, typename RT::rep_type *p, size_t n) const {
		io_ops<typename RT::bits_type>::load_iterative(_base + r.offset(), p, n);
	}

private:
	uint16_t _base;
};

static constexpr io_space global_io{};

} // namespace arch
