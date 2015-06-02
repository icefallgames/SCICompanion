(version 2)
(include "sci.sh")
(exports
    0 proc966_0
)
(use "Obj")
(script 966)



(procedure public (proc966_0 param1 param2 param3)
    (var temp0, temp1, temp2, newList)
    = newList (List:new())
    (while (= temp1 FirstNode((send param1:elements)))
        = temp2 NodeValue((= temp1 FirstNode((send param1:elements))))
        (while (temp1)
            = temp0 NodeValue(temp1)
            (if ((send param2:doit(temp0 temp2 rest param3)))
                = temp2 temp0
            )
            = temp1 NextNode(temp1)
        )
        (TE:doit(temp2 param1 newList))
    )
    (send newList:
        eachElementDo(#perform TE newList param1)
        dispose()
    )
)


(instance TE of Code
    (properties)

    (method (doit param1 param2 param3)
        (send param3:addToEnd(param1))
        (send param2:delete(param1))
    )

)
