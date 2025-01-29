(deftemplate texture
	(slot id)
	(slot width)
	(slot height)
	(slot mipmaps)
	(slot format)
	(slot total-frames)
	(slot frames-counter (default 0))
	(slot current-frame (default 0))
	(slot frames-speed (default 8))
	(slot current-x (default -1))
	(slot x (default -1))
	(slot increase-frame-speed (default FALSE))
	(slot decrease-frame-speed (default FALSE)))

(deffacts init
	(frame 0)
	(total-width-of-textures 0)
	(number-of-textures 0)
	(texture-path "examples/scarfy.png" 6)
	(texture-path "examples/spinny-orange.png" 11)
	(texture-path "examples/blobbyboiwalking.png" 7)
	(texture-path "examples/purple hero running.png" 13)
	(window-should-close FALSE)
	(mouse-position -1 -1))

(defrule texture-path-to-texture
	?fff <- (total-width-of-textures ?tw)
	?ff <- (number-of-textures ?number)
	?f <- (texture-path ?path ?total-frames)
	=>
	(retract ?fff ?ff ?f)
	(bind ?texture (raylib-load-texture ?path))
	(assert
		(number-of-textures (+ 1 ?number))
		(total-width-of-textures (+ ?tw (/ (nth$ 2 ?texture) ?total-frames)))
		(texture
			(id (nth$ 1 ?texture))
			(width (nth$ 2 ?texture))
			(height (nth$ 3 ?texture))
			(mipmaps (nth$ 4 ?texture))
			(format (nth$ 5 ?texture))
			(total-frames ?total-frames))))

(defrule current-texture
	?t <- (texture)
	(not (current-texture ?))
	=>
	(assert (current-texture ?t)))

(defrule render
	(total-width-of-textures ?tw)
	(number-of-textures ?number-of-textures)
	(window-should-close FALSE)
	?t <- (texture
		(id ?id)
		(width ?width)
		(height ?height)
		(mipmaps ?mipmaps)
		(format ?format)
		(total-frames ?total-frames)
		(frames-speed ?frames-speed)
		(current-x ?current-x))
	(current-texture ?t)
	?m <- (mouse-position ? ?)
	?f <- (frame ?frame)
	(not (texture-path ? ?))
	(forall
		(texture
			(current-frame ?current-frame)
			(total-frames ?tf)
			(frames-speed ?fs)
			(frames-counter ?frames-counter)
			(increase-frame-speed ?i)
			(decrease-frame-speed ?d)
			(current-x ?x))
		(test (<> ?x -1))
		(test (<= ?current-frame ?tf))
		(test (< ?frames-counter (/ 60 ?fs)))
		(test (eq FALSE ?i ?d)))
	=>
	(retract ?f ?m)
	(bind ?start-x (/ (- (raylib-get-screen-width) ?width) 2))
	(raylib-begin-drawing)
		(raylib-clear-background RAYWHITE)
		(raylib-draw-texture ?id ?width ?height ?mipmaps ?format ?start-x 40 WHITE)
		(raylib-draw-rectangle-lines ?start-x 40 ?width ?height LIME)
		(raylib-draw-rectangle-lines (+ ?start-x ?current-x) 40 (/ ?width ?total-frames) ?height RED)

		(raylib-draw-text "FRAME SPEED: " 665 210 10 DARKGRAY)
		(raylib-draw-text (format nil "%02d FPS" ?frames-speed) 1075 210 10 DARKGRAY)
		(raylib-draw-text "PRESS RIGHT/LEFT KEYS to CHANGE SPEED!" 790 240 10 DARKGRAY)
		(raylib-draw-text "LEFT CLICK MOUSE to CHANGE CURRENT TEXTURE!" 790 270 10 DARKGRAY)
		(loop-for-count (?cnt 0 14) do
			(if (<= (+ ?cnt 1) ?frames-speed) then
				(raylib-draw-rectangle (integer (+ (* ?cnt 21) 750)) 205 20 20 RED))
			(raylib-draw-rectangle-lines (integer (+ (* ?cnt 21) 750)) 205 20 20 MAROON)
		)
		(bind ?start-x (/ (- (raylib-get-screen-width) ?tw) 2))
		(do-for-all-facts ((?texture texture)) TRUE
			(if (eq ?texture ?t)
				then
				(raylib-draw-rectangle-lines ?start-x 280 (/ ?texture:width ?texture:total-frames) ?texture:height RED)
				(modify ?texture
					(decrease-frame-speed (raylib-is-key-pressed KEY_LEFT))
					(increase-frame-speed (raylib-is-key-pressed KEY_RIGHT)))
				else
				(raylib-draw-rectangle-lines ?start-x 280 (/ ?texture:width ?texture:total-frames) ?texture:height LIGHTGRAY)
			)
			(raylib-draw-texture-rec
				?texture:id ?texture:width ?texture:height ?texture:mipmaps ?texture:format
				?texture:current-x 0 (/ ?texture:width ?texture:total-frames) ?texture:height
				?start-x 280
				WHITE)
			(modify ?texture
				(x ?start-x)
				(current-x -1)
				(frames-counter (+ ?texture:frames-counter 1)))
			(bind ?start-x (+ ?start-x (/ ?texture:width ?texture:total-frames)))
		)

		(raylib-draw-text "(c) Scarfy sprite by Eiden Marsal" (- (raylib-get-screen-width) 200) (- (raylib-get-screen-height) 20) 10 GRAY)

		(assert
			(frame (+ ?frame 1))
			(window-should-close (raylib-window-should-close))
			(change-current-texture (raylib-is-mouse-button-pressed MOUSE_BUTTON_LEFT))
			(mouse-position (raylib-get-mouse-position)))
	(raylib-end-drawing))

