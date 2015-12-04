(version 2)
(include "sci.sh")
(use "Main")
(use "Cycle")
(use "System")
(script 986)


/*
	A motion class that moves an Actor in an orbit around another object.
*/
(class Orbit of Motion
    (properties
        client 0			// The :class:`Actor` to which this is attached.
        caller 0			// The object that will get cue()'d when the motion is complete.
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
        centerObj 0
        radius 50
        xTilt 0
        yTilt 0
        angleStep 10
        winding 1
        curAngle 0
    )

    (method (init theClient theCenterObj theRadius theXTilt theYTilt theAngleStep theWinding theCurAngle)
        (var centerObjX, centerObjY, temp2, temp3)
        (if (>= paramTotal 1)
            = client theClient
            (if (>= paramTotal 2)
                = centerObj theCenterObj
                (if (>= paramTotal 3)
                    = radius theRadius
                    (if (>= paramTotal 4)
                        = xTilt theXTilt
                        (if (>= paramTotal 5)
                            = yTilt theYTilt
                            (if (>= paramTotal 6)
                                = angleStep theAngleStep
                                (if (>= paramTotal 7)
                                    = winding theWinding
                                    (if (>= paramTotal 8)
                                        = curAngle theCurAngle
                                    )
                                )
                            )
                        )
                    )
                )
            )
        )
        (if (centerObj)
            = centerObjX (send centerObj:x)
            = centerObjY (send centerObj:y)
        )(else
            = centerObjX 160
            = centerObjY 100
        )
        = temp2 SinMult(curAngle radius)
        = temp3 CosMult((+ yTilt gPicAngle) CosMult(curAngle radius))
        (if (xTilt)
            = temp2 CosMult(xTilt temp2)
            = temp3 (+ temp3 SinMult(xTilt temp2))
        )
        = x (+ centerObjX temp2)
        = y (- centerObjY temp3)
        = curAngle UModulo((+ curAngle (* winding angleStep)) 360)
        (super:init(client x y))
    )


    (method (moveDone)
        (self:init())
    )

)
