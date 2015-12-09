(version 2)
(include "sci.sh")
(use "Main")
(use "Cycle")
(script 941)

/*
	A cycler that oscillates back and forth between cels in a loop. See the init method for information on parameters.
	
	Sample usage::
	
		; Make the star cycle between random cels 5 times.
		(star setCycle: RandCycle 5)
*/
(class RandCycle of Cycle
    (properties
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
        count -1
        reset 0
    )

	/*
	.. function:: init(theClient [theCount theCaller shouldReset])
	
		:param heapPtr theClient: The object to which the cycler is attached.
		:param number theCount: Optional number of times to cycle. If not specified, or -1, cycling continues indefinitely.
		:param heapPtr theCaller: Optional caller object that will be cue'd when the cycling is complete.
		:param boolean shouldReset: Should the object go back to cel 0 after cycling is complete.
	*/
    (method (init theClient theCount theCaller shouldReset)
        (super:init(theClient))
        (if (>= paramTotal 4)
            = reset shouldReset
        )
        (if (reset)
            (send client:cel(0))
        )
        = cycleCnt GetTime()
        (if (>= paramTotal 2)
            (if (<> theCount -1)
                = count (+ GetTime() theCount)
            )(else
                = count -1
            )
            (if (>= paramTotal 3)
                = caller theCaller
            )
        )(else
            = count -1
        )
    )


    (method (doit)
        (var temp0)
        = temp0 GetTime()
        (if ((> count temp0) or (== count -1))
            (if (> (- temp0 cycleCnt) (send client:cycleSpeed))
                (send client:cel((self:nextCel())))
                = cycleCnt GetTime()
            )
        )(else
            (if (reset)
                (send client:cel(0))
            )
            (self:cycleDone())
        )
    )


    (method (nextCel)
        (var temp0)
        return 
            (if (<> NumCels(client) 1)
                (while (== (= temp0 Random(0 (send client:lastCel()))) (send client:cel))
                )
                temp0
            )(else
                0
            )
    )


    (method (cycleDone)
        = completed 1
        (if (caller)
            = gDoMotionCue TRUE
        )(else
            (self:motionCue())
        )
    )
)
