(raylib-init-window 800 450 "CLIPSraylib [shapes] example - collision area")
(raylib-set-target-fps 60)

(deffacts init
	(box-a 10 175 200 100)
	(box-b 370 175 60 60)
	(pause FALSE)
	(unpause FALSE)
	(window-should-close FALSE)
	(screen-upper-limit 40))

(defrule begin-render
	(box-a ?ax ?ay ?awidth ?aheight)
	(box-b ?bx ?by ?bwidth ?bheight)
	(screen-upper-limit ?u)
	(not (drawing))
	?w <- (window-should-close FALSE)
	=>
	(retract ?w)
	(raylib-begin-drawing)
		(raylib-draw-fps 10 10)
		(raylib-draw-text "Press SPACE to PAUSE/RESUME" 20 (- (raylib-get-screen-height) 35) 20 LIGHTGRAY)
		(raylib-clear-background RAYWHITE)
		(raylib-draw-rectangle ?ax ?ay ?awidth ?aheight GOLD)
		(raylib-draw-rectangle ?bx ?by ?bwidth ?bheight BLUE)
		(assert
			(drawing)
			(window-should-close (raylib-window-should-close))))

(defrule draw-no-collision
	?d <- (drawing)
	(box-a ?ax ?ay ?awidth ?aheight)
	(box-b ?bx ?by ?bwidth ?bheight)
	(screen-upper-limit ?u)
	(window-should-close FALSE)
	(test (not (raylib-check-collision-recs
		?ax ?ay ?awidth ?aheight
		?bx ?by ?bwidth ?bheight)))
	=>
		(raylib-draw-rectangle 0 0 (raylib-get-screen-width) ?u BLACK)
	(raylib-end-drawing)
	(assert (done)))

(defrule draw-collision
	(drawing)
	(box-a ?ax ?ay ?awidth ?aheight)
	(box-b ?bx ?by ?bwidth ?bheight)
	(screen-upper-limit ?u)
	(window-should-close FALSE)
	(test (raylib-check-collision-recs
		?ax ?ay ?awidth ?aheight
		?bx ?by ?bwidth ?bheight))
	=>
		(raylib-draw-rectangle 0 0 (raylib-get-screen-width) ?u RED)
		(raylib-draw-text
			"COLLISION!"
			(integer (-
				(/ (raylib-get-screen-width) 2)
				(/ (raylib-measure-text "COLLISION!" 20) 2)))
			(integer (- (/ ?u 2) 10))
			20
			BLACK)
	(assert (collision-rectangle (raylib-get-collision-rec
		?ax ?ay ?awidth ?aheight
		?bx ?by ?bwidth ?bheight))))

(defrule draw-collision-rectangle
	(drawing)
	(screen-upper-limit ?u)
	(window-should-close FALSE)
	?c <- (collision-rectangle ?x ?y ?width ?height)
	=>
		(raylib-draw-text
			(format nil "Collision Area: %d" (* ?width ?height))
		       	(integer (- (/ (raylib-get-screen-width) 2) 100))
			(integer (+ ?u 10))
			20
			BLACK)
		(raylib-draw-rectangle
			(integer ?x)
			(integer ?y)
			(integer ?width)
			(integer ?height)
			LIME)
	(raylib-end-drawing)
	(assert (done))
	(retract ?c))

(defrule recalculate-b
	?b <- (box-b ?bx ?by ?bwidth ?bheight)
	?d <- (drawing)
	?dd <- (done)
	?p <- (pause FALSE)
	(window-should-close FALSE)
	=>
	(retract ?b ?d ?dd ?p)
	(assert
		(pause (raylib-is-key-pressed KEY_SPACE))
		(box-b
			(integer (- (raylib-get-mouse-x) (/ ?bwidth 2)))
			(integer (- (raylib-get-mouse-y) (/ ?bheight 2)))
			?bwidth ?bheight)))

(defrule pause-and-wait
	(pause TRUE)
	?unpause <- (unpause FALSE)
	?w <- (window-should-close FALSE)
	=>
	(retract ?unpause ?w)
	(assert
		(unpause (raylib-is-key-pressed KEY_SPACE))
		(window-should-close (raylib-window-should-close))))

(defrule unpause
	?p <- (pause TRUE)
	?unpause <- (unpause TRUE)
	(window-should-close FALSE)
	=>
	(retract ?p ?unpause)
	(assert
		(pause (raylib-is-key-pressed KEY_SPACE))
		(unpause FALSE)))

(defrule die
	(window-should-close TRUE)
	=>
	(println "Done. Exiting...")
	(raylib-close-window)
	(exit))

(watch all)
(reset)
(run)
