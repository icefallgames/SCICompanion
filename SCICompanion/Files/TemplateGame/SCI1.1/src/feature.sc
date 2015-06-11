(version 2)
(include "sci.sh")
(use "Main")
(use "PolyPath")
(use "Obj")
(script 950)



(class CueObj of Script
    (properties
        client 0
        state $ffff
        start 0
        timer 0
        cycles 0
        seconds 0
        lastSeconds 0
        ticks 0
        lastTicks 0
        register 0
        script 0
        caller 0
        next 0
        theVerb 0
    )

    (method (changeState newState)
        (switch (= state newState)
            (case 1
                (send gEgo:setHeading(GetAngle((send gEgo:x) (send gEgo:y) (send client:x) (send client:y)) self))
                (send gTheDoits:add(self))
            )
            (case 2
                = cycles 3
            )
            (case 3
                (send gTheDoits:delete(self))
                (if (not 
                (if (IsObject(client) and IsObject((send client:actions)))
                    (send ((send client:actions)):doVerb(theVerb))
                )
)
                    (send client:doVerb(theVerb))
                )
                = state 0
            )
        )
    )

)
(class Feature of Obj
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
    )

    (method (init param1)
        (self:initialize(
            (if (paramTotal)
                param1
            )(else
                0
            )
						)
		)
        (if ((self:respondsTo(#underBits)))
            (send gOldCast:add(self))
        )(else
            (send gOldFeatures:add(self))
        )
    )


    (method (dispose)
        (if (actions)
            (send actions:dispose())
            = actions 0
        )
        (if (IsObject(onMeCheck) and not (& state $0004))
            (send onMeCheck:dispose())
            = onMeCheck 0
        )
        (send gOldFeatures:delete(self))
        (if (& state $0001)
            Memory(memFREE name)
            = name 0
        )
        (super:dispose())
    )


    (method (initialize param1)
        (if (paramTotal and param1)
            (self:perform(param1))
        )(else
            (if (gFeatureInit)
                (self:perform(gFeatureInit))
            )
        )
    )


    (method (handleEvent pEvent)
        (if ((send pEvent:claimed))
            return 1
        )(else
            (if (((& (send pEvent:type) evVERB) and (self:onMe(pEvent))) and (self:isNotHidden()))
                (CueObj:
                    state(0)
                    cycles(0)
                    client(self)
                    theVerb((send pEvent:message))
                )
                (send pEvent:claimed(1))
                (if (((((send gUser:canControl()) and (& (send gEgo:state) $0002)) and (> GetDistance((send gEgo:x) (send gEgo:y) approachX approachY) approachDist)) and gApproachCode) and (& _approachVerbs (send gApproachCode:doit((send pEvent:message)))))
                    (send gEgo:setMotion(PolyPath approachX (+ (send gEgo:z) approachY) CueObj))
                )(else
                    (send gEgo:setMotion(0))
                    (if ((self:facingMe()))
                        (CueObj:changeState(3))
                    )
                )
            )
        )
        return (send pEvent:claimed)
    )


    (method (doVerb theVerb)
        (var temp0, temp1)
        = temp0 
            (if (gDoVerbCode)
            )(else
                dftDoVerb
            )
        (if (== modNum -1)
            = modNum gModNum
        )
        (if (global90 and Message(msgGET modNum noun theVerb 0 1))
            (send gTestMessager:say(noun theVerb 0 0 0 modNum))
        )(else
            (send temp0:doit(theVerb self))
        )
    )


    (method (notFacing)
        (var temp0)
        (send gEgo:setMotion(0))
        (CueObj:
            client(self)
            state(0)
            cycles(0)
            cue()
        )
    )


    (method (facingMe theTheGEgo)
        (var theGEgo, temp1)
        (if (paramTotal)
            = theGEgo theTheGEgo
        )(else
            (if ((send gOldCast:contains(gEgo)))
                = theGEgo gEgo
            )(else
                return 1
            )
        )
        = temp1 Abs((- GetAngle((send theGEgo:x) (send theGEgo:y) x y) (send theGEgo:heading)))
        (if (> temp1 180)
            = temp1 (- 360 temp1)
        )
        return 
            (if (<= temp1 sightAngle)
                return 1
            )(else
                (if (<> sightAngle $6789)
                    (self:notFacing())
                )
                return 0
            )
    )


    (method (isNotHidden)
        return 1
    )


    (method (onMe param1 param2)
        (var temp0, temp1)
        (if (IsObject(param1))
            = temp0 (send param1:x)
            = temp1 (send param1:y)
        )(else
            = temp0 param1
            = temp1 param2
        )
        return 
            (if (& state $0004)
                (if (not 
                (if ((nsLeft or nsRight) or nsTop)
                )(else
                    nsBottom
                )
 or ((((<= nsLeft temp0) and (<= temp0 nsRight)) and (<= nsTop temp1)) and (<= temp1 nsBottom)))
                    & onMeCheck OnControl(4 temp0 temp1)
                )(else
                    0
                )
            )(else
                (if (IsObject(onMeCheck))
                    AvoidPath(temp0 temp1 onMeCheck)
                )(else
                    (if (not 
                    (if ((nsLeft or nsRight) or nsTop)
                    )(else
                        nsBottom
                    )
 or ((((<= nsLeft temp0) and (<= temp0 nsRight)) and (<= nsTop temp1)) and (<= temp1 nsBottom)))
                        1
                    )(else
                        0
                    )
                )
            )
    )


    (method (approachVerbs param1)
        (var temp0, temp1)
        = _approachVerbs 0
        (if ((paramTotal and gApproachCode) and param1[0])
            = temp0 0
            (while (< temp0 paramTotal)
                = temp1 (send gApproachCode:doit(param1[temp0]))
                (self:_approachVerbs((| (self:_approachVerbs) temp1)))
                ++temp0
            )
        )
    )


    (method (setName param1)
        = state (| state $0001)
        = name Memory(memALLOC_CRIT (+ StrLen(param1) 1))
        StrCpy(name param1)
    )


    (method (setOnMeCheck checkStyle params)
        (var temp0)
        (switch (checkStyle)
            (case omcDISABLE
                = onMeCheck checkStyle
                = state (& state $fffb)
            )
            (case omcPOLYGON
                = onMeCheck params[0]
                = state (& state $fffb)
            )
            (case omcCOLORS
                = temp0 (= onMeCheck 0)
                (while (< temp0 (- paramTotal 1))
                    = onMeCheck (| onMeCheck params[temp0])
                    ++temp0
                )
                = state (| state $0004)
            )
        )
    )

)
(instance dftDoVerb of Code
    (properties)

    (method (doit)
        return 1
    )

)
(class Actions of Code
    (properties)

    (method (doVerb)
        return 0
    )

)
