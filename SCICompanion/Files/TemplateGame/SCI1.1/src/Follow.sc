(version 2)
(include "sci.sh")
(use "Cycle")
(script 971)


/*
	A motion class that is used to make an Actor move so it stays within a certain distance of another (possibly moving) object.

	Example usage::	

		; Make the dog follow the ego at a distance of 20
		(aDog setMotion: Follow gEgo 20)
		
	Note that this class doesn't intelligently move the Actor around polygon obstacles. For that, use :class:`PFollow`.
*/
(class Follow of Motion
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
	.. function:: init(theClient theWho [theDistance])
	
		Initializes the Follow instance.
		
		:param heapPtr theClient: The :class:`Actor` to which this is attached.
		:param heapPtr theWho: The target to chase.
		:param number theDistance: How close the client needs to get from the target.
	*/
    (method (init theClient theWho theDistance)
        (if (>= paramTotal 1)
            = client theClient
            (if (>= paramTotal 2)
                = who theWho
                (if (>= paramTotal 3)
                    = distance theDistance
                )
            )
        )
        (if (> (send client:distanceTo(who)) distance)
            (super:init(client (send who:x) (send who:y)))
        )
    )


    (method (doit)
        (var temp0)
        (if (> (send client:distanceTo(who)) distance)
            (if (== {b-moveCnt} 0)
                (super:init(client (send who:x) (send who:y)))
            )
            (super:doit())
        )(else
            = xLast (send client:x)
            = yLast (send client:y)
            = temp0 GetAngle(xLast yLast (send who:x) (send who:y))
            (if (<> temp0 (send client:heading))
                (send client:heading(temp0))
                (if ((send client:looper))
                    (send ((send client:looper)):doit(client temp0))
                )(else
                    DirLoop(client temp0)
                )
            )
        )
    )


    (method (moveDone)
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
