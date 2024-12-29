(raylib-init-window 800 450 "CLIPSraylib [shapes] example - raylib logo animation")
(raylib-set-target-fps 60)

(deffacts init
	(logoPositionX (integer (- (/ (raylib-get-screen-width) 2) 128)))
	(logoPositionY (integer (- (/ (raylib-get-screen-height) 2) 128)))
	(framesCounter 0)
	(lettersAppearingFramesCounter 0)
	(lettersCount 0)

	(topSideRecWidth 16)
	(leftSideRecHeight 16)

	(bottomSideRecWidth 16)
	(rightSideRecHeight 16)

	(alpha 1.0)
	(replay FALSE)
	(WindowShouldClose FALSE))

(defrule reset-and-replay
	(alpha ?alpha&:(<= ?alpha 0))
	?r <- (replay TRUE)
	=>
	(reset))

(defrule start-drawing
	?w <- (WindowShouldClose FALSE)
	(not (drawing))
	=>
	(retract ?w)
	(assert
		(drawing)
		(WindowShouldClose (raylib-window-should-close)))
	(raylib-begin-drawing)
		(raylib-clear-background RAYWHITE))

(defrule small-box-blinking-on
	(logoPositionX ?logoPositionX)
	(logoPositionY ?logoPositionY)
	(drawing)
	(not (done-drawing))
	?f <- (framesCounter ?frames&:(< ?frames 120))
	(WindowShouldClose FALSE)
	(test (> (mod (/ ?frames 15) 2) 0))
	=>
		(raylib-draw-rectangle ?logoPositionX ?logoPositionY 16 16 BLACK)
	(retract ?f)
	(assert
		(framesCounter (+ 1 ?frames))))

(defrule small-box-blinking-off
	(logoPositionX ?logoPositionX)
	(logoPositionY ?logoPositionY)
	(drawing)
	(not (done-drawing))
	?f <- (framesCounter ?frames&:(< ?frames 120))
	(WindowShouldClose FALSE)
	(test (= (mod (/ ?frames 15) 2) 0))
	=>
	(retract ?f)
	(assert
		(framesCounter (+ 1 ?frames))))

(defrule top-and-left-bars-growing
	(logoPositionX ?logoPositionX)
	(logoPositionY ?logoPositionY)
	(drawing)
	(not (done-drawing))
	(framesCounter 120)
	(WindowShouldClose FALSE)
	?t <- (topSideRecWidth ?topSideRecWidth&:(< ?topSideRecWidth 256))
	?l <- (leftSideRecHeight ?leftSideRecHeight)
	=>
                (raylib-draw-rectangle ?logoPositionX ?logoPositionY ?topSideRecWidth 16 BLACK)
                (raylib-draw-rectangle ?logoPositionX ?logoPositionY 16 ?leftSideRecHeight BLACK)
	(retract ?t ?l)
	(assert
		(done-drawing)
		(topSideRecWidth (+ ?topSideRecWidth 4))
		(leftSideRecHeight (+ ?leftSideRecHeight 4))))

(defrule bottom-and-right-bars-growing
	(logoPositionX ?logoPositionX)
	(logoPositionY ?logoPositionY)
	(drawing)
	(not (done-drawing))
	(framesCounter 120)
	(WindowShouldClose FALSE)
	(topSideRecWidth 256)
	(leftSideRecHeight ?leftSideRecHeight)
	?b <- (bottomSideRecWidth ?bottomSideRecWidth&:(< ?bottomSideRecWidth 256))
	?r <- (rightSideRecHeight ?rightSideRecHeight)
	=>
                (raylib-draw-rectangle ?logoPositionX ?logoPositionY 256 16 BLACK)
                (raylib-draw-rectangle ?logoPositionX ?logoPositionY 16 ?leftSideRecHeight BLACK)

                (raylib-draw-rectangle (+ 240 ?logoPositionX) ?logoPositionY 16 ?rightSideRecHeight BLACK)
                (raylib-draw-rectangle ?logoPositionX (+ 240 ?logoPositionY) ?bottomSideRecWidth 16 BLACK)
	(retract ?b ?r)
	(assert
		(done-drawing)
		(bottomSideRecWidth (+ 4 ?bottomSideRecWidth))
		(rightSideRecHeight (+ 4 ?rightSideRecHeight))))

