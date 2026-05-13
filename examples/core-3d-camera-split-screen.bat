(defglobal
	?*screen-width* = 800
	?*half-screen-width* = (/ ?*screen-width* 2)
	?*screen-height* = 450
	?*texture-width* = (integer ?*half-screen-width*)
	?*spacing* = 4
	?*half-grid* = (* ?*spacing* 5))

(deftemplate camera
	(slot number)
	(slot fovy (default 45.0))
	(slot up.y (default 1.0))
	(slot target.x (default 0.0))
	(slot target.y)
	(slot target.z (default 0.0))
	(slot position.x (default 0.0))
	(slot position.y)
	(slot position.z (default 0.0))
	(slot text)
	(slot text-color)
	(slot x (default 0)))

(deftemplate texture
	(slot number)
	(slot color)
	(multislot pointer (default-dynamic (raylib-load-render-texture ?*texture-width* ?*screen-height*))))

(deffacts cameras
	(camera
		(number 1)
		(position.z -3.0)
		(position.y 1.0)
		(target.y 1.0)
		(text "PLAYER1: W/S to move")
		(text-color MAROON))
	(camera
		(number 2)
		(position.x -3.0)
		(position.y 3.0)
		(target.y 3.0)
		(text "PLAYER2: UP/DOWN to move")
		(text-color DARKBLUE)
		(x ?*half-screen-width*)))

(deffacts textures
	(texture (number 1) (color RED))
	(texture (number 2) (color BLUE)))

(deffacts split-screen-rect
	(split-screen-rect 0.0 0.0 ?*texture-width* (- 1 ?*screen-height*)))

(deffacts init
	(is-key-down KEY_W FALSE)
	(is-key-down KEY_S FALSE)
	(is-key-down KEY_UP FALSE)
	(is-key-down KEY_DOWN FALSE)
	(offset-this-frame -1)
	(should-window-close FALSE))

(raylib-set-target-fps 60)

(defrule is-KEY_W-down
	(offset-this-frame ?offset-this-frame)
	?k <- (is-key-down KEY_W TRUE)
	?c <- (camera
		(number 1)
		(position.z ?position.z)
		(target.z ?target.z))
	=>
	(retract ?k)
	(assert (is-key-down KEY_W FALSE))
	(modify ?c
		(position.z (+ ?position.z ?offset-this-frame))
		(target.z (+ ?target.z ?offset-this-frame))))

(defrule is-KEY_S-down
	(offset-this-frame ?offset-this-frame)
	?k <- (is-key-down KEY_S TRUE)
	?c <- (camera
		(number 1)
		(position.z ?position.z)
		(target.z ?target.z))
	=>
	(retract ?k)
	(assert (is-key-down KEY_S FALSE))
	(modify ?c
		(position.z (- ?position.z ?offset-this-frame))
		(target.z (- ?target.z ?offset-this-frame))))

(defrule is-KEY_UP-down
	(offset-this-frame ?offset-this-frame)
	?k <- (is-key-down KEY_UP TRUE)
	?c <- (camera
		(number 2)
		(position.x ?position.x)
		(target.x ?target.x))
	=>
	(retract ?k)
	(assert (is-key-down KEY_UP FALSE))
	(modify ?c
		(position.x (+ ?position.x ?offset-this-frame))
		(target.x (+ ?target.x ?offset-this-frame))))

(defrule is-KEY_DOWN-down
	(offset-this-frame ?offset-this-frame)
	?k <- (is-key-down KEY_DOWN TRUE)
	?c <- (camera
		(number 2)
		(position.x ?position.x)
		(target.x ?target.x))
	=>
	(retract ?k)
	(assert (is-key-down KEY_DOWN FALSE))
	(modify ?c
		(position.x (- ?position.x ?offset-this-frame))
		(target.x (- ?target.x ?offset-this-frame))))

