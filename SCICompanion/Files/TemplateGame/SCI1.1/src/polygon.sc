(version 2)
(include "sci.sh")
(use "Obj")
(script 946)



(class Polygon of Obj
    (properties
        size 0
        points 0
        type $0001
        dynamic 0
    )

    (method (init param1)
        (var temp0[2])
        = size (/ paramTotal 2)
        = points Memory(memALLOC_CRIT (* 2 paramTotal))
        StrCpy(points @param1 neg * paramTotal 2)
        = dynamic 1
    )


    (method (dispose)
        (if (dynamic and points)
            Memory(memFREE points)
        )
        (super:dispose())
    )

)
