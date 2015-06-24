(version 2)
(include "sci.sh")
(use "Obj")
(script 993)



(class File of Obj
    (properties
        handle 0
    )

    (method (dispose)
        (self:close())
        (super:dispose())
    )


    (method (showStr param1)
        Format(param1 "File: %s" name)
    )


    (method (open openMode)
        = handle 
            (switch (paramTotal)
                (case 0
                    FileIO(fiOPEN name fOPENCREATE)
                )
                (case 1
                    FileIO(fiOPEN name openMode)
                )
                (default 
                    0
                )
            )
        (if (== handle -1)
            = handle 0
        )
        return 
            (if (handle)
                self
            )(else
                0
            )
    )


    (method (readString param1 param2)
        (if (<> paramTotal 2)
            return 0
        )
        (if (not handle)
            (self:open(fOPENFAIL))
        )
        return 
            (if (handle)
                FileIO(fiREAD_STRING param1 param2 handle)
            )(else
                0
            )
    )


    (method (writeString param1)
        (var temp0)
        (if (not handle)
            (self:open())
        )
        (if (handle)
            = temp0 0
            (while (< temp0 paramTotal)
                (if (not FileIO(fiWRITE_STRING handle param1[temp0]))
                    return 0
                )
                ++temp0
            )
        )
        return 1
    )


    (method (write param1 param2)
        (var temp0)
        (if (not handle)
            (self:open())
        )
        return 
            (if (handle)
                FileIO(fiWRITE handle param1 param2)
            )(else
                0
            )
    )


    (method (read param1 param2)
        (if (<> paramTotal 2)
            return 0
        )
        (if (not handle)
            (self:open(fOPENFAIL))
        )
        return 
            (if (handle)
                FileIO(fiREAD handle param1 param2)
            )(else
                0
            )
    )


    (method (seek param1 param2)
        (var temp0)
        = temp0 
            (if (>= paramTotal 2)
                param2
            )(else
                0
            )
        (if (not handle)
            (self:open(fOPENFAIL))
        )
        return 
            (if (handle)
                FileIO(fiSEEK handle param1 temp0)
            )(else
                0
            )
    )


    (method (close)
        (if (handle)
            FileIO(fiCLOSE handle)
            = handle 0
        )
    )


    (method (delete)
        (if (handle)
            (self:close())
        )
        FileIO(fiUNLINK name)
    )

)
