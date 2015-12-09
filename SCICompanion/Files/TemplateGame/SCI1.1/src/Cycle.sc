/*
	The Cycle.sc script contains the most commonly used cyclers. Cyclers define
	different ways that animation cels are shown over time (e.g. forward, reverse, repeating, random, etc...).

	It also contains the Motion base class, and the MoveTo motion.
*/
(version 2)
(include "sci.sh")
(use "Main")
(use "System")
(script 992)


/*
	The base class for all cyclers. Cyclers are responsible for changing the cel
	of a :class:`Prop` or :class:`Actor` over time, giving the illusion of animation.
	
	Example usage::
	
		; Cycle the cels of myProp forward.
		(myProp setCycle: Forward)
*/
(class Cycle of Object
    (properties
        client 0					// The object to which this is attached.
        caller 0					// The object that is cue()'d when the cycle is complete.
        cycleDir cdFORWARD			// cdFORWARD or cdBACKWARD.
        cycleCnt 0
        completed 0
    )

    (method (init theClient)
        (if (paramTotal)
            = client theClient
        )
        = cycleCnt gGameTime
        = completed 0
    )


    (method (nextCel)
        return 
            (if (< Abs((- gGameTime cycleCnt)) (send client:cycleSpeed))
                (send client:cel)
            )(else
                = cycleCnt gGameTime
                + (send client:cel) cycleDir
            )
    )


    (method (cycleDone)
    )


    (method (motionCue)
        (send client:cycler(0))
        (if (completed and IsObject(caller))
            (send caller:cue())
        )
        (self:dispose())
    )

)

/*
	Forward is a cycler that cycles through cels forward.
	
	Example usage::
	
		(bird setCycle: Forward)
		
	See also: :class:`Reverse`.
*/
(class Forward of Cycle
    (properties
    	name "Fwd"
        client 0					// The object to which this is attached.
        caller 0					// The object that is cue()'d when the cycle is complete.
        cycleDir cdFORWARD			// cdFORWARD or cdBACKWARD.
        cycleCnt 0
        completed 0
    )

    (method (doit)
        (var fwdNextCel)
        = fwdNextCel (self:nextCel())
        (if (> fwdNextCel (send client:lastCel()))
            (self:cycleDone())
        )(else
            (send client:cel(fwdNextCel))
        )
    )


    (method (cycleDone)
        (send client:cel(0))
    )
)

/*
	Walk is a cycler used on Actors. It stops the cel cycling when
	the Actor is not moving.
*/
(class Walk of Forward
    (properties
        client 0					// The object to which this is attached.
        caller 0					// The object that is cue()'d when the cycle is complete.
        cycleDir cdFORWARD			// cdFORWARD or cdBACKWARD.
        cycleCnt 0
        completed 0
    )

    (method (doit)
        (var temp0)
        (if (not (send client:isStopped()))
            (super:doit())
        )
    )
)

