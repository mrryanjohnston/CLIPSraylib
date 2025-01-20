(deffacts pixels
	(map-tile-size 32)
	(player-size 16)
	(player-tile-visibility 2))

(deffacts map
	(tiles-x 25)
	(tiles-y 15))

(deffacts player
	(player-position 180 130))

(deffacts init
	(texture (raylib-load-render-texture 25 15))
	(window-should-close FALSE)
	(KEY_RIGHT FALSE)
	(KEY_LEFT FALSE)
	(KEY_DOWN FALSE)
	(KEY_UP FALSE))

(raylib-init-window 800 450 "CLIPSraylib [textures] example - fog of war")
(raylib-set-target-fps 60)

(defrule init
	(declare (salience 1))
	(texture ? ?id ?width ?height ?mipmaps ?format ? ? ? ? ?)
	=>
	(raylib-set-texture-filter
		?id ?width ?height ?mipmaps ?format TEXTURE_FILTER_BILINEAR))

(defrule determine-player-tile-x-y
	(map-tile-size ?MAP_TILE_SIZE)
	(player-position ?x ?y)
	=>
	(assert
		(player-tile-x (integer (/ (+ ?x (/ ?MAP_TILE_SIZE 2)) ?MAP_TILE_SIZE)))
		(player-tile-y (integer (/ (+ ?y (/ ?MAP_TILE_SIZE 2)) ?MAP_TILE_SIZE)))))

(defrule move-right
	(tiles-x ?tiles-x)
	(map-tile-size ?map-tile-size)
	(player-size ?player-size)
	?f <- (KEY_RIGHT TRUE)
	?p <- (player-position ?x&:(< (+ ?x ?player-size) (* ?tiles-x ?map-tile-size)) ?y)
	?ptx <- (player-tile-x ?)
	?pty <- (player-tile-y ?)
	=>
	(retract ?f ?p ?ptx ?pty)
	(assert
		(KEY_RIGHT FALSE)
		(player-position (+ 5 ?x) ?y)))

(defrule cannot-move-right
	(tiles-x ?tiles-x)
	(map-tile-size ?map-tile-size)
	(player-size ?player-size)
	?f <- (KEY_RIGHT TRUE)
	?p <- (player-position ?x&:(>= (+ ?x ?player-size) (* ?tiles-x ?map-tile-size)) ?y)
	?ptx <- (player-tile-x ?)
	?pty <- (player-tile-y ?)
	=>
	(retract ?f ?p ?ptx ?pty)
	(assert
		(KEY_RIGHT FALSE)
		(player-position (* ?tiles-x ?map-tile-size) ?y)))

(defrule move-left
	(tiles-x ?tiles-x)
	(map-tile-size ?map-tile-size)
	(player-size ?player-size)
	?f <- (KEY_LEFT TRUE)
	?p <- (player-position ?x&:(> (+ ?x ?player-size) 0) ?y)
	?ptx <- (player-tile-x ?)
	?pty <- (player-tile-y ?)
	=>
	(retract ?f ?p ?ptx ?pty)
	(assert
		(KEY_LEFT FALSE)
		(player-position (- ?x 5) ?y)))

(defrule cannot-move-left
	(tiles-x ?tiles-x)
	(map-tile-size ?map-tile-size)
	(player-size ?player-size)
	?f <- (KEY_LEFT TRUE)
	?p <- (player-position ?x&:(<= (+ ?x ?player-size) 0) ?y)
	?ptx <- (player-tile-x ?)
	?pty <- (player-tile-y ?)
	=>
	(retract ?f ?p ?ptx ?pty)
	(assert
		(KEY_LEFT FALSE)
		(player-position 0 ?y)))

(defrule move-down
	(tiles-y ?tiles-y)
	(map-tile-size ?map-tile-size)
	(player-size ?player-size)
	?f <- (KEY_DOWN TRUE)
	?p <- (player-position ?x ?y&:(< (+ ?y ?player-size) (* ?tiles-y ?map-tile-size)))
	?ptx <- (player-tile-x ?)
	?pty <- (player-tile-y ?)
	=>
	(retract ?f ?p ?ptx ?pty)
	(assert
		(KEY_DOWN FALSE)
		(player-position ?x (+ 5 ?y))))

