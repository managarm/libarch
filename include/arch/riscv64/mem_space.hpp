#ifndef LIBARCH_MEM_SPACE_HPP
#define LIBARCH_MEM_SPACE_HPP

#include <stdint.h>

#include <arch/register.hpp>

namespace arch {

namespace _detail {
	template<typename B>
	struct mem_ops;

	template<>
	struct mem_ops<uint8_t> {
		static void store(uint8_t *p, uint8_t v) {
			asm volatile ("sb %0, %1" : : "r"(v), "m"(*p) : "memory");
		}

		static uint8_t load(const uint8_t *p) {
			uint8_t v;
			asm volatile ("lbu %0, %1" : "=r"(v) : "m"(*p) : "memory");
			return v;
		}
	};

	template<>
	struct mem_ops<uint16_t> {
		static void store(uint16_t *p, uint16_t v) {
			asm volatile ("sh %0, %1" : : "r"(v), "m"(*p) : "memory");
		}
		static void store_relaxed(uint16_t *p, uint16_t v) {
			asm volatile ("sh %0, %1" : : "r"(v), "m"(*p));
		}

		static uint16_t load(const uint16_t *p) {
			uint16_t v;
			asm volatile ("lhu %0, %1" : "=r"(v) : "m"(*p) : "memory");
			return v;
		}
		static uint16_t load_relaxed(const uint16_t *p) {
			uint16_t v;
			asm volatile ("lhu %0, %1" : "=r"(v) : "m"(*p));
			return v;
		}
	};

	template<>
	struct mem_ops<uint32_t> {
		static void store(uint32_t *p, uint32_t v) {
			asm volatile ("sw %0, %1" : : "r"(v), "m"(*p) : "memory");
		}
		static void store_relaxed(uint32_t *p, uint32_t v) {
			asm volatile ("sw %0, %1" : : "r"(v), "m"(*p));
		}

		static uint32_t load(const uint32_t *p) {
			uint32_t v;
			asm volatile ("lwu %0, %1" : "=r"(v) : "m"(*p) : "memory");
			return v;
		}
		static uint32_t load_relaxed(const uint32_t *p) {
			uint32_t v;
			asm volatile ("lwu %0, %1" : "=r"(v) : "m"(*p));
			return v;
		}
	};

	template<>
	struct mem_ops<uint64_t> {
		static void store(uint64_t *p, uint64_t v) {
			asm volatile ("sd %0, %1" : : "r"(v), "m"(*p) : "memory");
		}
		static void store_relaxed(uint64_t *p, uint64_t v) {
			asm volatile ("sd %0, %1" : : "r"(v), "m"(*p));
		}

		static uint64_t load(const uint64_t *p) {
			uint64_t v;
			asm volatile ("ld %0, %1" : "=r"(v) : "m"(*p) : "memory");
			return v;
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
        auto v = _detail::mem_ops<B>::load_relaxed(p);
        asm volatile("fence i, rw" ::: "memory");
        return v;
    }

    static void store(B *p, B v) {
        asm volatile("fence rw, o" ::: "memory");
        _detail::mem_ops<B>::store_relaxed(p, v);
        asm volatile("fence o, w" ::: "memory");
    }
};

template<typename B>
struct main_mem_ops {
    static B load(const B *p) {
        auto v = _detail::mem_ops<B>::load_relaxed(p);
        asm volatile("fence r, rw" ::: "memory");
        return v;
    }

    static void store(B *p, B v) {
        asm volatile("fence rw, w" ::: "memory");
        _detail::mem_ops<B>::store_relaxed(p, v);
    }
};

using _detail::mem_ops;

} // namespace arch

#endif // LIBARCH_MEM_SPACE_HPP
