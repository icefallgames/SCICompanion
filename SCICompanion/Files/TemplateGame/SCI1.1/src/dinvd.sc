(version 2)
(include "sci.sh")
(exports
    0 dInvD
    1 proc16_1
    2 proc16_2
)
(use "Main")
(use "Controls")
(use "DIcon")
(use "InvI")
(script 16)


(local
    newDButton
    local1[2]

)
(procedure public (proc16_1)
)


(procedure public (proc16_2)
)


(instance public dInvD of Dialog
    (properties)

    (method (init)
        (var temp0, temp1, temp2, temp3, newDText, gSq5InvFirst, temp6)
        = temp1 4
        = temp0 temp1
        = temp2 temp0
        = temp3 0
        = gSq5InvFirst (send gSq5Inv:first())
        (while (gSq5InvFirst)
            = temp6 NodeValue(gSq5InvFirst)
            ++temp3
            (if ((send temp6:isKindOf(InvI)))
                = newDText (DText:new())
                (self:add((send newDText:
                        value(temp6)
                        text((send temp6:name))
                        nsLeft(temp0)
                        nsTop(temp1)
                        state(3)
                        font(gSmallFont)
                        setSize()
                        yourself()
                    )
))
            )
            (if (< temp2 (- (send newDText:nsRight) (send newDText:nsLeft)))
                = temp2 (- (send newDText:nsRight) (send newDText:nsLeft))
            )
            = temp1 (+ temp1 (+ (- (send newDText:nsBottom) (send newDText:nsTop)) 1))
            (if (> temp1 140)
                = temp1 4
                = temp0 (+ temp0 (+ temp2 10))
                = temp2 0
            )
            = gSq5InvFirst (send gSq5Inv:next(gSq5InvFirst))
        )
        = window gSq5Win
        (self:setSize())
        = newDButton (DButton:new())
        (send newDButton:
            text("All Done!")
            setSize()
            moveTo(- nsRight (+ 4 (send newDButton:nsRight)) nsBottom)
        )
        (send newDButton:move((- (send newDButton:nsLeft) (send newDButton:nsRight)) 0))
        (self:
            add(newDButton)
            setSize()
            center()
        )
        return temp3
    )


    (method (doit)
        (var theNewDButton)
        (self:init())
        (self:open(4 15))
        = theNewDButton newDButton
        (while (TRUE)
            = theNewDButton (super:doit(theNewDButton))
            (if ((not theNewDButton or (== theNewDButton -1)) or (== theNewDButton newDButton))
                break
            )
            (send gEgo:get((send gSq5Inv:indexOf((send theNewDButton:value)))))
        )
        (self:
            eachElementDo(#dispose 1)
            dispose()
        )
    )


    (method (handleEvent pEvent)
        (var pEventMessage, pEventType)
        = pEventMessage (send pEvent:message)
        (switch (= pEventType (send pEvent:type))
            (case 4
                (switch (pEventMessage)
                    (case KEY_UP
                        = pEventMessage 3840
                    )
                    (case KEY_NUMPAD2
                        = pEventMessage 9
                    )
                )
            )
            (case 64
                (switch (pEventMessage)
                    (case JOY_UP
                        = pEventMessage 3840
                        = pEventType 4
                    )
                    (case JOY_DOWN
                        = pEventMessage 9
                        = pEventType 4
                    )
                )
            )
        )
        (send pEvent:
            type(pEventType)
            message(pEventMessage)
        )
        (super:handleEvent(pEvent))
    )

)
