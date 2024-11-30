(raylib-init-window 800 400 "Hello, mouse")

(deffacts setup
	(should-exit FALSE)
	(new-mouse-delta 0 0)
	(new-mouse-position 0 0)
	(new-mouse-button FALSE FALSE))

(defrule continue
	?d <- (new-mouse-delta ?ndx ?ndy)
	?p <- (new-mouse-position ?npx ?npy)
	?c <- (new-mouse-color ?color)
	?b <- (new-mouse-button ? ?)
	?s <- (should-exit FALSE)
	=>
	(retract ?d ?p ?c ?b ?s)
	(raylib-begin-drawing)
		(raylib-clear-background RAYWHITE)
		(raylib-draw-circle (integer ?npx) (integer ?npy) 100 ?color)
	(raylib-end-drawing)
	(assert
		(should-exit (raylib-window-should-close))
		(new-mouse-delta (raylib-get-mouse-delta))
		(new-mouse-position (raylib-get-mouse-position))
		(new-mouse-button
			(raylib-is-mouse-button-down MOUSE_BUTTON_LEFT)
		      	(raylib-is-mouse-button-down MOUSE_BUTTON_RIGHT))))

(defrule determine-color-gray
	(new-mouse-button FALSE FALSE)
	=>
	(assert (new-mouse-color DARKGRAY)))

(defrule determine-color-red
	(new-mouse-button TRUE FALSE)
	=>
	(assert (new-mouse-color RED)))

(defrule determine-color-blue
	(new-mouse-button FALSE TRUE)
	=>
	(assert (new-mouse-color BLUE)))

(defrule determine-color-purple
	(new-mouse-button TRUE TRUE)
	=>
	(assert (new-mouse-color PURPLE)))

(defrule die
	(should-exit TRUE)
	=>
	(raylib-close-window)
	(println "Closing program...")
	(exit))

(reset)
(run)
