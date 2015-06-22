(version 2)
(include "sci.sh")
(use "Main")
(use "PolyPath")
(use "Obj")
(script 930)



(class PChase of PolyPath
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

    (method (init theClient theWho theDistance theCaller theObstacles)
        (var temp0[20])
        (if (paramTotal)
            (if (>= paramTotal 5)
                = obstacles theObstacles
            )(else
                (if (not IsObject(obstacles))
                    = obstacles (send gRoom:obstacles)
                )
            )
            (if (>= paramTotal 1)
                = client theClient
                (if (>= paramTotal 2)
                    = who theWho
                    = targetX (send who:x)
                    = targetY (send who:y)
                    (if (>= paramTotal 3)
                        = distance theDistance
                        (if (>= paramTotal 4)
                            = caller theCaller
                        )
                    )
                )
            )
            (super:init(client targetX targetY caller 1 obstacles))
        )(else
            (super:init())
        )
    )


    (method (doit)
        (var temp0)
        (if (> GetDistance(targetX targetY (send who:x) (send who:y)) distance)
            (if (points)
                Memory(memFREE points)
            )
            = points 0
            = value 2
            (self:init(client who))
        )(else
            = temp0 (send client:distanceTo(who))
            (if (<= temp0 distance)
                (self:moveDone())
            )(else
                (super:doit())
            )
        )
    )


    (method (moveDone)
        (var temp0, temp1[20])
        = temp0 (send client:distanceTo(who))
        (if (<= temp0 distance)
            (super:moveDone())
        )(else
            (if (== GetValueAt(points value) 30583)
                (if (points)
                    Memory(memFREE points)
                )
                = points 0
                = value 2
                (self:init(client who))
            )(else
                (self:
                    setTarget()
                    init()
                )
            )
        )
    )

)
