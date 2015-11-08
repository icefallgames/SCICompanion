(version 2)
(include "sci.sh")
(use "Cycle")
(script 955)


/*
	This Motion class appears to simply make the :class:`Actor` face in the direction of a target.
*/
(class Track of Motion
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
        who 0
        xOffset 0
        yOffset 0
        zOffset 0
    )

    (method (init theClient theWho theXOffset theYOffset theZOffset theCaller)
        = client theClient
        = who theWho
        (if (>= paramTotal 3)
            = xOffset theXOffset
            (if (>= paramTotal 4)
                = yOffset theYOffset
                (if (>= paramTotal 5)
                    = zOffset theZOffset
                    (if (>= paramTotal 6)
                        = caller theCaller
                    )
                )
            )
        )
        (send client:
            ignoreActors()
            illegalBits(0)
        )
        (self:doit())
    )


    (method (doit)
        (var whoHeading)
        = whoHeading (send who:heading)
        (send client:
            heading(whoHeading)
            x(+ (send who:x) xOffset)
            y(+ (send who:y) yOffset)
            z(+ (send who:z) zOffset)
        )
        (if ((send client:looper))
            (send ((send client:looper)):doit(client whoHeading))
        )(else
            DirLoop(client whoHeading)
        )
    )

)
