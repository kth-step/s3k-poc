project('s3k-poc', 'c', 
  version : '0.1',
  meson_version : '>=1.1'
  )

s3k = subproject('s3k')
boot = subproject('boot')
comm = subproject('comm')
fcs = subproject('fcs')
mon = subproject('mon')
nav = subproject('nav')

elfs = [
  s3k.get_variable('elf'),
  boot.get_variable('elf'), 
  ]

qemu_gen = find_program('scripts/qemu_gen.sh')
qemu = custom_target('qemu',
  output : 'qemu.sh',
  input : elfs ,
  command : [qemu_gen, '@OUTPUT@', '@INPUT@'],
  build_by_default: true,
  )

