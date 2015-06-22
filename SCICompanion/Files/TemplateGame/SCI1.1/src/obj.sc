(version 2)
(include "sci.sh")
(exports
    0 Sign
    1 Modulo
    2 Min
    3 Max
    4 DoesRectContain
    5 IsOneOf
    6 GetValueAt
    7 Perform
)
(use "Main")
(use "Print")
(script 999)



(procedure public (Sign param1)
    return 
        (if (< param1 0)
            -1
        )(else
            > param1 0
        )
)


(procedure public (Modulo param1 param2)
    = param1 (- param1 (* param2 (/ param1 param2)))
    (if (< param1 0)
        = param1 (+ param1 param2)
    )
    return param1
)


(procedure public (Min param1 param2)
    (var temp0)
    return 
        (if ((== paramTotal 1) or (< param1 (= temp0 Min(rest param2))))
            param1
        )(else
            temp0
        )
)


(procedure public (Max param1 param2)
    (var temp0)
    return 
        (if ((== paramTotal 1) or (> param1 (= temp0 Max(rest param2))))
            param1
        )(else
            temp0
        )
)

// DoesRectContain(left top right bottom x y)
// DoesRectContain(left top right bottom obj)
(procedure public (DoesRectContain left top right bottom param5 param6)
    return 
        (if ((<= left 
        (if (< paramTotal 6)
            (send param5:x)
        )(else
            param5
        )
) and (<= 
        (if (< paramTotal 6)
            (send param5:x)
        )(else
            param5
        )
 right))
            (if (<= top 
            (if (< paramTotal 6)
                (send param5:y)
            )(else
                param6
            )
)
                <= 
                (if (< paramTotal 6)
                    (send param5:y)
                )(else
                    param6
                )
 bottom
            )
        )(else
            0
        )
)


(procedure public (IsOneOf param1 param2)
    (var temp0)
    = temp0 0
    (while (< temp0 (- paramTotal 1))
        (if (== param1 param2[temp0])
            return 
                (if (param1)
                )(else
                    1
                )
        )
        ++temp0
    )
    return 0
)


(procedure public (GetValueAt param1 param2)
    return Memory(memPEEK (+ param1 (* 2 param2)))
)


(procedure public (Perform param1 param2 param3)
    (send param1:param2(rest param3))
)


