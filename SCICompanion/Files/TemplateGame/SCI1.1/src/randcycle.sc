(version 2)
(include "sci.sh")
(use "Main")
(use "Cycle")
(script 941)



(class RandCycle of Cycle
    (properties
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
        count -1
        reset 0
    )

    (method (init param1 param2 theCaller theReset)
        (super:init(param1))
        (if (>= paramTotal 4)
            = reset theReset
        )
        (if (reset)
            (send client:cel(0))
        )
        = cycleCnt GetTime()
        (if (>= paramTotal 2)
            (if (<> param2 -1)
                = count (+ GetTime() param2)
            )(else
                = count -1
            )
            (if (>= paramTotal 3)
                = caller theCaller
            )
        )(else
            = count -1
        )
    )


    (method (doit)
        (var temp0)
        = temp0 GetTime()
        (if ((> count temp0) or (== count -1))
            (if (> (- temp0 cycleCnt) (send client:cycleSpeed))
                (send client:cel((self:nextCel())))
                = cycleCnt GetTime()
            )
        )(else
            (if (reset)
                (send client:cel(0))
            )
            (self:cycleDone())
        )
    )


    (method (nextCel)
        (var temp0)
        return 
            (if (<> NumCels(client) 1)
                (while (== (= temp0 Random(0 (send client:lastCel()))) (send client:cel))
                )
                temp0
            )(else
                0
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
(class RTRandCycle of RandCycle
    (properties
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
        count -1
        reset 0
    )
)
