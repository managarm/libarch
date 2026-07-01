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
		static uint8_t load_relaxed(const uint8_t *p) {
			uint8_t v;
			asm volatile("ldrb %0, %1"
				: "=l"(v) : "m"(*p));
			return v;
		}

		static void store_relaxed(uint8_t *p, uint8_t v) {
			asm volatile("strb %0, %1"
				: : "l"(v), "m"(*p));
		}

		uint8_t atomic_exchange(uint8_t *p, uint8_t v) {
			uint32_t t, s = 1;
			while (s) {
				asm volatile("ldrexb %1, %2\n\tstrexb %0, %3, %2"
						: "=&l"(s), "=&l"(t) : "m"(*p), "l"(v)
						: "memory");
			}
			return t;
		}
	};

	template<>
	struct relaxed_mem_ops<uint16_t> {
		static uint16_t load_relaxed(const uint16_t *p) {
			uint16_t v;
			asm volatile("ldrh %0, %1"
				: "=l"(v) : "m"(*p));
			return v;
		}

		static void store_relaxed(uint16_t *p, uint16_t v) {
			asm volatile("strh %0, %1"
				: : "l"(v), "m"(*p));
		}

		uint16_t atomic_exchange(uint16_t *p, uint16_t v) {
			uint32_t t, s = 1;
			while (s) {
				asm volatile("ldrexh %1, %2\n\tstrexh %0, %3, %2"
						: "=&l"(s), "=&l"(t) : "m"(*p), "l"(v)
						: "memory");
			}
			return t;
		}
	};

	template<>
	struct relaxed_mem_ops<uint32_t> {
		static uint32_t load_relaxed(const uint32_t *p) {
			uint32_t v;
			asm volatile("ldr %0, %1"
				: "=l"(v) : "m"(*p));
			return v;
		}

		static void store_relaxed(uint32_t *p, uint32_t v) {
			asm volatile("str %0, %1"
				: : "l"(v), "m"(*p));
		}

		uint32_t atomic_exchange(uint32_t *p, uint32_t v) {
			uint32_t t, s = 1;
			while (s) {
				asm volatile("ldrex %1, %2\n\tstrex %0, %3, %2"
						: "=&l"(s), "=&l"(t) : "m"(*p), "l"(v)
						: "memory");
			}
			return t;
		}
	};
}

// TODO: These only issue a compiler barrier; real dmb barriers are still required.
template<typename B>
struct io_mem_ops {
	static B load(const B *p) {
		auto v = _detail::relaxed_mem_ops<B>::load_relaxed(p);
		asm volatile("" ::: "memory");
		return v;
	}

	static void store(B *p, B v) {
		asm volatile("" ::: "memory");
		_detail::relaxed_mem_ops<B>::store_relaxed(p, v);
	}

	static B load_relaxed(const B *p) {
		return _detail::relaxed_mem_ops<B>::load_relaxed(p);
	}

	static void store_relaxed(B *p, B v) {
		_detail::relaxed_mem_ops<B>::store_relaxed(p, v);
	}
};

// TODO: This is not correct.
template<typename B>
using main_mem_ops = io_mem_ops<B>;

// TODO: This is not correct.
template<typename B>
struct mem_ops : io_mem_ops<B> {
	static B atomic_exchange(B *p, B v) {
		return _detail::relaxed_mem_ops<B>::atomic_exchange(p, v);
	}
};

} // namespace arch

#endif // LIBARCH_MEM_SPACE_HPP
