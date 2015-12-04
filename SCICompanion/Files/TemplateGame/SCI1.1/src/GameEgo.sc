(version 2)
(include "sci.sh")
(include "Verbs.sh")
(include "0.shm")
(use "Main")
(use "Smopper")
(use "Ego")
(use "Cycle")
(use "System")
(script 18)


/*
	GameEgo is a game-specific subclass of :class:`Ego`. Here, you can put default answers for
	looking, talking and performing actions on yourself.
*/
(class GameEgo of Ego
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


    (method (doVerb theVerb params)
        (switch (theVerb)
            (case V_LOOK
                (send gMessager:say(N_EGO V_LOOK 0 Random(1 2) 0 0))
            )
            (case V_DO
                (send gMessager:say(N_EGO V_DO 0 Random(1 2) 0 0))
            )
            (case V_TALK
                (send gMessager:say(N_EGO V_TALK 0 Random(1 2) 0 0))
            )
            (default 
                (super:doVerb(theVerb rest params))
            )
        )
    )


    (method (setSpeed newSpeed)
        = gGEgoMoveSpeed (super:setSpeed(newSpeed))
    )

)

/*
	FiddleStopWalk is a game-specific subclass of :class:`Smopper` (which allows separate views
	for starting, stopping and stopped animations). It extends Smopper by providing some random
	idle animations. The version of FiddleStopWalk in the SCI1.1 template
	game uses SQ5-specific views (so it won't work with the template game), but is included as an example of what you
	might want to implement in your game.
*/
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

    (method (doit sendParams)
        (var temp0[2])
        (if ((send client:isStopped()))
            (if (<> (send gEgo:loop) 8)
                (if (IsOneOf(vInMotion 0 1))
                    = oldCycSpeed gGEgoMoveSpeed
                    (if (== vInMotion 1)
                        = vSlow 3
                    )(else
                        (if (IsOneOf((send gEgo:loop) 2 4 5))	// The loops that face the screen.
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
                (super:doit(rest sendParams))
            )(else
                (if (((== (send gEgo:loop) 8) and (<> curTicks -1)) and (<= (= curTicks (- curTicks Abs((- gGameTime lastTicks)))) 0))
                    = curTicks -1
                    (super:doit(rest sendParams))
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
										   )
							)
                        )(else
                            (send gEgo:view(10))
                        )
                        (send gEgo:
                            loop(lCel)
                            cel(0)
                            cycleSpeed(15)
                            setCycle(EndLoop self)
                        )
                    )(else
                        (super:doit(rest sendParams))
                    )
                )
            )
        )(else
            (super:doit(rest sendParams))
        )
        = lastTicks gGameTime
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
