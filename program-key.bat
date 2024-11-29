(raylib-init-window 800 400 "Hello, keyboard")

(deffunction check-if-should-exit (?delta)
	(raylib-window-should-close))

(defrule before
	?f <- (new-spacebar-pressed FALSE)
	(not (test (check-if-should-exit ?f)))
	=>
	(retract ?f)
	(raylib-begin-drawing)
		(raylib-clear-background RAYWHITE)
		(raylib-draw-text "Press the spacebar for as long as you can" 190 200 20 BLACK)
	(raylib-end-drawing)
	(assert (new-spacebar-pressed (raylib-is-key-pressed KEY_SPACE))))

(defrule end-before
	?d <- (new-spacebar-pressed FALSE)
	(test (check-if-should-exit ?d))
	=>
	(println "Exiting early...")
	(raylib-close-window)
	(exit))

(defrule start
	?f <- (new-spacebar-pressed TRUE)
	(not (test (check-if-should-exit ?f)))
	=>
	(assert
		(new-spacebar-hold (raylib-is-key-down KEY_SPACE))
		(began-holding (time))))

(defrule continue
	?d <- (new-spacebar-hold TRUE)
	(began-holding ?began)
	(not (test (check-if-should-exit ?d)))
	=>
	(retract ?d)
	(raylib-begin-drawing)
		(raylib-clear-background GREEN)
		(raylib-draw-text (format nil "Keep holding! Seconds held so far: %f" (- (time) ?began)) 190 200 20 DARKGREEN)
	(raylib-end-drawing)
	(assert (new-spacebar-hold (raylib-is-key-down KEY_SPACE))))

(defrule stopped-holding-spacebar
	?d <- (new-spacebar-hold FALSE)
	(began-holding ?time)
	(not (stopped-holding ?time))
	(not (test (check-if-should-exit ?d)))
	=>
	(assert
		(stopped-holding (time))
		(time (time))))

(defrule report
	(new-spacebar-hold FALSE)
	(began-holding ?began)
	(stopped-holding ?ended)
	?d <- (time ?now)
	(not (test (check-if-should-exit ?d)))
	=>
	(retract ?d)
	(raylib-begin-drawing)
		(raylib-clear-background SKYBLUE)
		(raylib-draw-text (format nil "Nice! You held for %f seconds!" (- ?ended ?began)) 190 200 20 DARKBLUE)
	(raylib-end-drawing)
	(assert (time (time))))

(defrule end-after
	?t <- (time ?time)
	(test (check-if-should-exit ?t))
	=>
	(println "Done. Exiting...")
	(raylib-close-window)
	(exit))

(reset)
(assert (new-spacebar-pressed FALSE))
(run)
