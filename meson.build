project('libarch', 'cpp', default_options: ['cpp_std=c++20'])

libarch_inc = include_directories('include/')

if host_machine.system() == 'managarm'
	if not get_option('header_only')
		if meson.get_compiler('cpp').get_id() == 'clang'
			add_project_arguments('-fsized-deallocation', language: 'cpp')
		endif

		frigg_dep = dependency(
			'frigg',
			default_options: 'frigg_no_install=true',
			fallback: ['frigg', 'frigg_dep']
		)

		libarch_lib = shared_library('arch', 'src/dma_pool.cpp',
			cpp_args: ['-std=c++20'],
			dependencies: frigg_dep,
			include_directories: [libarch_inc, 'subprojects/managarm/hel/include'],
			install: true)

		libarch_dep = declare_dependency(
			link_with: libarch_lib,
			include_directories: libarch_inc)
	else
		libarch_dep = declare_dependency(include_directories: libarch_inc)
	endif
else
	libarch_dep = declare_dependency(include_directories: libarch_inc)
endif

if get_option('install_headers')
	install_headers(
		'include/arch/bits.hpp',
		'include/arch/dma_pool.hpp',
		'include/arch/dma_structs.hpp',
		'include/arch/io_space.hpp',
		'include/arch/mem_space.hpp',
		'include/arch/register.hpp',
		'include/arch/variable.hpp',
		'include/arch/bit.hpp',
		subdir: 'arch/')
	install_headers(
		'include/arch/os-managarm/dma_pool.hpp',
		subdir: 'arch/os-managarm/')
	if host_machine.cpu_family() == 'x86' or host_machine.cpu_family() == 'x86_64'
		install_headers(
			'include/arch/x86/mem_space.hpp',
			'include/arch/x86/io_space.hpp',
			subdir: 'arch/x86/')
	elif host_machine.cpu_family() == 'arm'
		install_headers(
			'include/arch/arm/mem_space.hpp',
			subdir: 'arch/arm/')
	elif host_machine.cpu_family() == 'aarch64'
		install_headers(
			'include/arch/aarch64/mem_space.hpp',
			subdir: 'arch/aarch64/')
	else
		error('Unknown CPU architecture')
	endif
endif
