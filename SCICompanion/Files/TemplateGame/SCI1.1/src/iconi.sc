(version 2)
(include "sci.sh")
(use "Main")
(use "Print")
(use "Tutorial")
(use "Obj")
(script 937)



(class IconI of Obj
    (properties
        view -1
        loop -1
        cel -1
        nsLeft 0
        nsTop -1
        nsRight 0
        nsBottom 0
        state $0000
        cursor -1
        type evVERB
        message -1
        modifiers $0000
        signal $0001
        maskView 0
        maskLoop 0
        maskCel 0
        highlightColor 0
        lowlightColor 0
        noun 0
        modNum 0
        helpVerb 0
    )

    (method (show theNsLeft theNsTop)
        (var temp0[7])
        = signal (| signal icVISIBLE)
        (if (paramTotal)
            = nsRight (+ (= nsLeft theNsLeft) CelWide(view loop cel))
            = nsBottom (+ (= nsTop theNsTop) CelHigh(view loop cel))
        )(else
            = nsRight (+ nsLeft CelWide(view loop cel))
            = nsBottom (+ nsTop CelHigh(view loop cel))
        )
        DrawCel(view loop cel nsLeft nsTop -1)
        (if (& signal icDISABLED)
            (self:mask())
        )
        (if (gPseudoMouse and (send gPseudoMouse:respondsTo(#stop)))
            (send gPseudoMouse:stop())
        )
    )


    (method (select param1)
        (var newEvent, temp1, gGameScript)
        return 
            (if (& signal icDISABLED)
                0
            )(else
                (if ((paramTotal and param1) and (& signal notUpd))
                    = temp1 1
                    DrawCel(view loop temp1 nsLeft nsTop -1)
                    Graph(grUPDATE_BOX nsTop nsLeft nsBottom nsRight 1)
                    (while (<> (send ((= newEvent (Event:new()))):type) 2)
                        (send newEvent:localize())
                        (if ((self:onMe(newEvent)))
                            (if (not temp1)
                                = temp1 1
                                DrawCel(view loop temp1 nsLeft nsTop -1)
                                Graph(grUPDATE_BOX nsTop nsLeft nsBottom nsRight 1)
                            )
                        )(else
                            (if (temp1)
                                = temp1 0
                                DrawCel(view loop temp1 nsLeft nsTop -1)
                                Graph(grUPDATE_BOX nsTop nsLeft nsBottom nsRight 1)
                            )
                        )
                        (send newEvent:dispose())
                    )
                    (send newEvent:dispose())
                    (if (== temp1 1)
                        DrawCel(view loop 0 nsLeft nsTop -1)
                        Graph(grUPDATE_BOX nsTop nsLeft nsBottom nsRight 1)
                    )
                    = gGameScript (send gGame:script)
                    (if (gGameScript and (send gGameScript:isKindOf(Tutorial)))
                        (if ((== (send gGameScript:nextItem) self) and (<> (send gGameScript:nextAction) (send ((send gIconBar:helpIconItem)):message)))
                            (send gGameScript:cue())
                        )(else
                            (if (not temp1)
                                return 0
                            )(else
                                (send gGameScript:report())
                                return 0
                            )
                        )
                    )
                    temp1
                )(else
                    = gGameScript (send gGame:script)
                    (if (gGameScript and (send gGameScript:isKindOf(Tutorial)))
                        (if ((== (send gGameScript:nextItem) self) and (<> (send gGameScript:nextAction) (send ((send gIconBar:helpIconItem)):message)))
                            (send gGameScript:cue())
                        )(else
                            (send gGameScript:report())
                            return 0
                        )
                    )
                    1
                )
            )
    )


    (method (highlight param1)
        (var temp0, temp1, temp2, temp3, temp4)
        (if (not (& signal icVISIBLE) or (== highlightColor -1))
            return 
        )
        = temp4 
            (if (paramTotal and param1)
                highlightColor
            )(else
                lowlightColor
            )
        = temp0 (+ nsTop 2)
        = temp1 (+ nsLeft 2)
        = temp2 (- nsBottom 3)
        = temp3 (- nsRight 4)
        Graph(grDRAW_LINE temp0 temp1 temp0 temp3 temp4 -1 -1)
        Graph(grDRAW_LINE temp0 temp3 temp2 temp3 temp4 -1 -1)
        Graph(grDRAW_LINE temp2 temp3 temp2 temp1 temp4 -1 -1)
        Graph(grDRAW_LINE temp2 temp1 temp0 temp1 temp4 -1 -1)
        Graph(grUPDATE_BOX - nsTop 2 - nsLeft 2 nsBottom + nsRight 3 1)
    )


    (method (onMe param1)
        return 
            (if (((>= (send param1:x) nsLeft) and (>= (send param1:y) nsTop)) and (<= (send param1:x) nsRight))
                <= (send param1:y) nsBottom
            )(else
                0
            )
    )


    (method (mask)
        DrawCel(maskView maskLoop maskCel + nsLeft (/ (- CelWide(view loop cel) CelWide(maskView maskLoop maskCel)) 2) + nsTop (/ (- CelHigh(view loop cel) CelHigh(maskView maskLoop maskCel)) 2) -1)
    )

)
(class IconBar of Set
    (properties
        elements 0
        size 0
        height 0
        underBits 0
        oldMouseX 0
        oldMouseY 0
        curIcon 0
        highlightedIcon 0
        prevIcon 0
        curInvIcon 0
        useIconItem 0
        helpIconItem 0
        walkIconItem 0
        port 0
        window 0
        state $0400
        activateHeight 0
        y 0
    )

    (method (doit)
        (var temp0, temp1, temp2, temp3, gGameScript)
        (while ((& state $0020) and (= temp0 (send ((send gUser:curEvent)):new())))
            = temp1 (send temp0:type)
            = temp2 (send temp0:message)
            = temp3 (send temp0:modifiers)
            Wait(1)
            = gLastTicks (+ global86 GetTime())
            (if (gNewSet)
                (send gNewSet:eachElementDo(#doit))
            )
            = gGameScript (send gGame:script)
            (if (gGameScript and (send gGameScript:isKindOf(Tutorial)))
                (send gGameScript:doit())
            )
            (if (== temp1 256)
                = temp1 evKEYBOARD
                = temp2 
                    (if (& temp3 emSHIFT)
                        KEY_ESCAPE
                    )(else
                        KEY_RETURN
                    )
                = temp3 0
                (send temp0:
                    type(temp1)
                    message(temp2)
                    modifiers(temp3)
                )
            )
            (send temp0:localize())
            (if ((((== temp1 evMOUSEBUTTON) or ((== temp1 evKEYBOARD) and (== temp2 KEY_RETURN))) and IsObject(helpIconItem)) and (& (send helpIconItem:signal) $0010))
                (send temp0:
                    type(evHELPVERB)
                    message((send helpIconItem:message))
                )
            )
            MapKeyToDir(temp0)
            (if ((self:dispatchEvent(temp0)))
                break
            )
        )
    )


    (method (handleEvent pEvent)
        (var temp0, pEventType, temp2, temp3, theGCursorNumber, theCurIcon, theCurInvIcon)
        (send pEvent:localize())
        = pEventType (send pEvent:type)
        (if (& state $0004)
        )(else
            (if (((((((not pEventType and (& state $0400)) and (<= -10 (send pEvent:y))) and (<= (send pEvent:y) height)) and (<= 0 (send pEvent:x))) and (<= (send pEvent:x) 320)) and not (= temp0 0)) or (((== pEventType evKEYBOARD) and ((== (send pEvent:message) KEY_ESCAPE) or (== (send pEvent:message) KEY_DELETE))) and (= temp0 1)))
                (send pEvent:globalize())
                = oldMouseX (send pEvent:x)
                = oldMouseY (send pEvent:y)
                = theGCursorNumber gCursorNumber
                = theCurIcon curIcon
                = theCurInvIcon curInvIcon
                (self:show())
                (send gGame:setCursor(999))
                (if (temp0)
                    (send gGame:setCursor(gCursorNumber 1 (+ (send curIcon:nsLeft) (/ (- (send curIcon:nsRight) (send curIcon:nsLeft)) 2)) (- (send curIcon:nsBottom) 3)))
                )
                (self:doit())
                = temp3 
                    (if ((send gUser:canControl()) or (send gUser:canInput()))
                        (send curIcon:cursor)
                    )(else
                        gInvisibleCursor
                    )
                (if (temp0)
                    (send gGame:setCursor(temp3 1 oldMouseX oldMouseY))
                )(else
                    (send gGame:setCursor(temp3 1 (send ((send pEvent:new())):x) Max((send pEvent:y) (+ 1 height))))
                )
                (self:hide())
            )(else
                (if (& pEventType evKEYBOARD)
                    (switch ((send pEvent:message))
                        (case KEY_RETURN
                            (if (not IsObject(curIcon))
                            )(else
                                (if ((<> curIcon useIconItem) or curInvIcon)
                                    (send pEvent:
                                        type((send curIcon:type))
                                        message(
                                            (if (== curIcon useIconItem)
                                                (send curInvIcon:message)
                                            )(else
                                                (send curIcon:message)
                                            )
											   )
                                    )
                                )(else
                                    (send pEvent:type(0))
                                )
                            )
                        )
                        (case KEY_NUMPAD0
                            (if ((send gUser:canControl()))
                                (self:swapCurIcon())
                            )
                            (send pEvent:claimed(1))
                        )
                        (case JOY_NULL
                            (if (& (send pEvent:type) evJOYSTICK)
                                (self:advanceCurIcon())
                                (send pEvent:claimed(1))
                            )
                        )
                    )
                )(else
                    (if (& pEventType evMOUSEBUTTON)
                        (if (& (send pEvent:modifiers) emSHIFT)
                            (self:advanceCurIcon())
                            (send pEvent:claimed(1))
                        )(else
                            (if (& (send pEvent:modifiers) emCTRL)
                                (if ((send gUser:canControl()))
                                    (self:swapCurIcon())
                                )
                                (send pEvent:claimed(1))
                            )(else
                                (if (IsObject(curIcon))
                                    (send pEvent:
                                        type((send curIcon:type))
                                        message(
                                            (if (== curIcon useIconItem)
                                                (send curInvIcon:message)
                                            )(else
                                                (send curIcon:message)
                                            )
											   )
                                    )
                                )
                            )
                        )
                    )
                )
            )
        )
    )


    (method (show)
        (var temp0, temp1, temp2, temp3, theY, temp5, temp6, temp7)
        (send gSounds:pause())
        = state (| state $0020)
        (send gGame:setCursor(999 1))
        = temp0 (self:at(0))
        = height CelHigh((send temp0:view) (send temp0:loop) (send temp0:cel))
        = port GetPort()
        SetPort(-1)
        = underBits Graph(grSAVE_BOX y 0 (+ y height) 320 1)
        = temp1 PicNotValid()
        PicNotValid(1)
        = temp3 0
        = theY y
        = temp5 FirstNode(elements)
        (while (temp5)
            = temp6 NextNode(temp5)
            = temp7 NodeValue(temp5)
            (if (not IsObject(temp7))
                return 
            )
            (if (<= (send temp7:nsRight) 0)
                (send temp7:show(temp3 theY))
                = temp3 (send temp7:nsRight)
            )(else
                (send temp7:show())
            )
            = temp5 temp6
        )
        (if (curInvIcon)
            (if ((send gEgo:has((send gInv:indexOf(curInvIcon)))))
                = temp3 (+ (/ (- (- (send useIconItem:nsRight) (send useIconItem:nsLeft)) CelWide((send curInvIcon:view) (send curInvIcon:loop) (send curInvIcon:cel))) 2) (send useIconItem:nsLeft))
                = theY (+ (+ y (/ (- (- (send useIconItem:nsBottom) (send useIconItem:nsTop)) CelHigh((send curInvIcon:view) (send curInvIcon:loop) (send curInvIcon:cel))) 2)) (send useIconItem:nsTop))
                DrawCel((send curInvIcon:view) (send curInvIcon:loop) (send curInvIcon:cel) temp3 theY -1)
                (if (& (send useIconItem:signal) icDISABLED)
                    (send useIconItem:mask())
                )
            )(else
                = curInvIcon 0
            )
        )
        PicNotValid(temp1)
        Graph(grUPDATE_BOX y 0 + y height 320 1)
        (self:highlight(curIcon))
    )


    (method (hide)
        (var temp0, temp1, temp2)
        (if (& state $0020)
            (send gSounds:pause(0))
            = state (& state $ffdf)
            = temp0 FirstNode(elements)
            (while (temp0)
                = temp1 NextNode(temp0)
                = temp2 NodeValue(temp0)
                (if (not IsObject(temp2))
                    return 
                )
                = temp2 NodeValue(temp0)
                (send temp2:signal((& (send temp2:signal) (bnot icVISIBLE) )))
                = temp0 temp1
            )
            (if ((not (& state $0800) and IsObject(helpIconItem)) and (& (send helpIconItem:signal) $0010))
                (send helpIconItem:signal((& (send helpIconItem:signal) $ffef)))
            )
            Graph(grRESTORE_BOX underBits)
            Graph(grUPDATE_BOX y 0 + y height 320 1)
            Graph(grREDRAW_BOX y 0 + y height 320)
            SetPort(port)
            = height activateHeight
        )
    )


    (method (advance)
        (var temp0, temp1)
        = temp1 1
        (while (<= temp1 size)
            = temp0 (self:at((% (+ temp1 (self:indexOf(highlightedIcon))) size)))
            (if (not IsObject(temp0))
                = temp0 NodeValue((self:first()))
            )
            (if (not & (send temp0:signal) icDISABLED)
                break
            )
            = temp1 (% (+ temp1 1) size)
        )
        (self:highlight(temp0 (& state $0020)))
    )


    (method (retreat)
        (var temp0, temp1)
        = temp1 1
        (while (<= temp1 size)
            = temp0 (self:at((% (- (self:indexOf(highlightedIcon)) temp1) size)))
            (if (not IsObject(temp0))
                = temp0 NodeValue((self:last()))
            )
            (if (not & (send temp0:signal) icDISABLED)
                break
            )
            = temp1 (% (+ temp1 1) size)
        )
        (self:highlight(temp0 (& state $0020)))
    )


    (method (select theCurIcon param2)
        return 
            (if ((send theCurIcon:select(
                (if (>= paramTotal 2)
                    param2
                )
)))
                (if (not & (send theCurIcon:signal) $0002)
                    = curIcon theCurIcon
                )
                1
            )(else
                0
            )
    )


    (method (highlight theHighlightedIcon param2)
        (var temp0)
        (if (not & (send theHighlightedIcon:signal) icDISABLED)
            (if (IsObject(highlightedIcon))
                (send highlightedIcon:highlight(0))
            )
            (send (= highlightedIcon theHighlightedIcon):highlight(1))
        )
        (if ((>= paramTotal 2) and param2)
            (send gGame:setCursor(gCursorNumber 1 (+ (send theHighlightedIcon:nsLeft) (/ (- (send theHighlightedIcon:nsRight) (send theHighlightedIcon:nsLeft)) 2)) (- (send theHighlightedIcon:nsBottom) 3)))
        )
    )


    (method (swapCurIcon)
        (var temp0)
        (if (& state icDISABLED)
            return 
        )(else
            = temp0 NodeValue((self:first()))
            (if ((<> curIcon temp0) and not (& (send temp0:signal) icDISABLED))
                = prevIcon curIcon
                = curIcon NodeValue((self:first()))
            )(else
                (if (prevIcon and not (& (send prevIcon:signal) icDISABLED))
                    = curIcon prevIcon
                )
            )
        )
        (send gGame:setCursor((send curIcon:cursor) 1))
    )


    (method (advanceCurIcon)
        (var theCurIcon, temp1, temp2)
        (if (& state $0004)
            return 
        )
        = theCurIcon curIcon
        = temp1 0
        (while (& (send ((= theCurIcon (self:at((% (+ (self:indexOf(theCurIcon)) 1) size))))):signal) $0006)
            (if (> temp1 (+ 1 size))
                return 
            )(else
                ++temp1
            )
        )
        = curIcon theCurIcon
        (send gGame:setCursor((send curIcon:cursor) 1))
    )


    (method (dispatchEvent pEvent)
        (var temp0, temp1, eventType, eventMessage, theHighlightedIcon, temp5, temp6, temp7[50], temp57, theHighlightedIconSignal, temp59, temp60)
        = temp1 (send pEvent:x)
        = temp0 (send pEvent:y)
        = eventType (send pEvent:type)
        = eventMessage (send pEvent:message)
        = temp5 (send pEvent:claimed)
        = theHighlightedIcon (self:firstTrue(#onMe pEvent))
        (if (theHighlightedIcon)
            = temp57 (send ((= theHighlightedIcon (self:firstTrue(#onMe pEvent)))):cursor)
            = theHighlightedIconSignal (send theHighlightedIcon:signal)
            = temp59 (== theHighlightedIcon helpIconItem)
        )
        (if (& eventType evJOYSTICK)
            (switch (eventMessage)
                (case JOY_RIGHT
                    (self:advance())
                )
                (case JOY_LEFT
                    (self:retreat())
                )
            )
        )(else
            (switch (eventType)
                (case evNULL
                    (if (not 
                    (if (((<= 0 temp0) and (<= temp0 (+ y height))) and (<= 0 temp1))
                        <= temp1 320
                    )
						)
                        (if ((& state $0400) and (not IsObject(helpIconItem) or not (& (send helpIconItem:signal) $0010)))
                            = oldMouseY 0
                            = temp5 1
                        )
                    )(else
                        (if (theHighlightedIcon and (<> theHighlightedIcon highlightedIcon))
                            = oldMouseY 0
                            (self:highlight(theHighlightedIcon))
                        )
                    )
                )
                (case evMOUSEBUTTON
                    (if (theHighlightedIcon and (self:select(theHighlightedIcon 1)))
                        (if (temp59)
                            (if (temp57)
                                (send gGame:setCursor(temp57))
                            )
                            (if (& state $0800)
                                (self:noClickHelp())
                            )(else
                                (send helpIconItem:signal((| (send helpIconItem:signal) $0010)))
                            )
                        )(else
                            = temp5 (& theHighlightedIconSignal $0040)
                        )
                        (send theHighlightedIcon:doit())
                    )
                )
                (case evKEYBOARD
                    (switch (eventMessage)
                        (case KEY_ESC
                            = temp5 1
                        )
                        (case KEY_DELETE
                            = temp5 1
                        )
                        (case KEY_RETURN
                            (if (not theHighlightedIcon)
                                = theHighlightedIcon highlightedIcon
                            )
                            (if (theHighlightedIcon and (== theHighlightedIcon helpIconItem))
                                (if (<> temp57 -1)
                                    (send gGame:setCursor(temp57))
                                )
                                (if (helpIconItem)
                                    (send helpIconItem:signal((| (send helpIconItem:signal) $0010)))
                                )
                            )(else
                                (if (IsObject(theHighlightedIcon) and (self:select(theHighlightedIcon)))
                                    (send theHighlightedIcon:doit())
                                    = temp5 (& theHighlightedIconSignal $0040)
                                )
                            )
                        )
                        (case KEY_SHIFTTAB
                            (self:retreat())
                        )
                        (case KEY_TAB
                            (self:advance())
                        )
                    )
                )
                (case evHELPVERB
                    (if (theHighlightedIcon and (send theHighlightedIcon:helpVerb))
                        (if (not HaveMouse())
                            = temp60 (send gGame:setCursor(996))
                        )
                        = temp6 GetPort()
                        (Print:
                            font(gFont)
                            width(250)
                            addText((send theHighlightedIcon:noun) (send theHighlightedIcon:helpVerb) 0 1 0 0 (send theHighlightedIcon:modNum))
                            init()
                        )
                        SetPort(temp6)
                        (if (not HaveMouse())
                            (send gGame:setCursor(temp60))
                        )
                    )
                    (if (helpIconItem)
                        (send helpIconItem:signal((& (send helpIconItem:signal) $ffef)))
                    )
                    (send gGame:setCursor(999))
                )
            )
        )
        return temp5
    )


    (method (disable param1)
        (var temp0, temp1)
        (if (paramTotal)
            = temp0 0
            (while (< temp0 paramTotal)
                = temp1 
                    (if (IsObject(param1[temp0]))
                        param1[temp0]
                    )(else
                        (self:at(param1[temp0]))
                    )
                (send temp1:signal((| (send temp1:signal) icDISABLED)))
                (if (== temp1 curIcon)
                    (self:advanceCurIcon())
                )(else
                    (if (== temp1 highlightedIcon)
                        (self:advance())
                    )
                )
                ++temp0
            )
        )(else
            = state (| state $0004)
        )
    )


    (method (enable param1)
        (var temp0, temp1)
        (if (paramTotal)
            = temp0 0
            (while (< temp0 paramTotal)
                = temp1 
                    (if (IsObject(param1[temp0]))
                        param1[temp0]
                    )(else
                        (self:at(param1[temp0]))
                    )
                (send temp1:signal((& (send temp1:signal) (bnot icDISABLED))))
                ++temp0
            )
        )(else
            = state (& state $fffb)
        )
    )


    (method (noClickHelp)
        (var temp0, temp1, temp2, temp3, gSq5WinEraseOnly, temp5)
        = temp2 0
        = temp1 temp2
        = temp3 GetPort()
        = gSq5WinEraseOnly (send gWindow:eraseOnly)
        (send gWindow:eraseOnly(1))
        (while (not (send (= temp0 (send ((send gUser:curEvent)):new())):type))
            (if (not (self:isMemberOf(IconBar)))
                (send temp0:localize())
            )
            = temp2 (self:firstTrue(#onMe temp0))
            (if (temp2)
                (if ((<> (= temp2 (self:firstTrue(#onMe temp0))) temp1) and (send temp2:helpVerb))
                    = temp1 temp2
                    (if (gDialog)
                        (send gDialog:dispose())
                    )
                    (Print:
                        font(gFont)
                        width(250)
                        addText((send temp2:noun) (send temp2:helpVerb) 0 1 0 0 (send temp2:modNum))
                        modeless(1)
                        init()
                    )
                    SetPort(temp3)
                )
            )(else
                (if (gDialog)
                    (send gDialog:dispose())
                )(else
                    = temp1 0
                )
            )
            (send temp0:dispose())
        )
        (send gWindow:eraseOnly(gSq5WinEraseOnly))
        (send gGame:setCursor(999 1))
        (if (gDialog)
            (send gDialog:dispose())
        )
        SetPort(temp3)
        (if (not (send helpIconItem:onMe(temp0)))
            (self:dispatchEvent(temp0))
        )
    )


    (method (findIcon param1)
        (var temp0, temp1)
        = temp0 0
        (while (< temp0 size)
            = temp1 (self:at(temp0))
            (if (== (send temp1:message) param1)
                return temp1
            )
            ++temp0
        )
        return 0
    )

)
