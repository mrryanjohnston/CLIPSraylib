(raylib-init-window 234 456 "Hello, world")

(println "---------------------" crlf "Render height: " (raylib-get-render-height) ". |")
(println "Render width: " (raylib-get-render-width) ".  |" crlf "---------------------")
(println "---------------------" crlf "Screen height: " (raylib-get-screen-height) ". |")
(println "Screen width: " (raylib-get-screen-width) ".  |" crlf "---------------------")

(raylib-close-window)
(exit)
