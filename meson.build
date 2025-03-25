project('s3k-poc', 'c', 
  version : '0.1',
  meson_version : '>=1.1',
  default_options : [
    'warning_level=2',
    'c_std=gnu11',
    'optimization=s',
    'b_lto=true',
    ]
  )

elfs = [
  subproject('boot').get_variable('elf'), 
  subproject('comm').get_variable('elf'),
  subproject('fcs').get_variable('elf'),
  subproject('nav').get_variable('elf'),
  subproject('s3k').get_variable('elf'),
  ]

qemu_gen = find_program('scripts/qemu_gen.sh')
qemu = custom_target('qemu',
  output : 'qemu.sh',
  input : elfs,
  command : [qemu_gen, '@OUTPUT@', '@INPUT@'],
  build_by_default: true,
  )

gdb_gen = find_program('scripts/gdb_gen.sh')
gdb = custom_target('gdb',
  output : 'gdb.sh',
  input : elfs,
  command : [gdb_gen, '@OUTPUT@', '@INPUT@'],
  build_by_default: true,
  )

gdbremote_gen = find_program('scripts/gdbremote_gen.sh')
gdbremote = custom_target('gdbremote',
  output : 'gdbremote.sh',
  input : elfs,
  command : [gdbremote_gen, '@OUTPUT@', '@INPUT@'],
  build_by_default: true,
  )
