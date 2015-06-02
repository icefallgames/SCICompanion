(version 2)
(include "sci.sh")
(use "Print")
(use "Cycle")
(script 983)



(class Path of MoveTo
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
        intermediate 0
        value 0
    )

    (method (init theClient param2 param3)
        (if (paramTotal)
            = client theClient
            = caller 
                (if (>= paramTotal 2)
                    param2
                )(else
                    0
                )
            = intermediate 
                (if (== paramTotal 3)
                    param3
                )(else
                    0
                )
            = value -1
            = x (send client:x)
            = y (send client:y)
        )
        (if ((self:atEnd()))
            (self:moveDone())
        )(else
            (self:next())
            (super:init(client x y))
        )
    )


    (method (moveDone)
        (if ((self:atEnd()))
            (super:moveDone())
        )(else
            (if (intermediate)
                (send intermediate:cue((/ value 2)))
            )
            (self:next())
            (super:init(client x y))
        )
    )


    (method (at)
        (var temp0[20])
        (Print:
            addTextF("%s needs an 'at:' method." name)
            init()
        )
        return 0
    )


    (method (next)
        = x (self:at(++value))
        = y (self:at(++value))
    )


    (method (atEnd)
        return 
            (if (== (self:at((+ value 1))) -32768)
            )(else
                == (self:at((+ value 2))) -32768
            )
    )

)
(class RelPath of Path
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
        intermediate 0
        value 0
    )

    (method (next)
        = x (+ x (self:at(++value)))
        = y (+ y (self:at(++value)))
    )

)
