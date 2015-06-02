(version 2)
(include "sci.sh")
(exports
    0 proc965_0
)
(script 965)



(procedure public (proc965_0 param1 param2 param3)
    (var temp0, temp1)
    = temp1 FirstNode((send param1:elements))
    = temp0 0
    (while (temp1)
        (if ((send param2:doit(NodeValue(temp1) rest param3)))
            ++temp0
        )
        = temp1 NextNode(temp1)
    )
    return temp0
)


