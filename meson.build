#
# SPDX-License-Identifier: BSD-3-Clause
#
# Copyright © 2019-2021 Keith Packard
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
#
project('picolibc', 'c',
	default_options: [
	  'buildtype=minsize',
	  'debug=true',
	  'c_std=c18',
	  'b_staticpic=false',
          'warning_level=2',
	],
	license : 'BSD',
	meson_version : '>= 0.53',
	version: '1.7.4'
       )

targets = []

cc = meson.get_compiler('c')
fs = import('fs')

# Find the picolibc name for the host cpu. Provide
# for some common aliases

cpu_family_aliases = {
		       # aarch64
		       'arm64' : 'aarch64',
		       # cris
		       'crisv32' : 'cris',
		       # i386
		       'i486' : 'i386',
		       'i586' : 'i386',
		       'i686' : 'i386',
		       'x86' : 'i386',
		       # m68hc11
		       'm6811' : 'm68hc11',
		       'm6812' : 'm68hc11',
		       'm68hc12' : 'm68hc11',
		       # m68k
		       'fido' : 'm68k',
		       # m88k
		       'm88110' : 'm88k',
		       # or1k
		       'or1knd': 'or1k',
		       # powerpc
		       'powerpc64' : 'powerpc',
		       'ppc64' : 'powerpc',
		       'ppc64le' : 'powerpc',
		       # riscv
		       'riscv32' : 'riscv',
		       'riscv64' : 'riscv',
		       # x86_64
		       'amd64' : 'x86_64',
		     }

# Check for an alias, default to provided name
host_cpu_family = cpu_family_aliases.get(host_machine.cpu_family(), host_machine.cpu_family())

# Make sure we have meson build support for the machine-specific files
if not fs.is_file('newlib/libc/machine' / host_cpu_family / 'meson.build')
  message = '''

Unsupported architecture: "@0@"

    Read the Supported Architectures section in README.md
    to learn how to add a new architecture.
'''
  error(message.format(host_cpu_family))
endif

# Find out about long double type

long_double_code = '''
#include <float.h>
#ifndef __LDBL_MANT_DIG__
#error No long double support in float.h
#endif
long double test()
{
	long double ld = 0.0L;
	 return ld;
}
'''
have_long_double = cc.compiles(long_double_code, name : 'long double check')

long_double_size_code = '''
#include <float.h>
char size_test[sizeof(double) == sizeof(long double) ? 1 : -1];
'''

long_double_mant_64_code = '''
#include <float.h>
char size_test[__LDBL_MANT_DIG__ == 64 ? 1 : -1];
'''

long_double_mant_113_code = '''
#include <float.h>
char size_test[__LDBL_MANT_DIG__ == 113 ? 1 : -1];
'''

if have_long_double
  long_double_equals_double = cc.compiles(long_double_size_code, name : 'long double same as double')
  if not long_double_equals_double
    long_double_mant_is_64 = cc.compiles(long_double_mant_64_code, name : 'long double mantissa is 64 bits')
    long_double_mant_is_113 = cc.compiles(long_double_mant_113_code, name : 'long double mantissa is 113 bits')
  endif
endif

enable_multilib = get_option('multilib')
multilib_list = get_option('multilib-list')
enable_picolib = get_option('picolib')
enable_picocrt = get_option('picocrt')
enable_semihost = get_option('semihost')
enable_tests = get_option('tests')
enable_native_tests = get_option('native-tests')
tests_enable_stack_protector = get_option('tests-enable-stack-protector')
tests_enable_full_malloc_stress = get_option('tests-enable-full-malloc-stress')
tests_enable_posix_io = get_option('tests-enable-posix-io')
have_alias_attribute_option = get_option('have-alias-attribute')
have_format_attribute_option = get_option('have-format-attribute')

newlib_iconv_encodings = get_option('newlib-iconv-encodings')
newlib_iconv_from_encodings = get_option('newlib-iconv-from-encodings')
newlib_iconv_to_encodings = get_option('newlib-iconv-to-encodings')
newlib_iconv_external_ccs = get_option('newlib-iconv-external-ccs')

newlib_atexit_dynamic_alloc = get_option('newlib-atexit-dynamic-alloc')
newlib_nano_malloc = get_option('newlib-nano-malloc')
lite_exit = get_option('lite-exit')

newlib_elix_level = get_option('newlib-elix-level')
c_args = ['-include', '@0@/@1@'.format(meson.current_build_dir(), 'picolibc.h')]

# Disable ssp and fortify source while building picolibc (it's enabled
# by default by the ubuntu native compiler)
c_args += cc.get_supported_arguments(['-fno-common', '-frounding-math', '-Wno-unsupported-floating-point-opt', '-fno-stack-protector'])

# Select a fortify source option
fortify_source = get_option('fortify-source')
if fortify_source == 'none'
  c_args += '-U_FORTIFY_SOURCE'
else
  c_args += '-D_FORTIFY_SOURCE=' + fortify_source
endif

if cc.symbols_have_underscore_prefix()
  global_prefix = '_'
else
  global_prefix = ''
endif
fast_strcmp = get_option('fast-strcmp')

newlib_mb = get_option('newlib-mb')
newlib_locale_info = get_option('newlib-locale-info')

newlib_obsolete_math = get_option('newlib-obsolete-math')
newlib_obsolete_math_float = get_option('newlib-obsolete-math-float')
newlib_obsolete_math_double = get_option('newlib-obsolete-math-double')

