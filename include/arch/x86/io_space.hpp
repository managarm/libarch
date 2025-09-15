
#ifndef LIBARCH_IO_SPACE_HPP
#define LIBARCH_IO_SPACE_HPP

#include <stdint.h>

#include <arch/register.hpp>

namespace arch {

namespace _detail {
	template<typename B>
	struct io_ops;

	template<>
	struct io_ops<uint8_t> {
		static void store(uint16_t addr, uint8_t v) {
			asm volatile ("outb %0, %1" : : "a"(v), "d"(addr) : "memory");
		}
		static uint8_t load(uint16_t addr) {
			uint8_t v;
			asm volatile ("inb %1, %0" : "=a"(v) : "d"(addr) : "memory");
			return v;
		}

		static void store_iterative(uint16_t addr, const uint8_t *p, size_t n) {
			asm volatile ("cld\n"
				"\trep outsb" : "+c"(n), "+S"(p) : "d"(addr) : "memory");
		}
		static void load_iterative(uint16_t addr, uint8_t *p, size_t n) {
			asm volatile ("cld\n"
				"\trep insb" : "+c"(n), "+D"(p) : "d"(addr) : "memory");
		}
	};

	template<>
	struct io_ops<uint16_t> {
		static void store(uint16_t addr, uint16_t v) {
			asm volatile ("outw %0, %1" : : "a"(v), "d"(addr) : "memory");
		}
		static uint16_t load(uint16_t addr) {
			uint16_t v;
			asm volatile ("inw %1, %0" : "=a"(v) : "d"(addr) : "memory");
			return v;
		}

		static void store_iterative(uint16_t addr, const uint16_t *p, size_t n) {
			asm volatile ("cld\n"
				"\trep outsw" : "+c"(n), "+S"(p) : "d"(addr) : "memory");
		}
		static void load_iterative(uint16_t addr, uint16_t *p, size_t n) {
			asm volatile ("cld\n"
				"\trep insw" : "+c"(n), "+D"(p) : "d"(addr) : "memory");
		}
	};

	template<>
	struct io_ops<uint32_t> {
		static void store(uint16_t addr, uint32_t v) {
			asm volatile ("outl %0, %1" : : "a"(v), "d"(addr) : "memory");
		}
		static uint32_t load(uint16_t addr) {
			uint32_t v;
			asm volatile ("inl %1, %0" : "=a"(v) : "d"(addr) : "memory");
			return v;
		}
	};
}

using _detail::io_ops;

} // namespace arch

#endif // LIBARCH_IO_SPACE_HPP

