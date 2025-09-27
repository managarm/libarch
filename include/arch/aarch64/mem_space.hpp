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
		static uint8_t load(const uint8_t *p) {
			uint8_t v;
			asm volatile("ldrb %w[value], [%[src]]"
				: [value] "=r"(v) : [src] "r"(p) : "memory");
			asm volatile("dsb ld" ::: "memory");
			return v;
		}

		static void store(uint8_t *p, uint8_t v) {
			asm volatile("dsb st" ::: "memory");
			asm volatile("strb %w[value], [%[src]]"
				: : [value] "r"(v), [src] "r"(p) : "memory");
		}

		static uint8_t load_relaxed(const uint8_t *p) {
			uint8_t v;
			asm volatile("ldrb %w[value], [%[src]]"
				: [value] "=r"(v) : [src] "r"(p));
			return v;
		}

		static void store_relaxed(uint8_t *p, uint8_t v) {
			asm volatile("strb %w[value], [%[src]]"
				: : [value] "r"(v), [src] "r"(p));
		}

		static uint8_t atomic_exchange(uint64_t *p, uint8_t v) {
			uint64_t t, s = 1;
			while (s) {
				asm volatile("ldxrb %w1, %2\n\tstxrb %w0, %w3, %2"
						: "=&r"(s), "=&r"(t) : "m"(*p), "r"(v)
						: "memory");
			}
			return t;
		}
	};

	template<>
	struct mem_ops<uint16_t> {
		static uint16_t load(const uint16_t *p) {
			uint16_t v;
			asm volatile("ldrh %w[value], [%[src]]"
				: [value] "=r"(v) : [src] "r"(p) : "memory");
			asm volatile("dsb ld" ::: "memory");
			return v;
		}

		static void store(uint16_t *p, uint16_t v) {
			asm volatile("dsb st" ::: "memory");
			asm volatile("strh %w[value], [%[src]]"
				: : [value] "r"(v), [src] "r"(p) : "memory");
		}

		static uint16_t load_relaxed(const uint16_t *p) {
			uint16_t v;
			asm volatile("ldrh %w[value], [%[src]]"
				: [value] "=r"(v) : [src] "r"(p));
			return v;
		}

		static void store_relaxed(uint16_t *p, uint16_t v) {
			asm volatile("strh %w[value], [%[src]]"
				: : [value] "r"(v), [src] "r"(p));
		}

		static uint16_t atomic_exchange(uint64_t *p, uint16_t v) {
			uint64_t t, s = 1;
			while (s) {
				asm volatile("ldxrh %w1, %2\n\tstxrh %w0, %w3, %2"
						: "=&r"(s), "=&r"(t) : "m"(*p), "r"(v)
						: "memory");
			}
			return t;
		}
	};

	template<>
	struct mem_ops<uint32_t> {
		static uint32_t load(const uint32_t *p) {
			uint32_t v;
			asm volatile("ldr %w[value], [%[src]]"
				: [value] "=r"(v) : [src] "r"(p) : "memory");
			asm volatile("dsb ld" ::: "memory");
			return v;
		}

		static void store(uint32_t *p, uint32_t v) {
			asm volatile("dsb st" ::: "memory");
			asm volatile("str %w[value], [%[src]]"
				: : [value] "r"(v), [src] "r"(p) : "memory");
		}

		static uint32_t load_relaxed(const uint32_t *p) {
			uint32_t v;
			asm volatile("ldr %w[value], [%[src]]"
				: [value] "=r"(v) : [src] "r"(p));
			return v;
		}

		static void store_relaxed(uint32_t *p, uint32_t v) {
			asm volatile("str %w[value], [%[src]]"
				: : [value] "r"(v), [src] "r"(p));
		}

		static uint32_t atomic_exchange(uint64_t *p, uint32_t v) {
			uint64_t t, s = 1;
			while (s) {
				asm volatile("ldxr %w1, %2\n\tstxr %w0, %w3, %2"
						: "=&r"(s), "=&r"(t) : "m"(*p), "r"(v)
						: "memory");
			}
			return t;
		}
	};

	template<>
	struct mem_ops<uint64_t> {
		static uint64_t load(const uint64_t *p) {
			uint64_t v;
			asm volatile("ldr %[value], [%[src]]"
				: [value] "=r"(v) : [src] "r"(p) : "memory");
			asm volatile("dsb ld" ::: "memory");
			return v;
		}

		static void store(uint64_t *p, uint64_t v) {
			asm volatile("dsb st" ::: "memory");
			asm volatile("str %[value], [%[src]]"
				: : [value] "r"(v), [src] "r"(p) : "memory");
		}

		static uint64_t load_relaxed(const uint64_t *p) {
			uint64_t v;
			asm volatile("ldr %[value], [%[src]]"
				: [value] "=r"(v) : [src] "r"(p));
			return v;
		}

		static void store_relaxed(uint64_t *p, uint64_t v) {
			asm volatile("str %[value], [%[src]]"
				: : [value] "r"(v), [src] "r"(p));
		}

		static uint64_t atomic_exchange(uint64_t *p, uint64_t v) {
			uint64_t t, s = 1;
			while (s) {
				asm volatile("ldxr %1, %2\n\tstxr %w0, %3, %2"
						: "=&r"(s), "=&r"(t) : "m"(*p), "r"(v)
						: "memory");
			}
			return t;
		}
	};
}

template<typename B>
struct io_mem_ops {
    static B load(const B *p) {
        auto v = _detail::mem_ops<B>::load_relaxed(p);
        asm volatile("dmb oshld" ::: "memory");
        return v;
    }

    static void store(B *p, B v) {
        asm volatile("dmb osh" ::: "memory");
        _detail::mem_ops<B>::store_relaxed(p, v);
    }
};

template<typename B>
struct main_mem_ops {
    static B load(const B *p) {
        auto v = _detail::mem_ops<B>::load_relaxed(p);
        asm volatile("dmb ishld" ::: "memory");
        return v;
    }

    static void store(B *p, B v) {
        asm volatile("dmb ish" ::: "memory");
        _detail::mem_ops<B>::store_relaxed(p, v);
    }
};

using _detail::mem_ops;

} // namespace arch

#endif // LIBARCH_MEM_SPACE_HPP