sysroot_install = get_option('sysroot-install')
prefix = get_option('prefix')

nm = find_program('nm', required : false)

check_duplicate_names = nm.found()

if nm.found()
  duplicate_names = find_program('scripts/duplicate-names', required : true)
endif

# Select exit code
picoexit = get_option('picoexit')

# Shared stdio options
io_long_long = get_option('io-long-long')
newlib_io_long_long = get_option('newlib-io-long-long')
if newlib_io_long_long != 'auto'
  io_long_long = newlib_io_long_long == 'true'
endif

io_c99_formats = get_option('io-c99-formats')
newlib_io_c99_formats = get_option('newlib-io-c99-formats')
if newlib_io_c99_formats != 'auto'
  io_c99_formats = newlib_io_c99_formats == 'true'
endif

# Select stdio implementation
tinystdio = get_option('tinystdio')

has_link_defsym = meson.get_cross_property('has_link_defsym', cc.has_link_argument('-Wl,--defsym=' + global_prefix + '_start=0'))
has_link_alias = meson.get_cross_property('has_link_alias', cc.has_link_argument('-Wl,-alias,' + global_prefix + 'main,testalias'))

# tinystdio options
posix_io = tinystdio and get_option('posix-io')
posix_console = posix_io and get_option('posix-console')
io_float_exact = not tinystdio or get_option('io-float-exact')
atomic_ungetc = tinystdio and get_option('atomic-ungetc')
format_default = get_option('format-default')

double_printf_compile_args=['-DPICOLIBC_DOUBLE_PRINTF_SCANF']
double_printf_link_args=double_printf_compile_args
float_printf_compile_args=['-DPICOLIBC_FLOAT_PRINTF_SCANF']
float_printf_link_args=float_printf_compile_args
int_printf_compile_args=['-DPICOLIBC_INTEGER_PRINTF_SCANF']
int_printf_link_args=int_printf_compile_args
if tinystdio
  vfprintf_symbol = global_prefix + 'vfprintf'
  __d_vfprintf_symbol = global_prefix + '__d_vfprintf'
  __f_vfprintf_symbol = global_prefix + '__f_vfprintf'
  __i_vfprintf_symbol = global_prefix + '__i_vfprintf'
  vfscanf_symbol = global_prefix + 'vfscanf'
  __d_vfscanf_symbol = global_prefix + '__d_vfscanf'
  __f_vfscanf_symbol = global_prefix + '__f_vfscanf'
  __i_vfscanf_symbol = global_prefix + '__i_vfscanf'

  if format_default == 'double'
    c_args += '-DFORMAT_DEFAULT_DOUBLE'
  elif format_default == 'float'
    c_args += '-DFORMAT_DEFAULT_FLOAT'
  elif format_default == 'integer'
    c_args += '-DFORMAT_DEFAULT_INTEGER'
  endif

  if has_link_defsym
    if format_default != 'double'
      double_printf_link_args += '-Wl,--defsym=' + vfprintf_symbol +  '=' + __d_vfprintf_symbol
      double_printf_link_args += '-Wl,--defsym=' + vfscanf_symbol + '=' + __d_vfscanf_symbol
    endif
    if format_default != 'float'
      float_printf_link_args += '-Wl,--defsym=' + vfprintf_symbol +  '=' + __f_vfprintf_symbol
      float_printf_link_args += '-Wl,--defsym=' + vfscanf_symbol + '=' + __f_vfscanf_symbol
    endif
    if format_default != 'integer'
      int_printf_link_args += '-Wl,--defsym=' + vfprintf_symbol +  '=' + __i_vfprintf_symbol
      int_printf_link_args += '-Wl,--defsym=' + vfscanf_symbol + '=' + __i_vfscanf_symbol
    endif
  elif has_link_alias
    if format_default == 'double'
      float_printf_link_args += '-Wl,-alias,' + vfprintf_symbol +  ',' + __d_vfprintf_symbol
      float_printf_link_args += '-Wl,-alias,' + vfscanf_symbol + ',' + __d_vfscanf_symbol
      int_printf_link_args += '-Wl,-alias,' + vfprintf_symbol +  ',' + __d_vfprintf_symbol
      int_printf_link_args += '-Wl,-alias,' + vfscanf_symbol + ',' + __d_vfscanf_symbol
    endif
    if format_default == 'float'
      double_printf_link_args += '-Wl,-alias,' + vfprintf_symbol +  ',' + __f_vfprintf_symbol
      double_printf_link_args += '-Wl,-alias,' + vfscanf_symbol + ',' + __f_vfscanf_symbol
      int_printf_link_args += '-Wl,-alias,' + vfprintf_symbol +  ',' + __f_vfprintf_symbol
      int_printf_link_args += '-Wl,-alias,' + vfscanf_symbol + ',' + __f_vfscanf_symbol
    endif
    if format_default == 'int'
      double_printf_link_args += '-Wl,-alias,' + vfprintf_symbol +  ',' + __i_vfprintf_symbol
      double_printf_link_args += '-Wl,-alias,' + vfscanf_symbol + ',' + __i_vfscanf_symbol
      float_printf_link_args += '-Wl,-alias,' + vfprintf_symbol +  ',' + __i_vfprintf_symbol
      float_printf_link_args += '-Wl,-alias,' + vfscanf_symbol + ',' + __i_vfscanf_symbol
    endif
    if format_default != 'double'
      double_printf_link_args += '-Wl,-alias,' + __d_vfprintf_symbol +  ',' + vfprintf_symbol
      double_printf_link_args += '-Wl,-alias,' + __d_vfscanf_symbol + ',' + vfscanf_symbol
    endif
    if format_default != 'float'
      float_printf_link_args += '-Wl,-alias,' + __f_vfprintf_symbol +  ',' + vfprintf_symbol
      float_printf_link_args += '-Wl,-alias,' + __f_vfscanf_symbol + ',' + vfscanf_symbol
    endif
    if format_default != 'integer'
      int_printf_link_args += '-Wl,-alias,' + __i_vfprintf_symbol +  ',' + vfprintf_symbol
      int_printf_link_args += '-Wl,-alias,' + __i_vfscanf_symbol + ',' + vfscanf_symbol
    endif
  endif
