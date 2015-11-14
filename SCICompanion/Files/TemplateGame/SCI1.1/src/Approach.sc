(version 2)
(include "sci.sh")
(use "Cycle")
(script 953)


/*
	A motion class that causes the :class:`Actor` to move approach a target until within a certain distance, and then stop.
	This Motion class ignores the room obstacles. In SCI1.1, if path-finding is required, one would generally use the :class:`PolyPath` class instead.
*/
(class Approach of Motion
    (properties
        client 0			// The :class:`Actor` to which this is attached.
        caller 0			// The object that will get cue()'d when the motion is complete.
        x 0
        y 0
        dx 0
        dy 0
        {b-moveCnt} 0
        {b-i1} 0
        {b-i2} 0
        {b-di} 0
        {b-xAxis} 0
        {b-incr} 0
        completed 0
        xLast 0
        yLast 0
        who 0
        distance 20
    )

	/*
	.. function:: init(theClient theWho theDistance)
	
		Initializes the Approach instance.
		
		:param heapPtr theClient: The :class:`Actor` to which this is attached.
		:param heapPtr theWho: The target object.
		:param number theDistance: How close the Actor needs to get to theWho before stopping.
	*/
    (method (init theClient theWho theDistance theCaller)
        (if (>= paramTotal 1)
            = client theClient
            (if (>= paramTotal 2)
                = who theWho
                (if (>= paramTotal 3)
                    = distance theDistance
                    (if (>= paramTotal 4)
                        = caller theCaller
                    )
                )
            )
        )
        (super:init(client (send who:x) (send who:y) caller))
    )


    (method (doit)
        (if ((self:onTarget()))
            (self:moveDone())
        )(else
            (super:doit())
        )
    )


    (method (setTarget param1)
        (if (paramTotal)
            (super:setTarget(rest param1))
        )(else
            (if (not (self:onTarget()))
                (super:setTarget((send who:x) (send who:y)))
            )
        )
    )


    (method (onTarget)
        return <= (send client:distanceTo(who)) distance
    )

)
