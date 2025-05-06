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

	struct mem_space {
		constexpr mem_space()
		: _base(0) { }

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
