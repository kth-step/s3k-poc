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
  subproject('monitor').get_variable('elf'), 
  subproject('app1').get_variable('elf'),
  subproject('app2').get_variable('elf'),
  subproject('app3').get_variable('elf'),
  subproject('app4').get_variable('elf'),
  subproject('s3k').get_variable('elf'),
  ]

qemu_gen = find_program('../scripts/qemu_gen.sh')
qemu = custom_target('qemu',
  output : 'qemu.sh',
  input : elfs,
  command : [qemu_gen, '@OUTPUT@', '@INPUT@'],
  build_by_default: true,
  )

gdb_gen = find_program('../scripts/gdb_gen.sh')
gdb = custom_target('gdb',
  output : 'gdb.sh',
  input : elfs,
  command : [gdb_gen, '@OUTPUT@', '@INPUT@'],
  build_by_default: true,
  )

gdbremote_gen = find_program('../scripts/gdbremote_gen.sh')
gdbremote = custom_target('gdbremote',
  output : 'gdbremote.sh',
  input : elfs,
  command : [gdbremote_gen, '@OUTPUT@', '@INPUT@'],
  build_by_default: true,
  )
