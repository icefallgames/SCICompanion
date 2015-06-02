(version 2)
(include "sci.sh")
(use "Main")
(use "Cycle")
(use "Obj")
(script 25)



(class VelocityMover of MoveTo
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
        vel 30
        grav1x 160
        grav1y 100
        oldGravx 160
        oldGravy 100
        targetScale 0
        isScale 0
        willLand 0
        grav1 10
    )

    (method (init theClient theGrav1x theGrav1y theCaller theWillLand)
        (var temp0[3], clientCycler)
        (if (paramTotal)
            = vel 10
            = willLand (= isScale (= caller 0))
            = client theClient
            (if (> paramTotal 1)
                = grav1x theGrav1x
                = grav1y theGrav1y
                = oldGravx theGrav1x
                = oldGravy theGrav1y
                (if (> paramTotal 3)
                    = caller theCaller
                    (if (> paramTotal 4)
                        = willLand theWillLand
                    )
                )
            )
            (self:moveDone())
        )(else
            = yLast (= xLast (= completed 0))
            = {b-moveCnt} (+ (+ 1 (send client:moveSpeed)) gLastTicks)
            = clientCycler (send client:cycler)
            (if (clientCycler)
                (send (= clientCycler (send client:cycler)):cycleCnt({b-moveCnt}))
            )
            InitBresen(self)
        )
    )


    (method (moveDone)
        (var temp0, temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9[100])
        (if ((willLand and (<= Abs((- (send client:x) grav1x)) 5)) and (<= Abs((- (send client:y) grav1y)) 5))
            = completed 1
            (if (caller)
                = gCastMotionCue TRUE
                return 
            )(else
                (self:motionCue())
                return 
            )
        )
        = temp2 (+ (- 360 (send client:heading)) 90)
        (if (>= temp2 360)
            = temp2 (- temp2 360)
        )
        = temp1 (+ (- 360 GetAngle((send client:x) (send client:y) grav1x grav1y)) 90)
        (if (>= temp1 360)
            = temp1 (- temp1 360)
        )
        (self:readjustWell(temp1 temp2))
        = temp3 (* CosMult(temp2 vel) 100)
        = temp3 (+ (+ (send client:x) 
            (if (> (% temp3 100) 44)
                1
            )(else
                0
            )
) (/ temp3 100))
        = temp4 (* SinMult(temp2 vel) 100)
        = temp4 (- (send client:y) (+ 
            (if (> (% temp4 100) 44)
                1
            )(else
                0
            )
 (/ temp4 100)))
        = temp5 (* CosMult(temp1 grav1) 100)
        = temp5 (+ 
            (if (> (% temp5 100) 44)
                1
            )(else
                0
            )
 (/ temp5 100))
        = temp6 (* SinMult(temp1 grav1) 100)
        = temp6 (+ 
            (if (> (% temp6 100) 44)
                1
            )(else
                0
            )
 (/ temp6 100))
        = temp7 (+ temp5 temp3)
        = temp8 (- temp4 temp6)
        = vel Sqrt((+ (* (- temp7 (send client:x)) (- temp7 (send client:x))) (* (- (send client:y) temp8) (- (send client:y) temp8))))
        (send client:setHeading(GetAngle((send client:x) (send client:y) temp7 temp8)))
        (self:
            x(temp7)
            y(temp8)
        )
        (self:init())
    )


    (method (onTarget)
        return 
            (if (willLand)
                (if ((<= Abs((- (send client:x) grav1x)) 5) and (<= Abs((- (send client:y) grav1y)) 5))
                    = completed 1
                    (if (caller)
                        = gCastMotionCue TRUE
                        return 1
                    )(else
                        (self:motionCue())
                        return 1
                    )
                )(else
                    (if (< vel 0)
                        = vel (+ vel 1)
                    )(else
                        = vel (- vel 1)
                    )
                    return 0
                )
            )(else
                return 0
            )
    )


    (method (motionCue)
        (send client:posn(grav1x grav1y))
        (super:motionCue())
    )


    (method (readjustWell param1 param2)
        (if (IsOneOf(param1 0 90 180 270))
            = grav1x (- oldGravx (- Random(1 20) 10))
        )(else
            = grav1x oldGravx
        )
        (if (IsOneOf(param2 0 90 180 270))
            = grav1y (- oldGravy (- Random(1 20) 10))
        )(else
            = grav1y oldGravy
        )
    )

)
