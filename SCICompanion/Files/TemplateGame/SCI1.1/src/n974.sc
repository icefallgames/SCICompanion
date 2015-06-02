(version 2)
(include "sci.sh")
(exports
    0 proc974_0
)
(use "Obj")
(script 974)



(procedure public (proc974_0 param1 param2)
    (send param2:firstTrue(#perform NC param1))
)


(instance NC of Code
    (properties)

    (method (doit param1 param2)
        return == 0 StrCmp((send param1:name) param2)
    )

)
