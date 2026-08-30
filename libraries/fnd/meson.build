subdir('src')

libnnsfnd_includes = include_directories('include')

libnnsfnd = library('nnsfnd',
    dependencies: libtwl_dep,
    include_directories: [libtwlsystem_includes, libnnsfnd_includes],
    pic: build_target == 'nx',
    sources: libnnsfnd_srcs,
    native: (build_target == 'linux' or build_target == 'win64')
)

libtwlsystem_libs += libnnsfnd