(defrule draw
	(split-screen-rect ?ssr-x ?ssr-y ?ssr-width ?ssr-height)
	?KEY_W <- (is-key-down KEY_W FALSE)
	?KEY_S <- (is-key-down KEY_S FALSE)
	?KEY_UP <- (is-key-down KEY_UP FALSE)
	?KEY_DOWN <- (is-key-down KEY_DOWN FALSE)
	?o <- (offset-this-frame ?)
	?s <- (should-window-close FALSE)
	=>
	(retract ?KEY_W ?KEY_S ?KEY_UP ?KEY_DOWN ?o ?s)

	(do-for-all-facts ((?t texture) (?c camera) (?tt texture) (?cc camera)) (and (= ?t:number ?c:number) (= ?tt:number ?cc:number) (<> ?c:number ?cc:number))
		(raylib-begin-texture-mode
			(nth$ 1 ?t:pointer)
			(nth$ 2 ?t:pointer)
			(nth$ 3 ?t:pointer)
			(nth$ 4 ?t:pointer)
			(nth$ 5 ?t:pointer)
			(nth$ 6 ?t:pointer)
			(nth$ 7 ?t:pointer)
			(nth$ 8 ?t:pointer)
			(nth$ 9 ?t:pointer)
			(nth$ 10 ?t:pointer)
			(nth$ 11 ?t:pointer))

			(raylib-clear-background SKYBLUE)
			(raylib-begin-mode-3d
				?c:position.x
				?c:position.y
				?c:position.z
				?c:target.x
				?c:target.y
				?c:target.z
				0 ?c:up.y 0
				?c:fovy)

				(raylib-draw-plane 0 0 0 50 50 BEIGE)

				(bind ?start (- 0 ?*half-grid*))
				(bind ?x ?start)
				(while (<= ?x ?*half-grid*)
					(bind ?z ?start)
					(while (<= ?z ?*half-grid*)
						(raylib-draw-cube ?x 1.5 ?z 1    1 1    LIME)
						(raylib-draw-cube ?x 0.5 ?z 0.25 1 0.25 BROWN)
						(bind ?z (+ ?z ?*spacing*)))
					(bind ?x (+ ?x ?*spacing*)))

				(raylib-draw-cube ?c:position.x ?c:position.y ?c:position.z 1 1 1 ?t:color)
				(raylib-draw-cube ?cc:position.x ?cc:position.y ?cc:position.z 1 1 1 ?tt:color)

			(raylib-end-mode-3d)

			(raylib-draw-rectangle 0 0 ?*texture-width* 40 (raylib-fade RAYWHITE 0.8))
			(raylib-draw-text ?c:text 10 10 20 ?c:text-color)
		(raylib-end-texture-mode))

	(raylib-begin-drawing)
		(raylib-clear-background BLACK)
		(do-for-all-facts ((?t texture) (?c camera)) (= ?t:number ?c:number)
			(raylib-draw-texture-rec
				(nth$ 2 ?t:pointer)
				(nth$ 3 ?t:pointer)
				(nth$ 4 ?t:pointer)
				(nth$ 5 ?t:pointer)
				(nth$ 6 ?t:pointer)
				?ssr-x ?ssr-y ?ssr-width ?ssr-height
				?c:x 0
				WHITE))
	(raylib-end-drawing)

	(assert
		(should-window-close (raylib-window-should-close))
		(offset-this-frame (* 10 (raylib-get-frame-time)))
		(is-key-down KEY_W (raylib-is-key-down KEY_W))
		(is-key-down KEY_S (raylib-is-key-down KEY_S))
		(is-key-down KEY_UP (raylib-is-key-down KEY_UP))
		(is-key-down KEY_DOWN (raylib-is-key-down KEY_DOWN))))

(defrule should-window-close
	(should-window-close TRUE)
	(texture
		(number 1)
		(pointer ?id1 ?tid1 ?twidth1 ?theight1 ?tmipmaps1 ?tformat1 ?did1 ?dwidth1 ?dheight1 ?dmipmaps1 ?dformat1))
	(texture
		(number 2)
		(pointer ?id2 ?tid2 ?twidth2 ?theight2 ?tmipmaps2 ?tformat2 ?did2 ?dwidth2 ?dheight2 ?dmipmaps2 ?dformat2))
	=>
	(raylib-unload-render-texture
		?id1 ?tid1 ?twidth1 ?theight1 ?tmipmaps1 ?tformat1 ?did1 ?dwidth1 ?dheight1 ?dmipmaps1 ?dformat1)
	(raylib-unload-render-texture
		?id2 ?tid2 ?twidth2 ?theight2 ?tmipmaps2 ?tformat2 ?did2 ?dwidth2 ?dheight2 ?dmipmaps2 ?dformat2)
	(raylib-close-window))

(raylib-init-window ?*screen-width* ?*screen-height* "raylib [core] example - 3d camera split screen")
(reset)
(run)
(exit)
