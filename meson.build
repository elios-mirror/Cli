project('elios-cli','cpp',
        version: '0.0.1',
        default_options : ['cpp_std=c++2a'],
        license: 'GPLv2')

cpp_args = ['-Wall', '-Wextra', '-Wpedantic', '-flto', '-Wimplicit-fallthrough']

# if meson.get_compiler('cpp').get_id() == 'clang'
#   cpp_args += []
# endif
# if meson.get_compiler('cpp').get_id() == 'gcc'
#   cpp_args += ['']
# endif

message('Build Type: ' + get_option('buildtype'))
if get_option('buildtype').startswith('debug')
  cpp_args += ['-g3']
endif
if get_option('buildtype').startswith('release')
  cpp_args += ['-O3', '-frename-registers']
endif

src = ['src/main.cpp'
]

inc_dir = [include_directories('src')]

clara_dep = dependency('clara', fallback : ['clara', 'clara_dep'])

executable('elios-cli',
        sources : src,
        include_directories: inc_dir,
        cpp_args : cpp_args,
        install: true,
        dependencies : clara_dep)
