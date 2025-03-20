(deffacts cameras
	(camera center)
	(camera center center-inside-map)
	(camera center-inside-map center-smooth-follow)
	(camera center-smooth-follow even-out-landing)
	(camera even-out-landing player-bounds-push)
	(camera player-bounds-push center))

(deffacts surfaces
	(surface 300 200 400 10) ;highest platform
	(surface 250 300 100 10) ;left platform
	(surface 650 300 100 10) ;right platform
	(surface 0 400 1000 200)) ;ground

(deffacts physics
	(gravity 400))

(deftemplate player
	(slot x (default 400))
	(slot y (default 280))
	(slot x-delta (default 0))
	(slot y-delta (default 0))
	(slot walk-speed (default 200.0))
	(slot jump-speed (default 350.0)))

(deffunction c-pressed (?time)
	(raylib-is-key-pressed KEY_C))

(deffunction r-pressed (?time)
	(raylib-is-key-pressed KEY_R))

(deffunction space-pressed (?time)
	(raylib-is-key-down KEY_SPACE))

(deffunction left-pressed (?time)
	(raylib-is-key-down KEY_LEFT))

(deffunction right-pressed (?time)
	(raylib-is-key-down KEY_RIGHT))

;(defrule c-pressed
;	(frame-time ?time)
;	(camera ?current)
;	(camera ?current ?next)
;	(not (camera-next ?next))
;	(test (c-pressed ?time))
;	=>
;	(assert (camera-next ?next)))
;
;(defrule switch-camera
;	(frame-time ?time)
;	?camera <- (camera ?current)
;	(camera ?current ?next)
;	?camera-next <- (camera-next ?next)
;	(test (not (c-pressed ?time)))
;	=>
;	(retract ?camera ?camera-next)
;	(assert (camera ?next)))

(defrule reset-player
	?player <- (player)
	(frame-time ?time)
	(should-exit FALSE)
	(test (r-pressed ?time))
	=>
	(modify ?player
		(x (deftemplate-slot-default-value player x))
		(y (deftemplate-slot-default-value player y))))

(defrule jump
	?player <- (player
		(x ?playerx)
		(y ?playery)
		(y-delta 0)
		(jump-speed ?jump-speed))
	(frame-time ?time)
	(should-exit FALSE)
	(surface ?surfacex ?playery
		?width
			&:(<= ?surfacex ?playerx)
			&:(>= (+ ?surfacex ?width) ?playerx)
		?)
	(not (new-player-y-delta ?))
	(test (space-pressed ?time))
	=>
	(assert
		(new-player-y (+ ?playery (* -1 ?jump-speed ?time)))
		(new-player-y-delta (* -1 ?jump-speed))))

(defrule fall
	?player <- (player
		(x ?playerx)
		(y ?playery)
		(y-delta ?ydelta))
	(frame-time ?time)
	(gravity ?gravity)
	(should-exit FALSE)
	(not 
		(surface ?surfacex
			?surfacey
				&:(>= ?surfacey ?playery)
				&:(<= ?surfacey (+ ?playery (* ?ydelta ?time)))
			?width
				&:(<= ?surfacex ?playerx)
				&:(>= (+ ?surfacex ?width) ?playerx)
			?))
	(not (new-player-y-delta ?))
	=>
	(assert
		(new-player-y (+ ?playery (* ?ydelta ?time)))
		(new-player-y-delta (+ ?ydelta (* ?time ?gravity)))))

(defrule land
	?player <- (player
		(x ?playerx)
		(y ?playery)
		(y-delta ?ydelta&:(> ?ydelta 0)))
	(frame-time ?time)
	(surface ?surfacex
		?surfacey
			&:(>= ?surfacey ?playery)
			&:(<= ?surfacey (+ ?playery (* ?ydelta ?time)))
		?width
			&:(<= ?surfacex ?playerx)
			&:(>= (+ ?surfacex ?width) ?playerx)
		?)
	(should-exit FALSE)
	(not (new-player-y-delta ?))
	=>
	(assert
		(new-player-y ?surfacey)
		(new-player-y-delta 0)))

