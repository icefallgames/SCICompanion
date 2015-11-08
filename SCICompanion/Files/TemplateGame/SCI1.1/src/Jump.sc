(version 2)
(include "sci.sh")
(use "Main")
(use "Cycle")
(script 991)

/*
	This serves as a base class for the :class:`JumpTo` motion.
*/
(class Jump of Motion
    (properties
        client 0
        caller 0
        x 20000
        y 20000
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
        gx 0
        gy 3
        xStep 20000
        yStep 0
        signal $0000
        illegalBits $0000
        waitApogeeX 1
        waitApogeeY 1
    )

    (method (init theClient theCaller)
        (var clientHeading)
        = client theClient
        (if (== paramTotal 2)
            = caller theCaller
        )
        = illegalBits (send client:illegalBits)
        = signal (send client:signal)
        (send client:
            illegalBits(0)
            setPri()
        )
        (if (== xStep 20000)
            = clientHeading (send client:heading)
            = xStep 
                (if (((> clientHeading 330) or (< clientHeading 30)) or ((< 150 clientHeading) and (< clientHeading 210)))
                    0
                )(else
                    (if (< clientHeading 180)
                        (send client:xStep)
                    )(else
                        neg (send client:xStep)
                    )
                )
        )
        (if (not 
        (if (waitApogeeX)
            < (* xStep gx) 0
        )
)
            = waitApogeeX 0
        )
        (if (not 
        (if (waitApogeeY)
            < (* yStep gy) 0
        )
)
            = waitApogeeY 0
        )
        = {b-moveCnt} gLastTicks
        (self:setTest())
    )


    (method (doit)
        (var theXStep, theYStep)
        (if (>= Abs((- gLastTicks {b-moveCnt})) (send client:moveSpeed))
            = {b-moveCnt} gLastTicks
            = xLast (send client:x)
            = yLast (send client:y)
            (send client:
                x(+ xLast xStep)
                y(+ yLast yStep)
            )
            = theXStep xStep
            = theYStep yStep
            = xStep (+ xStep gx)
            = yStep (+ yStep gy)
            (if ((not waitApogeeX and (<> x 20000)) and (<= 0 (* dx (- (send client:x) x))))
                (send client:x(x))
                (self:moveDone())
                return 
            )
            (if ((not waitApogeeY and (<> y 20000)) and (<= 0 (* dy (- (send client:y) y))))
                (send client:y(y))
                (self:moveDone())
                return 
            )
            (if (<= (* theXStep xStep) 0)
                = waitApogeeX 0
                (self:setTest())
            )
            (if (<= (* theYStep yStep) 0)
                = waitApogeeY 0
                (self:setTest())
            )
        )
    )


    (method (moveDone)
        (send client:
            illegalBits(illegalBits)
            signal(signal)
        )
        (if (caller)
            = gCastMotionCue TRUE
            = completed TRUE
        )
    )


    (method (motionCue)
        (send client:mover(0))
        (if (completed and IsObject(caller))
            (send caller:cue())
        )
        (self:dispose())
    )


    (method (setTest)
        = dx 
            (if ((> (send client:x) x) or ((== (send client:x) x) and (> xStep 0)))
                -1
            )(else
                1
            )
        = dy 
            (if ((> (send client:y) y) or ((== (send client:y) y) and (> yStep 0)))
                -1
            )(else
                1
            )
    )
)

/*
	This makes an :class:`Actor` jump to a certain location.
	
	Example usage::
	
		(theRobot:setMotion(JumpTo 240 150))
*/
(class JumpTo of Jump
    (properties
        client 0
        caller 0
        x 20000
        y 20000
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
        gx 0
        gy 3
        xStep 20000
        yStep 0
        signal $0000
        illegalBits $0000
        waitApogeeX 1
        waitApogeeY 1
    )

	/*
	.. function:: init(theClient theX theY [theCaller])
	
		Initializes the JumpTo motion.
		
		:param heapPtr theClient: The :class:`Actor` to which this applies.
		:param number theX: The x target.
		:param number theY: The y target.
		:param heapPtr theCaller: The optional object that will be cue()'d when the jump is complete.
		
	*/
    (method (init theClient theX theY param4)
        (var temp0, temp1, temp2[52])
        = client theClient
        = x theX
        = y theY
        (if ((== x (send theClient:x)) and (== y (send theClient:y)))
            = illegalBits (send client:illegalBits)
            = signal (send client:signal)
            (self:moveDone())
            return 
        )
        = temp0 (- x (send theClient:x))
        = temp1 (- y (send theClient:y))
        SetJump(self temp0 temp1 gy)
        (if (not temp0)
            = x 20000
        )
        (if (not temp1)
            = y 20000
        )
        (switch (paramTotal)
            (case 3
                (super:init(theClient))
            )
            (case 4
                (super:init(theClient param4))
            )
        )
    )


    (method (moveDone)
        (if (<> x 20000)
            (send client:x(x))
        )
        (if (<> y 20000)
            (send client:y(y))
        )
        (super:moveDone())
    )

)
