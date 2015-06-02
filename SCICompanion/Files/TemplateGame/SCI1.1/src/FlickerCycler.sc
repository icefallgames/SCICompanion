(version 2)
(include "sci.sh")
(use "Main")
(use "Cycle")
(script 21)



(class FlickerCycler of Cycle
    (properties
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
        cycleSpeed 8
    )

    (method (doit)
        (var temp0)
        (self:nextCel())
    )


    (method (nextCel)
        (if (< Abs((- gLastTicks cycleCnt)) cycleSpeed)
            (send client:cel)
        )(else
            = cycleSpeed Random(5 30)
            (send client:cel)
            = cycleCnt gLastTicks
            (if ((send client:isNotHidden()))
                (send client:hide())
            )(else
                (send client:show())
            )
        )
    )

)