(defrule cannot-move-down
	(tiles-y ?tiles-y)
	(map-tile-size ?map-tile-size)
	(player-size ?player-size)
	?f <- (KEY_DOWN TRUE)
	?p <- (player-position ?x ?y&:(>= (+ ?y ?player-size) (* ?tiles-y ?map-tile-size)))
	?ptx <- (player-tile-x ?)
	?pty <- (player-tile-y ?)
	=>
	(retract ?f ?p ?ptx ?pty)
	(assert
		(KEY_DOWN FALSE)
		(player-position ?x (* ?tiles-y ?map-tile-size))))

(defrule move-up
	(tiles-y ?tiles-y)
	(player-size ?player-size)
	?f <- (KEY_UP TRUE)
	?p <- (player-position ?x ?y&:(> (+ ?y ?player-size) 0))
	?ptx <- (player-tile-x ?)
	?pty <- (player-tile-y ?)
	=>
	(retract ?f ?p ?ptx ?pty)
	(assert
		(KEY_UP FALSE)
		(player-position ?x (- ?y 5))))

(defrule cannot-move-up
	(tiles-y ?tiles-y)
	(player-size ?player-size)
	?f <- (KEY_UP TRUE)
	?p <- (player-position ?x ?y&:(<= (+ ?y ?player-size) 0))
	?ptx <- (player-tile-x ?)
	?pty <- (player-tile-y ?)
	=>
	(retract ?f ?p ?ptx ?pty)
	(assert
		(KEY_UP FALSE)
		(player-position ?x 0)))

(defrule set-unseen-tile-to-seen
	?f <- (tile ?x ?y ?blue ~1)
	(player-tile-visibility ?player-tile-visibility)
	(player-tile-y ?player-tile-y)
	(player-tile-x ?player-tile-x)
	(and
		(test	(>= ?y (- ?player-tile-y ?player-tile-visibility)))
		(test	(<= ?y (+ ?player-tile-y ?player-tile-visibility)))
		(test	(>= ?x (- ?player-tile-x ?player-tile-visibility)))
		(test	(<= ?x (+ ?player-tile-x ?player-tile-visibility))))
	=>
	(retract ?f)
	(assert (tile ?x ?y ?blue 1)))

(defrule set-previous-seen-tile-to-partial-fog
	?f <- (tile ?x ?y ?blue 1)
	(player-tile-visibility ?player-tile-visibility)
	(player-tile-y ?player-tile-y)
	(player-tile-x ?player-tile-x)
	(or
		(test	(< ?y (- ?player-tile-y ?player-tile-visibility)))
		(test	(> ?y (+ ?player-tile-y ?player-tile-visibility)))
		(test	(< ?x (- ?player-tile-x ?player-tile-visibility)))
		(test	(> ?x (+ ?player-tile-x ?player-tile-visibility))))
	=>
	(retract ?f)
	(assert (tile ?x ?y ?blue 2)))

