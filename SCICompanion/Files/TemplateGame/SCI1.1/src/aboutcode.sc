(version 2)
(include "sci.sh")
(include "13.shm")
(exports
    0 aboutCode
)
(use "Main")
(use "Obj")
(script 13)



(instance public aboutCode of Code
    (properties)

    (method (doit)
        (send gTestMessager:say(N_MAIN 0 0 0 0 13))
    )

)
