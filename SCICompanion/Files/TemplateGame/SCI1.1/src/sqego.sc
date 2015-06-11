(version 2)
(include "sci.sh")
(include "Verbs.sh")
(include "0.shm")
(use "Main")
(use "Smopper")
(use "Ego")
(use "Cycle")
(use "Obj")
(script 18)



(class SQEgo of Ego
    (properties
        x 0
        y 0
        z 0
        heading 0
        noun N_EGO
        modNum 0
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        sightAngle $6789
        actions 0
        onMeCheck omcDISABLE
        state $0002
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

    (method (handleEvent pEvent param2)
        (var temp0, temp1, temp2)
        = temp1 (send pEvent:type)
        = temp2 (send pEvent:message)
        return 
            (if (script and (send script:handleEvent(pEvent)))
                1
            )(else
                (if (& temp1 $0040)
                    return 0
                )(else
                    (super:handleEvent(pEvent rest param2))
                )
            )
    )


    (method (doVerb theVerb param2)
        (switch (theVerb)
            (case V_LOOK
                (if (Btest(22))
                    (send gTestMessager:say(30 1 0 Random(1 2) 0 0))
                )(else
                    (send gTestMessager:say(22 1 0 Random(1 2) 0 0))
                )
            )
            (case V_DO
                (send gTestMessager:say(22 4 0 Random(1 2) 0 0))
            )
            (case V_TALK
                (send gTestMessager:say(22 2 0 Random(1 2) 0 0))
            )
            (case 32
                (if ((send gRoom:script))
                    (super:doVerb(theVerb rest param2))
                )(else
                    (if (IsOneOf(gModNum 730 740 760 790 620 640 400 410 420))
                        (send gRoom:setScript(ScriptID(gModNum 20)))
                    )(else
                        (super:doVerb(theVerb rest param2))
                    )
                )
            )
            (default 
                (super:doVerb(theVerb rest param2))
            )
        )
    )


    (method (setSpeed newSpeed)
        = gGEgoMoveSpeed (super:setSpeed(newSpeed))
    )

)

// Complex ego view cyclers
(class FiddleStopWalk of Smopper
    (properties
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
        vInMotion 0
        vStopped 0
        vSlow 0
        vStart 0
        stopState 0
        useSlow 0
        cSpeed 0
        oldSpeed 0
        newCel 0
        tempMover 0
        lCel 0
        ticks 1200
        curTicks -1
        lastTicks 0
        oldControl 0
        oldCycSpeed 0
    )

    (method (doit param1)
        (var temp0[2])
        (if ((send client:isStopped()))
            (if (<> (send gEgo:loop) 8)
                (if (IsOneOf(vInMotion 0 1))
                    = oldCycSpeed gGEgoMoveSpeed
                    (if (== vInMotion 1)
                        = vSlow 3
                    )(else
                        (if (IsOneOf((send gEgo:loop) 2 4 5))
                            = vSlow 
                                (if (Random(0 1))
                                    12
                                )(else
                                    2
                                )
                        )(else
                            = vSlow 12
                        )
                    )
                    = curTicks ticks
                )(else
                    = vSlow 0
                )
                (super:doit(rest param1))
            )(else
                (if (((== (send gEgo:loop) 8) and (<> curTicks -1)) and (<= (= curTicks (- curTicks Abs((- gLastTicks lastTicks)))) 0))
                    = curTicks -1
                    (super:doit(rest param1))
                )(else
                    (if ((((((== curTicks -1) and not (send gRoom:script)) and not (send gEgo:script)) and (send gUser:canControl())) and (== (send gEgo:view) 0)) and (== (send gEgo:loop) (- NumLoops(gEgo) 1)))
                        = curTicks ticks
                        = lCel (send gEgo:cel)
                        = oldCycSpeed (send gEgo:cycleSpeed)
                        (if (IsOneOf((send gEgo:cel) 2 4 5))
                            (send gEgo:view(
                                (if (Random(0 1))
                                    10
                                )(else
                                    2
                                )
))
                        )(else
                            (send gEgo:view(10))
                        )
                        (send gEgo:
                            loop(lCel)
                            cel(0)
                            cycleSpeed(15)
                            setCycle(End self)
                        )
                    )(else
                        (super:doit(rest param1))
                    )
                )
            )
        )(else
            (super:doit(rest param1))
        )
        = lastTicks gLastTicks
    )


    (method (cue)
        (send gEgo:
            view(0)
            cel(lCel)
            cycleSpeed(oldCycSpeed)
            cycler(self)
        )
        (send gEgo:loop((- NumLoops(gEgo) 1)))
        (self:client(gEgo))
    )

)
