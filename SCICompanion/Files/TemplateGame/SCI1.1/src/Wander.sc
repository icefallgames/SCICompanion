(version 2)
(include "sci.sh")
(use "Cycle")
(script 970)



(class Wander of Motion
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
        distance 30
    )

    (method (init theClient theDistance)
        (if (>= paramTotal 1)
            = client theClient
            (if (>= paramTotal 2)
                = distance theDistance
            )
        )
        (self:setTarget())
        (super:init(client))
    )


    (method (doit)
        (super:doit())
        (if ((send client:isStopped()))
            (self:moveDone())
        )
    )


    (method (moveDone)
        (self:init())
    )


    (method (setTarget)
        (var temp0)
        = temp0 (* distance 2)
        = x (+ (send client:x) (- distance Random(0 temp0)))
        = y (+ (send client:y) (- distance Random(0 temp0)))
    )


    (method (onTarget)
        return 0
    )

)
