(version 2)
(include "sci.sh")
(include "Verbs.sh")
(include "game.sh")
(include "110.shm")
(include "110.shp")
(exports
    0 rm110
)
(use "Main")
(use "DisposeLoad")
(use "Sound")
(use "Cycle")
(use "Game")
(use "Actor")
(use "System")
(use "Print")
(use "Polygon")

(script 110)


(instance public rm110 of Room
    (properties
        picture 110
        style (| dpANIMATION_BLACKOUT dpOPEN_FADEPALETTE)
        horizon 50
        vanishingX 130
        vanishingY 50
        noun N_ROOM
    )

    (method (init)
        AddPolygonsToRoom(@P_Default110)
        (super:init())
        
        (switch (gPreviousRoomNumber)
        	// Add room numbers here to set up the ego when coming from different directions.
        	(default
		        SetUpEgo(-1 1)
        		(send gEgo:
        			posn(150 130)
				)
			)
		)
        (send gEgo:init())
        
        // We just came from the title screen, so we need to call this to give control
        // to the player.
        (send gGame:handsOn())
    )
)
