project('moosic', 'cpp', default_options: ['cpp_std=c++17', 'warning_level=3', 'werror=true'])

deps = []
deps += dependency('sdl2')
deps += dependency('sdl2_image')
deps += dependency('imgui')
deps += dependency('cpp-httplib')
deps += dependency('jsoncpp')

inc = []

cpp_files = []

subdir('src')

executable('moosic', sources: cpp_files, include_directories: inc, dependencies: deps)
