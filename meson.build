project('libarch', default_options: ['cpp_std=c++20'], meson_version: '>=1.1')

pkg = import('pkgconfig')

libarch_inc = include_directories('include/')
libarch_dep = declare_dependency(include_directories: libarch_inc)

pkg.generate(name: 'libarch', description: 'libarch headers', subdirs: ['.'])

if get_option('install_headers')
	install_headers(
		'include/arch/bits.hpp',
		'include/arch/dma_structs.hpp',
		'include/arch/dma_pool.hpp',
		'include/arch/io_space.hpp',
		'include/arch/mem_space.hpp',
		'include/arch/register.hpp',
		'include/arch/variable.hpp',
		'include/arch/bit.hpp',
		'include/arch/cache.hpp',
		'include/arch/barrier.hpp',
		subdir: 'arch/')

	install_headers(
		'include/arch/x86/cache.hpp',
		'include/arch/x86/mem_space.hpp',
		'include/arch/x86/io_space.hpp',
		subdir: 'arch/x86/')

	install_headers(
		'include/arch/arm/mem_space.hpp',
		subdir: 'arch/arm/')

	install_headers(
		'include/arch/aarch64/cache.hpp',
		'include/arch/aarch64/mem_space.hpp',
		subdir: 'arch/aarch64/')

	install_headers(
		'include/arch/riscv64/cache.hpp',
		'include/arch/riscv64/mem_space.hpp',
		subdir: 'arch/riscv64/')
endif
