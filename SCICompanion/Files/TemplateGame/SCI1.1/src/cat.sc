(version 2)
(include "sci.sh")
(use "Main")
(use "Controls")
(use "Feature")
(use "Obj")
(script 976)



(class Cat of Actor
    (properties
        x 0
        y 0
        z 0
        heading 0
        noun 0
        modNum -1
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        sightAngle $6789
        actions 0
        onMeCheck $6789
        state $0000
        approachX 0
        approachY 0
        approachDist 0
        _approachVerbs 0
        yStep 2
        view -1
        loop 0
        cel 0
        priority 0
        underBits 0
        signal $0000
        lsTop 0
        lsLeft 0
        lsBottom 0
        lsRight 0
        brTop 0
        brLeft 0
        brBottom 0
        brRight 0
        scaleSignal $0000
        scaleX 128
        scaleY 128
        maxScale 128
        cycleSpeed 6
        script 0
        cycler 0
        timer 0
        detailLevel 0
        scaler 0
        illegalBits $8000
        xLast 0
        yLast 0
        xStep 3
        origStep 770
        moveSpeed 6
        blocks 0
        baseSetter 0
        mover 0
        looper 0
        viewer 0
        avoider 0
        code 0
        top -1
        left -1
        bottom -1
        right -1
        diagonal 0
        doCast 0
        outOfTouch 1
        caller 0
        active 0
        dx 0
        dy 0
    )

    (method (doit)
        (var gUserCurEvent)
        (if (not doCast)
            = active 0
        )(else
            (if (active)
                = gUserCurEvent (send gUser:curEvent)
                (self:posn((+ (send gUserCurEvent:x) dx) (+ (send gUserCurEvent:y) dy) z))
            )
        )
        (super:doit())
    )


    (method (handleEvent pEvent)
        (if ((super:handleEvent(pEvent)))
        )(else
            (if (active and (== (send pEvent:type) evMOUSERELEASE))
                = active 0
                (send pEvent:claimed(1))
                LocalToGlobal(pEvent)
                (self:posn((+ (send pEvent:x) dx) (+ (send pEvent:y) dy) z))
                GlobalToLocal(pEvent)
                (if (caller)
                    (send caller:cue(self))
                )
            )(else
                (if (proc255_2(self pEvent))
                    (send pEvent:claimed(1))
                    (self:track(pEvent))
                )
            )
        )
    )


    (method (posn theX theY theZ)
        (var temp0)
        (if (paramTotal)
            = x theX
            (if (>= paramTotal 2)
                = y theY
                (if (>= paramTotal 3)
                    = z theZ
                )
            )
        )
        = temp0 Sign(diagonal)
        (if (not 
        (if (((== -1 top) and (== top bottom)) and (== bottom left))
            == left right
        )
)
            (switch (Abs(diagonal))
                (case 1
                    = y (+ 
                        (if (> temp0 0)
                            top
                        )(else
                            bottom
                        )
 (/ (* (* (- right x) (- bottom top)) temp0) (- right left)))
                )
                (case 2
                    = x (+ 
                        (if (> temp0 0)
                            left
                        )(else
                            right
                        )
 (/ (* (* (- bottom y) (- right left)) temp0) (- bottom top)))
                )
            )
            = x Max(left Min(right x))
            = y Max(top Min(bottom y))
        )
        (super:posn(x y z))
    )


    (method (canBeHere)
        return 1
    )


    (method (findPosn)
        return 1
    )


    (method (track param1)
        (var newCollect)
        LocalToGlobal(param1)
        = dx (- x (send param1:x))
        = dy (- y (send param1:y))
        (if (doCast)
            = active 1
        )(else
            = newCollect (Collect:new())
            (send newCollect:add(self))
            (while ((<> 2 (send param1:type)) and (outOfTouch or proc255_2(self (send param1:
                    type(1)
                    yourself()
                )
)))
                (self:posn((+ (send param1:x) dx) (+ (send param1:y) dy) z))
                Animate((send newCollect:elements) 1)
                GetEvent(32767 param1)
            )
            (send newCollect:
                release()
                dispose()
            )
            (if (caller)
                (send caller:cue(self))
            )
            GlobalToLocal(param1)
        )
    )

)
