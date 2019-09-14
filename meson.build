project('elios-cli','cpp',
        version: '0.0.1',
        default_options : ['cpp_std=c++2a', 'b_lto=true', 'warning_level=3'],
        license: 'GPLv2')

src = ['src/main.cpp'
]

inc_dir = [include_directories('src')]

cmake = import('cmake')

# Configure the CMake project
sub_proj = cmake.subproject('yaml-cpp')

# Fetch the dependency object
yaml_dep = sub_proj.dependency('yaml-cpp')

executable('elios-cli',
        sources : src,
        include_directories: inc_dir,
        # cpp_args : cpp_args,
        install: true,
        dependencies : yaml_dep
        )
