(version 2)
(include "sci.sh")
(use "Main")
(use "PolyPath")
(use "Cycle")
(use "View")
(script 988)



(class Ego of Actor
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
        onMeCheck omcDISABLE
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
        signal $2000
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
        edgeHit 0
    )

    (method (init)
        (super:init())
        (if (not cycler)
            (self:setCycle(Walk))
        )
    )


    (method (doit)
        (super:doit())
        = edgeHit 
            (if (<= x 0)
                4
            )(else
                (if (>= x 319)
                    2
                )(else
                    (if (>= y 189)
                        3
                    )(else
                        (if (<= y (send gRoom:horizon))
                            1
                        )(else
                            0
                        )
                    )
                )
            )
    )


    (method (handleEvent pEvent)
        (var temp0, temp1, temp2)
        = temp1 (send pEvent:type)
        = temp2 (send pEvent:message)
        (if (script and (send script:handleEvent(pEvent)))
            1
        )(else
            (if (& temp1 $0040)
                = temp0 temp2
                (if ((== temp0 0) and (& temp1 $0004))
                    (send pEvent:claimed)
                    return 
                )
                (if (((& temp1 $0004) and (== temp0 (send gUser:prevDir))) and IsObject(mover))
                    = temp0 0
                )
                (send gUser:prevDir(temp0))
                (self:setDirection(temp0))
                (send pEvent:claimed(1))
            )(else
                (if (& temp1 evVERB)
                    (if (& temp1 evMOVE)
                        (switch (global67)
                            (case 0
                                (self:setMotion(MoveTo (send pEvent:x) (+ (send pEvent:y) z)))
                            )
                            (case 1
                                (self:setMotion(PolyPath (send pEvent:x) (+ (send pEvent:y) z)))
                            )
                            (case 2
                                (self:setMotion(PolyPath (send pEvent:x) (+ (send pEvent:y) z) 0 0))
                            )
                        )
                        (send gUser:prevDir(0))
                        (send pEvent:claimed(1))
                    )(else
                        (super:handleEvent(pEvent))
                    )
                )(else
                    (super:handleEvent(pEvent))
                )
            )
        )
        (send pEvent:claimed)
    )


    (method (facingMe)
        return 1
    )


    (method (get param1)
        (var temp0)
        = temp0 0
        (while (< temp0 paramTotal)
            (send ((send gSq5Inv:at(param1[temp0]))):moveTo(self))
            ++temp0
        )
    )


    (method (put param1 param2)
        (var temp0)
        (if ((self:has(param1)))
            = temp0 (send gSq5Inv:at(param1))
            (send temp0:moveTo(
                (if (== paramTotal 1)
                    -1
                )(else
                    param2
                )
))
            (if (gSq5IconBar and (== (send gSq5IconBar:curInvIcon) temp0))
                (send gSq5IconBar:
                    curInvIcon(0)
                    disable((send ((send gSq5IconBar:useIconItem)):
                            cursor(999)
                            yourself()
                        )
)
                )
            )
        )
    )


    (method (has param1)
        (var temp0)
        = temp0 (send gSq5Inv:at(param1))
        (if (temp0)
            (send (= temp0 (send gSq5Inv:at(param1))):ownedBy(self))
        )
    )

)
