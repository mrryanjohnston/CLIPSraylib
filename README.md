# CLIPSraylib

Using
[CLIPS](https://www.clipsrules.net/)
and
[raylib](https://www.raylib.com/)
to build videogames.

![2d-platformer-CLIPSraylib](https://github.com/user-attachments/assets/52a75193-6ff8-40bd-8c70-83ceddcb7b68)

## Description

Provides some very light wrapper functions in `userfunctions.c`
that expose C-level raylib functions to a CLIPS rules engine.
See the files in the `examples` directory:

```
shapes-bouncing-ball.bat
shapes-collision-area.bat
shapes-draw-rectangle-rounded.bat
shapes-logo-raylib-anim.bat
program-2d-camera-platformer.bat
program.bat
program-circles.bat
program-flat.bat
program-height-width.bat
program-key.bat
program-rectangles.bat
program-report-mouse.bat
program-report-mouse-no-time.bat
```

`program-2d-camera-platformer.bat` attempts to recreate
[this example](https://www.raylib.com/examples/core/loader.html?name=core_automation_events)
from the raylib website.

## Running the examples

This installation process assumes you already have `raylib` installed on your computer
and available from within C files like so:

```c
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
```

This is based on of CLIPS version 6.4.1, and can be installed just as vanilla CLIPS
with `make`. This will build a `clips` binary in this directory. You may then run
the examples like so:

```
./clips -f2 examples/program.bat
```

You may use `make clean` to tidy up and run `make` again.

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
