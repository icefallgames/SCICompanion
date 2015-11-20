(version 2)
(include "sci.sh")
(include "game.sh")
(use "Cycle")
(use "Object")
(script DIRECTPATH_SCRIPT)


/*
	DPath (or "directed path") uses a list of locations to direct the :class:`Actor` towards its destination.
	
	Example usage::
	
		// Send the hoverGuys along 3 points to the destination, then cue() the caller.
		(hoverGuys:setMotion(DPath 255 137 154 138 140 137 self))
*/
(class DPath of Motion
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
        points 0
        value 0
    )

	/*
	.. function:: init(theClient [points... theCaller])
	
		:param heapPtr theClient: The Actor to which this is attached.
		:param number points: Pairs of (x, y) coordinates.
		:param heapPtr theCaller: Object that will have its cue() method called when the Actor reaches the target.
	*/
    (method (init theClient theCaller)
        (var temp0)
        = points 
            (if (points)
            )(else
                (List:new())
            )
        (if (paramTotal)
            = client theClient
            = temp0 0
            (while (<= temp0 (- paramTotal 3))
                (send points:add(theCaller[temp0] theCaller[++temp0]))
                ++temp0
            )
            (if (<= temp0 (- paramTotal 2))
                = caller theCaller[temp0]
            )
        )
        (if ((send points:contains(-32768)))
        )(else
            (send points:add(-32768))
        )
        (self:setTarget())
        (super:init())
        (if (not paramTotal)
            (self:doit())
        )
    )


    (method (dispose)
        (if (IsObject(points))
            (send points:dispose())
        )
        (super:dispose())
    )


    (method (moveDone)
        (if (== (send points:at(value)) -32768)
            (super:moveDone())
        )(else
            (self:init())
        )
    )


    (method (setTarget)
        (if (<> (send points:at(value)) -32768)
            = x (send points:at(value))
            = y (send points:at(++value))
            ++value
        )
    )

)
