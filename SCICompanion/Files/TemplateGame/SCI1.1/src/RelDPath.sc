(version 2)
(include "sci.sh")
(use "DPath")
(script 963)



(class RelDPath of DPath
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
        points 0
        value 0
    )

    (method (setTarget)
        (if (<> (send points:at(value)) -32768)
            = x (+ x (send points:at(value)))
            = y (+ y (send points:at(++value)))
            ++value
        )
    )

)
