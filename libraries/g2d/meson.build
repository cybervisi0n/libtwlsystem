subdir('src')

libnnsg2d_includes = include_directories('include')

libnnsg2d = library('nnsg2d',
    dependencies: libtwl_dep,
    include_directories: [libtwlsystem_includes, libnnsg2d_includes],
    pic: build_target == 'nx',
    sources: libnnsg2d_srcs,
    native: (build_target == 'linux' or build_target == 'win64')
)

libtwlsystem_libs += libnnsg2d