endif

# A bunch of newlib-stdio only options
newlib_io_pos_args = get_option('newlib-io-pos-args')
newlib_io_long_double = get_option('newlib-io-long-double')
newlib_global_stdio_streams = get_option('newlib-global-stdio-streams')
newlib_fvwrite_in_streamio = get_option('newlib-fvwrite-in-streamio')
newlib_fseek_optimization = get_option('newlib-fseek-optimization')
newlib_nano_formatted_io = get_option('newlib-nano-formatted-io')
newlib_io_float = get_option('newlib-io-float')
newlib_stdio64 = get_option('newlib-stdio64')
newlib_wide_orient = get_option('newlib-wide-orient')
newlib_have_fcntl = get_option('newlib-have-fcntl')

# Check for a bunch of newlib-stdio only options and complain
# if they are selected while using tinystdio

if tinystdio
  if newlib_io_pos_args
    error('tinystdio does not support positional arguments (newlib-io-pos-args)')
  endif
  if newlib_io_long_double
    error('tinystdio does not support long double (newlib-io-long-double)')
  endif
  if newlib_io_float
    error('tinystdio uses a run-time mechanism to select floating point io (newlib-io-float)')
  endif
  if newlib_global_stdio_streams
    error('tinystdio always has (reentrant) global stdio streams (newlib-global-stdio-streams)')
  endif
  if newlib_fvwrite_in_streamio
    error('tinystdio has no fvwrite support (newlib-fvwrite-in-streamio)')
  endif
  if newlib_fseek_optimization
    error('tinystdio has no fseek support (newlib-fseek-optimization)')
  endif
  if newlib_nano_formatted_io
    error('tinystdio uses a run-time mechanism to select smaller printf code (newlib-nano-formatted-io)')
  endif
  if newlib_wide_orient
    error('tinystdio does not support the wide-orient option (newlib-wide-orient)')
  endif
else
  if posix_console
    error('newlib stdio does not support the posix-console option (posix-console)')
  endif
  if newlib_io_long_double and not long_double_equals_double and not long_double_mant_is_64
    error('newlib-io-long-double not supported on target')
  endif
endif

if host_cpu_family == ''
  host_cmds = cc.cmd_array() + ['-dumpmachine']
  host_cc_machine=run_command(host_cmds).stdout().strip().split('-')
  host_cpu_family=host_cc_machine[0]
  message('Computed host_cpu_family as ' + host_cpu_family)
endif

if have_alias_attribute_option == 'auto'
  have_alias_attribute = cc.has_function_attribute('alias')
else
  have_alias_attribute = have_alias_attribute_option == 'true'
endif

if have_format_attribute_option == 'auto'
  have_format_attribute = cc.has_function_attribute('format')
else
  have_format_attribute = have_format_attribute_option == 'true'
endif

tls_model_spec = ''
thread_local_storage = false
if get_option('thread-local-storage') == 'auto'
  # We assume that _set_tls() is defined in the arch specific tls.c
  if fs.is_file('newlib/libc/picolib/machine' / host_cpu_family / 'tls.c')
    thread_local_storage = not cc.has_function('__emutls_get_address', args: ['-nostdlib', '-lgcc'])
  endif
else
  thread_local_storage = get_option('thread-local-storage') == 'true'
endif
if thread_local_storage
  tls_model_spec = '%{!ftls-model:-ftls-model=' + get_option('tls-model') + '}'
endif

if sysroot_install
  sysroot = run_command(cc.cmd_array() + ['-print-sysroot']).stdout().split('\n')[0]
  if sysroot == ''
    error('sysroot install requested, but compiler has no sysroot')
  endif
  sysroot = run_command('realpath', sysroot).stdout().split('\n')[0]
  prefix_path = run_command('realpath', prefix).stdout().split('\n')[0]
  if sysroot != prefix_path
    error('sysroot install requires --prefix=' + sysroot)
  endif
endif

lib_dir = join_paths(prefix, get_option('libdir'))

newlib_iconv_dir = get_option('newlib-iconv-dir')
if newlib_iconv_dir == ''
  newlib_iconv_dir = join_paths(lib_dir, 'locale')
endif

newlib_iconv_runtime_dir = get_option('newlib-iconv-runtime-dir')
if newlib_iconv_runtime_dir == ''
  newlib_iconv_runtime_dir = newlib_iconv_dir
endif

specs_dir_option = get_option('specsdir')
if specs_dir_option == ''
  search_cmds = cc.cmd_array() + ['-print-search-dirs']
  install_dir=run_command(search_cmds).stdout().split('\n')[0]
  # Meson 0.56 adds a 'substring' method which can be used here
  specs_dir=run_command(['expr', install_dir, ':', 'install: *\(.*\)']).stdout().split('\n')[0]
  specs_install = specs_dir != ''
