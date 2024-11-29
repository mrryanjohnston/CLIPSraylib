(raylib-init-window 800 400 "Hello, mouse")

(deffunction check-if-should-exit (?delta)
	(raylib-window-should-close))

(deffacts setup
	(new-mouse-delta 0 0)
	(new-mouse-position 0 0)
	(new-mouse-button FALSE FALSE))

(defrule continue
	?d <- (new-mouse-delta ?ndx ?ndy)
	?p <- (new-mouse-position ?npx ?npy)
	?c <- (new-mouse-color ?color)
	?b <- (new-mouse-button ? ?)
	(not (test (check-if-should-exit ?d)))
	=>
	(retract ?d ?p ?c ?b)
	(raylib-begin-drawing)
		(raylib-clear-background RAYWHITE)
		(raylib-draw-circle (integer ?npx) (integer ?npy) 100 ?color)
	(raylib-end-drawing)
	(assert
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
	?d <- (new-mouse-delta ?dx ?dy)
	(test (check-if-should-exit ?d))
	=>
	(raylib-close-window)
	(println "Closing program...")
	(exit))

(reset)
(run)
