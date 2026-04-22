(defglobal
	?*bg-color*   = (raylib-color-lerp DARKBLUE BLACK 0.69)
	?*STAR_COUNT* = 420
	?*width*      = 800
	?*height*     = 450
	?*x-min*      = (integer (/ ?*width* -2))
	?*x-max*      = (integer (/ ?*width* 2))
	?*y-min*      = (integer (/ ?*height* -2))
	?*y-max*      = (integer (/ ?*height* 2)))

(deffacts init
	(mouse-wheel-move 0.0)
	(is-spacebar-pressed FALSE)
	(window-should-close (raylib-window-should-close))
	(speed (/ 10 9))
	(draw Lines))

(deftemplate star
	(slot x (default-dynamic (raylib-get-random-value ?*x-min* ?*x-max*)))
	(slot y (default-dynamic (raylib-get-random-value ?*y-min* ?*y-max*)))
	(slot screen-x (default 0))
	(slot screen-y (default 0))
	(slot z (default 1.0))
	(slot dt (default -0))
	(slot t (default -0))
	(slot drawn (default FALSE)))

(defrule adjust-speed-when-mouse-wheel-moved
	?m <- (mouse-wheel-move ?moved&~0.0)
	?s <- (speed ?speed)
	=>
	(retract ?m ?s)
	(assert
		(mouse-wheel-move 0.0)
		(speed (+ ?speed (* 2 (/ ?moved 9))))))

(defrule speed-too-high
	?s <- (speed ?speed&:(> ?speed 2.0))
	=>
	(retract ?s)
	(assert (speed 2.0)))

(defrule speed-too-low
	?s <- (speed ?speed&:(< ?speed 0.0))
	=>
	(retract ?s)
	(assert (speed 0.1)))

(defrule change-to-lines
	?d <- (draw Circles)
	?i <- (is-spacebar-pressed TRUE)
	=>
	(retract ?d ?i)
	(assert
		(draw Lines)
		(is-spacebar-pressed FALSE)))

(defrule change-to-circles
	?d <- (draw Lines)
	?i <- (is-spacebar-pressed TRUE)
	=>
	(retract ?d ?i)
	(assert
		(draw Circles)
		(is-spacebar-pressed FALSE)))

(defrule extinguish-old-star
	?s <- (star (z ?z) (dt ?dt) (screen-x ?star-screen-x&~nil) (screen-y ?star-screen-y&~nil))
	(speed ?speed&:(>= ?speed 0.0)&:(<= ?speed 2.0))
	(test (or
		(< (- ?z (* ?dt ?speed)) 0)
		(< ?star-screen-x 0)
		(< ?star-screen-y 0)
		(> ?star-screen-x ?*width*)
		(> ?star-screen-y ?*height*)))
	=>
	(retract ?s)
	(assert (star)))

(defrule get-frame-time
	(speed ?speed)
	(not (frame-time ?))
	(not (star
		(screen-x ?star-screen-x)
		(screen-y ?star-screen-y)
		(dt ?dt)
		(z ?z&:(or
			(< (- ?z (* ?dt ?speed)) 0)
			(< ?star-screen-x 0)
			(< ?star-screen-y 0)
			(> ?star-screen-x ?*width*)
			(> ?star-screen-y ?*height*)))))
	=>
	(assert (frame-time (raylib-get-frame-time))))

(defrule begin-drawing
	(speed ?speed&:(>= ?speed 0.0)&:(<= ?speed 2.0))
	(frame-time ?dt)
	(draw ?lines-or-circles)
	?i <- (is-spacebar-pressed FALSE)
	?m <- (mouse-wheel-move 0.0)
	?w <- (window-should-close FALSE)
	(not (star (dt ?dt)))
	=>
	(raylib-begin-drawing)
		(raylib-clear-background ?*bg-color*)
		(raylib-draw-text (format nil "[MOUSE WHEEL] Current Speed: %.0f" (/ (* 9.0 ?speed) 2.0)) 10 40 20 RAYWHITE)
		(raylib-draw-text (format nil "[SPACE] Current draw mode: %s" ?lines-or-circles) 10 70 20 RAYWHITE)
		(raylib-draw-fps 10 10))

(defrule update-star
	(speed ?speed)
	(frame-time ?dt)
	?s <- (star (x ?x) (y ?y) (z ?z) (dt ~?dt) (t ?t))
	=>
		(modify ?s
			(z (- ?z (* ?dt ?speed)))
			(screen-x (+ (* 0.5 ?*width*)  (/ ?x ?z)))
			(screen-y (+ (* 0.5 ?*height*) (/ ?y ?z)))
			(dt ?dt)
			(drawn FALSE)
			(t (raylib-clamp (+ ?z (/ 1.0 32.0)) 0.0 1.0))))

(defrule draw-star-line-skip
	(draw Lines)
	(speed ?speed)
	(frame-time ?dt)
	?s <- (star (x ?x) (y ?y) (z ?z) (dt ?dt) (t ?t) (screen-x ?star-screen-x) (screen-y ?star-screen-y) (drawn FALSE))
	(test (<= (- ?t ?z) 1e-3))
	=>
		(modify ?s (drawn TRUE)))

(defrule draw-star-line
	(draw Lines)
	(speed ?speed)
	(frame-time ?dt)
	?s <- (star (x ?x) (y ?y) (z ?z) (dt ?dt) (t ?t) (screen-x ?star-screen-x) (screen-y ?star-screen-y) (drawn FALSE))
	(test (> (- ?t ?z) 1e-3))
	=>
		(raylib-draw-line-v
			(+ (* 0.5 ?*width*)  (/ ?x ?t))
			(+ (* 0.5 ?*height*) (/ ?y ?t))
			?star-screen-x
			?star-screen-y
			RAYWHITE)
		(modify ?s (drawn TRUE)))

(defrule draw-star-circle
	(draw Circles)
	(speed ?speed)
	(frame-time ?dt)
	?s <- (star (x ?x) (y ?y) (z ?z) (dt ?dt) (t ?t) (screen-x ?star-screen-x) (screen-y ?star-screen-y) (drawn FALSE))
	=>
		(raylib-draw-circle-v
			?star-screen-x
			?star-screen-y
			(raylib-lerp ?z 1.0 5.0)
			RAYWHITE)
		(modify ?s (drawn TRUE)))

(defrule end-drawing
	(speed ?speed)
	?f <- (frame-time ?dt)
	?i <- (is-spacebar-pressed FALSE)
	?m <- (mouse-wheel-move 0.0)
	?w <- (window-should-close FALSE)
	(not (star (dt ~?dt)))
	(not (star (drawn FALSE)))
	=>
		(retract ?f ?i ?m ?w)
		(assert
			(mouse-wheel-move (raylib-get-mouse-wheel-move))
			(is-spacebar-pressed (raylib-is-key-pressed KEY_SPACE))
			(window-should-close (raylib-window-should-close)))
	(raylib-end-drawing))

(defrule die
	(window-should-close TRUE)
	=>
	(println "Done. Exiting...")
	(raylib-close-window)
	(exit))

(raylib-init-window 800 450 "CLIPSraylib [shapes] example - starfield effect")
(raylib-set-target-fps 60)
(reset)
(loop-for-count (?cnt 1 ?*STAR_COUNT*) do (assert (star)))
(run)
