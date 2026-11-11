#pragma once

#include <stddef.h>
#include <stdint.h>

namespace arch {

// See memory-order.md on why this is enough for most cases on SPARC64.
// (or, at least, as much as we can do with this interface in a decently simple
// and quick way)
inline void cache_writeback(uintptr_t, size_t) {
	asm volatile("membar #Sync" ::: "memory");
}

inline void cache_clean_or_invalidate(uintptr_t, size_t) {
	asm volatile("membar #Sync" ::: "memory");
}

inline void cache_invalidate(uintptr_t, size_t) {
	asm volatile("membar #Sync" ::: "memory");
}

} // namespace arch
