#pragma once

#include <stdint.h>
#include <stddef.h>

namespace arch {

namespace detail_ {

inline size_t dcache_line_size() {
	uint64_t ctr;
	asm ("mrs %0, ctr_el0" : "=r"(ctr));

	return ((ctr >> 16) & 0b1111) << 4;
}

// Clean cache lines by VA to PoC.
inline void cache_clean_poc(uintptr_t addr, size_t size) {
	auto dsz = dcache_line_size();
	for (auto cur = addr & ~(dsz - 1); cur < addr + size; cur += dsz) {
		asm volatile ("dc cvac, %0" :: "r"(cur) : "memory");
	}
	asm volatile ("dmb sy" ::: "memory");
}

// Clean and invalidate cache lines by VA to PoC.
inline void cache_clean_invalidate_poc(uintptr_t addr, size_t size) {
	auto dsz = dcache_line_size();
	for (auto cur = addr & ~(dsz - 1); cur < addr + size; cur += dsz) {
		asm volatile ("dc civac, %0" :: "r"(cur) : "memory");
	}
	asm volatile ("dmb sy" ::: "memory");
}

} // namespace detail_


inline void cache_writeback(uintptr_t addr, size_t size) {
	detail_::cache_clean_poc(addr, size);
}

inline void cache_clean_or_invalidate(uintptr_t addr, size_t size) {
	detail_::cache_clean_poc(addr, size);
}

inline void cache_invalidate(uintptr_t addr, size_t size) {
	detail_::cache_clean_invalidate_poc(addr, size);
}

} // namespace arch
