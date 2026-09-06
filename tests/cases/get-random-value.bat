; raylib-get-random-value: an integer in min..max, both included. raylib
; swaps the bounds if they are given the wrong way round.
;
; raylib-set-random-seed makes the sequence repeatable, so the sequence is
; checked for being repeatable rather than for particular values, which
; would tie the test to one raylib release's generator.
(deffunction in-range (?lo ?hi ?n)
   (bind ?ok TRUE)
   (loop-for-count ?n do
      (bind ?v (raylib-get-random-value ?lo ?hi))
      (if (or (< ?v (min ?lo ?hi)) (> ?v (max ?lo ?hi))) then
         (bind ?ok FALSE)))
   ?ok)

(deffunction sample (?seed ?n)
   (raylib-set-random-seed ?seed)
   (bind ?out (create$))
   (loop-for-count ?n do
      (bind ?out (create$ ?out (raylib-get-random-value 0 1000000))))
   ?out)

(println (raylib-get-random-value 7 7))
(println (raylib-get-random-value -3 -3))
(println (in-range 1 6 200))
(println (in-range 6 1 200))
(println (in-range -10 10 200))
(println (length$ (sample 1 8)))
(println (eq (sample 42 5) (sample 42 5)))
(println (eq (sample 42 5) (sample 43 5)))
(exit)
