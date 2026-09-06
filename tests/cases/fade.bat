; raylib-fade: a named colour or four channels, and an alpha in 0.0..1.0.
; The result is the colour with its alpha scaled; alpha outside the range
; is clamped.
(println (raylib-fade RED 0.5))
(println (raylib-fade RAYWHITE 1.0))
(println (raylib-fade BLUE 0.0))
(println (raylib-fade 10 20 30 40 0.25))
(println (raylib-fade 10 20 30 40 2.0))
(println (raylib-fade 10 20 30 40 -1.0))
; wrong number of arguments
(println (raylib-fade 1 2 3 4))
(exit)
