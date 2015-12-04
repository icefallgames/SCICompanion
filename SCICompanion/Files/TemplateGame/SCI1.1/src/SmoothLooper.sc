(version 2)
(include "sci.sh")
(exports
    0 SmoothLooper
)
(use "Cycle")
(use "System")
(script 968)


/*
	SmoothLooper is a looper (a class that determines the view loop based on Actor direction)
	that lets you specify a different view that is used when changing directions.
	This allows for smooth turns when an :class:`Actor` heads in a new direction.
	
	Example usage::

		(instance mySmooper of SmoothLooper
			(properties
				cycleSpeed 6
				vChangeDir 3931
			)
		)
		
		// Then in code:
		(theGuy looper: mySmooper)
		
*/
(class public SmoothLooper of Code
    (properties
        nextLoop 0
        client 0
        oldCycler 0
        oldMover 0
        newMover 0
        oldCycleSpeed 0
        cycleSpeed 0
        inProgress 0
        vNormal 0
        vChangeDir 0
    )

    (method (doit theClient theAngle)
        (var clientLoop, temp1)
        (if (& (send theClient:signal) $0800)
            return 
        )
        = temp1 0
        (if (inProgress)
            (if (newMover)
                (send newMover:dispose())
            )
            = newMover (send theClient:mover)
            (send theClient:mover(0))
            return 
        )(else
            (if (not vNormal)
                = vNormal (send theClient:view)
            )
            = client theClient
            = inProgress 1
        )
        (if ((> (send client:loop) 3) and (== (send client:view) vNormal))
            (if (inProgress)
                (if (IsObject(oldMover))
                    (send oldMover:dispose())
                )
            )(else
                (send client:view(vNormal))
                DirLoop(client theAngle)
            )
        )
        (switch (= clientLoop (send client:loop))
            (case 3
                (if ((<= theAngle 45) or (> theAngle 315))
                )(else
                    (if (<= theAngle 135)
                        = clientLoop 4
                        = nextLoop 0
                        = temp1 1
                    )(else
                        (if (<= theAngle 225)
                            = clientLoop 4
                            = nextLoop 16
                            = temp1 1
                        )(else
                            (if (<= theAngle 315)
                                = clientLoop 5
                                = temp1 (= nextLoop 1)
                            )
                        )
                    )
                )
            )
            (case 0
                (if ((<= theAngle 45) or (> theAngle 315))
                    = clientLoop 6
                    = nextLoop 3
                    = temp1 1
                )(else
                    (if (<= theAngle 135)
                    )(else
                        (if (<= theAngle 225)
                            = clientLoop 0
                            = nextLoop 2
                            = temp1 1
                        )(else
                            (if (<= theAngle 315)
                                = clientLoop 6
                                = nextLoop 21
                                = temp1 1
                            )
                        )
                    )
                )
            )
            (case 1
                (if ((<= theAngle 45) or (> theAngle 315))
                    = clientLoop 7
                    = nextLoop 3
                    = temp1 1
                )(else
                    (if (<= theAngle 135)
                        = clientLoop 1
                        = nextLoop 18
                        = temp1 1
                    )(else
                        (if (<= theAngle 225)
                            = clientLoop 1
                            = nextLoop 2
                            = temp1 1
                        )(else
                            <= theAngle 315
                        )
                    )
                )
            )
            (case 2
                (if ((<= theAngle 45) or (> theAngle 315))
                    = clientLoop 3
                    = nextLoop 23
                    = temp1 1
                )(else
                    (if (<= theAngle 135)
                        = clientLoop 2
                        = nextLoop 0
                        = temp1 1
                    )(else
                        (if (<= theAngle 225)
                        )(else
                            (if (<= theAngle 315)
                                = clientLoop 3
                                = temp1 (= nextLoop 1)
                            )
                        )
                    )
                )
            )
        )
        (if (temp1)
            = oldCycler (send client:cycler)
            = oldMover (send client:mover)
            = oldCycleSpeed (send client:cycleSpeed)
            (send client:
                view(vChangeDir)
                cycleSpeed(cycleSpeed)
                mover(0)
                cycler(0)
                loop(clientLoop)
                cel(0)
                setCycle(EndLoop self)
            )
        )(else
            = inProgress 0
        )
    )


    (method (dispose)
        (if (oldMover)
            (send oldMover:dispose())
        )
        (if (newMover)
            (send newMover:dispose())
        )
        (if (oldCycler)
            (send oldCycler:dispose())
        )
        = inProgress (= oldMover (= newMover (= oldCycler 0)))
        (send client:
            view(vNormal)
            looper(0)
        )
        DirLoop(client (send client:heading))
        (super:dispose())
    )


    (method (cue)
        (var temp0[2])
        (if (< nextLoop 15)
            (send client:
                view(vNormal)
                loop(nextLoop)
                mover(oldMover)
                cycler(oldCycler)
                cycleSpeed(oldCycleSpeed)
            )
            = inProgress (= oldCycler (= oldMover 0))
            (if (newMover)
                (send client:setMotion(newMover))
                = newMover 0
            )
        )(else
            = nextLoop (- nextLoop 16)
            (send client:
                loop(nextLoop)
                cel(0)
                setCycle(EndLoop self)
            )
            = nextLoop 
                (switch (nextLoop)
                    (case 0
                        2
                    )
                    (case 5
                        1
                    )
                    (case 2
                        0
                    )
                    (case 7
                        3
                    )
                )
        )
    )

)