(class Obj
    (properties)

    (method (new)
        Clone(self)
    )


    (method (init)
    )


    (method (doit)
        return self
    )


    (method (dispose)
        DisposeClone(self)
    )


    (method (showStr param1)
        StrCpy(param1 name)
    )


    (method (showSelf)
        (var temp0[200])
        TextPrint((self:showStr(@temp0)))
    )


    (method (perform param1 param2)
        (send param1:doit(self rest param2))
    )


    (method (isKindOf param1)
        (var obj_super_)
        (if ( (== {-propDict-} (send param1:{-propDict-})) and  (== {-classScript-} (send param1:{-classScript-})))
        	return TRUE
		)
		(= obj_super_ (self:{-super-}))
		(if (not obj_super_)
			return FALSE
		)
		(if (IsObject(obj_super_))
			return (send obj_super_:isKindOf(param1))
		)
		return FALSE
    )

    (method (isMemberOf param1)
    	(if (== param1 self)
    		return TRUE
		)
		(if ( (& (send param1:{-info-}) $8000) and (not (& {-info-} $8000)))
			return (== {-propDict-} (send param1:{-propDict-}))
		)
		return FALSE
    )

    (method (respondsTo param1)
        RespondsTo(self param1)
    )


    (method (yourself)
        return self
    )

)
(class Code of Obj
    (properties)

    (method (doit)
    )

)
(class Collect of Obj
    (properties
        elements 0
        size 0
    )

    (method (doit param1)
        (self:eachElementDo(#doit rest param1))
    )


    (method (dispose)
        (if (elements)
            (self:eachElementDo(#dispose))
            DisposeList(elements)
        )
        = size (= elements 0)
        (super:dispose())
    )


    (method (showStr param1)
        Format(param1 999 0 name size)
    )


    (method (showSelf)
        (var temp0[40])
        TextPrint((self:showStr(@temp0)))
        (self:eachElementDo(#showSelf))
    )


    (method (add param1)
        (var temp0, temp1, temp2)
        (if (not elements)
            = elements NewList()
        )
        = temp1 0
        (while (< temp1 paramTotal)
            (if (not (self:isDuplicate(param1[temp1])))
                AddToEnd(elements NewNode(param1[temp1] param1[temp1]))
                ++size
            )
            ++temp1
        )
        return self
    )


    (method (delete param1)
        (var temp0)
        = temp0 0
        (while (< temp0 paramTotal)
            (if (DeleteKey(elements param1[temp0]))
                --size
            )
            ++temp0
        )
        return self
    )


    (method (eachElementDo param1 param2)
        (var temp0, temp1, temp2)
        = temp0 FirstNode(elements)
        (while (temp0)
            = temp1 NextNode(temp0)
            = temp2 NodeValue(temp0)
            (if (not IsObject(temp2))
                return 
            )
            (send temp2:param1(rest param2))
            = temp0 temp1
        )
    )


    (method (firstTrue param1 param2)
        (var temp0, temp1, temp2)
        = temp0 FirstNode(elements)
        (while (temp0)
            = temp1 NextNode(temp0)
            = temp2 NodeValue(temp0)
            (if ((send temp2:param1(rest param2)))
                return temp2
            )
            = temp0 temp1
        )
        return 0
    )


    (method (allTrue param1 param2)
        (var temp0, temp1, temp2)
        = temp0 FirstNode(elements)
        (while (temp0)
            = temp1 NextNode(temp0)
            = temp2 NodeValue(temp0)
            (if (not (send temp2:param1(rest param2)))
                return 0
            )
            = temp0 temp1
        )
        return 1
    )


    (method (contains param1)
        FindKey(elements param1)
    )


    (method (isEmpty)
        (if (== elements 0)
        )(else
            EmptyList(elements)
        )
    )


    (method (first)
        FirstNode(elements)
    )


    (method (next param1)
        NextNode(param1)
    )


    (method (release)
        (var temp0, temp1)
        = temp0 FirstNode(elements)
        (while (temp0)
            = temp1 NextNode(temp0)
            (self:delete(NodeValue(temp0)))
            = temp0 temp1
        )
    )


    (method (isDuplicate)
        return 0
    )

)
(class List of Collect
    (properties
        elements 0
        size 0
    )

    (method (showStr param1)
        Format(param1 999 1 name size)
    )


    (method (at param1)
        (var temp0)
        = temp0 FirstNode(elements)
        (while (param1 and temp0)
            --param1
            = temp0 NextNode(temp0)
        )
        return 
            (if (temp0)
                NodeValue(temp0)
            )(else
                0
            )
    )


    (method (last)
        LastNode(elements)
    )


    (method (prev param1)
        PrevNode(param1)
    )


    (method (addToFront param1)
        (var temp0)
        (if (not elements)
            = elements NewList()
        )
        = temp0 (- paramTotal 1)
        (while (<= 0 temp0)
            (if (not (self:isDuplicate(param1[temp0])))
                AddToFront(elements NewNode(param1[temp0] param1[temp0]))
                ++size
            )
            --temp0
        )
        return self
    )


    (method (addToEnd param1)
        (var temp0)
        (if (not elements)
            = elements NewList()
        )
        = temp0 0
        (while (< temp0 paramTotal)
            (if (not (self:isDuplicate(param1[temp0])))
                AddToEnd(elements NewNode(param1[temp0] param1[temp0]))
                ++size
            )
            ++temp0
        )
        return self
    )


    (method (addAfter param1 param2)
        (var temp0, temp1, temp2)
        = temp2 FindKey(elements param1)
        (if (temp2)
            --paramTotal
            = temp0 0
            (while (< temp0 paramTotal)
                (if (not (self:isDuplicate(param2[temp0])))
                    = temp2 AddAfter(elements temp2 NewNode(param2[temp0] param2[temp0]))
                    ++size
                )
                ++temp0
            )
        )
        return self
    )


    (method (indexOf param1)
        (var temp0, temp1)
        = temp0 0
        = temp1 FirstNode(elements)
        (while (temp1)
            (if (== param1 NodeValue(temp1))
                return temp0
            )
            ++temp0
            = temp1 NextNode(temp1)
        )
        return -1
    )

)
(class Set of List
    (properties
        elements 0
        size 0
    )

    (method (showStr param1)
        Format(param1 999 2 name size)
    )


    (method (isDuplicate param1)
        (self:contains(param1))
    )

)
(class EventHandler of Set
    (properties
        elements 0
        size 0
    )

    (method (handleEvent pEvent)
        (var temp0, temp1, temp2, temp3, temp4)
        = temp3 Clone(pEvent)
        = temp0 FirstNode(elements)
        (while (temp0 and not (send temp3:claimed))
            = temp1 NextNode(temp0)
            = temp2 NodeValue(temp0)
            (if (not IsObject(temp2))
                break
            )
            (send temp2:handleEvent(temp3))
            = temp0 temp1
        )
        = temp4 (send temp3:claimed)
        (send temp3:dispose())
        return temp4
    )

)
(class Script of Obj
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
    )

    (method (init theClient theCaller theRegister)
        = lastTicks gLastTicks
        (if (>= paramTotal 1)
            (send (= client theClient):script(self))
            (if (>= paramTotal 2)
                = caller theCaller
                (if (>= paramTotal 3)
                    = register theRegister
                )
            )
        )
        = state (- start 1)
        (self:cue())
    )


    (method (doit)
        (var theLastSeconds)
        (if (script)
            (send script:doit())
        )
        (if (cycles)
            (if (not --cycles)
                (self:cue())
            )
        )(else
            (if (seconds)
                = theLastSeconds GetTime(gtTIME_OF_DAY)
                (if (<> lastSeconds theLastSeconds)
                    = lastSeconds theLastSeconds
                    (if (not --seconds)
                        (self:cue())
                    )
                )
            )(else
                (if (ticks and (<= (= ticks (- ticks Abs((- gLastTicks lastTicks)))) 0))
                    = ticks 0
                    (self:cue())
                )
            )
        )
        = lastTicks gLastTicks
    )


    (method (dispose)
        (var temp0)
        (if (IsObject(script))
            (send script:dispose())
        )
        (if (IsObject(timer))
            (send timer:dispose())
        )
        (if (IsObject(client))
            = temp0 
                (if (IsObject(next))
                    next
                )(else
                    (if (next)
                        ScriptID(next)
                    )
                )
            (send client:script(temp0))
            (if (not temp0)
                0
            )(else
                (if (== gRoomNumber gModNum)
                    (send temp0:init(client))
                )(else
                    (send temp0:dispose())
                )
            )
        )
        (if (IsObject(caller) and (== gRoomNumber gModNum))
            (send caller:cue(register))
        )
        = script (= timer (= client (= next (= caller 0))))
        (super:dispose())
    )


    (method (changeState newState)
        = state newState
    )


    (method (cue param1)
        (if (client)
            (self:changeState((+ state 1) rest param1))
        )
    )


    (method (handleEvent pEvent)
        (if (script)
            (send script:handleEvent(pEvent))
        )
        (send pEvent:claimed)
    )


    (method (setScript theScript sendParams)
        (if (IsObject(script))
            (send script:dispose())
        )
        (if (theScript)
            (send theScript:init(self rest sendParams))
        )
    )

)
(class Event of Obj
    (properties
        type $0000
        message 0
        modifiers $0000
        y 0
        x 0
        claimed 0
        port 0
    )

    (method (new param1)
        (var newSuper)
        = newSuper (super:new())
        GetEvent(
            (if (paramTotal)
                param1
            )(else
                evALL_EVENTS
            )
 newSuper)
 
        return newSuper
    )


	// Brings it to the current port's local coordinates.
    (method (localize)
        (var thePort)
        (if (not & type evVERB)
            = thePort GetPort()
            (if (not port)
                GlobalToLocal(self)
            )(else
                (if (<> port thePort)
                    SetPort(port)
                    LocalToGlobal(self)
                    SetPort(thePort)
                    GlobalToLocal(self)
                )
            )
            = port thePort
        )
        return self
    )


    (method (globalize)
        (var temp0)
        (if (not & type evVERB)
            = temp0 GetPort()
            (if (== port temp0)
                LocalToGlobal(self)
            )(else
                (if (port)
                    SetPort(port)
                    LocalToGlobal(self)
                    SetPort(temp0)
                )
            )
            = port 0
        )
        return self
    )

)
(class Cursor of Obj
    (properties
        view 0
        loop 0
        cel 0
        x 0
        y 0
        hotSpotX 0
        hotSpotY 0
        hidden 0
    )

    (method (init)
        (if (hotSpotX or hotSpotY)
            SetCursor(view loop cel hotSpotX hotSpotY)
        )(else
            SetCursor(view loop cel)
        )
    )


    (method (posn param1 param2)
        SetCursor(param1 param2)
    )


    (method (posnHotSpot theHotSpotX theHotSpotY)
        = hotSpotX theHotSpotX
        = hotSpotY theHotSpotY
        (self:init())
    )


    (method (setLoop theLoop)
        = loop theLoop
        (self:init())
    )


    (method (setCel theCel)
        = cel theCel
        (self:init())
    )


    (method (showCursor param1)
        (if (paramTotal)
            SetCursor(param1)
        )
    )

)
