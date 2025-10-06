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
			asm volatile("ldrb %0, %1"
				: "=l"(v) : "m"(*p) : "memory");
			return v;
		}

		static void store(uint8_t *p, uint8_t v) {
			asm volatile("strb %0, %1"
				: : "l"(v), "m"(*p) : "memory");
		}

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
	struct mem_ops<uint16_t> {
		static uint16_t load(const uint16_t *p) {
			uint16_t v;
			asm volatile("ldrh %0, %1"
				: "=l"(v) : "m"(*p) : "memory");
			return v;
		}

		static void store(uint16_t *p, uint16_t v) {
			asm volatile("strh %0, %1"
				: : "l"(v), "m"(*p) : "memory");
		}

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
	struct mem_ops<uint32_t> {
		static uint32_t load(const uint32_t *p) {
			uint32_t v;
			asm volatile("ldr %0, %1"
				: "=l"(v) : "m"(*p) : "memory");
			return v;
		}

		static void store(uint32_t *p, uint32_t v) {
			asm volatile("str %0, %1"
				: : "l"(v), "m"(*p) : "memory");
		}

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

// TODO: This is not correct.
template<typename B>
using io_mem_ops = mem_ops<B>;

// TODO: This is not correct.
template<typename B>
using main_mem_ops = mem_ops<B>;

using _detail::mem_ops;

} // namespace arch

#endif // LIBARCH_MEM_SPACE_HPP
