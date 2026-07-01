#ifndef LIBARCH_MEM_SPACE_HPP
#define LIBARCH_MEM_SPACE_HPP

#include <stdint.h>

#include <arch/register.hpp>

namespace arch {

namespace _detail {
	// Raw accesses without any ordering guarantees (not even a compiler barrier
	// beyond the volatile access itself). These are the building blocks that the
	// ordered *_mem_ops below combine with the appropriate architectural barriers.
	template<typename B>
	struct relaxed_mem_ops;

	template<>
	struct relaxed_mem_ops<uint8_t> {
		static void store_relaxed(uint8_t *p, uint8_t v) {
			asm volatile ("sb %0, %1" : : "r"(v), "m"(*p));
		}
		static uint8_t load_relaxed(const uint8_t *p) {
			uint8_t v;
			asm volatile ("lbu %0, %1" : "=r"(v) : "m"(*p));
			return v;
		}
	};

	template<>
	struct relaxed_mem_ops<uint16_t> {
		static void store_relaxed(uint16_t *p, uint16_t v) {
			asm volatile ("sh %0, %1" : : "r"(v), "m"(*p));
		}
		static uint16_t load_relaxed(const uint16_t *p) {
			uint16_t v;
			asm volatile ("lhu %0, %1" : "=r"(v) : "m"(*p));
			return v;
		}
	};

	template<>
	struct relaxed_mem_ops<uint32_t> {
		static void store_relaxed(uint32_t *p, uint32_t v) {
			asm volatile ("sw %0, %1" : : "r"(v), "m"(*p));
		}
		static uint32_t load_relaxed(const uint32_t *p) {
			uint32_t v;
			asm volatile ("lwu %0, %1" : "=r"(v) : "m"(*p));
			return v;
		}
	};

	template<>
	struct relaxed_mem_ops<uint64_t> {
		static void store_relaxed(uint64_t *p, uint64_t v) {
			asm volatile ("sd %0, %1" : : "r"(v), "m"(*p));
		}
		static uint64_t load_relaxed(const uint64_t *p) {
			uint64_t v;
			asm volatile ("ld %0, %1" : "=r"(v) : "m"(*p));
			return v;
		}

		// TODO: Implement:
		static uint64_t atomic_exchange(uint64_t *p, uint64_t v);
	};
}

template<typename B>
struct io_mem_ops {
	static B load(const B *p) {
		asm volatile("fence r, i" ::: "memory");
		auto v = _detail::relaxed_mem_ops<B>::load_relaxed(p);
		asm volatile("fence i, rw" ::: "memory");
		return v;
	}

	static void store(B *p, B v) {
		asm volatile("fence rw, o" ::: "memory");
		_detail::relaxed_mem_ops<B>::store_relaxed(p, v);
		asm volatile("fence o, w" ::: "memory");
	}

	static B load_relaxed(const B *p) {
		return _detail::relaxed_mem_ops<B>::load_relaxed(p);
	}

	static void store_relaxed(B *p, B v) {
		_detail::relaxed_mem_ops<B>::store_relaxed(p, v);
	}
};

template<typename B>
struct main_mem_ops {
	static B load(const B *p) {
		auto v = _detail::relaxed_mem_ops<B>::load_relaxed(p);
		asm volatile("fence r, rw" ::: "memory");
		return v;
	}

	static void store(B *p, B v) {
		asm volatile("fence rw, w" ::: "memory");
		_detail::relaxed_mem_ops<B>::store_relaxed(p, v);
	}

	static B load_relaxed(const B *p) {
		return _detail::relaxed_mem_ops<B>::load_relaxed(p);
	}

	static void store_relaxed(B *p, B v) {
		_detail::relaxed_mem_ops<B>::store_relaxed(p, v);
	}
};

template<typename B>
struct mem_ops {
	static B load(const B *p) {
		asm volatile("fence r, i" ::: "memory");
		auto v = _detail::relaxed_mem_ops<B>::load_relaxed(p);
		asm volatile("fence ir, rw" ::: "memory");
		return v;
	}

	static void store(B *p, B v) {
		asm volatile("fence rw, ow" ::: "memory");
		_detail::relaxed_mem_ops<B>::store_relaxed(p, v);
		asm volatile("fence o, w" ::: "memory");
	}

	static B load_relaxed(const B *p) {
		return _detail::relaxed_mem_ops<B>::load_relaxed(p);
	}

	static void store_relaxed(B *p, B v) {
		_detail::relaxed_mem_ops<B>::store_relaxed(p, v);
	}

	static B atomic_exchange(B *p, B v) {
		return _detail::relaxed_mem_ops<B>::atomic_exchange(p, v);
	}
};

} // namespace arch

#endif // LIBARCH_MEM_SPACE_HPP