elif specs_dir_option == 'none'
  specs_install = false
  specs_dir = ''
else
  specs_dir = join_paths(prefix, specs_dir_option)
  specs_install = true
endif

# When passing the specs file to CompCert, the libcompcert needs to be included there as well
additional_libs = ''
if cc.get_id() == 'ccomp'
  additional_libs = '-lcompcert'
endif

specs_extra = ''
specs_extra_list = meson.get_cross_property('specs_extra', [])
if specs_extra_list != []
  specs_extra = '\n' + '\n'.join(specs_extra_list)
endif

specs_printf = ''
if tinystdio
  specs_printf=('%{DPICOLIBC_FLOAT_PRINTF_SCANF:--defsym=vfprintf=__f_vfprintf}' +
		' %{DPICOLIBC_FLOAT_PRINTF_SCANF:--defsym=vfscanf=__f_vfscanf}' +
		' %{DPICOLIBC_DOUBLE_PRINTF_SCANF:--defsym=vfprintf=__d_vfprintf}' +
		' %{DPICOLIBC_DOUBLE_PRINTF_SCANF:--defsym=vfscanf=__d_vfscanf}' +
		' %{DPICOLIBC_INTEGER_PRINTF_SCANF:--defsym=vfprintf=__i_vfprintf}' +
		' %{DPICOLIBC_INTEGER_PRINTF_SCANF:--defsym=vfscanf=__i_vfscanf}')
endif

specs_data = configuration_data()
specs_data.set('PREFIX', sysroot_install? '%R' : prefix)
specs_data.set('INCLUDEDIR', get_option('includedir'))
specs_data.set('LIBDIR', get_option('libdir'))
specs_data.set('TLSMODEL', tls_model_spec)
specs_data.set('LINK_SPEC', meson.get_cross_property('link_spec', ''))
specs_data.set('CC1_SPEC', meson.get_cross_property('cc1_spec', ''))
specs_data.set('CC1PLUS_SPEC', meson.get_cross_property('cc1plus_spec', ''))
specs_data.set('ADDITIONAL_LIBS', additional_libs)
specs_data.set('SPECS_EXTRA', specs_extra)
specs_data.set('SPECS_PRINTF', specs_printf)

# Create C and C++ specific specs data,
# that includes setting the correct linker script
# and adding the C++ startup/shutdown files

specs_c_data = specs_data

specs_c_data.set('PICOLIBC_LD', 'picolibc.ld')
specs_c_data.set('CRTBEGIN', '')
specs_c_data.set('CRTEND', '')

specs_cpp_data = specs_data

specs_cpp_data.set('PICOLIBC_LD', 'picolibcpp.ld')
specs_cpp_data.set('CRTBEGIN', 'crtbegin%O%s')
specs_cpp_data.set('CRTEND', 'crtend%O%s')

picolibc_specs = configure_file(input: 'picolibc.specs.in',
				output: '@BASENAME@',
				configuration: specs_c_data,
				install_dir: specs_dir,
				install: specs_install)

picolibcpp_specs = configure_file(input: 'picolibc.specs.in',
				  output: 'picolibcpp.specs',
				  configuration: specs_cpp_data,
				  install_dir: specs_dir,
				  install: specs_install)

test_specs = configure_file(input: 'test.specs.in',
			    output: '@BASENAME@',
			    configuration: specs_data,
			    install: false)

picolibc_ld_data = configuration_data()
picolibc_ld_data.set('CPP_START', '/*')
picolibc_ld_data.set('CPP_END', '*/')
picolibc_ld_data.set('C_START', '')
picolibc_ld_data.set('C_END', '')

picolibc_ld = configure_file(input: 'picolibc.ld.in',
			     output: 'picolibc.ld',
			     configuration: picolibc_ld_data,
			     install_dir: lib_dir)

picolibcpp_ld_data = configuration_data()
picolibcpp_ld_data.set('CPP_START', '')
picolibcpp_ld_data.set('CPP_END', '')
picolibcpp_ld_data.set('C_START', '/*')
picolibcpp_ld_data.set('C_END', '*/')

picolibcpp_ld = configure_file(input: 'picolibc.ld.in',
			     output: 'picolibcpp.ld',
			     configuration: picolibcpp_ld_data,
			     install_dir: lib_dir)

# Not all compilers necessarily support all warnings; only use these which are:
c_warnings = ['-Werror=implicit-function-declaration', '-Werror=vla', '-Warray-bounds', '-Wold-style-definition']
disabled_warnings = ['-Wno-missing-braces', '-Wno-implicit-int', '-Wno-return-type', '-Wno-unused-command-line-argument']
c_args += cc.get_supported_arguments(c_warnings, disabled_warnings)

# CompCert does not support bitfields in packed structs, so avoid using this optimization
bitfields_in_packed_structs_code = '''
struct test { int part: 24; } __attribute__((packed));
unsigned int foobar (const struct test *p) { return p->part; }
'''
have_bitfields_in_packed_structs = cc.compiles(bitfields_in_packed_structs_code, name : 'packed structs may contain bitfields')

# CompCert does not have __builtin_mul_overflow
builtin_mul_overflow_code = '''
#include <stddef.h>
int overflows (size_t a, size_t b) { size_t x; return __builtin_mul_overflow(a, b, &x); }
volatile size_t aa = 42;
int main (void) { return overflows(aa, aa); }
'''
have_builtin_mul_overflow = cc.links(builtin_mul_overflow_code, name : 'has __builtin_mul_overflow')

