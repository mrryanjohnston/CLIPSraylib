(raylib-init-window 800 450 "CLIPSraylib [shapes] example - bouncing ball")
(raylib-set-target-fps 60)
(deffacts init
	(xposition 20.0)
	(yposition 20.0)
	(xspeed 5.0)
	(yspeed 4.0)
	(radius 20)
	(pause FALSE)
	(frames 0)
	(window-should-close FALSE))

(defrule ball-hits-wall
	(xposition ?xposition)
	(radius ?radius)
	?f <- (xspeed ?xspeed)
	(pause FALSE)
	(window-should-close FALSE)
	(test (or
		(and
			(< ?xspeed 0)
			(<= ?xposition ?radius))
		(and
			(> ?xspeed 0)
			(>= ?xposition (- (raylib-get-screen-width) ?radius)))))
	=>
	(retract ?f)
	(assert
		(xspeed (* -1 ?xspeed))))

(defrule ball-hits-ceiling-or-floor
	(yposition ?yposition)
	(radius ?radius)
	?f <- (yspeed ?yspeed)
	(pause FALSE)
	(window-should-close FALSE)
	(test (or
		(and
			(< ?yspeed 0)
			(<= ?yposition ?radius))
		(and
			(> ?yspeed 0)
			(>= ?yposition (- (raylib-get-screen-height) ?radius)))))
	=>
	(retract ?f)
	(assert
		(yspeed (* -1 ?yspeed))))

(defrule render
	?x <- (xposition ?xposition)
	?y <- (yposition ?yposition)
	(xspeed ?xspeed)
	(yspeed ?yspeed)
	(radius ?radius)
	?pause <- (pause FALSE)
	?w <- (window-should-close FALSE)
	(test (and
		(or
			(= ?xspeed 0)
			(and
				(> ?xspeed 0)
				(<= ?xposition (- (raylib-get-screen-width) ?radius)))
			(and
				(< ?xspeed 0)
				(>= ?xposition ?radius)))
		(or
			(= ?yspeed 0)
			(and
				(< ?yspeed 0)
				(>= ?yposition ?radius))
			(and
				(> ?yspeed 0)
				(<= ?yposition (- (raylib-get-screen-height) ?radius))))))
	=>
	(retract ?x ?y ?w ?pause)
	(assert
		(window-should-close (raylib-window-should-close))
		(pause (raylib-is-key-pressed KEY_SPACE))
		(unpause FALSE)
		(xposition (+ ?xposition ?xspeed))
		(yposition (+ ?yposition ?yspeed)))
	(raylib-begin-drawing)
		(raylib-clear-background RAYWHITE)
		(raylib-draw-circle-v ?xposition ?yposition ?radius MAROON)
		(raylib-draw-text "PRESS SPACE to PAUSE BALL MOVEMENT" 10 (- (raylib-get-screen-height) 25) 20 LIGHTGRAY)
		(raylib-draw-fps 10 10)
	(raylib-end-drawing))

(defrule wait
	?f <- (frames ?frames)
	(pause TRUE)
	?unpause <- (unpause FALSE)
	?w <- (window-should-close FALSE)
	=>
	(retract ?f ?unpause ?w)
	(raylib-begin-drawing)
	(if (> (mod (/ ?frames 30) 2) 2) then
		(raylib-draw-text "PAUSED" 350 200 30 GRAY))
	(assert
		(window-should-close (raylib-window-should-close))
		(frames (+ 1 ?frames))
		(unpause (raylib-is-key-pressed KEY_SPACE)))
	(raylib-end-drawing))

(defrule unpause
	?pause <- (pause TRUE)
	?unpause <- (unpause TRUE)
	?w <- (window-should-close FALSE)
	=>
	(retract ?pause ?unpause)
	(assert
		(pause FALSE)
		(window-should-close (raylib-window-should-close))))

(defrule die
	(window-should-close TRUE)
	=>
	(println "Done. Exiting...")
	(raylib-close-window)
	(exit))
(reset)
;(watch all)
(run)
