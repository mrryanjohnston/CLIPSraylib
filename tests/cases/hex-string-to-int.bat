; raylib-hex-string-to-int: a string or symbol of hex digits, with or
; without a 0x prefix, as an integer. With raylib-get-color that reads a
; colour written the way raylib's examples write them.
(println (raylib-hex-string-to-int "0x052c46ff"))
(println (raylib-hex-string-to-int "ff"))
(println (raylib-hex-string-to-int FF))
(println (raylib-hex-string-to-int "0X10"))
(println (raylib-hex-string-to-int "0"))
(println (raylib-get-color (raylib-hex-string-to-int "0x052c46ff")))
(println (raylib-get-color (raylib-hex-string-to-int "ffffffff")))
; not hex
(println (raylib-hex-string-to-int "zz"))
; nothing after the prefix
(println (raylib-hex-string-to-int "0x"))
(exit)
