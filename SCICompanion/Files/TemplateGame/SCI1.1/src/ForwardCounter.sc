(version 2)
(include "sci.sh")
(include "game.sh")
(use "Cycle")
(script FORWARDCOUNT_SCRIPT)

(class ForwardCounter of Fwd
    (properties
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
        count 0
    )

    (method (init param1 theCount theCaller)
        (super:init(param1))
        (if (>= paramTotal 2)
            = count theCount
            (if (>= paramTotal 3)
                = caller theCaller
            )
        )
    )


    (method (cycleDone)
        (if (--count)
            (super:cycleDone())
        )(else
            = completed 1
            (self:motionCue())
        )
    )

)
