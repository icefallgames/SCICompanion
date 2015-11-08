(version 2)
(include "sci.sh")
(include "game.sh")
(exports
    0 ColorInit
)
(use "Main")
(script COLORINIT_SCRIPT)

// Assigns color indices to the gColorWindowForeground, gLowlightColor and gColorWindowBackground global variables to be used in the GUI. Called at startup.
// You may customize this to provide custom colors for your game.
(procedure public (ColorInit)
    = gColorWindowForeground 0
    = gLowlightColor Palette(palFIND_COLOR 159 159 159)
    = gColorWindowBackground 5
)


