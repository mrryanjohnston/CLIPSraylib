;(watch all)
(raylib-init-window 800 400 "Hello, Rectangles")

(deffunction check-if-should-exit (?time)
	(raylib-window-should-close))

(defrule stay-alive
	?f <- (time ?time)
	(not (test (check-if-should-exit ?time)))
	=>
	(raylib-begin-drawing)
	(raylib-clear-background RED)
	(raylib-draw-rectangle 100 200 20 30 GREEN)
	(raylib-draw-rectangle-lines 80 150 60 90 GREEN)
	(raylib-draw-rectangle 150 300 20 30 SKYBLUE)
	(raylib-draw-rectangle-lines 180 150 60 90 SKYBLUE)
	(raylib-draw-rectangle-rounded 200 200 100 150 200 300 YELLOW)
	(raylib-draw-rectangle-rounded-lines 300 100 100 150 300 10 YELLOW)
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
