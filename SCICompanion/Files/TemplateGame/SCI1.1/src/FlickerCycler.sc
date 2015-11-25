// Contains the FlickerCycler class.
(version 2)
(include "sci.sh")
(use "Main")
(use "Cycle")
(script 21)

// A cycler that causes a :class:`Prop` to flicker on and off randomly.
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
        (if (< Abs((- gGameTime cycleCnt)) cycleSpeed)
            (send client:cel)
        )(else
            = cycleSpeed Random(5 30)
            (send client:cel)
            = cycleCnt gGameTime
            (if ((send client:isNotHidden()))
                (send client:hide())
            )(else
                (send client:show())
            )
        )
    )

)
