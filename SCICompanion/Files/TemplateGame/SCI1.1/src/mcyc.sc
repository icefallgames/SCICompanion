(version 2)
(include "sci.sh")
(use "Main")
(use "Cycle")
(use "Obj")
(script 942)



(class MCyc of Cycle
    (properties
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
        value 0
        points 0
        size 0
    )

    (method (init theClient thePoints theCaller theCycleDir)
        (var temp0[2])
        = client theClient
        = points thePoints
        (if (>= paramTotal 3)
            (if (>= paramTotal 4)
                = cycleDir theCycleDir
                = caller theCaller
            )(else
                (if (IsObject(theCaller))
                    = caller theCaller
                )(else
                    = cycleDir theCaller
                )
            )
        )
        = size 0
        (while (<> GetValueAt(points size) -32768)
            ++size
        )
        (if (== cycleDir 1)
            = value 0
        )(else
            = value (- size 4)
        )
        (super:init())
    )


    (method (doit)
        (if (>= Abs((- gLastTicks cycleCnt)) (send client:cycleSpeed))
            = cycleCnt gLastTicks
            (self:nextCel())
        )
    )


    (method (nextCel)
        (send client:
            loop(GetValueAt(points value))
            cel(GetValueAt(points (+ value 1)))
            x(GetValueAt(points (+ value 2)))
            y(GetValueAt(points (+ value 3)))
        )
        = value (+ value (* cycleDir 4))
        (if (((== cycleDir 1) and (>= value size)) or ((== cycleDir -1) and (< value 0)))
            (self:cycleDone())
        )
    )


    (method (cycleDone)
        = completed 1
        = value 0
        (if (caller)
            = gCastMotionCue TRUE
        )(else
            (self:motionCue())
        )
    )

)
