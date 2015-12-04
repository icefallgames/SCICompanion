(version 2)
(include "sci.sh")
(use "Cycle")
(script 938)

/*
	This cycler is similar to :class:`Oscillate`, but lets you specify specific first and last cels between which to oscillate (range oscillate).
	
	Sample usage::
	
		// Make the star cycle 5 times between cel 2 and 7.
		(star setCycle: RangeOscillate 5 2 7)
*/
(class RangeOscillate of Cycle
    (properties
    	name "ROsc"
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
        cycles -1
        firstC 0
        lastC 0
    )

	/*
		:param heapPtr theClient: The :class:`Prop` to which this is attached.
		:param number theCycles: The number of times to cycle, or -1 to cycle indefinitely.
		:param number theFirstC: The cel at which to start.
		:param number theLastC: The cel at which to end.
		:param heapPtr theCaller: Optional object to be cue()'d when the cycle is complete.
	*/
    (method (init theClient theCycles theFirstC theLastC theCaller)
        (if (>= paramTotal 2)
            = cycles theCycles
        )
        (if (>= paramTotal 5)
            = caller theCaller
        )
        (super:init(theClient))
        (if (>= paramTotal 3)
            = firstC theFirstC
            (if (>= paramTotal 4)
                (if (theLastC)
                    = lastC theLastC
                )(else
                    = lastC (send client:lastCel())
                )
            )(else
                = lastC (send client:lastCel())
            )
        )
        (send client:cel(firstC))
    )


    (method (doit)
        (var rOscNextCel)
        = rOscNextCel (self:nextCel())
        (if ((> rOscNextCel lastC) or (< rOscNextCel firstC))
            = cycleDir neg cycleDir
            (self:cycleDone())
        )(else
            (send client:cel(rOscNextCel))
        )
    )


    (method (cycleDone)
        (if (cycles)
            (send client:cel((self:nextCel())))
            (if (> cycles 0)
                --cycles
            )
        )(else
            = completed 1
            (self:motionCue())
        )
    )

)
