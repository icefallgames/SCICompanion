(version 2)
(include "sci.sh")
(include "game.sh")
(exports
    0 ColorInit
)
(use "Main")
(script COLORINIT_SCRIPT)



(procedure public (ColorInit)
    = gColorWindowForeground 0
    = gLowlightColor Palette(palFIND_COLOR 159 159 159)
    = gColorWindowBackground 5
)


