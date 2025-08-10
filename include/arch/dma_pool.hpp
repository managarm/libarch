#pragma once

#include <concepts>

#include <arch/os/dma_pool.hpp>

namespace arch {

static_assert(
	requires(contiguous_pool *self, size_t size, size_t count, size_t align, void *pointer) {
		{ self->allocate(size, count, align) } -> std::same_as<void *>;
		{ self->deallocate(pointer, size, count, align) } -> std::same_as<void>;
	}
);

} // namespace arch
