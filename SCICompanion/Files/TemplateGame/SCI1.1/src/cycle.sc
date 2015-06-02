(version 2)
(include "sci.sh")
(use "Main")
(use "Obj")
(script 992)



(class Cycle of Obj
    (properties
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
    )

    (method (init theClient)
        (if (paramTotal)
            = client theClient
        )
        = cycleCnt gLastTicks
        = completed 0
    )


    (method (nextCel)
        return 
            (if (< Abs((- gLastTicks cycleCnt)) (send client:cycleSpeed))
                (send client:cel)
            )(else
                = cycleCnt gLastTicks
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
(class Fwd of Cycle
    (properties
        client 0
        caller 0
        cycleDir 1
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
(class Walk of Fwd
    (properties
        client 0
        caller 0
        cycleDir 1
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
(class CT of Cycle
    (properties
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
        endCel 0
    )

    (method (init param1 param2 theCycleDir theCaller)
        (var clientLastCel)
        (super:init(param1))
        = cycleDir theCycleDir
        (if (== paramTotal 4)
            = caller theCaller
        )
        = clientLastCel (send client:lastCel())
        = endCel 
            (if (> param2 clientLastCel)
                clientLastCel
            )(else
                param2
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
        (if ((== gLastTicks cycleCnt) and (== endCel (send client:cel)))
            (self:cycleDone())
        )
    )


    (method (cycleDone)
        = completed 1
        (if (caller)
            = gCastMotionCue TRUE
        )(else
            (self:motionCue())
        )
    )

)
(class End of CT
    (properties
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
        endCel 0
    )

    (method (init param1 param2)
        (super:init(param1 (send param1:lastCel()) 1 
            (if (== paramTotal 2)
                param2
            )(else
                0
            )
))
    )

)
(class Beg of CT
    (properties
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
        endCel 0
    )

    (method (init param1 param2)
        (super:init(param1 0 -1 
            (if (== paramTotal 2)
                param2
            )(else
                0
            )
))
    )

)
(class SyncWalk of Fwd
    (properties
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
        xLast 0
        yLast 0
    )

    (method (doit)
        (var clientMover)
        = clientMover (send client:mover)
        (if (IsObject(clientMover) and ((<> (send client:x) xLast) or (<> (send client:y) yLast)))
            = xLast (send client:x)
            = yLast (send client:y)
            (super:doit())
        )
    )


    (method (nextCel)
        = cycleCnt (+ gLastTicks (send client:cycleSpeed))
        (super:nextCel())
    )

)
(class Motion of Obj
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
        completed 0
        xLast 0
        yLast 0
    )

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
        = {b-moveCnt} (+ (+ 1 (send client:moveSpeed)) gLastTicks)
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
        (if (>= Abs((- gLastTicks {b-moveCnt})) (send client:moveSpeed))
            = {b-moveCnt} gLastTicks
            DoBresen(self)
        )
    )


    (method (moveDone)
        = completed 1
        (if (caller)
            = gCastMotionCue TRUE
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
(class MoveTo of Motion
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
