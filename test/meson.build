#
# SPDX-License-Identifier: BSD-3-Clause
#
# Copyright © 2019 Keith Packard
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
foreach target : targets
  value = get_variable('target_' + target)

  _libs_base = [get_variable('lib_c' + target)]
  if is_variable('lib_semihost' + target)
    _libs_base += [get_variable('lib_semihost' + target)]
  endif

  if is_variable('lib_crt_hosted' + target)
    _libs = [get_variable('lib_crt_hosted'+ target)] + _libs_base
  else
    _libs = _libs_base
  endif

  have_crt_minimal = is_variable('lib_crt_minimal' + target)
  if have_crt_minimal
    _libs_minimal = [get_variable('lib_crt_minimal'+ target)] + _libs_base
  else
    _libs_minimal = _libs_base
  endif

  _c_args = value[1] + test_c_args
  _link_args = value[1] + test_link_args

  t1 = 'printf_scanf'
  if target == ''
    t1_name = t1
  else
    t1_name = t1 + '_' + target
  endif

  test(t1_name,
       executable(t1_name, 'printf_scanf.c',
		  c_args: double_printf_compile_args + _c_args,
		  link_args: double_printf_link_args + _link_args,
		  link_with: _libs,
		  link_depends:  test_link_depends,
		  include_directories: inc),
       depends: bios_bin,
       env: ['MESON_SOURCE_ROOT=' + meson.source_root()])

  t1 = 'printff_scanff'
  if target == ''
    t1_name = t1
  else
    t1_name = t1 + '_' + target
  endif

  test(t1_name,
       executable(t1_name, 'printf_scanf.c',
		  c_args: float_printf_compile_args + _c_args,
		  link_args: float_printf_link_args + _link_args,
		  link_with: _libs,
		  link_depends:  test_link_depends,
		  include_directories: inc),
       depends: bios_bin,
       env: ['MESON_SOURCE_ROOT=' + meson.source_root()])

  t1 = 'printf-tests'
  if target == ''
    t1_name = t1
  else
    t1_name = t1 + '_' + target
  endif

  test(t1_name,
       executable(t1_name, ['printf-tests.c'],
		  c_args: double_printf_compile_args + _c_args,
		  link_args: double_printf_link_args + _link_args,
		  link_with: _libs,
		  include_directories: inc),
       depends: bios_bin,
       env: ['MESON_SOURCE_ROOT=' + meson.source_root()])

  t1 = 'printff-tests'
  if target == ''
    t1_name = t1
  else
    t1_name = t1 + '_' + target
  endif

  test(t1_name,
       executable(t1_name, ['printf-tests.c'],
		  c_args: float_printf_compile_args + _c_args,
		  link_args: float_printf_link_args + _link_args,
		  link_with: _libs,
		  include_directories: inc),
       depends: bios_bin,
       env: ['MESON_SOURCE_ROOT=' + meson.source_root()])

  t1 = 'printfi-tests'
  if target == ''
    t1_name = t1
  else
    t1_name = t1 + '_' + target
  endif

  test(t1_name,
       executable(t1_name, ['printf-tests.c'],
		  c_args: int_printf_compile_args + _c_args,
		  link_args: int_printf_link_args + _link_args,
		  link_with: _libs,
		  include_directories: inc),
       depends: bios_bin,
       env: ['MESON_SOURCE_ROOT=' + meson.source_root()])

  t1 = 'try-ilp32'
  if target == ''
    t1_name = t1
  else
    t1_name = t1 + '_' + target
  endif

  test(t1_name,
       executable(t1_name, ['try-ilp32.c', 'try-ilp32-sub.c'],
		  c_args: _c_args,
		  link_args: _link_args,
		  link_with: _libs,
		  include_directories: inc),
       depends: bios_bin,
       env: ['MESON_SOURCE_ROOT=' + meson.source_root()])

  t1 = 'hosted-exit'
  if target == ''
    t1_name = t1
  else
    t1_name = t1 + '_' + target
  endif

  test(t1_name,
       executable(t1_name, 'hosted-exit.c',
		  c_args: _c_args,
		  link_args:  _link_args,
		  link_with: _libs,
		  include_directories: inc),
       depends: bios_bin,
       env: ['MESON_SOURCE_ROOT=' + meson.source_root()])

  test(t1 + '-fail' + target,
       executable(t1_name + '-fail', 'hosted-exit.c',
		  c_args: _c_args + ['-DRETVAL=1'],
		  link_args:  _link_args,
		  link_with: _libs,
		  include_directories: inc),
       depends: bios_bin,
       env: ['MESON_SOURCE_ROOT=' + meson.source_root()],
       should_fail: true
      )

  t1 = 'abort'
  if target == ''
    t1_name = t1
  else
    t1_name = t1 + '_' + target
  endif

  test(t1_name,
       executable(t1_name, 'abort.c',
		  c_args: _c_args,
		  link_args:  _link_args,
		  link_with: _libs,
		  include_directories: inc),
       depends: bios_bin,
       env: ['MESON_SOURCE_ROOT=' + meson.source_root()],
       should_fail: true
      )

  if have_crt_minimal
    t1 = 'constructor-skip'
    if target == ''
      t1_name = t1
    else
      t1_name = t1 + '_' + target
    endif

    test(t1_name,
	 executable(t1_name, 'constructor-skip.c',
		    c_args: _c_args,
		    link_args:  _link_args,
		    link_with: _libs_minimal,
		    include_directories: inc),
         depends: bios_bin,
	 env: ['MESON_SOURCE_ROOT=' + meson.source_root()])
  endif

  t1 = 'rounding-mode'
  if target == ''
    t1_name = t1
  else
    t1_name = t1 + '_' + target
  endif

  test(t1_name,
       executable(t1_name, ['rounding-mode.c', 'rounding-mode-sub.c'],
		  c_args: _c_args,
		  link_args: _link_args,
		  link_with: _libs,
		  include_directories: inc),
       depends: bios_bin,
       env: ['MESON_SOURCE_ROOT=' + meson.source_root()])

  plain_tests = ['rand', 'regex', 'ungetc', 'fenv',
		 'math_errhandling', 'malloc', 'tls',
		 'ffs', 'setjmp', 'atexit', 'on_exit',
		 'math-funcs', 'timegm', 'time-tests',
                 'test-strtod', 'test-strchr',
		 'test-memset', 'test-put',
		]

  if have_attr_ctor_dtor
    plain_tests += 'constructor'
  endif

  if have_complex
    plain_tests += 'complex-funcs'
  endif

  if newlib_nano_malloc or tests_enable_full_malloc_stress
    plain_tests += 'malloc_stress'
  endif

  if (posix_io or not tinystdio) and tests_enable_posix_io
    plain_tests += 'posix-io'
  endif

  if tests_enable_stack_protector
    plain_tests += 'stack-smash'
  endif

  foreach t1 : plain_tests
    t1_src = t1 + '.c'
    if target == ''
      t1_name = t1
    else
      t1_name = t1 + '_' + target
    endif

    test(t1_name,
	 executable(t1_name, [t1_src],
		    c_args: double_printf_compile_args + _c_args,
		    link_args: double_printf_link_args + _link_args,
		    link_with: _libs,
		    link_depends:  test_link_depends,
		    include_directories: inc),
         depends: bios_bin,
	 env: ['MESON_SOURCE_ROOT=' + meson.source_root()])
  endforeach

endforeach

if enable_native_tests

  native_lib_m = cc.find_library('m', required: false)

  if native_lib_m.found()
    test('math_errhandling_native',
	 executable('math_errhandling_native', 'math_errhandling.c',
		    c_args: ['-DNO_NEWLIB'],
		    dependencies: native_lib_m))
    test('rounding-mode_native',
	 executable('rounding-mode_native',
		    ['rounding-mode.c', 'rounding-mode-sub.c'],
		    c_args: test_c_args + ['-DNO_NEWLIB'],
		    dependencies: native_lib_m))
    test('printf-tests_native',
	 executable('printf-tests_native',
		    'printf-tests.c',
		    c_args: ['-DNO_NEWLIB'],
		    dependencies: native_lib_m))
  endif
endif

subdir('libc-testsuite')

if has_arm_semihost
  subdir('semihost')
endif
