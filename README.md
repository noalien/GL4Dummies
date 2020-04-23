[![Build Status](https://travis-ci.org/noalien/GL4Dummies.svg?branch=master)](https://travis-ci.org/noalien/GL4Dummies)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/763511e61710449e841821bafbd346e6)](https://www.codacy.com/app/Phundrak/GL4Dummies?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=noalien/GL4Dummies&amp;utm_campaign=Badge_Grade)
[![CodeFactor](https://www.codefactor.io/repository/github/noalien/gl4dummies/badge)](https://www.codefactor.io/repository/github/noalien/gl4dummies)
[![Documentation](http://phundrak.fr/img/docs-doxygen-blue.svg)](http://gl4d.api8.fr/doxygen/html/files.html)

# GL4Dummies
GL4Dummies is a C wrapper around OpenGL that aims to help C developers to easily
produce Multi-platform OpenGL 3.3+ applications.

# Installation

The build systemis based on autotools.

## Dependencies
GL4Dummies has two dependencies: OpenGL and the SDL2. You will at least need to
install the SDL development package to get GL4Dummies running. You can install
it with your favorite package manager if you have access to one. Here are some
common names for the package you will need for the SDL:
- `libsdl2-dev` on Linux distros based on Debian (including Ubuntu)
- `sdl2` on distros based on Arch Linux
- `SDL2-devel` on Void Linux
- `sdl2-dev` on Alpine Linux
- `libsdl2` with MacPorts on macOS
- `sdl2` with HomeBrew on macOS

## GNU Autotools
In order to launch autotools config, type `make -f Makefile.autotools`. The
following packages are required:
- `automake`
- `autoconf`
- `libtool`
- `make`

You can then run the following command:
```sh
make -f Makefile.autotools
```

### With superuser rights
If you have superuser rights and wish to install globally GL4Dummies, you can
run the following commands:
```sh
./configure
make # you can also run `make -j` for parallel compilation
sudo make install
```

You will then need to edit your `.profile`, `.bash_profile`, or your `.bashrc`
and add the following line:
```bash
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```
Or, if you use fish, edit your `~/.config/fish/config.fish` file and add the
following line:
```fish
set -gx LD_LIBRARY_PATH /usr/local/lib $LD_LIBRARY_PATH
```

### Without superuser rights
If you do not have superuser rights on your machine, or if you want to install
GL4Dummies locally, you can run the following instead:
```sh
[ -d $HOME/local ] || mkdir $HOME/local
./configure --prefix=$HOME/local
make
make install
```
You will then need to add to your `.profile`, `.bash_profile`, or `.bashrc` the
following lines:
```bash
export PATH=$HOME/local/bin:$PATH
export LD_LIBRARY_PATH=$HOME/local/lib:$LD_LIBRARY_PATH
```

And if you use fish, you will instead need to add the following lines to your
`~/.config/fish/config.fish` file:
```fish
set -gx PATH $HOME/local/bin $PATH
set -gx LD_LIBRARY_PATH $HOME/local/lib $LD_LIBRARY_PATH
```

## Online instructions
You can also refer to the manual ([PDF](http://gl4d.api8.fr/FR/gl4d.pdf) /
[HTML](http://gl4d.api8.fr/FR/gl4d.html)) for more in-depth instructions (in
French).

Under the Windows Operating System, please use Visual Studion
(Community 2019) or CodeBlocks files in the Windows directory.
