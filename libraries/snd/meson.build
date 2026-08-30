subdir('src')

libnnssnd = library('nnssnd',
    dependencies: libtwl_dep,
    include_directories: libtwlsystem_includes,
    pic: build_target == 'nx',
    sources: libnnssnd_srcs,
    native: (build_target == 'linux' or build_target == 'win64')
)

libtwlsystem_libs += libnnssnd
