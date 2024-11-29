;(watch all)
(raylib-init-window 800 400 "Hello, Circles")

(deffunction check-if-should-exit (?time)
	(raylib-window-should-close))

(defrule stay-alive
	?f <- (time ?time)
	(not (test (check-if-should-exit ?time)))
	=>
	(raylib-begin-drawing)
	(raylib-clear-background RED)
	(raylib-draw-circle 100 200 20 GREEN)
	(raylib-draw-circle-lines 80 150 60 GREEN)
	(raylib-draw-circle 150 300 35 SKYBLUE)
	(raylib-draw-circle-lines 180 150 90 SKYBLUE)
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
