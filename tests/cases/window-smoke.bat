; A window is opened, drawn to once and closed. This is the one case that
; needs a display; tests/run.sh runs it under xvfb-run when that is
; installed, on DISPLAY otherwise, and skips it when there is neither.
; raylib's INFO: lines on stdout are machine-specific and are stripped by
; the runner before the comparison.
(raylib-init-window 320 200 "CLIPSraylib smoke test")
(println (raylib-get-screen-width) " " (raylib-get-screen-height))
(println (raylib-window-should-close))
(raylib-begin-drawing)
(raylib-clear-background RAYWHITE)
(raylib-draw-text "Hello, world!" 10 10 20 BLACK)
(raylib-draw-rectangle 10 40 100 50 (raylib-fade RED 0.5))
(raylib-draw-circle-v 200.0 100.0 30.0 BLUE)
(raylib-end-drawing)
(println (raylib-check-collision-recs 0 0 10 10 5 5 10 10))
(println (raylib-is-key-down KEY_A))
(raylib-close-window)
(println "closed")
(exit)
