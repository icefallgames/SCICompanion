(version 2)
(include "sci.sh")
(use "Obj")
(script 949)



(class Blk of Obj
    (properties
        top 0
        left 0
        bottom 0
        right 0
    )

    (method (doit param1)
        return 
            (if (((<= (send param1:brBottom) top) or (> (send param1:brTop) bottom)) or (< (send param1:brRight) left))
            )(else
                >= (send param1:brLeft) right
            )
    )

)
(class Cage of Blk
    (properties
        top 0
        left 0
        bottom 0
        right 0
    )

    (method (doit param1)
        return 
            (if (((>= (send param1:brTop) top) and (>= (send param1:brLeft) left)) and (<= (send param1:brBottom) bottom))
                <= (send param1:brRight) right
            )(else
                0
            )
    )

)
