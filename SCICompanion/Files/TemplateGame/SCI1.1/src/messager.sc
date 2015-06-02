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
))
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


    (method (say theCaller)
        (var theTheCaller, theTheCaller_2, theTheCaller_3, temp3, temp4[20], temp24)
        = theTheCaller_3 (= curSequence 0)
        = theTheCaller_2 theTheCaller_3
        = theTheCaller theTheCaller_2
        = caller (= oneOnly (= killed 0))
        (if (gSq5IconBar and not oldIconBarState)
            = oldIconBarState (send gSq5IconBar:state)
        )
        = theTheCaller theCaller[0]
        (if (== theTheCaller -1)
            (if ((> paramTotal 1) and IsObject(theCaller[1]))
                = caller theCaller[1]
            )
            (self:sayNext())
        )(else
            (if ((> paramTotal 1) and theCaller[1])
                = theTheCaller_2 theCaller[1]
            )
            (if ((> paramTotal 2) and theCaller[2])
                = theTheCaller_3 theCaller[2]
            )
            (if ((> paramTotal 3) and theCaller[3])
                = oneOnly 1
                = curSequence theCaller[3]
            )(else
                = curSequence 1
            )
            = temp24 4
            (if (((> paramTotal temp24) and theCaller[temp24]) and not IsObject(theCaller[temp24]))
                = lastSequence theCaller[temp24]
                ++temp24
                = oneOnly 0
            )(else
                = lastSequence 0
            )
            (if ((> paramTotal temp24) and theCaller[temp24])
                = caller theCaller[temp24]
            )(else
                = caller 0
            )
            = temp3 
                (if (> paramTotal ++temp24)
                    theCaller[temp24]
                )(else
                    gModNum
                )
            (if (global90 and Message(msgGET temp3 theTheCaller theTheCaller_2 theTheCaller_3 curSequence))
                (self:sayNext(temp3 theTheCaller theTheCaller_2 theTheCaller_3 curSequence))
            )(else
                (Print:
                    addTextF("<Messager>\n\tmsgType set to 0 or\n\t%d: %d, %d, %d, %d not found" temp3 theTheCaller theTheCaller_2 theTheCaller_3 curSequence)
                    init()
                )
                (self:dispose())
            )
        )
    )


    (method (sayFormat param1 param2 theCaller param4)
        (var temp0, temp1, temp2)
        (if (gSq5IconBar and not oldIconBarState)
            = oldIconBarState (send gSq5IconBar:state)
        )
        = temp2 (self:findTalker(param1))
        = temp0 GetTotalLength(param2 theCaller rest param4)
        (if (IsObject(theCaller[- paramTotal 2]))
            = caller theCaller[(- paramTotal 2)]
        )
        = oneOnly 1
        = temp1 Memory(memALLOC_CRIT temp0)
        Format(temp1 param2 theCaller rest param4)
        (send temp2:say(temp1 self))
        Memory(memFREE temp1)
    )


    (method (sayNext param1 param2 param3 param4 param5)
        (var temp0, temp1[200], temp201)
        (if (paramTotal)
            = temp0 Message(msgGET param1 param2 param3 param4 param5 @temp1)
        )(else
            = temp0 Message(msgNEXT @temp1)
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
                        modNum(param1)
                        say(temp201 self)
                    )
                )(else
                    (send temp0:
                        modNum(param1)
                        say(@temp1 self param1 param2 param3 param4 param5)
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