(defrule change-current-texture
	?f <- (texture
		(x ?x)
		(width ?width)
		(height ?height)
		(total-frames ?t))
	(mouse-position ?mouse-x ?mouse-y)
	?ct <- (current-texture ?)
	?cct <- (change-current-texture TRUE)
	(test (raylib-check-collision-point-rec ?mouse-x ?mouse-y ?x 280 (/ ?width ?t) ?height))
	=>
	(retract ?ct ?cct)
	(assert
		(current-texture ?f)
		(change-current-texture FALSE)))

(defrule misclick
	(mouse-position ?mouse-x ?mouse-y)
	?cct <- (change-current-texture TRUE)
	(forall
		(texture
			(x ?x)
			(width ?width)
			(height ?height)
			(total-frames ?t))
		(test (not (raylib-check-collision-point-rec ?mouse-x ?mouse-y ?x 280 (/ ?width ?t) ?height))))
	=>
	(retract ?cct)
	(assert (change-current-texture FALSE)))

(defrule next-frame
	?f <- (texture
		(current-frame ?current-frame)
		(frames-speed ?frames-speed)
		(frames-counter ?frames-counter&:(>= ?frames-counter (/ 60 ?frames-speed))))
	=>
	(modify ?f
		(frames-counter 0)
		(current-frame (+ 1 ?current-frame))))

(defrule determine-current-x
	?f <- (texture
		(total-frames ?total)
		(current-frame ?current-frame&~?total)
		(width ?width)
		(current-x -1))
	=>
	(modify ?f (current-x (* ?current-frame (/ ?width ?total)))))

(defrule reset-current-frame
	?f <- (texture
		(total-frames ?total)
		(current-frame ?total))
	=>
	(modify ?f
		(current-frame 0)))

(defrule increase-frame-speed
	?f <- (texture
		(increase-frame-speed TRUE)
		(frames-speed ?frames-speed&:(< ?frames-speed 15)))
	=>
	(modify ?f
		(frames-speed (+ ?frames-speed 1))
		(increase-frame-speed FALSE)))

(defrule cannot-increase-frame-speed
	?f <- (texture
		(increase-frame-speed TRUE)
		(frames-speed 15))
	=>
	(modify ?f (increase-frame-speed FALSE)))

(defrule decrease-frame-speed
	?f <- (texture
		(decrease-frame-speed TRUE)
		(frames-speed ?frames-speed&:(> ?frames-speed 1)))
	=>
	(modify ?f
		(frames-speed(- ?frames-speed 1))
		(decrease-frame-speed FALSE)))

(defrule cannot-decrease-frame-speed
	?f <- (texture
		(decrease-frame-speed TRUE)
		(frames-speed 1))
	=>
	(modify ?f (decrease-frame-speed FALSE)))

(defrule die
	(window-should-close TRUE)
	=>
	(println "Done. Exiting...")
	(raylib-close-window)
	(exit))

(raylib-init-window 1800 450 "CLIPSraylib [texture] example - sprite anim")
(raylib-set-target-fps 60)
(watch all)
(reset)
(run)
