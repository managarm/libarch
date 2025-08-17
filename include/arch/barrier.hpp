#pragma once

#include <arch/cache.hpp>
#include <arch/dma_structs.hpp>

namespace arch {

// Helper class for performing cache maintenance when doing DMA to potentially non-coherent
// devices. Can be relaxed into no-ops when device is known to be cache-coherent. Typical use
// cases would be as follows.
//
// Host to device transfers:
//   1. write data for device into buffer
//   2. barrier.writeback()
//   3. notify device about data
//
// Device to host transfers:
//   1. allocate buffer for device
//   2. barrier.clean_or_invalidate()
//   3. tell device to write to buffer
//   4. barrier.invalidate()
//   5. use data in buffer
struct dma_barrier {
	dma_barrier(bool dma_coherent)
	: dma_coherent_{dma_coherent} { }

	dma_barrier(const dma_barrier &other)
	: dma_coherent_{other.dma_coherent_} { }
	dma_barrier(dma_barrier &&other)
	: dma_coherent_{other.dma_coherent_} { }

	dma_barrier &operator=(dma_barrier other) {
		dma_coherent_ = other.dma_coherent_;
		return *this;
	}


	bool is_dma_coherent() const { return dma_coherent_; }


	// Write the contents of the cache lines covering [addr,addr+size) back to memory.
	// This guarantees that writes done by the CPU are visible to other devices reading
	// from that memory.
	// This may be implemented as one of the following operations:
	//  - writeback and clean,
	//  - writeback and invalidate.
	void writeback(uintptr_t addr, size_t size) const {
		if (dma_coherent_) return;

		cache_writeback(addr, size);
	}

	// Clean or invalidate the cache lines covering [addr,addr+size).
	// This guarantees that the CPU does not write any data back to the given memory while
	// a device might be writing to it.
	// This may be implemented as one of the following operations:
	//  - writeback and clean,
	//  - writeback and invalidate,
	//  - discard and invalidate.
	void clean_or_invalidate(uintptr_t addr, size_t size) const {
		if (dma_coherent_) return;

		cache_clean_or_invalidate(addr, size);
	}

	// Invalidate the cache lines covering [addr,addr+size).
	// This guarantees that loads will see data written to memory by a device.
	// This may be implemented as one of the following operations:
	//  - writeback and invalidate,
	//  - discard and invalidate.
	void invalidate(uintptr_t addr, size_t size) const {
		if (dma_coherent_) return;

		cache_invalidate(addr, size);
	}


	void writeback(const void *data, size_t size) const {
		writeback(reinterpret_cast<uintptr_t>(data), size);
	}

	void clean_or_invalidate(const void *data, size_t size) const {
		clean_or_invalidate(reinterpret_cast<uintptr_t>(data), size);
	}

	void invalidate(const void *data, size_t size) const {
		invalidate(reinterpret_cast<uintptr_t>(data), size);
	}


	void writeback(arch::dma_buffer_view view) const {
		writeback(reinterpret_cast<uintptr_t>(view.data()), view.size());
	}

	void clean_or_invalidate(arch::dma_buffer_view view) const {
		clean_or_invalidate(reinterpret_cast<uintptr_t>(view.data()), view.size());
	}

	void invalidate(arch::dma_buffer_view view) const {
		invalidate(reinterpret_cast<uintptr_t>(view.data()), view.size());
	}

private:
	bool dma_coherent_;
};

} // namespace arch
