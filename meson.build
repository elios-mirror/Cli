project('elios-cli','cpp',
        version: '0.0.1',
        default_options : ['cpp_std=c++17', 'b_lto=true', 'warning_level=3'],
        license: 'GPLv2')

src = ['src/main.cpp'
]

inc_dir = [include_directories('src')]

cmake = import('cmake')

# Configure the CMake project
sub_proj_yaml = cmake.subproject('yaml-cpp', cmake_options: ['-DYAML_BUILD_SHARED_LIBS=ON', '-DYAML_CPP_BUILD_TESTS=OFF', '-DYAML_CPP_BUILD_TOOLS=OFF'])

yaml_dep = sub_proj_yaml.dependency('yaml-cpp')

cxx = meson.get_compiler('cpp')
curl_dep = [
    cxx.find_library('curl')
]

executable('elios',
        sources : src,
        include_directories: inc_dir,
        install: true,
        dependencies : [yaml_dep, curl_dep]
)