(defrule on-the-ground
	?player <- (player
		(x ?playerx)
		(y ?playery)
		(y-delta 0))
	(frame-time ?time)
	(surface ?surfacex ?playery
		?width
			&:(<= ?surfacex ?playerx)
			&:(>= (+ ?surfacex ?width) ?playerx)
		?)
	(should-exit FALSE)
	(not (new-player-y-delta ?))
	(test (not (space-pressed ?time)))
	=>
	(assert
		(new-player-y ?playery)
		(new-player-y-delta 0)))

(defrule standing-still
	?player <- (player
		(x ?playerx))
	(frame-time ?time)
	(should-exit FALSE)
	(not (new-player-x ?))
	(test (or
		(and
			(not (left-pressed ?time))
			(not (right-pressed ?time)))
		(and
			(left-pressed ?time)
			(right-pressed ?time))))
	=>
	(assert (new-player-x ?playerx)))

(defrule move-left
	?player <- (player
		(x ?playerx)
		(walk-speed ?walk-speed))
	(frame-time ?time)
	(should-exit FALSE)
	(not (test (right-pressed ?time)))
	(not (new-player-x ?))
	(test (left-pressed ?time))
	=>
	(assert (new-player-x (- ?playerx (* ?time ?walk-speed)))))

(defrule move-right
	?player <- (player
		(x ?playerx)
		(walk-speed ?walk-speed))
	(frame-time ?time)
	(should-exit FALSE)
	(not (test (left-pressed ?time)))
	(not (new-player-x ?))
	(test (right-pressed ?time))
	=>
	(assert (new-player-x (+ ?playerx (* ?time ?walk-speed)))))

(defrule draw-scene-camera-center
	?player <- (player)
	?nx <- (new-player-x ?x)
	?ny <- (new-player-y ?y)
	?nyd <- (new-player-y-delta ?ydelta)
	?f <- (frame-time ?time)
	(camera center)
	(should-exit FALSE)
	=>
	(retract ?f ?nx ?ny ?nyd)
	(modify ?player
		(x ?x)
		(y ?y)
		(y-delta ?ydelta))
	(raylib-begin-drawing)
		(raylib-clear-background LIGHTGRAY)
		(raylib-begin-mode-2d 400 280 (integer ?x) (integer ?y))
			(raylib-draw-rectangle 0 0 1000 400 LIGHTGRAY) ;background
			(do-for-all-facts ((?s surface)) TRUE
				(raylib-draw-rectangle (nth$ 1 ?s:implied) (nth$ 2 ?s:implied) (nth$ 3 ?s:implied) (nth$ 4 ?s:implied) GRAY))

			(raylib-draw-rectangle (- (integer ?x) 20) (- (integer ?y) 40) 40 40 RED) ;player
		(raylib-end-mode-2d)

		(raylib-draw-text "Controls:" 20 20 10 BLACK)
		(raylib-draw-text "- Right/Left to move" 40 40 10 DARKGRAY)
		(raylib-draw-text "- Space to jump" 40 60 10 DARKGRAY)
		(raylib-draw-text "- Mouse Wheel to Zoom in-out (not yet implemented), R to reset character" 40 80 10 DARKGRAY)
		(raylib-draw-text "- C to change camera mode (not yet implemented)" 40 100 10 DARKGRAY)
		(raylib-draw-text "Current camera mode:" 20 120 10 BLACK)
	(raylib-end-drawing)
	(assert
		(frame-time (raylib-get-frame-time))
		(should-exit (raylib-window-should-close))))

(defrule die
	(should-exit TRUE)
	=>
	(println "Done. Exiting...")
	(raylib-close-window)
	(exit))

(reset)
(assert
	(player)
	(frame-time 0)
	(should-exit FALSE))
(watch activations)
(watch rules)
(raylib-init-window 800 450 "2D Camera Platform")
(raylib-set-target-fps 60)
(run)
