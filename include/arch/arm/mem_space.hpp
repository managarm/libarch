
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

	struct mem_space {
		constexpr mem_space()
		: _base(0) { }

		constexpr mem_space(uintptr_t base)
		: _base(base) { }

		mem_space(void *base)
		: _base(reinterpret_cast<uintptr_t>(base)) { }

		mem_space subspace(ptrdiff_t offset) const {
			return mem_space(reinterpret_cast<void *>(_base + offset));
		}

		template<typename RT>
		void store(RT r, typename RT::rep_type value) const {
			auto p = reinterpret_cast<typename RT::bits_type *>(_base + r.offset());
			auto v = static_cast<typename RT::bits_type>(value);
			mem_ops<typename RT::bits_type>::store(p, v);
		}

		template<typename RT>
		typename RT::rep_type load(RT r) const {
			auto p = reinterpret_cast<const typename RT::bits_type *>(_base + r.offset());
			auto b = mem_ops<typename RT::bits_type>::load(p);
			return static_cast<typename RT::rep_type>(b);
		}

		template<typename RT>
		void store(RT r, ptrdiff_t offset, typename RT::rep_type value) const {
			auto p = reinterpret_cast<typename RT::bits_type *>(_base + r.offset() + offset);
			auto v = static_cast<typename RT::bits_type>(value);
			mem_ops<typename RT::bits_type>::store(p, v);
		}

		template<typename RT>
		typename RT::rep_type load(RT r, ptrdiff_t offset) const {
			auto p = reinterpret_cast<const typename RT::bits_type *>(_base + r.offset() + offset);
			auto b = mem_ops<typename RT::bits_type>::load(p);
			return static_cast<typename RT::rep_type>(b);
		}

	private:
		uintptr_t _base;
	};
}

using _detail::mem_ops;
using _detail::mem_space;

static constexpr mem_space global_mem{};

} // namespace arch

#endif // LIBARCH_MEM_SPACE_HPP

