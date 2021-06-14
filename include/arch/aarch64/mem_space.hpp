
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
			asm volatile("strb %w[value], [%[src]]"
				: : [value] "r"(v), [src] "r"(p) : "memory");
			asm volatile("dsb st" ::: "memory");
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

		static uint8_t atomic_exchange(uint64_t *p, uint64_t v) {
			uint64_t t, s = 1;
			while (s) {
				asm volatile("ldxrb %w0, [%1]"
					: "=r"(t) : "r"(p) : "memory");
				asm volatile("stxrb %w0, %w1, [%2]"
					: "=&r"(s) : "r"(v), "r"(p) : "memory");
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
			asm volatile("strh %w[value], [%[src]]"
				: : [value] "r"(v), [src] "r"(p) : "memory");
			asm volatile("dsb st" ::: "memory");
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

		static uint16_t atomic_exchange(uint64_t *p, uint64_t v) {
			uint64_t t, s = 1;
			while (s) {
				asm volatile("ldxrh %w0, [%1]"
					: "=r"(t) : "r"(p) : "memory");
				asm volatile("stxrh %w0, %w1, [%2]"
					: "=&r"(s) : "r"(v), "r"(p) : "memory");
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
			asm volatile("str %w[value], [%[src]]"
				: : [value] "r"(v), [src] "r"(p) : "memory");
			asm volatile("dsb st" ::: "memory");
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

		static uint32_t atomic_exchange(uint64_t *p, uint64_t v) {
			uint64_t t, s = 1;
			while (s) {
				asm volatile("ldxr %w0, [%1]"
					: "=r"(t) : "r"(p) : "memory");
				asm volatile("stxr %w0, %w1, [%2]"
					: "=&r"(s) : "r"(v), "r"(p) : "memory");
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
			asm volatile("str %[value], [%[src]]"
				: : [value] "r"(v), [src] "r"(p) : "memory");
			asm volatile("dsb st" ::: "memory");
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
				asm volatile("ldxr %0, [%1]"
					: "=r"(t) : "r"(p) : "memory");
				asm volatile("stxr %w0, %1, [%2]"
					: "=&r"(s) : "r"(v), "r"(p) : "memory");
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

	private:
		uintptr_t _base;
	};
}

using _detail::mem_ops;
using _detail::mem_space;

static constexpr mem_space global_mem{};

} // namespace arch

#endif // LIBARCH_MEM_SPACE_HPP

