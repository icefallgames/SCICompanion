(version 2)
(include "sci.sh")
(use "Main")
(use "PolyPath")
(use "Polygon")
(use "Sound")
(use "Cycle")
(use "View")
(script 954)



(class Door of Prop
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
        entranceTo 0
        locked 0
        lockedCase 0
        openSnd 0
        closeSnd 0
        openVerb 0
        listenVerb 0
        doubleDoor 0
        forceOpen 0
        forceClose 1
        caller 0
        moveToX 0
        moveToY 0
        enterType 2
        exitType 2
        closeScript 0
        openScript 0
        doorPoly 0
        polyAdjust 5
    )

    (method (init)
        (self:approachVerbs(openVerb listenVerb))
        (if (forceOpen or (gPreviousRoomNumber and (== gPreviousRoomNumber entranceTo)))
            = state 2
        )
        (super:init())
        (self:createPoly())
        (self:ignoreActors(1))
        (if (== state 0)
            = cel 0
            (if (doubleDoor)
                (send doubleDoor:setCel(0))
            )
        )(else
            (send gAltPolyList:delete(doorPoly))
            = cel (- NumCels(self) 1)
            (if (doubleDoor)
                (send doubleDoor:setCel((- NumCels(doubleDoor) 1)))
            )
        )
        (if (== state 2)
            (if (closeScript)
                (self:setScript(closeScript))
            )(else
                (switch (enterType)
                    (case 0
                        (send gEgo:
                            posn(moveToX moveToY)
                            setMotion(PolyPath approachX approachY self)
                        )
                    )
                    (case 1
                        (send gEgo:
                            edgeHit(0)
                            posn(approachX approachY)
                            setHeading(heading)
                        )
                        (if (forceClose)
                            (self:close())
                        )
                    )
                    (case 2
                        (if (forceClose)
                            (self:close())
                        )
                    )
                )
            )
        )(else
            (self:stopUpd())
        )
    )


    (method (dispose)
        (send gAltPolyList:delete(doorPoly))
        (send doorPoly:dispose())
        (super:dispose())
    )


    (method (doVerb theVerb)
        (switch (theVerb)
            (case openVerb
                (if (== state 2)
                    (self:close())
                )(else
                    (self:open())
                )
            )
            (case listenVerb
                (self:listen())
            )
            (default 
                (super:doVerb(theVerb))
            )
        )
    )


    (method (cue)
        (switch (state)
            (case 3
                = state 0
                (self:stopUpd())
                (send gAltPolyList:add(doorPoly))
                (if (caller)
                    (send caller:cue())
                )
                (if (not (send gUser:controls))
                    (send gGame:handsOn(1))
                )
            )
            (case 1
                = state 2
                (self:stopUpd())
                (send gAltPolyList:delete(doorPoly))
                (if (caller)
                    (send caller:cue())
                )
                (if (openScript)
                    (self:setScript(openScript))
                )(else
                    (switch (exitType)
                        (case 0
                            (if (moveToX or moveToY)
                                (send gEgo:
                                    illegalBits(0)
                                    setMotion(PolyPath moveToX moveToY self)
                                )
                            )
                        )
                        (case 1
                            (if (moveToX or moveToY)
                                (send gEgo:setMotion(PolyPath moveToX moveToY self))
                            )
                        )
                        (case 2
                            (if (not (send gUser:controls))
                                (send gGame:handsOn(1))
                            )
                        )
                    )
                )
            )
            (default 
                (if ((== (send gEgo:x) moveToX) and (== (send gEgo:y) moveToY))
                    (if (entranceTo)
                        (switch (entranceTo)
                            (case (send gRoom:north)
                                (send gEgo:edgeHit(1))
                            )
                            (case (send gRoom:south)
                                (send gEgo:edgeHit(3))
                            )
                            (case (send gRoom:west)
                                (send gEgo:edgeHit(4))
                            )
                            (case (send gRoom:east)
                                (send gEgo:edgeHit(2))
                            )
                        )
                        (send gRoom:newRoom(entranceTo))
                    )(else
                        (if (forceClose)
                            (self:close())
                        )(else
                            (if (caller)
                                (send caller:cue())
                            )
                        )
                    )
                )(else
                    (if ((== (send gEgo:x) approachX) and (== (send gEgo:y) approachY))
                        (if (forceClose)
                            (self:close())
                        )(else
                            (if (caller)
                                (send caller:cue())
                            )
                        )
                    )
                )
            )
        )
    )


    (method (open)
        (if (locked)
            (if (== modNum -1)
                = modNum gModNum
            )
            (send gTestMessager:say(noun 0 lockedCase 0 0 modNum))
        )(else
            (if ((send gUser:controls))
                (send gGame:handsOff())
            )
            = state 1
            (self:setCycle(End self))
            (if (openSnd)
                (doorSound:
                    number(openSnd)
                    play()
                )
            )
            (if (doubleDoor)
                (send doubleDoor:setCycle(End))
            )
        )
    )


    (method (close)
        = state 3
        (self:setCycle(Beg self))
        (if (closeSnd)
            (doorSound:
                number(closeSnd)
                play()
            )
        )
        (if (doubleDoor)
            (send doubleDoor:setCycle(Beg))
        )
    )


    (method (listen)
    )


    (method (createPoly param1)
        = doorPoly (send ((Polygon:new())):
                type(PBarredAccess)
                yourself()
            )
        (if (paramTotal)
            (send doorPoly:init(rest param1))
        )(else
            (send doorPoly:init((- brLeft polyAdjust) (+ brBottom polyAdjust) (- brLeft polyAdjust) (- brTop polyAdjust) (+ brRight polyAdjust) (- brTop polyAdjust) (+ brRight polyAdjust) (+ brBottom polyAdjust)))
        )
        (send gAltPolyList:add(doorPoly))
    )

)
(instance doorSound of Sound
    (properties
        flags $0001
    )
)