# CompCert does not support _Complex
complex_code = '''
float _Complex test(float _Complex z) { return z; }
'''
have_complex = cc.compiles(complex_code, name : 'supports _Complex')

# CompCert does not have __builtin_expect
builtin_expect_code = '''
volatile int a = 42;
int main (void) {
  return __builtin_expect(a, 1);
}
'''
have_builtin_expect = cc.links(builtin_expect_code, name : 'has __builtin_expect')

# CompCert uses the GCC preprocessor, which causes to
#  > #if __has_attribute(__alloc_size__)
# produce a wrong result. So test if the compiler has that attribute
alloc_size_code = '''
void *foobar(size_t) __attribute__((__alloc_size__(1)));
void *foobar2(size_t, size_t) __attribute__((__alloc_size__(1, 2)));
'''
have_alloc_size = cc.compiles(alloc_size_code, name : 'attribute __alloc_size__', args : (cc.has_argument('-Werror') ? '-Werror' : ''))

# attributes constructor/destructor are a GNU extension - if the compiler doesn't have them, don't test them.
attr_ctor_dtor_code = '''
void __attribute__((constructor(101))) ctor (void) {}
void __attribute__((destructor(101))) dtor (void) {}
'''
have_attr_ctor_dtor = cc.compiles(attr_ctor_dtor_code, name : 'attributes constructor/destructor', args : (cc.has_argument('-Werror') ? '-Werror' : ''))

# Add c_args from the cross file. This
# means that the command line values get
# added to the cross file values instead of
# replacing them.
c_args += meson.get_cross_property('c_args', [])

if enable_multilib
  used_libs = []

  # Ask the compiler for the set of available multilib configurations,
  # set up the build system to compile for all desired ones
  foreach target : run_command(cc, '--print-multi-lib').stdout().strip().split('\n')
    tmp = target.split(';')
    flags = c_args

    # Let the user specify a subset of the possible multilib
    # configurations to build for
    if multilib_list == [] or tmp[0] in multilib_list
      used_libs += tmp[0]
      if tmp.length() > 1
	foreach flag : tmp[1].strip('@').split('@')
	  if flag != ''
	    flags += '-' + flag
	  endif
	endforeach
	if tmp[0] == '.'
	  name = ''
	else
	  name = tmp[0].underscorify()
	endif
      else
	name = ''
      endif
      targets += name

      # rv64 needs to use a non-default mcmodel so that variables can
      # live in a broader range of memory addresses
      if name.startswith('rv64')
	flags += [ '-mcmodel=medany' ]
      endif

      # Add any extra flags for this target from the cross file
      flags += meson.get_cross_property('c_args_' + name, [])

      value = [tmp[0], flags]
      set_variable('target_' + name, value)
    endif
  endforeach

  # Make sure all requested multilib configurations
  # are actually available
  if multilib_list != []
    foreach lib : multilib_list
      if lib not in used_libs
	error('Unavailable multilib: ' + lib)
      endif
    endforeach
  endif
else
  targets = ['']
  target_ = ['.', c_args]
endif

conf_data = configuration_data()

# The supported builtins vary depending on compiler and target.
# If you want to check for a given builtin, add an array
#   ['some_builtin_name', '__call_to_builtin(1,2,3);']
# The below loop will then add the define HAVE_SOME_BUILTIN_NAME if the code snippet
#   > int main (void) { __call_to_builtin(1,2,3); return 0; }
# can be compiled + linked.
# The name should match the builtin, but technically it's not necessary
builtins = [
  ['builtin_alloca', '__builtin_alloca(1)', 'void *'],
  ['builtin_ffs', '__builtin_ffs(42)', 'int'],
  ['builtin_ffsl', '__builtin_ffsl((long)42)', 'long'],
  ['builtin_ffsll', '__builtin_ffsll((long long)42)', 'long long'],
  ['builtin_ctz', '__builtin_ctz((unsigned int)42)', 'int'],
  ['builtin_ctzl', '__builtin_ctzl((unsigned long)42)', 'int'],
  ['builtin_ctzll', '__builtin_ctzll((unsigned long long)42)', 'int'],
  ['builtin_copysignl', '__builtin_copysignl((long double)42, (long double) 42)', 'long double'],
  ['builtin_copysign', '__builtin_copysign(42, 42)', 'double'],
  ['builtin_isinfl', '__builtin_isinfl((long double)42)', 'int'],
  ['builtin_isinf', '__builtin_isinf((long double)42)', 'int'],
  ['builtin_isnanl', '__builtin_isnanl((long double)42)', 'int'],
  ['builtin_isnan', '__builtin_isnan((long double)42)', 'int'],
  ['builtin_finitel', '__builtin_finitel((long double)42)', 'int'],
  ['builtin_isfinite', '__builtin_isfinite((long double)42)', 'int'],
]
foreach builtin : builtins
  builtin_template='''
static int foo(@1@ i __attribute__((unused))) { return 0; }
int main(void) { return foo(@0@); }
'''
  builtin_code = builtin_template.format(builtin[1], builtin[2])
  have_current_builtin = cc.links(builtin_code, name : 'test for __' + builtin[0])
  conf_data.set('HAVE_' + builtin[0].to_upper(), have_current_builtin, description: 'The compiler supports __' + builtin[0])
endforeach

