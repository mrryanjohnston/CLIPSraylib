(raylib-init-window 800 400 "Hello, world")

(deffunction check-if-should-exit (?time)
	(raylib-window-should-close))

(defrule stay-alive
	?f <- (time ?time)
	(not (test (check-if-should-exit ?time)))
	=>
	(raylib-begin-drawing)
	(raylib-clear-background RAYWHITE)
	(raylib-draw-text "Congrats! You created your first window!" 190 200 20 LIGHTGRAY)
	(raylib-end-drawing)
	(retract ?f)
	(assert (time (time))))

(defrule die
	(time ?time)
	(test (check-if-should-exit ?time))
	=>
	(raylib-close-window)
	(println "Closing program...")
	(exit))

(assert (time (time)))
(run)
