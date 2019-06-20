project('GL4Dummies', 'c',
        version: '6.0.0',
        default_options: ['c_std=gnu99', 'warning_level=2', 'optimization=3',
                          'buildtype=release'])

subdir('lib_src')

gl4dummies_dep = declare_dependency(include_directories: 'lib_src/GL4D/',
                                    link_with: shlib)

pkg_mod = import('pkgconfig')
pkg_mod.generate(name: 'GL4Dummies',
                 filebase: 'GL4Dummies',
                 description: 'C wrapper around OpenGL 3.3+',
                 libraries: shlib,
                 version: '6.0.0',
                 requires: ['gl', 'sdl2'])