NEWLIB_VERSION='4.1.0'
NEWLIB_MAJOR_VERSION=4
NEWLIB_MINOR_VERSION=1
NEWLIB_PATCHLEVEL_VERSION=0

conf_data.set('_HAVE_CC_INHIBIT_LOOP_TO_LIBCALL',
	      cc.has_argument('-fno-tree-loop-distribute-patterns'),
	      description: 'Compiler flag to prevent detecting memcpy/memset patterns')

conf_data.set('_HAVE_NO_BUILTIN_ATTRIBUTE',
	      cc.compiles('int __attribute__((no_builtin)) foo(int x) { return x + 1; }',
			  name : 'no_builtin attribute',
			  args : (cc.has_argument('-Werror=attributes') ? '-Werror=attributes' : '')),
	      description: 'Compiler attribute to prevent the optimizer from adding new builtin calls')

conf_data.set('_HAVE_LONG_DOUBLE', have_long_double,
	      description: 'Compiler has long double type')
if have_long_double
  conf_data.set('_LDBL_EQ_DBL', long_double_equals_double,
		description: 'long double is the same as double')
endif
conf_data.set('HAVE_ALIAS_ATTRIBUTE', have_alias_attribute)
conf_data.set('HAVE_FORMAT_ATTRIBUTE', have_format_attribute)
conf_data.set('_WANT_REGISTER_FINI', get_option('newlib-register-fini'))
conf_data.set('_WANT_IO_LONG_LONG', io_long_long)
conf_data.set('_WANT_IO_C99_FORMATS', io_c99_formats)
conf_data.set('_IO_FLOAT_EXACT', io_float_exact)
if not tinystdio
  conf_data.set('_WANT_IO_LONG_DOUBLE', newlib_io_long_double)
  conf_data.set('_WANT_IO_POS_ARGS', newlib_io_pos_args)
  conf_data.set('NO_FLOATING_POINT', not newlib_io_float)
  conf_data.set('FLOATING_POINT', newlib_io_float)
  conf_data.set('_WANT_REENT_GLOBAL_STDIO_STREAMS', newlib_global_stdio_streams)
  conf_data.set('__LARGE64_FILES', newlib_stdio64)
endif
conf_data.set('_WANT_REENT_SMALL', get_option('newlib-reent-small'))
conf_data.set('_MB_CAPABLE', newlib_mb)
conf_data.set('_MB_LEN_MAX', newlib_mb ? '8' : '1')
conf_data.set('__SINGLE_THREAD__', get_option('newlib-multithread') == false)
conf_data.set('_ICONV_ENABLE_EXTERNAL_CCS', newlib_iconv_external_ccs)
if newlib_iconv_external_ccs
  conf_data.set_quoted('ICONV_DEFAULT_NLSPATH', newlib_iconv_runtime_dir)
endif
conf_data.set('_ATEXIT_DYNAMIC_ALLOC', newlib_atexit_dynamic_alloc)
conf_data.set('_REENT_GLOBAL_ATEXIT', get_option('newlib-global-atexit'))
conf_data.set('_FVWRITE_IN_STREAMIO', newlib_fvwrite_in_streamio)
conf_data.set('_FSEEK_OPTIMIZATION', newlib_fseek_optimization)
conf_data.set('_WIDE_ORIENT', newlib_wide_orient)
conf_data.set('HAVE_FCNTL', newlib_have_fcntl)
conf_data.set('_NANO_MALLOC', newlib_nano_malloc)
conf_data.set('_UNBUF_STREAM_OPT', get_option('newlib-unbuf-stream-opt'))
conf_data.set('_LITE_EXIT', lite_exit)
conf_data.set('_NANO_FORMATTED_IO', newlib_nano_formatted_io)
conf_data.set('_RETARGETABLE_LOCKING', get_option('newlib-retargetable-locking'))
conf_data.set('TINY_STDIO', tinystdio, description: 'Use tiny stdio from gcc avr')
conf_data.set('_IEEE_LIBM', not get_option('want-math-errno'), description: 'math library does not set errno (offering only ieee semantics)')
conf_data.set('_WANT_MATH_ERRNO', get_option('want-math-errno'), description: 'math library sets errno')
conf_data.set('PREFER_SIZE_OVER_SPEED', get_option('optimization') == 's', description: 'Optimize for space over speed')
conf_data.set('FAST_STRCMP', fast_strcmp, description: 'Always optimize strcmp for performance')
conf_data.set('__HAVE_LOCALE_INFO__', newlib_locale_info, description: 'locale support')
conf_data.set('__HAVE_LOCALE_INFO_EXTENDED__', get_option('newlib-locale-info-extended'), description: 'extended locale support')
conf_data.set('NEWLIB_GLOBAL_ERRNO', get_option('newlib-global-errno'), description: 'use global errno variable')
conf_data.set('HAVE_INITFINI_ARRAY', get_option('newlib-initfini-array'), description: 'compiler supports INIT_ARRAY sections')
conf_data.set('HAVE_INIT_FINI', get_option('newlib-initfini'), description: 'Support _init() and _fini() functions')
conf_data.set('NEWLIB_TLS', thread_local_storage, description: 'use thread local storage')
conf_data.set('PICOLIBC_TLS', thread_local_storage, description: 'use thread local storage')
conf_data.set('HAVE_PICOLIBC_TLS_API', thread_local_storage, description: '_set_tls and _init_tls functions available')
conf_data.set('POSIX_IO', posix_io, description: 'Use open/close/read/write in tinystdio')
conf_data.set('ATOMIC_UNGETC', atomic_ungetc, description: 'Use atomics for fgetc/ungetc for re-entrancy')
conf_data.set('HAVE_BITFIELDS_IN_PACKED_STRUCTS', have_bitfields_in_packed_structs, description: 'Use bitfields in packed structs')
conf_data.set('HAVE_BUILTIN_MUL', have_builtin_mul_overflow, description: 'Compiler has __builtin_mul_overflow')
conf_data.set('HAVE_COMPLEX', have_complex, description: 'Compiler supports _Complex')
conf_data.set('HAVE_BUILTIN_EXPECT', have_builtin_expect, description: 'Compiler has __builtin_expect')
conf_data.set('HAVE_ALLOC_SIZE', have_alloc_size, description: 'The compiler REALLY has the attribute __alloc_size__')
conf_data.set('_HAVE_ATTRIBUTE_ALWAYS_INLINE',
	      cc.has_function_attribute('always_inline'),
	      description: 'The compiler supports the always_inline function attribute')
