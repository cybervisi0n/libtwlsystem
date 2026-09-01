libtwlsystem_link_args = []

libntr_exe = executable(
    'libntrsystem',
    sources: files('main.c'),
    dependencies: [ libtwl_dep, libtwlsystem_dep ],
    link_args: [ libtwlsystem_link_args ],
    native: build_target != 'arm',
)
