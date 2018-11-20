tests = executable('StickTests', 'StickTests.cpp', 
    dependencies: [stickDep, threadsDep], 
    include_directories : incDirs,
    cpp_args : '-fsanitize=address',
    link_args : '-fsanitize=address')
test('Stick Tests', tests, workdir: meson.current_build_dir())