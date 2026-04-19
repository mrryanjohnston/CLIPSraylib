(deftemplate texture
	(slot name)
	(slot position (default 0.0))
	(multislot texture)
	(slot delta)
	(slot ready (default FALSE)))
(deffacts textures
	(texture (name background) (texture (raylib-load-texture "cyberpunk_street_background.png")) (delta 0.1))
	(texture (name foreground) (texture (raylib-load-texture "cyberpunk_street_foreground.png")) (delta 1.0))
	(texture (name midground) (texture (raylib-load-texture "cyberpunk_street_midground.png")) (delta 0.5)))

(defrule update-texture-position
	?t <- (texture (ready FALSE) (position ?position) (delta ?delta) (texture ? ?width ? ? ?))
	(test (>= (- ?position ?delta) (* -2 ?width)))
	=>
	(modify ?t (ready TRUE) (position (- ?position ?delta))))

(defrule reset-texture-position
	?t <- (texture (ready FALSE) (position ?position) (delta ?delta) (texture ? ?width ? ? ?))
	(test (< (- ?position ?delta) (* -2 ?width)))
	=>
	(modify ?t (ready TRUE) (position 0)))

(defrule draw
	?b <- (texture (name background) (ready TRUE) (position ?bposition) (texture ?bid ?bw ?bh ?bm ?bf))
	?f <- (texture (name foreground) (ready TRUE) (position ?fposition) (texture ?fid ?fw ?fh ?fm ?ff))
	?m <- (texture (name midground) (ready TRUE) (position ?mposition) (texture ?mid ?mw ?mh ?mm ?mf))
	=>
	(raylib-begin-drawing)
		(raylib-clear-background (raylib-get-color (hex-string-to-int "0x052c46ff")))

		(raylib-draw-texture-ex ?bid ?bw ?bh ?bm ?bf ?bposition 20 0.0 2.0 WHITE)
		(raylib-draw-texture-ex ?bid ?bw ?bh ?bm ?bf (+ (* 2 ?bw) ?bposition) 20 0.0 2.0 WHITE)

		(raylib-draw-texture-ex ?mid ?mw ?mh ?mm ?mf ?mposition 20 0.0 2.0 WHITE)
		(raylib-draw-texture-ex ?mid ?mw ?mh ?mm ?mf (+ (* 2 ?mw) ?mposition) 20 0.0 2.0 WHITE)

		(raylib-draw-texture-ex ?fid ?fw ?fh ?fm ?ff ?fposition 70 0.0 2.0 WHITE)
		(raylib-draw-texture-ex ?fid ?fw ?fh ?fm ?ff (+ (* 2 ?fw) ?fposition) 70 0.0 2.0 WHITE)


		(raylib-draw-text "BACKGROUND SCROLLING & PARALLAX" 10 10 20 RED)
		(raylib-draw-text "(c) Cyberpunk Street Environment by Luis Zuno (@ansimuz)" 470 430 10 RAYWHITE)
	(raylib-end-drawing)
	(modify ?b (ready FALSE))
	(modify ?f (ready FALSE))
	(modify ?m (ready FALSE)))

(raylib-init-window 800 450 "CLIPSraylib [textures] example - background scrolling")
(raylib-set-target-fps 60)
(reset)
(run)
