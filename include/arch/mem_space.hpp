#pragma once

#include <arch/bit.hpp>

#if defined(__i386__) || defined(__x86_64__)
#	include <arch/x86/mem_space.hpp>
#elif defined(__aarch64__)
#	include <arch/aarch64/mem_space.hpp>
#elif defined(__arm__)
#	include <arch/arm/mem_space.hpp>
#elif defined(__riscv) && (__riscv_xlen == 64)
#	include <arch/riscv64/mem_space.hpp>
#else
#	error Unsupported architecture
#endif

namespace arch {

namespace _details {

template<template<typename> typename Ops>
struct base_mem_space {
	constexpr base_mem_space()
	: _base(0) { }

	constexpr base_mem_space(uintptr_t base)
	: _base(base) { }

	base_mem_space(void *base)
	: _base(reinterpret_cast<uintptr_t>(base)) { }

	base_mem_space subspace(ptrdiff_t offset) const {
		return base_mem_space(reinterpret_cast<void *>(_base + offset));
	}

	uintptr_t base() const { return _base; }

	template<typename RT>
	void store(RT r, typename RT::rep_type value) const {
		auto p = reinterpret_cast<typename RT::bits_type *>(_base + r.offset());
		auto v = static_cast<typename RT::bits_type>(value);
		Ops<typename RT::bits_type>::store(p, v);
	}

	template<typename RT>
	typename RT::rep_type load(RT r) const {
		auto p = reinterpret_cast<const typename RT::bits_type *>(_base + r.offset());
		auto b = Ops<typename RT::bits_type>::load(p);
		return static_cast<typename RT::rep_type>(b);
	}

	template<typename RT>
	void store_relaxed(RT r, typename RT::rep_type value) const {
		auto p = reinterpret_cast<typename RT::bits_type *>(_base + r.offset());
		auto v = static_cast<typename RT::bits_type>(value);
		Ops<typename RT::bits_type>::store_relaxed(p, v);
	}

	template<typename RT>
	typename RT::rep_type load_relaxed(RT r) const {
		auto p = reinterpret_cast<const typename RT::bits_type *>(_base + r.offset());
		auto b = Ops<typename RT::bits_type>::load_relaxed(p);
		return static_cast<typename RT::rep_type>(b);
	}

private:
	uintptr_t _base;
};

// Endian policies for device registers. Keep the existing native spaces
// unchanged; a driver selects a fixed-endian space at its device boundary.
template<typename B, endian E>
struct endian_io_ops {
	static B load(const B *p) {
		return from_endian<E, B>(io_mem_ops<B>::load(p));
	}
	static void store(B *p, B value) {
		io_mem_ops<B>::store(p, to_endian<E, B>(value));
	}
	static B load_relaxed(const B *p) {
		return from_endian<E, B>(io_mem_ops<B>::load_relaxed(p));
	}
	static void store_relaxed(B *p, B value) {
		io_mem_ops<B>::store_relaxed(p, to_endian<E, B>(value));
	}
};
template<typename B> using little_endian_io_ops = endian_io_ops<B, endian::little>;
template<typename B> using big_endian_io_ops = endian_io_ops<B, endian::big>;

} // namespace _details

using little_endian_io_mem_space = _details::base_mem_space<_details::little_endian_io_ops>;
using big_endian_io_mem_space = _details::base_mem_space<_details::big_endian_io_ops>;
using io_mem_space = _details::base_mem_space<io_mem_ops>;
using main_mem_space = _details::base_mem_space<main_mem_ops>;
using mem_space = _details::base_mem_space<mem_ops>;

static constexpr mem_space global_mem{};

} // namespace arch
