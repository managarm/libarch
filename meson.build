project('libarch', 'cpp')

libarch_inc = include_directories('include/')

if host_machine.system() == 'managarm'
	libarch_lib = shared_library('arch', 'src/dma_pool.cpp',
		include_directories: libarch_inc,
		dependencies: lib_helix_dep, # ???
		install: true)

	libarch_dep = declare_dependency(
		link_with: libarch_lib,
		include_directories: libarch_inc)
elif host_machine.system() == 'bare'
	libarch_dep = declare_dependency(include_directories: libarch_inc)
else
	error('Unknown OS')
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
	else
		error('Unknown CPU architecture')
	endif
endif
