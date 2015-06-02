(version 2)
(include "sci.sh")
(use "Main")
(use "Cycle")
(use "Obj")
(script 19)


(local
    local0[8] = (7 9 11 13 0 2 3 5 )

)
(class GravMover of Motion
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
        curHeading 0
        {b-moveCnt2} 0
        theSpeed 0
    )

    (method (init theClient theCurHeading theY)
        (if (paramTotal)
            = client theClient
            (if (> paramTotal 1)
                (if (== paramTotal 2)
                    = curHeading theCurHeading
                    (self:setTarget())
                    = theSpeed (/ GetDistance(x y (send client:x) (send client:y)) 10)
                )(else
                    = curHeading GetAngle((send client:x) (send client:y) theCurHeading theY)
                    = x theCurHeading
                    = y theY
                    = theSpeed GetDistance(x y (send client:x) (send client:y))
                    (self:setTarget())
                )
            )(else
                = curHeading (send client:heading)
                (self:setTarget())
                = theSpeed (/ GetDistance(x y (send client:x) (send client:y)) 10)
            )
        )
        = {b-moveCnt2} (+ (+ 1 theSpeed) gLastTicks)
        (super:init())
    )


    (method (doit)
        (var temp0[8])
        (if (>= Abs((- gLastTicks {b-moveCnt2})) theSpeed)
            = {b-moveCnt2} gLastTicks
            (if (dx)
                (if (> dx 0)
                    --dx
                )(else
                    ++dx
                )
            )(else
                = dx (- Random(1 3) 2)
            )
            (if (dy)
                (if (> dy 0)
                    --dy
                )(else
                    ++dy
                )
            )(else
                = dy (- Random(1 3) 2)
            )
        )
        (if (>= Abs((- gLastTicks {b-moveCnt})) (send client:moveSpeed))
            = {b-moveCnt} gLastTicks
            DoBresen(self)
        )
    )


    (method (setTarget)
        = x (+ (send client:x) SinMult(curHeading 500))
        = y (- (send client:y) CosMult(curHeading 500))
    )


    (method (onTarget)
        return 0
    )

)
(class SpecialLooper of Code
    (properties
        oldDir -1
        client 0
        oldMover 0
    )

    (method (init theClient)
        = client theClient
        (send client:looper(self))
    )


    (method (doit theClient theOldDir)
        = client theClient
        (if (<> theOldDir oldDir)
            (send client:heading(theOldDir))
            (if (< local0[(/ theOldDir 45)] local0[(/ oldDir 45)])
                (send client:setCycle(CT local0[(/ theOldDir 45)] -1))
            )(else
                (send client:setCycle(CT local0[(/ theOldDir 45)] 1))
            )
            = oldDir theOldDir
        )
    )


    (method (dispose)
        (if (client)
            (send client:looper(0))
        )
        (super:dispose())
    )

)
(class SpecialCycler of Cycle
    (properties
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
        theCycleSpeed 6
        isMover 0
    )

    (method (doit)
        (var specialCyclerNextCel)
        (if ((not isMover and (send client:mover)) and (< (+ (send ((send client:mover)):dx) (send ((send client:mover)):dy)) 3))
            = isMover 1
            = cycleDir 
                (if (Random(0 1))
                    1
                )(else
                    -1
                )
            = theCycleSpeed Random(3 15)
        )(else
            (if ((send client:mover) and (>= (+ (send ((send client:mover)):dx) (send ((send client:mover)):dy)) 3))
                = theCycleSpeed (- 15 (+ (send ((send client:mover)):dx) (send ((send client:mover)):dy)))
                = isMover 0
            )
        )
        = specialCyclerNextCel (self:nextCel())
        (if ((> specialCyclerNextCel (send client:lastCel())) or (< specialCyclerNextCel 0))
            (self:cycleDone(specialCyclerNextCel))
        )(else
            (send client:cel(specialCyclerNextCel))
        )
    )


    (method (nextCel)
        return 
            (if (< Abs((- gLastTicks cycleCnt)) theCycleSpeed)
                (send client:cel)
            )(else
                = cycleCnt gLastTicks
                + (send client:cel) cycleDir
            )
    )


    (method (cycleDone param1)
        (if (> param1 (send client:lastCel()))
            (send client:cel(0))
        )(else
            (send client:cel((send client:lastCel())))
        )
    )

)
