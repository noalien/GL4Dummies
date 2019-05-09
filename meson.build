project('GL4Dummies', 'c',
        version: '6.0.0',
        default_options: ['c_std=gnu99', 'warning_level=2'])

lib_args = ['-DBUILDING_GL4DUMMIES']

srcfiles = [
  'lib_src/GL4D/gl4droid.c',
  'lib_src/GL4D/linked_list.c',
  'lib_src/GL4D/gl4dfCommon.c',
  'lib_src/GL4D/gl4dq.c',
  'lib_src/GL4D/gl4dfScattering.c',
  'lib_src/GL4D/fixed_heap.c',
  'lib_src/GL4D/gl4dfSegmentation.c',
  'lib_src/GL4D/gl4dp.c',
  'lib_src/GL4D/gl4dfConversion.c',
  'lib_src/GL4D/gl4da.c',
  'lib_src/GL4D/gl4dfOp.c',
  'lib_src/GL4D/gl4dfMedian.c',
  'lib_src/GL4D/gl4dfBlur.c',
  'lib_src/GL4D/gl4duw_SDL2.c',
  'lib_src/GL4D/gl4dfFractalPainting.c',
  'lib_src/GL4D/gl4dfHatching.c',
  'lib_src/GL4D/gl4dfOpticalFlow.c',
  'lib_src/GL4D/gl4dfFocus.c',
  'lib_src/GL4D/gl4dummies.c',
  'lib_src/GL4D/gl4dg.c',
  'lib_src/GL4D/aes.c',
  'lib_src/GL4D/gl4dfCanny.c',
  'lib_src/GL4D/gl4dhAnimeManager.c',
  'lib_src/GL4D/list.c',
  'lib_src/GL4D/bin_tree.c',
  'lib_src/GL4D/gl4dfMedia.c',
  'lib_src/GL4D/gl4dfSobel.c',
  'lib_src/GL4D/gl4dm.c',
  'lib_src/GL4D/gl4du.c',
  'lib_src/GL4D/vector.c',
]

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required: true)
sdl2_dep = dependency('sdl2')
gl_dep = dependency('gl')

shlib = shared_library('gl4dummies', srcfiles,
  install: true,
  c_args: lib_args,
  gnu_symbol_visibility: 'hidden',
  dependencies: [m_dep, sdl2_dep, gl_dep],
  )

gl4dummies_dep = declare_dependency(
  include_directories: 'lib_src/GL4D/',
  link_with: shlib)

header_files = [
  'lib_src/GL4D/fixed_heap.h',
  'lib_src/GL4D/gl4duw_SDL2.h',
  'lib_src/GL4D/gl4dq.h',
  'lib_src/GL4D/gl4wdummies.h',
  'lib_src/GL4D/vector.h',
  'lib_src/GL4D/gl4da.h',
  'lib_src/GL4D/gl4dg.h',
  'lib_src/GL4D/gl4dummies.h',
  'lib_src/GL4D/linked_list.h',
  'lib_src/GL4D/gl4dfCommon.h',
  'lib_src/GL4D/gl4dp.h',
  'lib_src/GL4D/gl4du.h',
  'lib_src/GL4D/list.h',
  'lib_src/GL4D/gl4df.h',
  'lib_src/GL4D/gl4dhAnimeManager.h',
  'lib_src/GL4D/gl4dm.h',
  'lib_src/GL4D/aes.h',
  'lib_src/GL4D/gl4dfBlurWeights.h',
  'lib_src/GL4D/gl4dh.h',
  'lib_src/GL4D/gl4droid.h',
  'lib_src/GL4D/bin_tree.h',
]

install_headers(header_files, subdir: 'GL4D')

pkg_mod = import('pkgconfig')
pkg_mod.generate(
  name: 'GL4Dummies',
  filebase: 'gl4dummies',
  description: 'GL4Dummies is an API that aims to help C developers to easily produce Multi-platform OpenGL 3.3+ applications.',
  libraries: shlib,
  version: '6.0.0',
  )
