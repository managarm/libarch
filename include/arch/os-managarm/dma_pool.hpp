#ifndef LIBARCH_OS_MANAGARM_DMA_POOL_HPP
#define LIBARCH_OS_MANAGARM_DMA_POOL_HPP

#include <stddef.h>
#include <stdint.h>
#include <mutex>
#include <utility>
#include <arch/dma_structs.hpp>
#include <frg/slab.hpp>

namespace arch {

struct contiguous_pool_options {
	size_t addressBits{32};
};

namespace os {

struct contiguous_policy {
public:
	contiguous_policy(contiguous_pool_options options = {})
	: _options{std::move(options)} { }

	uintptr_t map(size_t length);
	void unmap(uintptr_t address, size_t length);

private:
	contiguous_pool_options _options;
};

struct contiguous_pool : dma_pool {
	contiguous_pool(contiguous_pool_options options = {});

	void *allocate(size_t size, size_t count, size_t align) override;
	void deallocate(void *pointer, size_t size, size_t count, size_t align) override;

private:
	contiguous_policy _policy;

	frg::slab_pool<
		contiguous_policy,
		std::mutex
	> _slab;
};

} } // namespace arch::os

#endif // LIBARCH_OS_MANAGARM_DMA_POOL_HPP
