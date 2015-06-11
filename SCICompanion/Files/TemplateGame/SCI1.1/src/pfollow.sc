(version 2)
(include "sci.sh")
(use "Main")
(use "PolyPath")
(script 932)



(class PFollow of PolyPath
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
        value 2
        points 0
        finalX 0
        finalY 0
        obstacles 0
        who 0
        distance 0
        targetX 0
        targetY 0
    )

    (method (init theClient theWho theDistance param4)
        (var temp0)
        (if (paramTotal)
            = temp0 
                (if (>= paramTotal 4)
                    param4
                )(else
                    (send gRoom:obstacles)
                )
            (if (>= paramTotal 1)
                = client theClient
                (if (>= paramTotal 2)
                    = who theWho
                    = targetX (send who:x)
                    = targetY (send who:y)
                    (if (>= paramTotal 3)
                        = distance theDistance
                    )
                )
            )
            (super:init(client targetX targetY 0 1 temp0))
        )(else
            (super:init())
        )
    )


    (method (doit)
        (var temp0, temp1)
        (if (> GetDistance(targetX targetY (send who:x) (send who:y)) distance)
            (if (points)
                Memory(memFREE points)
            )
            = points 0
            = value 2
            (self:init(client who))
            0
        )(else
            = temp0 (send client:distanceTo(who))
            (if (<= temp0 distance)
                = temp1 GetAngle((send client:x) (send client:y) (send who:x) (send who:y))
                (if (<> (send client:heading) temp1)
                    (send client:setHeading(temp1))
                )
                = xLast (send client:x)
                = yLast (send client:y)
                = {b-moveCnt} gLastTicks
                0
            )(else
                (super:doit())
            )
        )
    )


    (method (moveDone)
        (self:
            setTarget()
            init()
        )
    )

)
