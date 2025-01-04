(deffacts init
	(texture (raylib-load-texture "examples/scarfy.png"))
	(total-frames 6)
	;(texture (raylib-load-texture "examples/spinny-orange.png"))
	;(total-frames 11)
	(current-frame 0)
	(frames-counter 0)
	(frames-speed 8)
	(window-should-close FALSE)
	(increase-frame-speed FALSE)
	(decrease-frame-speed FALSE))

(defrule render
	(total-frames ?total)
	(frames-speed ?frames-speed)
	(current-frame ?current-frame&:(<= ?current-frame ?total))
	(current-x ?x)
	?f <- (frames-counter ?frames-counter&:(< ?frames-counter (/ 60 ?frames-speed)))
	(texture ?id ?width ?height ?mipmaps ?format)
	?w <- (window-should-close FALSE)
	?i <- (increase-frame-speed FALSE)
	?d <- (decrease-frame-speed FALSE)
	=>
	(retract ?f ?w ?i ?d)
	(bind ?start-x (/ (- (raylib-get-screen-width) ?width) 2))
	(raylib-begin-drawing)
		(raylib-clear-background RAYWHITE)
		(raylib-draw-texture ?id ?width ?height ?mipmaps ?format ?start-x 40 WHITE)
		(raylib-draw-rectangle-lines ?start-x 40 ?width ?height LIME)
		(raylib-draw-rectangle-lines (+ ?start-x ?x) 40 (/ ?width ?total) ?height RED)

		(raylib-draw-text "FRAME SPEED: " 165 210 10 DARKGRAY)
		(raylib-draw-text (format nil "%02d FPS" ?frames-speed) 575 210 10 DARKGRAY)
		(raylib-draw-text "PRESS RIGHT/LEFT KEYS to CHANGE SPEED!" 290 240 10 DARKGRAY)
		(loop-for-count (?cnt 0 14) do
			(if (<= (+ ?cnt 1) ?frames-speed) then
				(raylib-draw-rectangle (+ (* ?cnt 21) 250) 205 20 20 RED))
			(raylib-draw-rectangle-lines (+ (* ?cnt 21) 250) 205 20 20 MAROON)
		)
		(raylib-draw-texture-rec
			?id ?width ?height ?mipmaps ?format ; texture
			?x 0 (/ ?width ?total) ?height ; rectangle
			(- (/ (raylib-get-screen-width) 2) (/ (/ ?width ?total) 2)) 280 ; position
			WHITE)

		(raylib-draw-text "(c) Scarfy sprite by Eiden Marsal" (- (raylib-get-screen-width) 200) (- (raylib-get-screen-height) 20) 10 GRAY)

		(assert (frames-counter (+ ?frames-counter 1))
			(window-should-close (raylib-window-should-close))
			(decrease-frame-speed (raylib-is-key-pressed KEY_LEFT))
			(increase-frame-speed (raylib-is-key-pressed KEY_RIGHT)))
	(raylib-end-drawing))

(defrule next-frame
	?c <- (current-frame ?current-frame)
	?x <- (current-x ?)
	(frames-speed ?frames-speed)
	?f <- (frames-counter ?frames-counter&:(>= ?frames-counter (/ 60 ?frames-speed)))
	=>
	(retract ?c ?f ?x)
	(assert (current-frame (+ 1 ?current-frame))
		(frames-counter 0)))

(defrule determine-current-x
	(total-frames ?total)
	(current-frame ?current-frame&~?total)
	(texture ?id ?width ?height ?mipmaps ?format)
	=>
	(assert (current-x (* ?current-frame (/ ?width ?total)))))

(defrule reset-current-frame
	(total-frames ?total)
	?c <- (current-frame ?total)
	=>
	(retract ?c)
	(assert (current-frame 0)))

(defrule increase-frame-speed
	?f <- (increase-frame-speed TRUE)
	?ff <- (frames-speed ?frames-speed&:(< ?frames-speed 15))
	=>
	(retract ?f ?ff)
	(assert (frames-speed (+ ?frames-speed 1))
		(increase-frame-speed FALSE)))

(defrule cannot-increase-frame-speed
	?f <- (increase-frame-speed TRUE)
	(frames-speed 15)
	=>
	(retract ?f)
	(assert (increase-frame-speed FALSE)))

(defrule decrease-frame-speed
	?f <- (decrease-frame-speed TRUE)
	?ff <- (frames-speed ?frames-speed&:(> ?frames-speed 1))
	=>
	(retract ?f ?ff)
	(assert (frames-speed (- ?frames-speed 1))
		(decrease-frame-speed FALSE)))

(defrule cannot-decrease-frame-speed
	?f <- (decrease-frame-speed TRUE)
	(frames-speed 1)
	=>
	(retract ?f)
	(assert (decrease-frame-speed FALSE)))

(defrule die
	(window-should-close TRUE)
	=>
	(println "Done. Exiting...")
	(raylib-close-window)
	(exit))

(raylib-init-window 800 450 "CLIPSraylib [texture] example - sprite anim")
(raylib-set-target-fps 60)
(reset)
(watch all)
(run)
