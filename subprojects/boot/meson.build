project('boot', 'c', version : '0.1')

libs3k = subproject('libs3k')
libs3k_dep = libs3k.get_variable('libs3k_dep')

libserio = subproject('libserio')
libserio_dep = libserio.get_variable('libserio_dep')

subdir('src')
subdir('platform' / get_option('platform'))

link_args = [
  '-nostartfiles', 
  '-T' + linker_script.full_path()
  ]

elf = executable('boot.elf', 
  sources : sources,
  link_args : link_args,
  dependencies : [libs3k_dep, libserio_dep],
  link_depends : linker_script
  )