(defrule render
	(texture ?id ?tid ?twidth ?theight ?tmipmaps ?tformat ?did ?dwidth ?dheight ?dmipmaps ?dformat)
	(map-tile-size ?MAP_TILE_SIZE)
	(player-size ?player-size)
	(player-position ?player-position-x ?player-position-y)
	(player-tile-visibility ?player-tile-visibility)
	(tiles-x ?tiles-x)
	(tiles-y ?tiles-y)
	(player-tile-x ?player-tile-x)
	(player-tile-y ?player-tile-y)
	?w <- (window-should-close FALSE)
	?r <- (KEY_RIGHT FALSE)
	?l <- (KEY_LEFT FALSE)
	?d <- (KEY_DOWN FALSE)
	?u <- (KEY_UP FALSE)
	(forall
		(tile ?x ?y ? 1)
		(and
			(test (>= ?y (- ?player-tile-y ?player-tile-visibility)))
			(test (<= ?y (+ ?player-tile-y ?player-tile-visibility)))
			(test (>= ?x (- ?player-tile-x ?player-tile-visibility)))
			(test (<= ?x (+ ?player-tile-x ?player-tile-visibility)))))
	(forall
		(tile ?x ?y ? 0|2)
		(or
			(test	(< ?y (- ?player-tile-y ?player-tile-visibility)))
			(test	(> ?y (+ ?player-tile-y ?player-tile-visibility)))
			(test	(< ?x (- ?player-tile-x ?player-tile-visibility)))
			(test	(> ?x (+ ?player-tile-x ?player-tile-visibility)))))
	=>
	(raylib-begin-texture-mode ?id ?tid ?twidth ?theight ?tmipmaps ?tformat ?did ?dwidth ?dheight ?dmipmaps ?dformat)
	    (raylib-clear-background BLANK)
		(do-for-all-facts ((?t tile)) (= 0 (nth$ 4 ?t:implied))
			(raylib-draw-rectangle (nth$ 1 ?t:implied) (nth$ 2 ?t:implied) 1 1 BLACK))
		(do-for-all-facts ((?t tile)) (= 2 (nth$ 4 ?t:implied))
			(raylib-draw-rectangle (nth$ 1 ?t:implied) (nth$ 2 ?t:implied) 1 1 (raylib-fade BLACK 0.8)))
	(raylib-end-texture-mode)

	(raylib-begin-drawing)
		(raylib-clear-background RAYWHITE)
		(do-for-all-facts ((?t tile)) (= 0 (nth$ 3 ?t:implied))
			(raylib-draw-rectangle (* (nth$ 1 ?t:implied)  ?MAP_TILE_SIZE) (* (nth$ 2 ?t:implied) ?MAP_TILE_SIZE) ?MAP_TILE_SIZE ?MAP_TILE_SIZE BLUE)
			(raylib-draw-rectangle-lines (* (nth$ 1 ?t:implied)  ?MAP_TILE_SIZE) (* (nth$ 2 ?t:implied) ?MAP_TILE_SIZE) ?MAP_TILE_SIZE ?MAP_TILE_SIZE (raylib-fade DARKBLUE 0.5)))
		(do-for-all-facts ((?t tile)) (= 1 (nth$ 3 ?t:implied))
			(raylib-draw-rectangle (* (nth$ 1 ?t:implied)  ?MAP_TILE_SIZE) (* (nth$ 2 ?t:implied) ?MAP_TILE_SIZE) ?MAP_TILE_SIZE ?MAP_TILE_SIZE (raylib-fade BLUE 0.9))
			(raylib-draw-rectangle-lines (* (nth$ 1 ?t:implied)  ?MAP_TILE_SIZE) (* (nth$ 2 ?t:implied) ?MAP_TILE_SIZE) ?MAP_TILE_SIZE ?MAP_TILE_SIZE (raylib-fade DARKBLUE 0.5)))

		(raylib-draw-rectangle ?player-position-x ?player-position-y ?player-size ?player-size RED)

		(raylib-draw-texture-pro ?tid ?twidth ?theight ?tmipmaps ?tformat 0 0 ?twidth (* -1 ?theight)
			0 0 (* ?MAP_TILE_SIZE ?tiles-x) (* ?MAP_TILE_SIZE ?tiles-y)
			0 0 0 WHITE)

		(raylib-draw-text (format nil "Current tile: [%d, %d]" ?player-tile-x ?player-tile-y) 10 10 20 RAYWHITE)
		(raylib-draw-text "ARROW KEYS to move" 10 (- (raylib-get-screen-height) 25) 20 RAYWHITE)
	(raylib-end-drawing)

	(retract ?w ?r ?l ?d ?u)
	(assert
		(window-should-close (raylib-window-should-close))
		(KEY_RIGHT (raylib-is-key-down KEY_RIGHT))
		(KEY_LEFT (raylib-is-key-down KEY_LEFT))
		(KEY_DOWN (raylib-is-key-down KEY_DOWN))
		(KEY_UP (raylib-is-key-down KEY_UP))))

(defrule die
	(window-should-close TRUE)
	=>
	(println "Done. Exiting...")
	(raylib-close-window)
	(exit))

(reset)
(watch all)
(loop-for-count (?x 0 25) do
	(loop-for-count (?y 0 15) do
		(assert (tile ?x ?y (random 0 1) 0))))
(run)