conf_data.set('_HAVE_ATTRIBUTE_GNU_INLINE',
	      cc.has_function_attribute('gnu_inline'),
	      description: 'The compiler supports the gnu_inline function attribute')

# Obsolete newlib options
conf_data.set('_WANT_USE_LONG_TIME_T', get_option('newlib-long-time_t'), description: 'Obsoleted. Define time_t to long instead of using a 64-bit type')
conf_data.set('REENTRANT_SYSCALLS_PROVIDED', get_option('newlib-reentrant-syscalls-provided'), description: 'Obsoleted. Reentrant syscalls provided for us')
conf_data.set('MISSING_SYSCALL_NAMES', get_option('newlib-missing-syscall-names'), description: 'Obsoleted. Use regular syscalls')

if newlib_obsolete_math == 'auto'
  obsolete_math_value = false
elif newlib_obsolete_math == 'true'
  obsolete_math_value = 1
elif newlib_obsolete_math == 'false'
  obsolete_math_value = 0
endif

if newlib_obsolete_math_float == 'auto'
  obsolete_math_float_value = false
elif newlib_obsolete_math_float == 'true'
  obsolete_math_float_value = 1
elif newlib_obsolete_math_float == 'false'
  obsolete_math_float_value = 0
endif

if newlib_obsolete_math_double == 'auto'
  obsolete_math_double_value = false
elif newlib_obsolete_math_double == 'true'
  obsolete_math_double_value = 1
elif newlib_obsolete_math_double == 'false'
  obsolete_math_double_value = 0
endif

conf_data.set('__OBSOLETE_MATH', obsolete_math_value, description: 'Use old math code (undef auto, 0 no, 1 yes)')
conf_data.set('__OBSOLETE_MATH_FLOAT', obsolete_math_float_value, description: 'Use old math code for float funcs (undef auto, 0 no, 1 yes)')
conf_data.set('__OBSOLETE_MATH_DOUBLE', obsolete_math_double_value, description: 'Use old math code for double funcs (undef auto, 0 no, 1 yes)')

# Check if compiler has -fno-builtin

arg_fnobuiltin = []
if cc.has_argument('-fno-builtin')
  arg_fnobuiltin = ['-fno-builtin']
endif

# Compute iconv encodings to support in the library

# Dig out the list of available encodings from the encoding.aliases file. Only
# accept the first entry from each line

available_encodings = run_command(['sed', '-e', '/^#/d', '-e', '/^$/d', '-e', 's/ .*$//', files('newlib/libc/iconv/encoding.aliases')[0]]).stdout().split('\n')

# Include all available encodings if none were specified on the command line

if newlib_iconv_encodings.length() == 0
  newlib_iconv_encodings = available_encodings
elif newlib_iconv_encodings.length() == 1 and newlib_iconv_encodings[0] == 'none'
  newlib_iconv_encodings = []
endif

# Use newlib_iconv_encodings for from/to by default

if newlib_iconv_from_encodings.length() == 0
  newlib_iconv_from_encodings = newlib_iconv_encodings
endif
if newlib_iconv_to_encodings.length() == 0
  newlib_iconv_to_encodings = newlib_iconv_encodings
endif

# Set config variables for each requested 'from' encoding

foreach from : newlib_iconv_from_encodings
  if not available_encodings.contains(from)
    error('Requested from encoding ' + from + ' is not available. Look in newlib/libc/iconv/encoding.aliases')
  endif
  conf_data.set('_ICONV_FROM_ENCODING_' + from.to_upper(), true)
endforeach

# Set config variables for each requested 'to' encoding

foreach to : newlib_iconv_to_encodings
  if not available_encodings.contains(to)
    error('Requested to encoding ' + to + ' is not available. Look in newlib/libc/iconv/encoding.aliases')
  endif
  conf_data.set('_ICONV_TO_ENCODING_' + to.to_upper(), true)
endforeach

version_array = meson.project_version().split('.')

if version_array.length() > 2
  picolibc_patch_level = version_array[2]
else
  picolibc_patch_level = 0
endif

conf_data.set('__PICOLIBC_VERSION__', '"@0@"'.format(meson.project_version()), description: 'The Picolibc version in string format.')
conf_data.set('__PICOLIBC__', version_array[0], description: 'The Picolibc major version number.')
conf_data.set('__PICOLIBC_MINOR__', version_array[1], description: 'The Picolibc minor version number.')
conf_data.set('__PICOLIBC_PATCHLEVEL__', picolibc_patch_level, description: 'The Picolibc patch level.')

