(version 2)
(include "sci.sh")
(exports
    0 Smopper
)
(use "Main")
(use "PFollow")
(use "Cycle")
(use "Obj")
(script 17)


(local
    local0[8] = (2 6 4 0 3 5 1 7 )
    local8[8] = (3 6 0 4 2 5 1 7 )

)
(class public Smopper of Cycle
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
    )

    (method (init theClient theVStopped theVSlow theVStart theCSpeed theCaller)
        = useSlow (= cycleCnt (= vSlow (= vStart (= vStopped (= caller 0)))))
        = cSpeed (send ((= client theClient)):cycleSpeed)
        = oldSpeed (send ((= client theClient)):cycleSpeed)
        (if (paramTotal)
            = vInMotion (send ((= client theClient)):view)
            (if (>= paramTotal 2)
                = vStopped theVStopped
                (if (>= paramTotal 3)
                    = vSlow theVSlow
                    (if (>= paramTotal 4)
                        (if (== theVStart -1)
                            = useSlow 1
                            = vStart vSlow
                        )(else
                            = vStart theVStart
                        )
                        (if (>= paramTotal 5)
                            (if (<> theCSpeed -1)
                                = cSpeed theCSpeed
                            )
                            (if (>= paramTotal 6)
                                = caller theCaller
                            )
                        )
                    )(else
                        = useSlow 1
                        = vStart vSlow
                    )
                )
            )
        )
        (if ((send client:isStopped()))
            (if (vSlow)
                = stopState 1
            )(else
                = stopState 3
            )
        )(else
            = stopState 7
        )
        (super:init(client))
    )


    (method (doit)
        (var temp0, clientMover, temp2[10])
        (if (((send client:isStopped()) or (send client:isBlocked())) or not (send client:mover))
            (if (== (send client:view) vInMotion)
                (if (vSlow and IsOneOf(stopState 0))
                    = stopState 1
                )(else
                    (if (((vSlow and (== stopState 4)) and (== vStopped -1)) and (<> (send client:loop) (- NumLoops(client) 1)))
                        = stopState 1
                    )(else
                        (if (not vSlow and IsOneOf(stopState 0))
                            = stopState 3
                        )(else
                            (if (not IsOneOf(stopState 2 3 1))
                                = stopState 4
                            )
                        )
                    )
                )
                = clientMover (send client:mover)
                (if ((clientMover and not (send ((= clientMover (send client:mover))):completed)) and not (send clientMover:isKindOf(PFollow)))
                    (send client:setMotion(0))
                )
            )
        )(else
            (if (IsOneOf(stopState 4 2 3 1))
                (if (vStart)
                    = stopState 5
                )(else
                    = stopState 7
                )
            )
        )
        (switch (stopState)
            (case 0
                = cycleDir 1
                = newCel (self:nextCel())
                (if (> newCel (send client:lastCel()))
                    = newCel 0
                )
                (send client:cel(newCel))
            )
            (case 1
                = cycleDir 1
                (if (not vSlow)
                    (if (<> vStopped -1)
                        (send client:view(vStopped))
                    )
                    = stopState 3
                )(else
                    = stopState 2
                    (if (== (send client:view) vInMotion)
                        = newCel 0
                        (send client:cel(0))
                    )
                    (send client:
                        cycleSpeed(cSpeed)
                        view(vSlow)
                    )
                )
            )
            (case 2
                (send client:cycleSpeed(cSpeed))
                (send client:
                    cel(newCel)
                    forceUpd()
                )
                = newCel (self:nextCel())
                (if (> newCel (send client:lastCel()))
                    = newCel 0
                    = stopState 3
                )
            )
            (case 3
                (send client:cycleSpeed(cSpeed))
                = stopState 4
                = cycleDir 1
                (if (== vStopped -1)
                    (send client:
                        view(vInMotion)
                        cel((send client:loop))
                    )
                    (send client:loop((- NumLoops(client) 1)))
                )(else
                    (send client:
                        view(vStopped)
                        cel(0)
                    )
                )
                (if (caller)
                    (send caller:cue(0))
                )
            )
            (case 4
                (if (<> vStopped -1)
                    (send client:cycleSpeed(cSpeed))
                    (if ((send client:lastCel()))
                        = newCel (self:nextCel())
                        (if (> newCel (send client:lastCel()))
                            = newCel 0
                        )
                        (send client:cel(newCel))
                    )(else
                        0
                    )
                )
            )
            (case 5
                (if (caller)
                    (send caller:cue(1))
                )
                (if (not vStart)
                    (send client:view(vInMotion))
                    = stopState 7
                )(else
                    = stopState 6
                    (if (== vStopped -1)
                        (send client:loop((send client:cel)))
                    )
                    (if (useSlow)
                        = cycleDir -1
                        (send client:
                            cel((send client:lastCel()))
                            view(vSlow)
                        )
                    )(else
                        = cycleDir 1
                        (send client:
                            cel(0)
                            view(vStart)
                        )
                    )
                    (send client:cycleSpeed(cSpeed))
                )
            )
            (case 6
                (send client:cycleSpeed(cSpeed))
                (if (useSlow)
                    (if (not newCel)
                        = stopState 7
                    )(else
                        (send client:cel(newCel))
                    )
                    = newCel (self:nextCel())
                )(else
                    = newCel (self:nextCel())
                    (if (> newCel (send client:lastCel()))
                        = stopState 7
                    )(else
                        (send client:cel(newCel))
                    )
                )
            )
            (case 7
                = stopState 0
                = cycleDir 1
                (if ((send client:mover))
                    = cycleCnt (send ((send client:mover)):{b-moveCnt})
                )
                (if (== vStopped -1)
                    (send client:setLoop(-1))
                    (if (== (send client:loop) (- NumLoops(client) 1))
                        (send client:
                            view(vInMotion)
                            loop(local8[/ (send client:heading) 45])
                        )
                    )
                )
                = oldSpeed gGEgoMoveSpeed
                (send client:
                    view(vInMotion)
                    cycleSpeed(oldSpeed)
                    cel(0)
                )
            )
        )
    )


    (method (dispose param1)
        (send client:cycleSpeed(oldSpeed))
        (if (IsObject(client))
            (send client:cycler(0))
        )
        (self:client(0))
        (super:dispose(rest param1))
    )

)
