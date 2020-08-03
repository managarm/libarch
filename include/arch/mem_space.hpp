
#if defined(__i386__) || defined(__x86_64__)
#	include <arch/x86/mem_space.hpp>
#elif defined(__aarch64__)
#	include <arch/aarch64/mem_space.hpp>
#elif defined(__arm__)
#	include <arch/arm/mem_space.hpp>
#else
#	error Unsupported architecture
#endif

