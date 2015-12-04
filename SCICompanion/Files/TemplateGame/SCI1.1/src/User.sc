(version 2)
(include "sci.sh")
(use "Main")
(use "System")
(script 996)

(instance uEvt of Event
    (properties)

    (method (new)
        = type (= message (= modifiers (= y (= x (= claimed (= port 0))))))
        GetEvent(evALL_EVENTS self)
        return self
    )
)

/*
	This class manages user input. The main method of interest here is canControl().
	
	Example usage::
	
		(if (gUser canControl:)
			; The user is in control...
		)
*/
(class User of Object
    (properties
        alterEgo 0
        input 0
        controls 0
        prevDir 0
        x -1
        y -1
        mapKeyToDir 1
        curEvent 0
    )

    (method (init)
        = curEvent uEvt
    )


    (method (doit)
        (send curEvent:new())
        (self:handleEvent(curEvent))
    )

	/*
	.. function:: canControl()
	
		:returns: TRUE if the user is currently controlling the game, otherwise FALSE.
		
	.. function:: canControl(shouldControl)
	
		:param boolean shouldControl: TRUE if the user should be in control, otherwise FALSE.
		
		.. IMPORTANT::
			Generally, (gGame handsOff:) and (gGame handsOn:) should be used to give and take away user control of the game.
	*/
    (method (canControl shouldControl)
        (if (paramTotal)
            = controls shouldControl
            = prevDir 0
        )
        return controls
    )

    (method (handleEvent pEvent)
        (var pEventType, pEventMessage, pEventModifiers, temp3, temp4)
        
        = gPEventX (send pEvent:x)
        = gPEventY (send pEvent:y)
        = pEventType (send pEvent:type)
        = pEventModifiers (send pEvent:modifiers)
        (if (pEventType)
            = gPEvent pEvent
            (if (mapKeyToDir)
                MapKeyToDir(pEvent)
            )
            (if (== pEventType 256)
                = pEventType 4
                = pEventMessage 
                    (if (& pEventModifiers emSHIFT)
                        KEY_ESCAPE
                    )(else
                        KEY_RETURN
                    )
                = pEventModifiers 0
                (send pEvent:
                    type(pEventType)
                    message(pEventMessage)
                    modifiers(pEventModifiers)
                )
            )
            (if (gPrints and (send gPrints:handleEvent(pEvent)))
                return 1
            )
            (send pEvent:localize())
            = pEventType (send pEvent:type)
            = pEventMessage (send pEvent:message)
            (if (& pEventType evSAID)
                (if ((== pEventMessage JOY_UP) and (((= temp4 (send gCast:firstTrue(#perform findNoun))) or (= temp4 (send gFeatures:firstTrue(#perform findNoun)))) or (= temp4 (send gAddToPics:firstTrue(#perform findNoun)))))
                    (send temp4:doVerb((send ((send gIconBar:curIcon)):message)))
                )(else
                    = temp4 (send gIconBar:findIcon(pEventMessage))
                    (if (temp4)
                        (send gIconBar:select((= temp4 (send gIconBar:findIcon(pEventMessage)))))
                        (send gGame:setCursor((send temp4:cursor)))
                    )
                )
            )(else
                (if (& pEventType evJOYSTICK)
                    (if (gOldDH and (send gOldDH:handleEvent(pEvent)))
                        return 1
                    )(else
                        (if ((((((gIconBar and (== (send gIconBar:curIcon) (send gIconBar:walkIconItem))) or not gIconBar) and alterEgo) and controls) and (send gCast:contains(alterEgo))) and (send alterEgo:handleEvent(pEvent)))
                            return 1
                        )(else
                            (if ((gPseudoMouse and (not (& pEventType evKEYBOARD) or (<> pEventMessage JOY_NULL))) and (send gPseudoMouse:handleEvent(pEvent)))
                                return 1
                            )
                        )
                    )
                )(else
                    (if (((& pEventType evKEYBOARD) and gOldKH) and (send gOldKH:handleEvent(pEvent)))
                        return 1
                    )(else
                        (if (((& pEventType evMOUSE) and gOldMH) and (send gOldMH:handleEvent(pEvent)))
                            return 1
                        )
                    )
                )
            )
        )
        (if (gIconBar)
            (send gIconBar:handleEvent(pEvent))
        )
        = pEventType (send pEvent:type)
        = pEventMessage (send pEvent:message)
        (if (input and (& pEventType evVERB))
            (if (((& pEventType evMOVE) and gWalkHandler) and (send gWalkHandler:handleEvent(pEvent)))
                return 1
            )(else
                (if ((((& pEventType evMOVE) and (send gCast:contains(alterEgo))) and controls) and (send alterEgo:handleEvent(pEvent)))
                    return 1
                )(else
                    (if (gUseSortedFeatures)
                        (OnMeAndLowY:init())
                        (send gCast:eachElementDo(#perform OnMeAndLowY pEvent))
                        (send gFeatures:eachElementDo(#perform OnMeAndLowY pEvent))
                        (send gAddToPics:eachElementDo(#perform OnMeAndLowY pEvent))
                        (if ((OnMeAndLowY:theObj) and (send ((OnMeAndLowY:theObj)):handleEvent(pEvent)))
                            return 1
                        )
                    )(else
                        (if ((send gCast:handleEvent(pEvent)))
                            return 1
                        )(else
                            (if ((send gFeatures:handleEvent(pEvent)))
                                return 1
                            )(else
                                (if ((send gAddToPics:handleEvent(pEvent)))
                                    return 1
                                )
                            )
                        )
                    )
                )
            )
            (if (not (send pEvent:claimed) and (send gRegions:handleEvent(pEvent)))
                return 1
            )
        )
        (if (pEventType)
            (if ((send gGame:handleEvent(pEvent)))
                return 1
            )(else
                (if (gPrints and (send gPrints:handleEvent(pEvent)))
                    return 1
                )
            )
        )
        return 0
    )

	/*
	.. function:: canInput()
	
		:returns: TRUE if input is enabled, FALSE otherwise.
		
	.. function:: canInput(shouldInput)
	
		:param boolean shouldInput: TRUE if input should be enabled, otherwise FALSE.
	*/
    (method (canInput theInput)
        (if (paramTotal)
            = input theInput
        )
        return input
    )
)

/*
	This class is helper class used in determining which object the player has clicked on.
*/
(class OnMeAndLowY of Code
    (properties
        theObj 0
        lastY -1
    )

    (method (init)
        = theObj 0
        = lastY -1
    )


    (method (doit theTheObj param2)
        (if ((send theTheObj:onMe(param2)) and (> (send theTheObj:y) lastY))
            = lastY (send ((= theObj theTheObj)):y)
        )
    )
)

(instance findNoun of Code
    (properties)

    (method (doit param1 param2)
        return == (send param1:noun) param2
    )

)
