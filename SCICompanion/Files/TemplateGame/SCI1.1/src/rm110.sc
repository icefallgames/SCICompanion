(version 2)
(include "sci.sh")
(include "110.shm")
(exports
    0 rm110
)
(use "Main")
(use "starCon")
(use "DisposeLoad")
(use "Sound")
(use "Cycle")
(use "Game")
(use "Feature")
(use "Obj")
(use "Print")
(script 110)

(instance public rm110 of Rm
    (properties
        picture 110
        //style $800a
        style $0000		// 1: from center horizontal. 8 means close the old?
        horizon 50
        vanishingX 130
        vanishingY 50
        south 105		// This doesn't seem to work anymore
        noun N_ROOM
    )

    (method (init)
        (super:init())
		(send gEgo:view(0)) // View seems to default to -1?
        
        // Get the first inventory item
        (send gEgo:get(0))
        
        (anObject:init())
        
        (send gEgo:
            init()
            view(13)
            cel(0)
            x(112)
            y(162)
        )        
        (send gGame:handsOn())
        (brookSound:play())
    )
    
    (method (doit)
    	(var buf[100])
    	(super:doit())
    	// Testing why sound doesn't work.
    	// So, in SCI, frame isn't increasing.
    	// Format(@buf "%d %d %d %d %d %d %d" (brookSound:nodePtr) (brookSound:handle) (brookSound:flags) (brookSound:loop) (brookSound:signal) (brookSound:dataInc) (brookSound:frame))
    	// Display(@buf 100 10 10 dsFONT 4 dsCOLOR 10 dsBACKGROUND 0)
    	
	)
)

(instance public anObject of Prop
	(properties
		view 900
		x 200
		y 100
		noun N_ANOBJECT
	)
)

(instance brookSound of Sound
	(properties
		number 1
		loop 3
		priority 0
		flags 0
	)
)