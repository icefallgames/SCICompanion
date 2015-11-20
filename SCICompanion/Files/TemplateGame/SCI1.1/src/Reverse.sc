(version 2)
(include "sci.sh")
(include "game.sh")
(use "Cycle")
(script REVERSECYCLE_SCRIPT)

/*
	Reverse is a cycler that cycles through cels backward.
	
	Example usage::
	
		(bird:setCycle(Reverse))
		
	See also: :class:`Forward`.
*/
(class Reverse of Cycle
    (properties
    	name "Rev"
        client 0
        caller 0
        cycleDir cdBACKWARD
        cycleCnt 0
        completed 0
    )

    (method (doit)
        (var revNextCel)
        = revNextCel (self:nextCel())
        (if (< revNextCel 0)
            (self:cycleDone())
        )(else
            (send client:cel(revNextCel))
        )
    )


    (method (cycleDone)
        (send client:cel((send client:lastCel())))
    )

)
