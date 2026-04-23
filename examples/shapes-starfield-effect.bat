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
	(slot t (default -0)))

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

(defrule extinguish-old-stars
	(speed ?speed&:(>= ?speed 0.0)&:(<= ?speed 2.0))
	(is-spacebar-pressed FALSE)
	(mouse-wheel-move 0.0)
	(window-should-close FALSE)
	(not (frame-time ?))
	(exists (star
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
	(do-for-all-facts ((?s star))
		(or
			(< (- ?s:z (* ?s:dt ?speed)) 0)
			(< ?s:screen-x 0)
			(< ?s:screen-y 0)
			(> ?s:screen-x ?*width*)
			(> ?s:screen-y ?*height*))
		(retract ?s)
		(assert (star))))

(defrule get-frame-time
	(speed ?speed&:(>= ?speed 0.0)&:(<= ?speed 2.0))
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
	(bind ?dt (raylib-get-frame-time))
	(assert (frame-time ?dt))
	(do-for-all-facts ((?s star)) TRUE
		(modify ?s
			(z (- ?s:z (* ?dt ?speed)))
			(screen-x (+ (* 0.5 ?*width*)  (/ ?s:x ?s:z)))
			(screen-y (+ (* 0.5 ?*height*) (/ ?s:y ?s:z)))
			(dt ?dt)
			(t (raylib-clamp (+ ?s:z (/ 1.0 32.0)) 0.0 1.0)))))

(defrule begin-drawing
	(speed ?speed&:(>= ?speed 0.0)&:(<= ?speed 2.0))
	(frame-time ?dt)
	(draw ?lines-or-circles)
	?i <- (is-spacebar-pressed FALSE)
	?m <- (mouse-wheel-move 0.0)
	?w <- (window-should-close FALSE)
	(not (drawing))
	=>
	(raylib-begin-drawing)
		(raylib-clear-background ?*bg-color*)
		(raylib-draw-text (format nil "[MOUSE WHEEL] Current Speed: %.0f" (/ (* 9.0 ?speed) 2.0)) 10 40 20 RAYWHITE)
		(raylib-draw-text (format nil "[SPACE] Current draw mode: %s" ?lines-or-circles) 10 70 20 RAYWHITE)
		(raylib-draw-fps 10 10)
		(assert (drawing)))

(defrule draw-stars-line
	(draw Lines)
	(speed ?speed)
	(frame-time ?dt)
	(drawing)
	=>
		(do-for-all-facts ((?s star)) (> (- ?s:t ?s:z) 1e-3)
			(raylib-draw-line-v
				(+ (* 0.5 ?*width*)  (/ ?s:x ?s:t))
				(+ (* 0.5 ?*height*) (/ ?s:y ?s:t))
				?s:screen-x
				?s:screen-y
				RAYWHITE))
		(assert (done-drawing)))

(defrule draw-stars-circle
	(draw Circles)
	(speed ?speed)
	(frame-time ?dt)
	(drawing)
	=>
		(do-for-all-facts ((?s star)) TRUE
			(raylib-draw-circle-v
				?s:screen-x
				?s:screen-y
				(raylib-lerp ?s:z 1.0 5.0)
				RAYWHITE))
		(assert (done-drawing)))

(defrule end-drawing
	(speed ?speed)
	?d <- (drawing)
	?dd <- (done-drawing)
	?f <- (frame-time ?dt)
	?i <- (is-spacebar-pressed FALSE)
	?m <- (mouse-wheel-move 0.0)
	?w <- (window-should-close FALSE)
	=>
		(retract ?i ?m ?w ?f ?dd ?d)
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
