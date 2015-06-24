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
        //style $800a
        style $0000		// 1: from center horizontal. 8 means close the old?
        horizon 50
        vanishingX 130
        vanishingY 50
        south 105
        noun N_ROOM
    )

    (method (init)
        SetUpPolys_110()
        (super:init())
        // Get the first inventory item
        (send gEgo:get(0))
        
        (anObject:init())
        SetUpEgo()
        (switch (gPreviousRoomNumber)
        	(case south
        		(send gEgo:
        			posn(150 180)
        			loop(3)
				)
			)
        	(default
        		(send gEgo:
        			posn(150 100)
        			loop(1)
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
		x 200
		y 100
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