srcfiles = [
  './aes.c',
  './bin_tree.c',
  './fixed_heap.c',
  './gl4da.c',
  './gl4dfBlur.c',
  './gl4dfCanny.c',
  './gl4dfCommon.c',
  './gl4dfConversion.c',
  './gl4dfFocus.c',
  './gl4dfFractalPainting.c',
  './gl4dfHatching.c',
  './gl4dfMedia.c',
  './gl4dfMedian.c',
  './gl4dfOp.c',
  './gl4dfOpticalFlow.c',
  './gl4dfScattering.c',
  './gl4dfSegmentation.c',
  './gl4dfSobel.c',
  './gl4dg.c',
  './gl4dhAnimeManager.c',
  './gl4dm.c',
  './gl4dp.c',
  './gl4dq.c',
  './gl4droid.c',
  './gl4du.c',
  './gl4dummies.c',
  './gl4duw_SDL2.c',
  './linked_list.c',
  './list.c',
  './vector.c',
]

header_files = [
  './gl4dm.inl',
  './fixed_heap.h',
  './gl4duw_SDL2.h',
  './gl4dq.h',
  './gl4wdummies.h',
  './vector.h',
  './gl4da.h',
  './gl4dg.h',
  './gl4dummies.h',
  './linked_list.h',
  './gl4dfCommon.h',
  './gl4dp.h',
  './gl4du.h',
  './list.h',
  './gl4df.h',
  './gl4dhAnimeManager.h',
  './gl4dm.h',
  './aes.h',
  './gl4dfBlurWeights.h',
  './gl4dh.h',
  './gl4droid.h',
  './bin_tree.h',
]

lib_args = ['-DBUILDING_GL4DUMMIES']

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required: true)
sdl2_dep = dependency('sdl2')
gl_dep = dependency('gl')

shlib = shared_library('GL4Dummies', srcfiles,
                       install: true,
                       c_args: lib_args,
                       dependencies: [m_dep, sdl2_dep, gl_dep])

install_headers(header_files, subdir: 'GL4D')
