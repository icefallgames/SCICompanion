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
(use "View")
(use "Obj")
(use "Print")
(use "Polygon")

(script 110)

(instance public rm110 of Rm
    (properties
        picture 110
        style (| dpANIMATION_BLACKOUT dpOPEN_FADEPALETTE)
        horizon 50
        vanishingX 130
        vanishingY 50
        south 105
        noun N_ROOM
    )

    (method (init)
        AddPolygonsToRoom(@P_Default110)
        (super:init())
        // Get the first inventory item
        (send gEgo:get(0))
        
        (anObject:init())
        (switch (gPreviousRoomNumber)
        	(case south
		        SetUpEgo(-1 3)
        		(send gEgo:
        			posn(150 180)
				)
			)
        	(default
		        SetUpEgo(-1 1)
        		(send gEgo:
        			posn(150 130)
				)
			)
		)
        (send gEgo:init())
        
        (send gGame:handsOn())
        (brookSound:play())
    )
)

(instance public anObject of Prop
	(properties
		view 900
		x 99
		y 140
		z 10
		noun N_ANOBJECT
	)
	
	(method (doVerb theVerb)
        (switch (theVerb)
            (case V_DO
            	Die(1)
            )
            (default 
                (super:doVerb(rest theVerb))
            )
        )
	)
)

(instance brookSound of Sound
	(properties
		number 1
		loop -1
		priority 0
		flags 0
	)
)