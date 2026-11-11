#pragma once

#include <stdint.h>
#include <stddef.h>
#include <arch/register.hpp>

namespace arch {

namespace _detail {

// Native-endian accesses without ordering beyond the volatile access itself.
template<typename B>
struct sparc_relaxed_mem_ops {
	static B load_relaxed(const B *p) {
		return *reinterpret_cast<const volatile B *>(p);
	}
	static void store_relaxed(B *p, B value) {
		*reinterpret_cast<volatile B *>(p) = value;
	}
};

template<typename B>
struct sparc_atomic_mem_ops {
	static B atomic_exchange(B *p, B value) {
		static_assert(sizeof(B) == 4 || sizeof(B) == 8,
			"SPARC atomic variables require 32-bit or 64-bit storage");
		return __atomic_exchange_n(p, value, __ATOMIC_SEQ_CST);
	}
};

} // namespace _detail

// Full synchronization also orders accesses between cacheable main memory
// and non-cacheable device memory.
template<typename B>
struct io_mem_ops : _detail::sparc_relaxed_mem_ops<B> {
	static B load(const B *p) {
		asm volatile("membar #Sync" ::: "memory");
		B value = _detail::sparc_relaxed_mem_ops<B>::load_relaxed(p);
		asm volatile("membar #Sync" ::: "memory");
		return value;
	}
	static void store(B *p, B value) {
		asm volatile("membar #Sync" ::: "memory");
		_detail::sparc_relaxed_mem_ops<B>::store_relaxed(p, value);
		asm volatile("membar #Sync" ::: "memory");
	}
};

template<typename B>
struct main_mem_ops : _detail::sparc_relaxed_mem_ops<B>, _detail::sparc_atomic_mem_ops<B> {
	static B load(const B *p) {
		B value = _detail::sparc_relaxed_mem_ops<B>::load_relaxed(p);
		asm volatile("membar #LoadLoad | #LoadStore" ::: "memory");
		return value;
	}
	static void store(B *p, B value) {
		asm volatile("membar #LoadStore | #StoreStore" ::: "memory");
		_detail::sparc_relaxed_mem_ops<B>::store_relaxed(p, value);
	}
};

template<typename B>
struct mem_ops : io_mem_ops<B>, _detail::sparc_atomic_mem_ops<B> { };

} // namespace arch
