(version 2)
(include "sci.sh")
(use "Main")
(use "Print")
(use "Obj")
(script 980)

/*
	The Tutorial class appears to be a script that facilitates having the
	user perform a series of actions on objects. However, its usage remains unclear.
*/
(class Tutorial of Script
    (properties
        client 0
        state $ffff
        start 0
        timer 0
        cycles 0
        seconds 0
        lastSeconds 0
        ticks 0
        lastTicks 0
        register 0
        script 0
        caller 0
        next 0
        nextItem 0
        nextAction 0
        numTries 0
    )

    (method (cue param1)
        = numTries 0
        (super:cue(rest param1))
    )

    (method (waitFor theNextItem theNextAction param3 param4 param5 param6 param7)
        = nextItem theNextItem
        = nextAction theNextAction
        (if (== paramTotal 3)
            TextPrint(param3)
        )(else
            (if (> paramTotal 3)
                (send gTestMessager:say(param3 param4 param5 param6 param7))
            )
        )
    )


    (method (report)
    )
)
