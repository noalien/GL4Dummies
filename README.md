[![Build Status](https://travis-ci.org/noalien/GL4Dummies.svg?branch=master)](https://travis-ci.org/noalien/GL4Dummies)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/763511e61710449e841821bafbd346e6)](https://www.codacy.com/app/Phundrak/GL4Dummies?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=noalien/GL4Dummies&amp;utm_campaign=Badge_Grade)
[![CodeFactor](https://www.codefactor.io/repository/github/noalien/gl4dummies/badge)](https://www.codefactor.io/repository/github/noalien/gl4dummies)
[![Documentation](http://phundrak.fr/img/docs-doxygen-blue.svg)](http://gl4d.api8.fr/doxygen/html/files.html)

# GL4Dummies
GL4Dummies is a C wrapper around OpenGL that aims to help C developers to easily
produce Multi-platform OpenGL 3.3+ applications.

# Installation

Two different build systems are available for GL4Dummies: the Meson build system
and Autotools. While the former offers a faster compile time and pkg-config
files for an easier setup of your projects using GL4Dummies, its implementation
in this project is still experimental and does not support (yet) Windows builds
–though you can try to make it work, and if you succeed do not hesitate to
submit a pull request!

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

## Meson
To compile this project with project with Meson, you will need to install at
least three packages:
- Python3 and pip3
- Meson
- Ninja

To install Python3 and pip3, follow the instructions of your Linux distribution.
On macOS machines, pip will already be installed if you have already installed
Python3, same goes for Windows. For the former, check that the binaries from pip
are in your Path variable.

First, check which version of pip is the default:
```sh
pip --version
```

If it mentions Python3, you are free to simply use `pip`. Otherwise, verify if
`pip3` is installed, and if yes use it instead. You can install both Meson and
Ninja in one go:
```sh
pip install meson ninja
```
If you experience right issues with the above command, either add the option
`--user` after `install` to install it locally, or re-run the command with
`sudo` if you have the rights to do so and if you wish to install these tools
globally.

Alternatively, you can use your operating system’s package manager if packages
for these tools exist.

### With superuser rights
If you have superuser rights on your machine and you want to install this
project globally, go to the project's root and run the following:
```sh
meson build
cd build
ninja
sudo meson install
```

You will also need to add `/usr/local/lib/pkgconfig` to your `$PKG_CONFIG_PATH`,
often by editing your `.profile`, `.bash_profile`, or `.bashrc`. You can do so
by running the following command (edit it accordingly):
```sh
echo "export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig/" >> ~/.bashrc && source ~/.bashrc
```

If you are using fish, you can run this instead:
```fish
echo "set -gx PKG_CONFIG_PATH /usr/local/lib/pkgconfig/" >> ~/.config/fish/config.fish; and source ~/.config/fish/config.fish
```

### Without superuser rights
If you do not have any superuser rights, or wish to install locally GL4Dummies,
you can instead run the following:
```sh
meson build --prefix /path/to/install/dir
cd build
ninja
meson install
```

You will also need to add `/path/to/install/dir/lib/pkgconfig` to your
`$PKG_CONFIG_PATH`, often by editing your `.profile`, `~./.bash_profile`, or
`~/.bashrc`. You can do so by running the following command (edit it
accordingly):
```sh
echo "export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:/usr/local/lib" >> ~/.bashrc && source ~/.bashrc
```

If you are using fish, you can run this instead:
```fish
echo "set -gx PKG_CONFIG_PATH /path/to/install/dir/lib/pkgconfig/" >> ~/.config/fish/config.fish; and source ~/.config/fish/config.fish
```

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

Under the Windows Operating System, please use CodeBlocks files in the Windows
directory.
