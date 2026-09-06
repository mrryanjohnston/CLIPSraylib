; raylib-color-lerp: two colours, each a name or a multifield of four
; channels, and a factor in 0.0..1.0.
(println (raylib-color-lerp BLACK WHITE 0.5))
(println (raylib-color-lerp BLACK WHITE 0.0))
(println (raylib-color-lerp BLACK WHITE 1.0))
(println (raylib-color-lerp (create$ 0 0 0 0) (create$ 200 100 50 255) 0.5))
(println (raylib-color-lerp RED (create$ 0 0 0 0) 1.0))
(println (raylib-color-lerp DARKBLUE BLACK 0.0))
; a multifield colour needs exactly four channels
(println (raylib-color-lerp (create$ 1 2 3) BLACK 0.5))
(exit)
