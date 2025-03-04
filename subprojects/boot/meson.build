project('boot', 'c', version : '0.1')

libs3k = subproject('libs3k')
libs3k_dep = libs3k.get_variable('libs3k_dep')

subdir('src')

link_args = ['-nostartfiles']

executable('boot.elf', 
  sources : sources,
  link_args : link_args,
  dependencies : [libs3k_dep]
  )
