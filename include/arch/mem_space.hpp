#pragma once

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

} // namespace _details

using io_mem_space = _details::base_mem_space<io_mem_ops>;
using main_mem_space = _details::base_mem_space<main_mem_ops>;
using mem_space = _details::base_mem_space<mem_ops>;

static constexpr mem_space global_mem{};

} // namespace arch
