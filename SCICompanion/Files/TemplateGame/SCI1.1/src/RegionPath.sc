(version 2)
(include "sci.sh")
(use "Main")
(use "Print")
(use "Cycle")
(script 984)


/*
	RegionPath is used to describe a path that an :class:`Actor` follows through a region. A
	region can span many rooms. They can be used to give the illusion that a single character
	is wandering around an entire region.
	
	The region points are provided in a buffer that looks like the following::
	
		(local
			localPathPoints[15] = ($7fff room1 x1 y1 x2 y2 x3 y3 $7fff room2 x1 y1 x2 y2 $8000)
		)

	$7fff signals a new room, and $8000 signals the end of the buffer.		
	You are responsible for serving up value from this buffer by overriding the **at** method.
	
	The exact usage of this class is still unknown. Hopefully someone will write a tutorial that describes it well.
*/
(class RegionPath of MoveTo
    (properties
        client 0
        caller 0
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
        completed 1
        xLast 0
        yLast 0
        currentRoom 0
        value -1
        endType 1
        intermediate 0
        initialized 0
        savedOldStuff 0
        theRegion 0
        theOldBits 0
        theOldSignal 0
    )

    (method (init theClient theCaller theIntermediate)
        (if (>= paramTotal 4)
            = value -1
            = initialized 0
            = completed 1
        )
        (if (completed)
            (if (paramTotal)
                = client theClient
                (if (>= paramTotal 2)
                    = caller theCaller
                    (if (>= paramTotal 3)
                        = intermediate theIntermediate
                    )
                )
            )
            (if (not initialized)
                (if (not savedOldStuff)
                    = theOldBits (send client:illegalBits)
                    = theOldSignal (send client:signal)
                    = savedOldStuff 1
                )
                (if (& endType $0002)
                    (self:
                        findPathend()
                        next()
                    )
                    (send client:posn(x y))
                )(else
                    (self:
                        value(0)
                        nextRoom()
                    )
                )
                = initialized 1
                (send client:
                    illegalBits(0)
                    ignoreActors()
                    ignoreHorizon()
                )
            )
            = completed 0
            (self:next())
        )
        (super:init())
        (self:curRoomCheck())
    )


    (method (dispose)
        (if (theRegion)
            (if (not (send (ScriptID(theRegion)):keep))
                (super:dispose())
            )
        )(else
            Printf("%s theRegion: not defined." name)
        )
    )


    (method (moveDone)
        = completed 1
        (if ((self:atEnd()))
            (self:
                value(-1)
                initialized(0)
            )
            (if (& endType $0001)
                (self:init())
            )(else
                (super:moveDone())
            )
        )(else
            (if (intermediate)
                (send intermediate:cue((/ value 2)))
            )
            (if (== (self:at((self:nextValue()))) 32767)
                (self:
                    next()
                    nextRoom()
                )
            )
            (self:init())
        )
    )


    (method (next)
        = x (self:at((self:nextValue(1))))
        = y (self:at((+ value 1)))
    )


    (method (atEnd)
        return 
            (if ((& endType $0002) and (<= (- value 2) 0))
            )(else
                == (self:at((+ value 2))) -32768
            )
    )


    (method (nextRoom)
        (if (& endType $0002)
            (self:findPrevroom())
        )(else
            = currentRoom (self:at((+ value 1)))
        )
        (self:
            next()
            curRoomCheck()
        )
        (send client:posn(x y))
    )


    (method (at)
        Printf("%s needs an 'at:' method." name)
        return 0
    )


    (method (nextValue)
        (var temp0)
        = temp0 (- (* not (& endType $0002) 4) 2)
        return 
            (if (paramTotal)
                return = value (+ value temp0)
            )(else
                return + value temp0
            )
    )


    (method (findPathend)
        = value 0
        (while (<> (self:at(value)) -32768)
            (if (== (self:at(value)) 32767)
                = currentRoom (self:at((+ value 1)))
            )
            ++value
        )
    )


    (method (findPrevroom)
        (var temp0)
        = temp0 0
        (while (< temp0 value)
            (if (== (self:at(temp0)) 32767)
                = currentRoom (self:at((+ temp0 1)))
            )
            ++temp0
        )
    )


    (method (curRoomCheck)
        (var clientZ)
        = clientZ (send client:z)
        (if (== currentRoom gModNum)
            (send client:
                z(
                    (if (>= clientZ 1000)
                        - clientZ 1000
                    )(else
                        clientZ
                    )
				 )
                illegalBits(theOldBits)
                signal(theOldSignal)
            )
        )(else
            (send client:
                z(
                    (if (< clientZ 1000)
                        + clientZ 1000
                    )(else
                        clientZ
                    )
				 )
                illegalBits(0)
                ignoreActors(1)
                ignoreHorizon()
            )
        )
    )

)
