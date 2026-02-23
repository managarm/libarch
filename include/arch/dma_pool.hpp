#pragma once

#include <concepts>

#include <arch/os/dma_pool.hpp>

namespace arch {

static_assert(
	requires(contiguous_pool *self, size_t size, size_t count, size_t align, dma_ptr ptr) {
		{ self->allocate(size, count, align) } -> std::same_as<dma_ptr>;
		{ self->deallocate(ptr, size, count, align) } -> std::same_as<void>;
	}
);

} // namespace arch
