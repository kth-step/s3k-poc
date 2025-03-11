project('nav', 'c', version : '0.1')

libs3k_dep = subproject('libs3k').get_variable('dep')
libserio_dep = subproject('libserio').get_variable('dep')

subdir('src')
subdir('platform' / get_option('platform'))

link_args = [
  '-nostartfiles', 
  '-T' + linker_script.full_path()
  ]

elf = executable(meson.project_name() + '.elf', 
  sources : sources,
  link_args : link_args,
  dependencies : [libs3k_dep, libserio_dep],
  link_depends : linker_script
  )
