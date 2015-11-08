(version 2)
(include "sci.sh")
(use "DialogControls")
(use "Cycle")
(script 967)


/*
	This is a dialog icon that animates ("dialog cycle icon").
*/
(class DCIcon of DIcon
    (properties
        type $0004
        state $0000
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        key 0
        said 0
        value 0
        view 0
        loop 0
        cel 0
        cycler 0
        cycleSpeed 6
        signal $0000
    )

    (method (init)
        (send (= cycler (Fwd:new())):init(self))
    )


    (method (dispose)
        (if (cycler)
            (send cycler:dispose())
        )
        (super:dispose())
    )


    (method (cycle)
        (var theCel)
        (if (cycler)
            = theCel cel
            (send cycler:doit())
            (if (<> cel theCel)
                (self:draw())
            )
        )
    )


    (method (lastCel)
        return - NumCels(self) 1
    )

)
