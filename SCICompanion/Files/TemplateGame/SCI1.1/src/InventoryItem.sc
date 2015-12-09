(version 2)
(include "sci.sh")
(include "Verbs.sh")
(use "Main")
(use "Print")
(use "IconItem")
(use "SysWindow")
(use "System")
(script 995)


/*
	This extends :class:`IconItem` and adds the ability to automatically print messages (for the *look* and *do* verbs) that include a visual representation of the inventory item.
	It also provides an owner property that indicates who owns the item.
	
	An example inventory item might look like::
	
		(instance Hammer of InventoryItem
			(properties
				view 900
				loop 1
				cursor 900			; Optional cursor when using this item.
				message V_HAMMER	; Optional verb associated with the item.
				signal $0002
				noun N_HAMMER		; noun from message resource 0
			)
		)
*/
(class InventoryItem of IconItem
    (properties
        view 0
        loop 0
        cel 0
        nsLeft 0
        nsTop 0
        nsRight 0
        nsBottom 0
        state $0000
        cursor 999
        type evVERB
        message 0
        modifiers $0000
        signal $0000
        maskView 0
        maskLoop 0
        maskCel 0
        highlightColor 0
        lowlightColor 0
        noun 0
        modNum 0
        helpVerb 0
        owner 0
        script 0
        value 0
    )

    (method (show)
        (var temp0[4])
        DrawCel(view loop cel nsLeft nsTop -1)
    )


    (method (highlight param1)
        (var temp0, temp1, temp2, temp3, temp4)
        (if (== highlightColor -1)
            return 
        )
        = temp4 
            (if (paramTotal and param1)
                highlightColor
            )(else
                lowlightColor
            )
        = temp0 (- nsTop 2)
        = temp1 (- nsLeft 2)
        = temp2 (+ nsBottom 1)
        = temp3 (+ nsRight 1)
        Graph(grDRAW_LINE temp0 temp1 temp0 temp3 temp4 -1 -1)
        Graph(grDRAW_LINE temp0 temp3 temp2 temp3 temp4 -1 -1)
        Graph(grDRAW_LINE temp2 temp3 temp2 temp1 temp4 -1 -1)
        Graph(grDRAW_LINE temp2 temp1 temp0 temp1 temp4 -1 -1)
        Graph(grUPDATE_BOX - nsTop 2 - nsLeft 2 + nsBottom 2 + nsRight 2 1)
    )


    (method (onMe param1)
        return 
            (if ((super:onMe(param1)))
                not & signal icDISABLED
            )(else
                0
            )
    )


    (method (ownedBy theObj)
        return == owner theObj
    )


    (method (moveTo theOwner)
        = owner theOwner
        (if (value and (== theOwner gEgo))
        	AddToScore(value)
            = value 0
        )
        return self
    )


    (method (doVerb theVerb)
        (var temp0, temp1)
        (if (not modNum)
            = modNum gRoomNumber
        )
        (switch (theVerb)
            (case V_LOOK
                (if (Message(msgSIZE modNum noun V_LOOK 0 1))
                    = temp1 CelWide(view loop cel)
                    = temp0 GetPort()
                    (Print:
                        addIcon(view loop cel 0 0)
                        addText(noun V_LOOK 0 1 (+ temp1 4) 0 modNum)
                        init()
                    )
                    SetPort(temp0)
                )
            )
            (case V_DO
                (if (Message(msgSIZE modNum noun V_DO 0 1))
                    = temp0 GetPort()
                    (Print:
                        addText(noun V_DO 0 0 0 0 modNum)
                        init()
                    )
                    SetPort(temp0)
                )(else
                    = temp0 GetPort()
                    (Print:
                        addText(0 V_DO 0 0 0 0 modNum)
                        init()
                    )
                    SetPort(temp0)
                )
            )
            (default 
                (if (Message(msgSIZE modNum noun theVerb 0 1))
                    = temp0 GetPort()
                    (Print:
                        addText(noun theVerb 0 0 0 0 modNum)
                        init()
                    )
                    SetPort(temp0)
                )(else
                    = temp0 GetPort()
                    (Print:
                        addText(0 7 0 0 0 0 modNum)
                        init()
                    )
                    SetPort(temp0)
                )
            )
        )
    )

)
