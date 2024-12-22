(deffacts init
	(screen-width 800) (screen-height 450)
	(roundness 0.2)
	(width 200.0)
	(height 100.0)
	(segments 0.0)
	(thickness 1.0)
	(draw FALSE TRUE FALSE)
	(window-should-close FALSE))
	
(raylib-init-window 800 450 "CLIPSraylib [shapes] example - draw rectangle rounded")
(raylib-set-target-fps 60)

(defrule begin-drawing
	(window-should-close FALSE)
	(not (drawing))
	=>
	(raylib-begin-drawing)
		(raylib-clear-background RAYWHITE)

		(raylib-draw-text "Controls:" 20 20 10 BLACK)
		(raylib-draw-line 560 0 560 (raylib-get-screen-height) (raylib-fade LIGHTGRAY 0.6))
		(raylib-draw-rectangle 560 0 (- (raylib-get-screen-width) 500) (raylib-get-screen-height) (raylib-fade LIGHTGRAY 0.3))
	(assert (drawing)))


(defrule draw-rect
	(drawing)
	(draw TRUE ? ?)
	(height ?height)
	(width ?width)
	(screen-height ?screen-height)
	(screen-width ?screen-width)
	=>
	(raylib-draw-rectangle
		(integer (/ (- ?screen-width ?width 250) 2))
		(integer (/ (- ?screen-height ?height) 2))
		(integer ?width)
		(integer ?height)
		(raylib-fade GOLD 0.6))
	(assert (rectangle-drawn)))

(defrule do-not-draw-rect
	(drawing)
	(draw FALSE ? ?)
	=>
	(assert (rectangle-drawn)))
	
(defrule draw-rounded-rect
	(drawing)
	(draw ? TRUE ?)
	(height ?height)
	(width ?width)
	(roundness ?roundness)
	(segments ?segments)
	(screen-width ?screen-width)
	(screen-height ?screen-height)
	=>
	(raylib-draw-rectangle-rounded
		(/ (- ?screen-width ?width 250) 2)
		(/ (- ?screen-height ?height) 2)
		?width
		?height
		?roundness
		(integer ?segments)
		(raylib-fade MAROON 0.2))
	(assert (rounded-rectangle-drawn)))

(defrule do-not-draw-rounded-rect
	(drawing)
	(draw ? FALSE ?)
	=>
	(assert (rounded-rectangle-drawn)))
	
(defrule draw-rounded-lines
	(drawing)
	(draw ? ? TRUE)
	(height ?height)
	(width ?width)
	(thickness ?thickness)
	(roundness ?roundness)
	(segments ?segments)
	(screen-height ?screen-height)
	(screen-width ?screen-width)
	=>
	(raylib-draw-rectangle-rounded-lines-ex
		(/ (- ?screen-width ?width 250) 2)
		(/ (- ?screen-height ?height) 2)
		?width
		?height
		?roundness
		(integer ?segments)
		?thickness
		(raylib-fade MAROON 0.4))
	(assert (rounded-rectangle-with-lines-drawn)))

(defrule do-not-draw-rounded-lines
	(drawing)
	(draw ? ? FALSE)
	=>
	(assert (rounded-rectangle-with-lines-drawn)))
	
(defrule end-drawing
	?d <- (drawing)
	?rd <- (rectangle-drawn)
	?rrd <- (rounded-rectangle-drawn)
	?rrld <- (rounded-rectangle-with-lines-drawn)
	?w <- (width ?width)
	?h <- (height ?height)
	?r <- (roundness ?roundness)
	?t <- (thickness ?thickness)
	?s <- (segments ?segments)
	?f <- (window-should-close FALSE)
	?dr <- (draw ?rect ?rounded ?lines)
	=>
	(retract ?d ?rd ?rrd ?rrld ?w ?h ?r ?t ?s ?f ?dr)
	(assert
		(window-should-close (raylib-window-should-close))
		(width
			(raylib-gui-slider-bar
				640 40 105 20
				"Width"
				(format nil "%.2f" ?width)
				?width
				0.0
				(float (- (raylib-get-screen-width) 300))))
		(height
			(raylib-gui-slider-bar
				640 70 105 20
				"Height"
				(format nil "%.2f" ?height)
				?height
				0.0
				(float (- (raylib-get-screen-height) 50))))
		(roundness
			(raylib-gui-slider-bar
				640 140 105 20
				"Roundness"
				(format nil "%.2f" ?roundness)
				?roundness
				0.0 1.0))
		(thickness
			(raylib-gui-slider-bar
				640 170 105 20
				"Thickness"
				(format nil "%.2f" ?thickness)
				?thickness
				0.0 20.0))
		(segments
			(raylib-gui-slider-bar
				640 240 105 20
				"Segments"
				(format nil "%.2f" ?segments)
				?segments
				0.0 60.0))
		(draw
			(raylib-gui-check-box
				640 320 20 20
				"DrawRect"
				?rect)
			(raylib-gui-check-box
				640 350 20 20
				"DrawRoundedRect"
				?rounded)
			(raylib-gui-check-box
				640 380 20 20
				"DrawRoundedLines"
				?lines)))
	(raylib-end-drawing))

(defrule die
	(window-should-close TRUE)
	=>
	(println "Done. Exiting...")
	(raylib-close-window)
	(exit))

(reset)
(watch all)
(run)
