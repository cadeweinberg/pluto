# SPDX-License-Identifier: GPL-3.0-or-later

project('pluto', 'c', 
  default_options : ['c_std=c99']
)

include_dir = include_directories('include')

support = library('pluto-support', 
  [
   'src/support/cli_parser.c',
   'src/support/string_constant.c',
   'src/support/string_view.c'
  ],
  include_directories : [include_dir]
)

executable('charon', 'src/main.c',
    include_directories : include_dir
)

cmocka = dependency('cmocka')

test_clip = executable('test-cli-parser', 
    ['tests/src/support/cli_parser.c'],
    include_directories : [include_dir],
    link_with : [support],
    dependencies: [cmocka]
)
test('cli-parser', test_clip)

test_sv = executable('test-string-view',
  ['tests/src/support/string_view.c'],
  include_directories : [include_dir],
  link_with : [support],
  dependencies : [cmocka]
)
test('string-view', test_sv)

test_sc = executable('test-string-constant',
  ['tests/src/support/string_constant.c'],
  include_directories : [include_dir],
  link_with : [support],
  dependencies : [cmocka]
)
test('string-constant', test_sc)


