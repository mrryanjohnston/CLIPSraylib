;(watch all)
(raylib-init-window 800 400 "Hello, mouse")

(deffunction check-if-should-exit (?time)
	(raylib-window-should-close))

(deftemplate mouse
	(multislot delta (default 0.0 0.0))
	(multislot position (default 0.0 0.0)))

(defrule draw-ball-and-get-mouse
	(mouse
		(delta ?dx ?dy)
		(position ?px ?py))
	(not (new-mouse-delta ? ?))
	(not (new-mouse-position ? ?))
	=>
	(bind ?np (raylib-get-mouse-position))
	(assert
		(new-mouse-delta (raylib-get-mouse-delta))
		(new-mouse-position ?np))
	(raylib-begin-drawing)
		(raylib-clear-background RAYWHITE)
		(raylib-draw-circle (integer (nth$ 1 ?np)) (integer (nth$ 2 ?np)) 10 PURPLE)
	(raylib-end-drawing))

(defrule report-mouse-position-and-delta
	?t <- (time ?time)
	?m <- (mouse
		(delta ?dx ?dy)
		(position ?px ?py))
	?d <- (new-mouse-delta ?ndx ?ndy)
	?p <- (new-mouse-position ?npx ?npy)
	(test (or
		(<> ?dx ?ndx)
		(<> ?dy ?ndy)
		(<> ?px ?npx)
		(<> ?py ?npy)))
	(not (test (check-if-should-exit ?time)))
	=>
	(println "Mouse position: " ?npx ", " ?npy)
	(println "Mouse delta: " ?ndx ", " ?ndy)
	(retract ?m ?t ?d ?p)
	(assert
		(time (time))
		(mouse
			(delta ?ndx ?ndy)
			(position ?npx ?npy))))

(defrule mouse-has-not-moved
	?t <- (time ?time)
	(mouse
		(delta ?dx ?dy)
		(position ?px ?py))
	?d <- (new-mouse-delta ?dx ?dy)
	?p <- (new-mouse-position ?px ?py)
	(not (test (check-if-should-exit ?time)))
	=>
	(retract ?t ?d ?p)
	(assert (time (time))))

(defrule die
	(time ?time)
	(test (check-if-should-exit ?time))
	=>
	(raylib-close-window)
	(println "Closing program...")
	(exit))

(assert (time (time)) (mouse))
(run)
