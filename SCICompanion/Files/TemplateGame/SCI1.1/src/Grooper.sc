(version 2)
(include "sci.sh")
(use "Main")
(use "StopWalk")
(use "Helpers")
(use "Cycle")
(use "Obj")
(script 977)


(local
    local0[8] = (2 6 4 0 3 5 1 7 )
    local8[8] = (3 6 0 4 2 5 1 7 )
)

/*
	This code is responsible for determining the correct view loop to use
	based on an object's heading.
*/
(class Grooper of Code
    (properties
        client 0
        oldCycler 0
        oldMover 0
        caller 0
    )

    (method (doit theClient theAngle theCaller param4)
        (var temp0, temp1)
        (if (not client)
            = client theClient
        )
        (if (>= paramTotal 3)
            = caller theCaller
        )
        (if (& (send client:signal) $0800)
            (if (caller)
                (send caller:cue())
            )
            = caller 0
            return 
        )
        = temp1 
            (if (< NumLoops(client) 8)
                4
            )(else
                8
            )
        (if (not (send gOldCast:contains(client)) or ((>= paramTotal 4) and param4))
            (send client:loop(local8[(* 
                (if (== temp1 4)
                    2
                )(else
                    1
                )
 (/ Modulo((+ (send client:heading) (/ 180 temp1)) 360) (/ 360 temp1)))]))
            (if (caller)
                (send caller:cue())
            )
            = caller 0
            return 
        )
        = temp0 
            (if (((== (send client:loop) (- NumLoops(client) 1)) and (send ((send client:cycler)):isKindOf(StopWalk))) and (== (send ((send client:cycler)):vStopped) -1))
                local0[(send client:cel)]
            )(else
                local0[(send client:loop)]
            )
        (if (oldMover)
            (send oldMover:dispose())
            = oldMover 0
        )
        (if (IsObject(oldCycler) and ((send oldCycler:isMemberOf(Grycler)) or not (send ((send client:cycler)):isMemberOf(Grycler))))
            (send oldCycler:dispose())
            = oldCycler 0
        )
        (if (not oldCycler)
            = oldCycler (send client:cycler)
        )
        (if ((send client:cycler) and (send ((send client:cycler)):isMemberOf(Grycler)))
            (send ((send client:cycler)):dispose())
        )
        = oldMover (send client:mover)
        (send client:
            cycler(0)
            mover(0)
            setMotion(0)
            setCycle(Grycler self temp0)
        )
    )

    (method (dispose)
        (if (IsObject(oldCycler))
            (send oldCycler:dispose())
            = oldCycler 0
        )
        (if (IsObject(oldMover))
            (send oldMover:dispose())
            = oldMover 0
        )
        (if (client)
            (send client:looper(0))
        )
        (super:dispose())
    )


    (method (cue param1)
        (var theCaller)
        (if (not IsObject((send client:mover)))
            (send client:mover(oldMover))
        )
        (if (IsObject(oldCycler))
            (send client:cycler(oldCycler))
        )
        = theCaller caller
        = caller (= oldMover (= oldCycler 0))
        (if (theCaller)
            (send theCaller:cue(rest param1))
        )
    )
)

/*
	This class is used internally by :class:`Grooper`.
*/
(class Grycler of Cycle
    (properties
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
        loopIndex 0
        numOfLoops 0
    )

    (method (init param1 theCaller theLoopIndex)
        (super:init(param1))
        = caller theCaller
        = numOfLoops 
            (if (< NumLoops(client) 8)
                4
            )(else
                8
            )
        = cycleDir neg Sign(AngleDiff((* theLoopIndex 45) (send param1:heading)))
        = loopIndex theLoopIndex
        (if ((self:loopIsCorrect()))
            (if ((send ((send ((send client:looper)):oldCycler)):isKindOf(StopWalk)) and (== (send ((send ((send client:looper)):oldCycler)):vStopped) -1))
                (send client:loop(local8[loopIndex]))
            )
            (self:cycleDone())
        )
    )


    (method (doit)
        (send client:loop((self:nextCel())))
        (if ((self:loopIsCorrect()))
            (self:cycleDone())
        )
    )


    (method (nextCel)
        return 
            (if ((< Abs((- gLastTicks cycleCnt)) (send client:cycleSpeed)) or (self:loopIsCorrect()))
                (send client:loop)
            )(else
                = cycleCnt gLastTicks
                = loopIndex (+ loopIndex (* cycleDir (/ 8 numOfLoops)))
                = loopIndex Modulo(loopIndex 8)
                local8[loopIndex]
            )
    )


    (method (cycleDone)
        = gCastMotionCue (= completed TRUE)
    )


    (method (loopIsCorrect)
        return < Abs(AngleDiff((* loopIndex 45) (send client:heading))) (+ (/ 180 numOfLoops) 1)
    )

)
