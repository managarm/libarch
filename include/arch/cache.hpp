#if defined(__i386__) || defined(__x86_64__)
#	include <arch/x86/cache.hpp>
#elif defined(__aarch64__)
#	include <arch/aarch64/cache.hpp>
#elif defined(__riscv) && (__riscv_xlen == 64)
#	include <arch/riscv64/cache.hpp>
#else
#	error Unsupported architecture
#endif
