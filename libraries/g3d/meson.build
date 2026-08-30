subdir('src')

libnnsg3d = library('nnsg3d',
    dependencies: libtwl_dep,
    include_directories: libtwlsystem_includes,
    pic: build_target == 'nx',
    sources: libnnsg3d_srcs,
    native: (build_target == 'linux' or build_target == 'win64')
)

libtwlsystem_libs += libnnsg3d
