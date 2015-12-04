(version 2)
(include "sci.sh")
(include "game.sh")
(use "Cycle")
(script FORWARDCOUNT_SCRIPT)

/*
	This extends the functionality of the :class:`Forward` cycler by allowing for cycling forward through the cels in a loop
	a specified number of times and then stopping.
	
	Example usage::
	
		; Cycle through the diamond's cels three times
		(diamond setCycle: ForwardCounter 3)
*/
(class ForwardCounter of Forward
    (properties
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
        count 0
    )

	/*
	.. function:: init(theClient theCount [theCaller])
	
		:param heapPtr theClient: The :class:`Prop` to which this cycler is attached.
		:param number theCount: The number of times to cycle through the prop's cels.
		:param heapPtr theCaller: Optional caller that will have its cue() method called when the cycling is done.
	*/
    (method (init theClient theCount theCaller)
        (super:init(theClient))
        (if (>= paramTotal 2)
            = count theCount
            (if (>= paramTotal 3)
                = caller theCaller
            )
        )
    )


    (method (cycleDone)
        (if (--count)
            (super:cycleDone())
        )(else
            = completed 1
            (self:motionCue())
        )
    )

)
