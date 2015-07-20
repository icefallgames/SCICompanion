(version 2)
(include "sci.sh")
(use "Main")
(use "Print")
(use "Game")
(use "Obj")
(script 924)



(class Messager of Obj
    (properties
        caller 0
        disposeWhenDone 1
        oneOnly 0
        killed 0
        oldIconBarState 0
        curSequence 0
        lastSequence 0
    )

    (method (dispose)
        (talkerSet:dispose())
        (if (gSq5IconBar)
            (send gSq5IconBar:state(oldIconBarState))
            = oldIconBarState 0
        )
        (if (caller)
            (if (not gNewSet)
                = gNewSet (Set:new())
            )
            (send gNewSet:add((send ((Cue:new())):
                    cuee(caller)
                    cuer(self)
                    register(killed)
                    yourself()
                )
							 )
			)
        )
        (super:dispose())
    )


    (method (cue param1)
        (if (paramTotal and param1)
            = killed 1
        )
        (if (oneOnly or killed)
            (if (gNewEventHandler)
                (send gNewEventHandler:
                    release()
                    dispose()
                )
                = gNewEventHandler 0
            )
            (self:dispose())
        )(else
            (self:sayNext())
        )
    )

	// say(noun [verb] [cond] [seq] [lastSeq] [caller] [modNum])
	// say(noun [verb] [cond] [seq] [caller] [modNum])
	// say(-1 caller)
    (method (say params)
        (var noun, verb, cond, modNum, temp4[20], temp24)
        = cond (= curSequence 0)
        = verb cond
        = noun verb
        = caller (= oneOnly (= killed 0))
        (if (gSq5IconBar and not oldIconBarState)
            = oldIconBarState (send gSq5IconBar:state)
        )
        = noun params[0]
        (if (== noun -1)
            (if ((> paramTotal 1) and IsObject(params[1]))
                = caller params[1]
            )
            (self:sayNext())
        )(else
            (if ((> paramTotal 1) and params[1])
                = verb params[1]
            )
            (if ((> paramTotal 2) and params[2])
                = cond params[2]
            )
            (if ((> paramTotal 3) and params[3])
                = oneOnly 1
                = curSequence params[3]
            )(else
                = curSequence 1
            )
            = temp24 4
            (if (((> paramTotal temp24) and params[temp24]) and not IsObject(params[temp24]))
                = lastSequence params[temp24]
                ++temp24
                = oneOnly 0
            )(else
                = lastSequence 0
            )
            (if ((> paramTotal temp24) and params[temp24])
                = caller params[temp24]
            )(else
                = caller 0
            )
            = modNum 
                (if (> paramTotal ++temp24)
                    params[temp24]
                )(else
                    gModNum
                )
            (if (global90 and Message(msgGET modNum noun verb cond curSequence))
                (self:sayNext(modNum noun verb cond curSequence))
            )(else
                (Print:
                    addTextF("<Messager>\n\tmsgType set to 0 or\n\t%d: %d, %d, %d, %d not found" modNum noun verb cond curSequence)
                    init()
                )
                (self:dispose())
            )
        )
    )


    (method (sayFormat talkerNumber formatString theCaller param4)
        (var temp0, buffer, theTalker)
        (if (gSq5IconBar and not oldIconBarState)
            = oldIconBarState (send gSq5IconBar:state)
        )
        = theTalker (self:findTalker(talkerNumber))
        = temp0 GetTotalLength(formatString theCaller rest param4)
        (if (IsObject(theCaller[- paramTotal 2]))
            = caller theCaller[(- paramTotal 2)]
        )
        = oneOnly 1
        = buffer Memory(memALLOC_CRIT temp0)
        Format(buffer formatString theCaller rest param4)
        (send theTalker:say(buffer self))
        Memory(memFREE buffer)
    )


    (method (sayNext theModNum noun verb cond seq)
        (var temp0, buffer[200], temp201)
        (if (paramTotal)
            = temp0 Message(msgGET theModNum noun verb cond seq @buffer)
        )(else
            = temp0 Message(msgNEXT @buffer)
        )
        (if (& global90 $0002)
            = temp201 Memory(memALLOC_CRIT 12)
            Message(msgLAST_MESSAGE temp201)
        )
        (if (temp0 and (not lastSequence or (lastSequence and (<= curSequence lastSequence))))
            = temp0 (self:findTalker(temp0))
            (if (<> temp0 -1)
                (talkerSet:add(temp0))
                (if (& global90 $0002)
                    (send temp0:
                        modNum(theModNum)
                        say(temp201 self)
                    )
                )(else
                    (send temp0:
                        modNum(theModNum)
                        say(@buffer self theModNum noun verb cond seq)
                    )
                )
                ++curSequence
            )(else
                (if (gNewEventHandler)
                    (send gNewEventHandler:
                        release()
                        dispose()
                    )
                    = gNewEventHandler 0
                )
                (self:dispose())
            )
        )(else
            (if (gNewEventHandler)
                (send gNewEventHandler:
                    release()
                    dispose()
                )
                = gNewEventHandler 0
            )
            (self:dispose())
        )
        (if (& global90 $0002)
            Memory(memFREE temp201)
        )
    )


    (method (findTalker)
        (Print:
            width(200)
            addText("<Messager findTalker:>\n\tCan't find talker or\n\tfindTalker method not over-ridden")
            init()
        )
        return gSQ5Narrator
    )

)
(instance talkerSet of Set
    (properties)

    (method (dispose)
        (self:
            eachElementDo(#caller 0)
            eachElementDo(#dispose (send gTestMessager:disposeWhenDone))
            release()
        )
        (super:dispose())
    )

)
