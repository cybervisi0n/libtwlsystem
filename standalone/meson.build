libtwlsystem_link_args = []

libtwlsystem_exe = executable(
    'libtwlsystem',
    sources: files('main.c'),
    dependencies: [ libtwl_dep, libtwlsystem_dep ],
    link_args: [ libtwlsystem_link_args ],
    native: build_target != 'arm',
)
