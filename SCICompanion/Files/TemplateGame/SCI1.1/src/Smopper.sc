/*
	Contains the Smopper cycler, which is used if you need to specify stopped, slowing and starting views in addition to in-motion views.
*/
(version 2)
(include "sci.sh")
(exports
    0 Smopper
)
(use "Main")
(use "PFollow")
(use "Cycle")
(use "System")
(script 17)

(local
    local0[8] = (2 6 4 0 3 5 1 7 )
    local8[8] = (3 6 0 4 2 5 1 7 )
)

(define SMOP_INMOTION 			0)
(define SMOP_SLOW_PENDING 		1)
(define SMOP_SLOW		 		2)
(define SMOP_STOPPED_PENDING 	3)
(define SMOP_STOPPED			4)
(define SMOP_STARTING_PENDING 	5)
(define SMOP_STARTING			6)
(define SMOP_INMOTION_PENDING 	7)

/*
	Smopper is a complex cycler similar to :class:`StopWalk`, but allowing the use
	of separate starting and stopping views. This can be used to allow for more realistic
	movement when starting and stopping.
	
	Example usage::
	
		// Cycle the boy with view 805 for stopped, 814 for slowing and 815 for starting. Cycle speed is 12.
		(boy:setCycle(Smopper 805 814 815 12))
*/
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

	/*
	.. function:: init(theClient [theVStopped theVSlow theVStart theCSpeed theCaller])
	
		Initializes the Smopper.
		
		:param heapPtr theClient: The :class:`Actor` to which this applies.
		:param number theVStopped: The view number for the stopped state.
		:param number theVSlow: The view number for the slowing down state.
		:param number theVStart: The view number for the starting state.
		:param number theCSpeed: The cycle speed used for starting, stopped and stopped.
		:param heapPtr theCaller: Optional object that gets cue(0)'d when stopped, and cue(1)'d when starting motion again.
	*/
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
                = stopState SMOP_SLOW_PENDING
            )(else
                = stopState SMOP_STOPPED_PENDING
            )
        )(else
            = stopState SMOP_INMOTION_PENDING
        )
        (super:init(client))
    )


    (method (doit)
        (var temp0, clientMover, temp2[10])
        (if (((send client:isStopped()) or (send client:isBlocked())) or not (send client:mover))
            (if (== (send client:view) vInMotion)
                (if (vSlow and IsOneOf(stopState SMOP_INMOTION))
                    = stopState SMOP_SLOW_PENDING
                )(else
                    (if (((vSlow and (== stopState SMOP_STOPPED)) and (== vStopped -1)) and (<> (send client:loop) (- NumLoops(client) 1)))
                        = stopState SMOP_SLOW_PENDING
                    )(else
                        (if (not vSlow and IsOneOf(stopState SMOP_INMOTION))
                            = stopState SMOP_STOPPED_PENDING
                        )(else
                            (if (not IsOneOf(stopState SMOP_SLOW SMOP_STOPPED_PENDING SMOP_SLOW_PENDING))
                                = stopState SMOP_STOPPED
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
            (if (IsOneOf(stopState SMOP_STOPPED SMOP_SLOW SMOP_STOPPED_PENDING SMOP_SLOW_PENDING))
                (if (vStart)
                    = stopState SMOP_STARTING_PENDING
                )(else
                    = stopState SMOP_INMOTION_PENDING
                )
            )
        )
        (switch (stopState)
            (case SMOP_INMOTION
            	// Just keep cycling forward
                = cycleDir 1
                = newCel (self:nextCel())
                (if (> newCel (send client:lastCel()))
                    = newCel 0
                )
                (send client:cel(newCel))
            )
            (case SMOP_SLOW_PENDING
            	// If there's no slowing view set state to 3.
            	// If there is a slowing view, set state to 2 and set the slowing view (and cSpeed)
                = cycleDir 1
                (if (not vSlow)
                    (if (<> vStopped -1)
                        (send client:view(vStopped))
                    )
                    = stopState SMOP_STOPPED_PENDING
                )(else
                    = stopState SMOP_SLOW
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
            (case SMOP_SLOW
            	// If we reach the last cel, set state to 3
                (send client:cycleSpeed(cSpeed))
                (send client:
                    cel(newCel)
                    forceUpd()
                )
                = newCel (self:nextCel())
                (if (> newCel (send client:lastCel()))
                    = newCel 0
                    = stopState SMOP_STOPPED_PENDING
                )
            )
            (case SMOP_STOPPED_PENDING
                (send client:cycleSpeed(cSpeed))
                = stopState SMOP_STOPPED
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
            (case SMOP_STOPPED
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
            (case SMOP_STARTING_PENDING
                (if (caller)
                    (send caller:cue(1))
                )
                (if (not vStart)
                    (send client:view(vInMotion))
                    = stopState SMOP_INMOTION_PENDING
                )(else
                    = stopState SMOP_STARTING
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
            (case SMOP_STARTING
                (send client:cycleSpeed(cSpeed))
                (if (useSlow)
                    (if (not newCel)
                        = stopState SMOP_INMOTION_PENDING
                    )(else
                        (send client:cel(newCel))
                    )
                    = newCel (self:nextCel())
                )(else
                    = newCel (self:nextCel())
                    (if (> newCel (send client:lastCel()))
                        = stopState SMOP_INMOTION_PENDING
                    )(else
                        (send client:cel(newCel))
                    )
                )
            )
            (case 7
                = stopState SMOP_INMOTION
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