# Old version macros. These have just a single leading underscore,
# which isn't consistent with the newlib macros; they were created
# by mistake.
conf_data.set('_PICOLIBC_VERSION', '"@0@"'.format(meson.project_version()), description: 'The Picolibc version in string format.')
conf_data.set('_PICOLIBC__', version_array[0], description: 'The Picolibc major version number.')
conf_data.set('_PICOLIBC_MINOR__', version_array[1], description: 'The Picolibc minor version number.')

conf_data.set('_NEWLIB_VERSION', '"@0@"'.format(NEWLIB_VERSION), description: 'The newlib version in string format.')
conf_data.set('__NEWLIB__', NEWLIB_MAJOR_VERSION, description: 'The newlib major version number.')
conf_data.set('__NEWLIB_MINOR__', NEWLIB_MINOR_VERSION, description: 'The newlib minor version number.')
conf_data.set('__NEWLIB_PATCHLEVEL__', NEWLIB_PATCHLEVEL_VERSION, description: 'The newlib patch level.')

if tinystdio
  stdio_inc_dir = 'newlib/libc/tinystdio'
else
  stdio_inc_dir = 'newlib/libc/stdio'
endif

inc_dirs = [stdio_inc_dir, '.', 'newlib/libc/include']

inc = include_directories(inc_dirs)

includedir = join_paths(prefix, get_option('includedir'))

# We don't need '-fdata-sections' currently as there aren't any
# files with data used in separate code paths. This works around
# versions of gcc for RISC-V which have a bug that mis-names
# initialized read-only data segments when -fdata-sections
# is defined
arguments = []
if cc.has_argument('-ffunction-sections')
  arguments += ['-ffunction-sections']
endif

if thread_local_storage
  tls_arg = '-ftls-model=' + get_option('tls-model')
  assert(cc.has_argument(tls_arg), 'Compiler does not support \'-ftls-model\'!')
  arguments += [tls_arg]
endif

add_project_arguments(arguments, language: 'c')

# libc will adjust this if supported
has_ieeefp_funcs = false

# semihost will adjust these if supported

# any kind of semihosting support, enough to run general
has_semihost = false

# arm compatible semihosting, enough to run arm semihost tests
has_arm_semihost = false

# some semihost needs bios to run the tests with qemu
bios_bin = []

# make sure to include semihost BEFORE picocrt!
if enable_semihost
  subdir('semihost')
endif
if tinystdio
  subdir('dummyhost')
endif

conf_data.set('HAVE_SEMIHOST', has_semihost, description: 'Semihost APIs supported')

# By default, tests don't require any special arguments

# disable compiler built-ins so we reach the library equivalents
test_c_args = c_args + arg_fnobuiltin
if tests_enable_stack_protector
  if cc.has_argument('-fstack-protector-all') and cc.has_argument('-fstack-protector-strong')
    test_c_args += ['-fstack-protector-all', '-fstack-protector-strong', '-DTESTS_ENABLE_STACK_PROTECTOR']
  else
    tests_enable_stack_protector = false
  endif
endif

# CompCert needs the '-WUl,' prefix to correctly pass the --spec parameters to the linker
specs_prefix = ''
if cc.get_id() == 'ccomp'
  specs_prefix = '-WUl,'
endif

if has_semihost
  # If we're using semihosting, we assume that there's a custom linker
  # script that includes picolibc.specs.  Linker args must be strings
  # (seems like a bug), so compute the path by hand.  Because at the
  # linker's runtime, its working directory is the build dir, linker
  # scripts under scripts/ can't be found. We use the absolute path
  # for our convinience.
  cpu_script = 'scripts' / 'test-' + host_machine.cpu() + '.ld'
  cpu_family_script = 'scripts' / 'test-' + host_cpu_family + '.ld'
  if fs.is_file(cpu_script)
    custom_script = cpu_script
  else
    custom_script = cpu_family_script
  endif
  test_link_args = ['-nostartfiles', '-nostdlib',
                    '-T', meson.current_source_dir() / custom_script,
                    '-T', '@0@'.format(picolibc_ld), '-lgcc']
  test_linker_files = files(custom_script) + [picolibc_ld]
  specs = picolibc_specs
else
  test_link_args = []
  test_linker_files = []
  specs = test_specs
endif

# Clang does not support spec files
specs_line = []
if cc.get_id() != 'clang'
  specs_line += [specs_prefix + '--specs', specs_prefix + meson.current_build_dir() / '@0@'.format(specs)]
endif

test_c_args += specs_line
test_link_args += specs_line
# Make sure all of the tests get re-linked if the linker scripts change.
test_link_depends = test_linker_files + [specs]

# make sure to include semihost BEFORE picocrt!
if enable_picocrt
  subdir('picocrt')
endif
subdir('newlib')

if enable_tests
  subdir('test')
endif

conf_data.set('HAVE_IEEEFP_FUNCS', has_ieeefp_funcs, description: 'IEEE fp funcs available')

configure_file(output : 'picolibc.h',
	       configuration: conf_data,
	       install_dir: includedir)

# Usage as an embedded subproject:
# If picolibc is embedded into the source as a subproject,
# provide a dependency to be used by the main project:
#   dependency('libc', fallback: ['picolibc', 'libc_dep'])
if not enable_multilib and meson.is_subproject()
  picolibc_dep = declare_dependency(include_directories: inc, link_with: [lib_c, lib_m])
endif
