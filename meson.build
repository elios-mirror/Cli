project('elios-cli','cpp',
        version: '0.0.1',
        default_options : ['cpp_std=c++17', 'b_lto=true', 'warning_level=3'],
        license: 'GPLv2')

src = ['src/main.cpp'
]

inc_dir = [include_directories('src')]

cmake = import('cmake')

# Configure the CMake project
sub_proj = cmake.subproject('yaml-cpp', cmake_options: ['-DYAML_BUILD_SHARED_LIBS=ON', '-DYAML_CPP_BUILD_TESTS=OFF', '-DYAML_CPP_BUILD_TOOLS=OFF'])

yaml_dep = sub_proj.dependency('yaml-cpp')

executable('elios-cli',
        sources : src,
        include_directories: inc_dir,
        install: true,
        dependencies : yaml_dep
        )
