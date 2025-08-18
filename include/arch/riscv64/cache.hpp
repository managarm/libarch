#pragma once

#include <stdint.h>
#include <stddef.h>

namespace arch {

namespace detail_ {

inline size_t dcache_line_size() {
	// TODO: This information has to be obtained from the device tree.
	// 64 should be a safe guess for current CPUs (might result in some extra
	// flush/clean instructions at worst).
	return 64;
}

// Clean cache lines.
[[gnu::target("arch=+zicbom")]] inline void cache_clean(uintptr_t addr, size_t size) {
	auto dsz = dcache_line_size();
	for (auto cur = addr & ~(dsz - 1); cur < addr + size; cur += dsz) {
		asm volatile ("cbo.clean 0(%0)" :: "r"(cur) : "memory");
	}
	asm volatile ("fence w, iorw" ::: "memory");
}

// Flush cache lines.
[[gnu::target("arch=+zicbom")]] inline void cache_flush(uintptr_t addr, size_t size) {
	auto dsz = dcache_line_size();
	for (auto cur = addr & ~(dsz - 1); cur < addr + size; cur += dsz) {
		asm volatile ("cbo.flush 0(%0)" :: "r"(cur) : "memory");
	}
	asm volatile ("fence w, iorw" ::: "memory");
}

} // namespace detail_


inline void cache_writeback(uintptr_t addr, size_t size) {
	detail_::cache_clean(addr, size);
}

inline void cache_clean_or_invalidate(uintptr_t addr, size_t size) {
	detail_::cache_clean(addr, size);
}

inline void cache_invalidate(uintptr_t addr, size_t size) {
	detail_::cache_flush(addr, size);
}

} // namespace arch
