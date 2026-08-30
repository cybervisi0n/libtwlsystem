subdir('src')

libnnsmcs_includes = include_directories('include')

libnnsmcs = library('nnsmcs',
    dependencies: libtwl_dep,
    include_directories: [libtwlsystem_includes, libnnsmcs_includes],
    pic: build_target == 'nx',
    sources: libnnsmcs_srcs,
    native: (build_target == 'linux' or build_target == 'win64')
)

libtwlsystem_libs += libnnsmcs
