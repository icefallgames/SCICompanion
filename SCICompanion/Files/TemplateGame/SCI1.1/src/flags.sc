(version 2)
(include "sci.sh")
(use "Obj")
(script 985)



(class Flags of Obj
    (properties
        size 0
        array 0
    )

    (method (init)
        (if (size and not array)
            (self:setSize(size))
        )
    )


    (method (dispose)
        (if (array)
            Memory(memFREE array)
            = array 0
        )
        (super:dispose())
    )


    (method (setSize param1)
        (var temp0, temp1)
        = temp0 (/ param1 16)
        (if (% param1 16)
            ++temp0
        )
        = size (* temp0 16)
        = array Memory(memALLOC_CRIT (* temp0 2))
        = temp1 0
        (while (< temp1 temp0)
            Memory(memPOKE + array (* temp1 2) 0)
            ++temp1
        )
    )


    (method (set param1)
        (var temp0, temp1)
        (while (paramTotal)
            = temp1 (* (/ param1[--paramTotal] 16) 2)
            = temp0 Memory(memPEEK (+ array temp1))
            = temp0 (| temp0 (>> $8000 (% param1[paramTotal] 16)))
            Memory(memPOKE + array temp1 temp0)
        )
    )


    (method (clear param1)
        (var temp0, temp1)
        (while (paramTotal)
            = temp1 (* (/ param1[--paramTotal] 16) 2)
            = temp0 Memory(memPEEK (+ array temp1))
            = temp0 (& temp0 bnot (>> $8000 (% param1[paramTotal] 16)))
            Memory(memPOKE + array temp1 temp0)
        )
    )


    (method (test param1)
        (var temp0, temp1)
        = temp1 (* (/ param1 16) 2)
        = temp0 Memory(memPEEK (+ array temp1))
        return & temp0 (>> $8000 (% param1 16))
    )

)