(defrule one-more-letter
	(drawing)
	(not (done-drawing))
	(framesCounter 120)
	?c <- (lettersCount ?count&:(< ?count 10))
	?l <- (lettersAppearingFramesCounter 12)
	(WindowShouldClose FALSE)
	(bottomSideRecWidth 256)
	(not (draw-letters))
	=>
	(retract ?c ?l)
	(assert
		(lettersAppearingFramesCounter 0)
		(lettersCount (+ 1 ?count))
		(draw-letters)))

(defrule same-amount-of-letters
	(drawing)
	(not (done-drawing))
	(framesCounter 120)
	(lettersCount ?count&:(< ?count 10))
	?l <- (lettersAppearingFramesCounter ?frames&~12)
	(WindowShouldClose FALSE)
	(bottomSideRecWidth 256)
	(not (draw-letters))
	=>
	(retract ?l)
	(assert
		(draw-letters)
		(lettersAppearingFramesCounter (+ 1 ?frames))))

(defrule letters-appearing
	(logoPositionX ?logoPositionX)
	(logoPositionY ?logoPositionY)
	(drawing)
	(not (done-drawing))
	(framesCounter 120)
	(lettersCount ?lettersCount)
	(WindowShouldClose FALSE)
	(bottomSideRecWidth 256)
	(rightSideRecHeight ?rightSideRecHeight)
	(leftSideRecHeight ?leftSideRecHeight)
	(alpha ?alpha&:(> ?alpha 0))
	(or
		(test (= ?lettersCount 10))
		(exists (draw-letters)))
	=>
                (raylib-draw-rectangle ?logoPositionX ?logoPositionY 256 16 (raylib-fade BLACK ?alpha))
                (raylib-draw-rectangle ?logoPositionX (+ ?logoPositionY 16) 16 (- ?leftSideRecHeight 32) (raylib-fade BLACK ?alpha))

                (raylib-draw-rectangle (+ 240 ?logoPositionX) (+ 16 ?logoPositionY) 16 (- ?rightSideRecHeight 32) (raylib-fade BLACK ?alpha))
                (raylib-draw-rectangle ?logoPositionX (+ 240 ?logoPositionY) 256 16 (raylib-fade BLACK ?alpha))

		(raylib-draw-rectangle
			(integer (- (/ (raylib-get-screen-width) 2) 112))
			(integer (- (/ (raylib-get-screen-height) 2) 112))
			224 224 (raylib-fade RAYWHITE ?alpha))

		(raylib-draw-text
			(sub-string 0 ?lettersCount raylib)
			(integer (- (/ (raylib-get-screen-width) 2) 44))
			(integer (+ (/ (raylib-get-screen-height) 2) 48))
			50
			(raylib-fade BLACK ?alpha))
	(assert (ready-to-fade)))

(defrule wait-to-fade
	(lettersCount ~10)
	(alpha ?alpha&:(> ?alpha 0))
	(drawing)
	(not (done-drawing))
	(ready-to-fade)
	?d <- (draw-letters)
	=>
	(retract ?d)
	(assert (done-drawing)))

(defrule fade-out
	(lettersCount 10)
	?a <- (alpha ?alpha&:(> ?alpha 0))
	(drawing)
	(not (done-drawing))
	?r <- (ready-to-fade)
	=>
	(retract ?a ?r)
	(assert
		(alpha (- ?alpha 0.02))
		(done-drawing)))

(defrule describe-replay
	(lettersCount 10)
	(drawing)
	(not (done-drawing))
	(alpha ?alpha&:(<= ?alpha 0))
	?r <- (replay FALSE)
	=>
		(raylib-draw-text "[R] REPLAY" 340 200 20 GRAY)
	(retract ?r)
	(assert
		(done-drawing)
		(replay (raylib-is-key-pressed KEY_R))))

(defrule end-drawing
	?d <- (drawing)
	?dd <- (done-drawing)
	=>
	(raylib-end-drawing)
	(retract ?d ?dd))

(defrule die
	(WindowShouldClose TRUE)
	=>
	(println "Done. Exiting...")
	(raylib-close-window)
	(exit))

(reset)
(run)
