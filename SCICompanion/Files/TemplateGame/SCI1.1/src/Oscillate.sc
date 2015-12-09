(version 2)
(include "sci.sh")
(include "game.sh")
(use "Cycle")
(script OSCILLATE_SCRIPT)

/*
	A cycler that oscillates back and forth between cels in a loop.  See the init method for information on parameters.
	
	Sample usage::
	
		; Make the star cycle its animation back and forth.
		(star setCycle: Oscillate -1)
*/
(class Oscillate of Cycle
    (properties
    	name "Osc"
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
        howManyCycles -1
    )

	/*
		:param heapPtr theClient: The :class:`Prop` to which this is attached.
		:param number theHowManyCycles: The number of times to cycle, or -1 to cycle indefinitely.
		:param heapPtr theCaller: Optional object to be cue()'d when the cycle is complete.
	*/
    (method (init theClient theHowManyCycles theCaller)
        (if (>= paramTotal 2)
            = howManyCycles theHowManyCycles
            (if (>= paramTotal 3)
                = caller theCaller
            )
        )
        (super:init(theClient))
    )


    (method (doit)
        (var oscNextCel)
        = oscNextCel (self:nextCel())
        (if ((> oscNextCel (send client:lastCel())) or (< oscNextCel 0))
            = cycleDir neg cycleDir
            (self:cycleDone())
        )(else
            (send client:cel(oscNextCel))
        )
    )


    (method (cycleDone)
        (if (howManyCycles)
            (send client:cel((self:nextCel())))
            (if (> howManyCycles 0)
                --howManyCycles
            )
        )(else
            = completed 1
            (self:motionCue())
        )
    )

)
