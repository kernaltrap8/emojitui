project('emojitui', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

executable('emojitui',
           'src/emojitui.c',
           install : true, link_args : ['-lncurses', '-ltinfo', '-lX11'])