/*
	CycleTo is a cycler that cycles to a specific cel, then stops.
	
	Example usage::
	
		; Cycle the door forward to cel 4, then stop.
		(door setCycle: CycleTo 4 cdFORWARD)
*/
(class CycleTo of Cycle
    (properties
        name "CT"
        client 0					// The object to which this is attached.
        caller 0					// The object that is cue()'d when the cycle is complete.
        cycleDir cdFORWARD			// cdFORWARD or cdBACKWARD.
        cycleCnt 0
        completed 0
        endCel 0
    )

	/*
	.. function:: init(theClient theEndCel theCycleDir [theCaller])
	
		:param heapPtr theClient: The :class:`Prop` to which this is attached.
		:param number theEndCel: The cel number that we are cycling to.
		:param number theCycleDir: cdFORWARD or cdBACKWARD.
		:param heapPtr theCaller: Optional object on which we call cue() when the cycle is finished.
	*/
    (method (init theClient theEndCel theCycleDir theCaller)
        (var clientLastCel)
        (super:init(theClient))
        = cycleDir theCycleDir
        (if (== paramTotal 4)
            = caller theCaller
        )
        = clientLastCel (send client:lastCel())
        = endCel 
            (if (> theEndCel clientLastCel)
                clientLastCel
            )(else
                theEndCel
            )
    )


    (method (doit)
        (var cTNextCel, clientLastCel)
        = clientLastCel (send client:lastCel())
        (if (> endCel clientLastCel)
            = endCel clientLastCel
        )
        = cTNextCel (self:nextCel())
        (send client:cel(
            (if (> cTNextCel clientLastCel)
                0
            )(else
                (if (< cTNextCel 0)
                    clientLastCel
                )(else
                    cTNextCel
                )
            )
))
        (if ((== gGameTime cycleCnt) and (== endCel (send client:cel)))
            (self:cycleDone())
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

/*
	End cycles from the first cel to the last cel in a loop and stops.
	
	Example usage::
	
		; Tell the door to cycle to its last cel and notify us when done.
		(aDoor setCycle: EndLoop self)
*/
(class EndLoop of CycleTo
    (properties
    	name "End"
        client 0					// The object to which this is attached.
        caller 0					// The object that is cue()'d when the cycle is complete.
        cycleDir cdFORWARD			// cdFORWARD or cdBACKWARD.
        cycleCnt 0
        completed 0
        endCel 0
    )

	/*
	.. function:: init(theClient [theCaller])
	
		:param heapPtr theClient: The :class:`Prop` to which this is attached.
		:param heapPtr theCaller: Optional object on which we call cue() when the cycle is finished.
	*/
    (method (init theClient theCaller)
        (super:init(theClient (send theClient:lastCel()) cdFORWARD 
            (if (== paramTotal 2)
                theCaller
            )(else
                0
            )
))
    )
)

/*
	BegLoop cycles from the last cel backward to the first cel in a loop and stops.
*/
(class BegLoop of CycleTo
    (properties
    	name "Beg"
        client 0					// The object to which this is attached.
        caller 0					// The object that is cue()'d when the cycle is complete.
        cycleDir cdFORWARD			// cdFORWARD or cdBACKWARD.
        cycleCnt 0
        completed 0
        endCel 0
    )

	/*
	.. function:: init(theClient [theCaller])
	
		:param heapPtr theClient: The :class:`Prop` to which this is attached.
		:param heapPtr theCaller: Optional object on which we call cue() when the cycle is finished.
	*/
    (method (init theClient theCaller)
        (super:init(theClient 0 cdBACKWARD 
            (if (== paramTotal 2)
                theCaller
            )(else
                0
            ))
		)
    )
)

/*
	Motion is the base class for movers: objects that are responsible for moving Actors around the screen.
*/
(class Motion of Object
    (properties
        client 0			// The :class:`Actor` to which this is attached.
        caller 0			// The object that will get cue()'d when the motion is complete.
        x 0
        y 0
        dx 0				// Step size.
        dy 0
        {b-moveCnt} 0
        {b-i1} 0
        {b-i2} 0
        {b-di} 0
        {b-xAxis} 0			// Which axis is the motion along?
        {b-incr} 0			// Bresenham adjustment.
        completed 0
        xLast 0
        yLast 0
    )

	/*
	.. function::init(theClient theX theY [theCaller])
	
		:param heapPtr theClient: The :class:`Actor` to which this is attached.
		:param number theX: The x destination.
		:param number theY: The y destination.
		:param heapPtr theCaller: Optional object on which we call cue() when the move is finished.
	*/
    (method (init theClient theX theY theCaller)
        (var temp0[2], clientX, clientCycler)
        (if (>= paramTotal 1)
            = client theClient
            (if (>= paramTotal 2)
                = x theX
                (if (>= paramTotal 3)
                    = y theY
                    (if (>= paramTotal 4)
                        = caller theCaller
                    )
                )
            )
        )
        = yLast (= xLast (= completed 0))
        = {b-moveCnt} (+ (+ 1 (send client:moveSpeed)) gGameTime)
        = clientCycler (send client:cycler)
        (if (clientCycler)
            (send (= clientCycler (send client:cycler)):cycleCnt({b-moveCnt}))
        )
        = clientX (send client:x)
        = clientCycler (send client:y)
        (if (GetDistance(clientCycler clientCycler x y))
            (send client:setHeading(GetAngle(clientX clientCycler x y)))
        )
        InitBresen(self)
    )


    (method (doit)
        (var temp0[6])
        (if (>= Abs((- gGameTime {b-moveCnt})) (send client:moveSpeed))
            = {b-moveCnt} gGameTime
            DoBresen(self)
        )
    )


    (method (moveDone)
        = completed 1
        (if (caller)
            = gDoMotionCue TRUE
        )(else
            (self:motionCue())
        )
    )


    (method (setTarget theX theY)
        (if (paramTotal)
            = x theX
            = y theY
        )
    )


    (method (onTarget)
        return 
            (if (== (send client:x) x)
                == (send client:y) y
            )(else
                0
            )
    )


    (method (motionCue)
        (send client:mover(0))
        (if (completed and IsObject(caller))
            (send caller:cue())
        )
        (self:dispose())
    )
)

/*
	MoveTo is the simplest of movers. It moves an Actor to a destination, then stops.
	If the Actor needs to maneuver around room obstacles, :class:`PolyPath` should be used instead.
	
	Example usage::

		; Move a bird to (200, 37), then call the cue() method on us.	
		(bird setMotion: MoveTo 200 37 self)
*/
(class MoveTo of Motion
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
    )

    (method (onTarget)
        return 
            (if (<= Abs((- (send client:x) x)) (send client:xStep))
                <= Abs((- (send client:y) y)) (send client:yStep)
            )(else
                0
            )
    )

)
