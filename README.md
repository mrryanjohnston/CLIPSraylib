![CLIPSraylib3](https://github.com/user-attachments/assets/092ab906-0266-4462-82dc-9eb335ba2609)

# CLIPSraylib

Using
[CLIPS](https://www.clipsrules.net/)
and
[raylib](https://www.raylib.com/)
to build videogames.

## Description

Provides some very light wrapper functions that expose C-level raylib
functions to a CLIPS rules engine. The wrappers are two files:

- `userfunctions.c` holds every user-defined function (`raylib-init-window`,
  `raylib-draw-text`, ...) and `RaylibUserFunctions`, which registers them.
  It includes `clips.h`.
- `raylib_glue.c` is the raylib side. It includes `raylib.h`, `rlgl.h`,
  `raymath.h` and the bundled `raygui.h`.

`raylib_glue.h` ties them together. raylib and CLIPS both
export a function called `GetFileName`, and instead of patching CLIPS the
raylib side is compiled with that name renamed (see "The rename" below).

See the files in the `examples` directory:

```
textures-fog-of-war.bat
textures-multiple-sprite-anim.bat
textures-sprite-anim.bat
shapes-bouncing-ball.bat
shapes-collision-area.bat
shapes-draw-rectangle-rounded.bat
shapes-logo-raylib-anim.bat
shapes-starfield-effect.bat
program-2d-camera-platformer.bat
program.bat
program-circles.bat
program-flat.bat
program-height-width.bat
program-key.bat
program-rectangles.bat
program-report-mouse.bat
program-report-mouse-no-time.bat
core-3d-camera-split-screen.bat
```

`program-2d-camera-platformer.bat` attempts to recreate
[this example](https://www.raylib.com/examples/core/loader.html?name=core_automation_events)
from the raylib website and looks like this:

![2d-platformer-CLIPSraylib](https://github.com/user-attachments/assets/52a75193-6ff8-40bd-8c70-83ceddcb7b68)

## Building

You need a C compiler, `make`, `curl` or `wget`, and the X11 and OpenGL
development headers. You do not need raylib installed: the build downloads
and builds its own copy. On Ubuntu and Debian the headers are

```
sudo apt install build-essential libgl1-mesa-dev libx11-dev libxrandr-dev \
    libxinerama-dev libxcursor-dev libxi-dev
```

Then:

```
make
```

This fetches the CLIPS 6.4.2 source tarball from SourceForge (checked
against its published SHA-256), fetches the raylib 6.0 source tarball from
GitHub (checked against the SHA-256 pinned in the makefile), builds raylib as
a static library under `vendor/raylib-6.0/`, builds CLIPS under
`vendor/clips-build/6.4.2/` as a `clips` binary in this directory.

The targets:

| Target                    | What it does                                                     |
| ------------------------- | ---------------------------------------------------------------- |
| `make`                    | Build `clips` against the selected CLIPS and raylib               |
| `make test`               | Run the display-free test suite in `tests/`                       |
| `make check-symbols`      | Fail if the CLIPS core and raylib define a function of one name   |
| `make raylib`             | Fetch, verify and build the selected raylib only                  |
| `make print-raylib-ldlibs`| Print the raylib archive and the system libraries it links with   |
| `make clean`              | Remove the build trees, the binary and the raylib archive         |
| `make distclean`          | Also remove the fetched CLIPS and raylib sources                  |
| `make help`               | List the targets and variables                                    |

### Which CLIPS

`CLIPS_VERSION` selects the CLIPS source:

```
make                        # the 6.4.2 release tarball (the default)
make CLIPS_VERSION=svn-6x   # branches/64x of the CLIPS Subversion repository
make CLIPS_VERSION=svn-7x   # branches/70x
```

The two branches are pinned to a revision.
Building against a branch needs `svn` installed. Each version is fetched
into `vendor/clips-source/<tag>` and built in a copy of it under
`vendor/clips-build/<tag>`, so switching versions never downloads again or
mixes one version's objects into another's.

### Which raylib

`RAYLIB_VERSION` selects the raylib source. The makefile holds a table of
the releases it knows, each with the URL of its source tarball and the
SHA-256 of that file, plus a `main` row that clones the git repository:

```
make                          # raylib 6.0 (the default)
make RAYLIB_VERSION=5.5
make RAYLIB_VERSION=main      # a shallow clone of raylib's main branch
```

Each one is fetched and built under `vendor/raylib-<version>/`. A release
that is not in the table can be built by passing its URL and digest:

```
make RAYLIB_VERSION=5.0 \
     RAYLIB_URL=https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz \
     RAYLIB_SHA256=<the digest of that file>
```

The wrappers use raylib 5.5 API, so releases before 5.5 will not compile
without changes. `make print-raylib` says which raylib the build uses and
where it is, and `make print-raylib-ldlibs` prints the link line for it.

If your X11 or OpenGL headers live somewhere the compiler does not look by
default, pass the include path with `RAYLIB_CFLAGS=-I/that/include`; it is
applied to both raylib and `raylib_glue.c`.

### The rename

raylib exports `GetFileName(const char *)` and the CLIPS core exports
`GetFileName(UDFContext *)`. The CLIPS core is never patched. Instead, every
name in the makefile's `RAYLIB_RENAMES` (today just `GetFileName`) is
compiled, in raylib and in `raylib_glue.c`, as `Raylib<name>`, with
`-DGetFileName=RaylibGetFileName`. `make check-symbols` compares the
symbols the CLIPS core defines with every `RLAPI` function raylib declares
and fails on any clash that is not in that list.

## Running the examples

```
./clips -f2 examples/program.bat
```

The examples open a window, so they need a display. The test suite mostly
does not:

```
make test
```

runs each `tests/cases/*.bat` with `clips -f2` and compares its output with
the file of the same name in `tests/expected/`. It covers the functions that
work without a window: `raylib-fade`, `raylib-lerp`, `raylib-clamp`,
`raylib-color-lerp`, `raylib-hex-string-to-int` and
`raylib-get-random-value` (with `raylib-set-random-seed`). One case,
`window-smoke`, opens a window, draws to it and closes it; it runs under
`xvfb-run` if that is installed (`sudo apt install xvfb`), on your display
otherwise, and is skipped when there is neither. The suite runs in CI on
every push, against each CLIPS version and each raylib release the makefile
knows.

This can also be installed via [CLIPSenv](https://github.com/mrryanjohnston/CLIPSenv),
a CLIPS environment manager for local development machines.

## Reading the source code

Start out in `program-flat.bat` as it is the easiest example.
If you run it with `clips -f2 examples/program-flat.bat`, it'll leave CLIPS open
so that you may interact with the environment like so:

```
CLIPS> (raylib-begin-drawing)
CLIPS> (raylib-clear-background SKYBLUE)
CLIPS> (raylib-draw-text "Live interaction with your GUI!" 200 350 20 RED)
CLIPS> (raylib-end-drawing)
```

This should update your window by turning the background sky blue
and writing the text "Live interaction with your GUI!" in red.

# License information for logo

This logo was created in GIMP and is based on the CLIPS logo
and the [raylib logo](https://commons.wikimedia.org/wiki/File:Raylib_logo.png).
The raylib logo is covered by the
[Attribution-ShareAlike 4.0 International](https://creativecommons.org/licenses/by-sa/4.0/deed.en)
license.
