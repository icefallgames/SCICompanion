(version 2)
(include "sci.sh")
(exports
    0 TextPrint
    1 FormatPrint
    2 EditPrint
    3 GetTotalLength
)
(use "Main")
(use "DColorButton")
(use "Controls")
(use "DIcon")
(use "Obj")
(script 921)



(procedure public (TextPrint param1)
    (Print:
        addText(rest param1)
        init()
    )
)


(procedure public (FormatPrint param1)
    (Print:
        addTextF(rest param1)
        init()
    )
)


(procedure public (EditPrint param1 param2 param3 param4)
    (if ((Print:
        font(
            (if (> paramTotal 3)
                param4
            )(else
                gFont
            )
			)
        addText(
            (if ((> paramTotal 2) and param3)
                param3
            )(else
                ""
            )
			   )
        addEdit(param1 param2 0 12 param1)
        init()
		 )
		)
        StrLen(param1)
	)
)


(procedure public (GetTotalLength param1 param2)
    (var temp0, temp1, temp2, temp3)
    = temp1 StrLen(param1)
    = temp0 temp1
    = temp2 0
    = temp3 0
    (while (< temp3 temp1)
        (if (== StrAt(param1 temp3) 37)
            (switch (StrAt(param1 ++temp3))
                (case 100
                    = temp0 (+ temp0 5)
                )
                (case 120
                    = temp0 (+ temp0 4)
                )
                (case 115
                    = temp0 (+ temp0 StrLen(param2[temp2]))
                )
            )
            ++temp2
        )
        ++temp3
    )
    return ++temp0
)


(class Print of Obj
    (properties
        dialog 0
        window 0
        title 0
        mode 0
        font -1
        width 0
        x -1
        y -1
        ticks 0
        caller 0
        retValue 0
        modeless 0
        first 0
        saveCursor 0
    )

    (method (init theCaller param2)
        = caller 0
        (if (paramTotal)
            = caller theCaller
        )
        (if (> paramTotal 1)
            (self:addText(rest param2))
        )
        (if (not modeless)
            (if (not IsObject(gPrintEventHandler))
                = gPrintEventHandler (send ((EventHandler:new())):name("prints"))
            )
            (send gPrintEventHandler:add(self))
        )
        (self:showSelf())
    )


    (method (doit)
        (send dialog:eachElementDo(#doit))
    )


    (method (dispose)
        (if (gPrintEventHandler and (send gPrintEventHandler:contains(self)))
            (send gPrintEventHandler:delete(self))
            (if ((send gPrintEventHandler:isEmpty()))
                (send gPrintEventHandler:dispose())
                = gPrintEventHandler NULL
            )
        )
        (if (title)
            Memory(memFREE title)
        )
        = width (= mode (= title (= first (= saveCursor (= window 0)))))
        = x (= y -1)
        = modeless 0
        (send gSounds:pause(0))
        (super:dispose())
    )


    (method (showSelf)
        (var theFirst, temp1, temp2, temp3, temp4)
        (if (saveCursor)
            (send gGame:setCursor(999))
        )
        (if (not dialog)
            = dialog (Dialog:new())
        )
        (send dialog:
            window(
                (if (window)
                )(else
                    gSq5Win
                )
)
            name("PODialog")
            caller(self)
        )
        (send dialog:
            text(title)
            time(ticks)
            setSize()
        )
        (send dialog:center())
        = temp3 
            (if (== x -1)
                (send dialog:nsLeft)
            )(else
                x
            )
        = temp4 
            (if (== y -1)
                (send dialog:nsTop)
            )(else
                y
            )
        (send dialog:moveTo(temp3 temp4))
        = temp1 GetPort()
        (send dialog:open(
            (if (title)
                4
            )(else
                0
            )
 			15))
        return 
            (if (modeless)
                = gOldPort GetPort()
                SetPort(temp1)
                = gDialog dialog
            )(else
                (send gSounds:pause(1))
                = theFirst first
                (if (not theFirst)
                    = theFirst (send dialog:firstTrue(#checkState 1))
                    (if (theFirst and not (send dialog:firstTrue(#checkState 2)))
                        (send theFirst:state((| (send theFirst:state) $0002)))
                    )
                )(else
                    (if (not IsObject(theFirst))
                        = theFirst (send dialog:at(theFirst))
                    )
                )
                = retValue (send dialog:doit(theFirst))
                SetPort(temp1)
                (if (== retValue -1)
                    = retValue 0
                )(else
                    (if (IsObject(retValue) and (send retValue:isKindOf(DButton)))
                        = retValue (send retValue:value)
                    )(else
                        (if (not (send dialog:theItem))
                            = retValue 1
                        )
                    )
                )
                (if (saveCursor)
                    (send gGame:setCursor((send ((send gSq5IconBar:curIcon)):cursor)))
                )
                (send dialog:dispose())
                return retValue
            )
    )


    (method (addButton param1 theTheGModNum)
        (var theTheTheGModNum, theTheTheGModNum_2, theTheTheGModNum_3, temp3, theTheTheGModNum_4, theTheTheGModNum_5, theGModNum, temp7, temp8)
        (if (not dialog)
            = dialog (Dialog:new())
        )
        (if (== font -1)
            = font gFont
        )
        (if (> paramTotal 4)
            = theTheTheGModNum theTheGModNum[0]
            = theTheTheGModNum_2 theTheGModNum[1]
            = theTheTheGModNum_3 theTheGModNum[2]
            = temp3 
                (if (theTheGModNum[3])
                    theTheGModNum[3]
                )(else
                    1
                )
            = theTheTheGModNum_4 0
            = theTheTheGModNum_5 0
            = theGModNum gModNum
            (if (> paramTotal 5)
                = theTheTheGModNum_4 theTheGModNum[4]
                (if (> paramTotal 6)
                    = theTheTheGModNum_5 theTheGModNum[5]
                    (if (> paramTotal 7)
                        = theGModNum theTheGModNum[6]
                    )
                )
            )
            = temp8 Message(msgSIZE theGModNum theTheTheGModNum theTheTheGModNum_2 theTheTheGModNum_3 temp3)
            (if (temp8)
                = temp7 Memory(memALLOC_CRIT (= temp8 Message(msgSIZE theGModNum theTheTheGModNum theTheTheGModNum_2 theTheTheGModNum_3 temp3)))
                (if (not Message(msgGET theGModNum theTheTheGModNum theTheTheGModNum_2 theTheTheGModNum_3 temp3 temp7))
                    = temp7 0
                )
            )
        )(else
            = theTheTheGModNum_4 0
            = theTheTheGModNum_5 0
            (if (> paramTotal 2)
                = theTheTheGModNum_4 theTheGModNum[1]
                (if (> paramTotal 3)
                    = theTheTheGModNum_5 theTheGModNum[2]
                )
            )
            = temp7 Memory(memALLOC_CRIT (+ StrLen(theTheGModNum[0]) 1))
            StrCpy(temp7 theTheGModNum[0])
        )
        (if (temp7)
            (send dialog:
                add((send ((DButton:new())):
                        value(param1)
                        font(font)
                        text(temp7)
                        setSize()
                        moveTo((+ 4 theTheTheGModNum_4) (+ 4 theTheTheGModNum_5))
                        yourself()
                    )
				   )
                setSize()
            )
        )
    )

    // addColorButton(theValue noun verb cond seq x y modNum)
    (method (addColorButton theValue theTheGModNum)
        (var theTheTheGModNum, theTheTheGModNum_2, theTheTheGModNum_3, temp3, theTheTheGModNum_4, theTheTheGModNum_5, theGModNum, temp7, temp8, theTheTheGModNum_6, theTheTheGModNum_9, theTheTheGModNum_7, theTheTheGModNum_10, theTheTheGModNum_8, theTheTheGModNum_11)
        (if (not dialog)
            = dialog (Dialog:new())
        )
        (if (== font -1)
            = font gFont
        )
        = theTheTheGModNum_6 0
        = theTheTheGModNum_7 15
        = theTheTheGModNum_8 31
        = theTheTheGModNum_9 5
        = theTheTheGModNum_10 5
        = theTheTheGModNum_11 5
        (if (< Abs(theTheGModNum[0]) 1000)
            = theTheTheGModNum theTheGModNum[0]
            = theTheTheGModNum_2 theTheGModNum[1]
            = theTheTheGModNum_3 theTheGModNum[2]
            = temp3 
                (if (theTheGModNum[3])
                    theTheGModNum[3]
                )(else
                    1
                )
            = theTheTheGModNum_4 0
            = theTheTheGModNum_5 0
            = theGModNum gModNum
            (if (> paramTotal 5)
                = theTheTheGModNum_4 theTheGModNum[4]
                (if (> paramTotal 6)
                    = theTheTheGModNum_5 theTheGModNum[5]
                    (if (> paramTotal 7)
                        = theGModNum theTheGModNum[6]
                        (if (> paramTotal 8)
                            = theTheTheGModNum_6 theTheGModNum[7]
                            (if (> paramTotal 9)
                                = theTheTheGModNum_7 theTheGModNum[8]
                                (if (> paramTotal 10)
                                    = theTheTheGModNum_8 theTheGModNum[9]
                                    (if (> paramTotal 11)
                                        = theTheTheGModNum_9 theTheGModNum[10]
                                        (if (> paramTotal 12)
                                            = theTheTheGModNum_10 theTheGModNum[11]
                                            (if (> paramTotal 13)
                                                = theTheTheGModNum_11 theTheGModNum[12]
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    )
                )
            )
            = temp8 Message(msgSIZE theGModNum theTheTheGModNum theTheTheGModNum_2 theTheTheGModNum_3 temp3)
            (if (temp8)
                = temp7 Memory(memALLOC_CRIT (= temp8 Message(msgSIZE theGModNum theTheTheGModNum theTheTheGModNum_2 theTheTheGModNum_3 temp3)))
                (if (not Message(msgGET theGModNum theTheTheGModNum theTheTheGModNum_2 theTheTheGModNum_3 temp3 temp7))
                    = temp7 0
                )
            )
        )(else
            = theTheTheGModNum_4 0
            = theTheTheGModNum_5 0
            (if (> paramTotal 2)
                = theTheTheGModNum_4 theTheGModNum[1]
                (if (> paramTotal 3)
                    = theTheTheGModNum_5 theTheGModNum[2]
                    (if (> paramTotal 4)
                        = theTheTheGModNum_6 theTheGModNum[3]
                        (if (> paramTotal 5)
                            = theTheTheGModNum_7 theTheGModNum[4]
                            (if (> paramTotal 6)
                                = theTheTheGModNum_8 theTheGModNum[5]
                                (if (> paramTotal 7)
                                    = theTheTheGModNum_9 theTheGModNum[6]
                                    (if (> paramTotal 8)
                                        = theTheTheGModNum_10 theTheGModNum[7]
                                        (if (> paramTotal 9)
                                            = theTheTheGModNum_11 theTheGModNum[8]
                                        )
                                    )
                                )
                            )
                        )
                    )
                )
            )
            = temp7 Memory(memALLOC_CRIT (+ StrLen(theTheGModNum[0]) 1))
            StrCpy(temp7 theTheGModNum[0])
        )
        (if (temp7)
            (send dialog:
                add((send ((DColorButton:new())):
                        value(theValue)
                        font(font)
                        text(temp7)
                        mode(mode)
                        nfc(theTheTheGModNum_6)
                        nbc(theTheTheGModNum_9)
                        sfc(theTheTheGModNum_8)
                        sbc(theTheTheGModNum_11)
                        hfc(theTheTheGModNum_7)
                        hbc(theTheTheGModNum_10)
                        setSize(width)
                        moveTo(+ 4 theTheTheGModNum_4 + 4 theTheTheGModNum_5)
                        yourself()
                    )
)
                setSize()
            )
        )
    )


    (method (addEdit param1 param2 param3 param4 param5)
        (var temp0, temp1)
        (if (not dialog)
            = dialog (Dialog:new())
        )
        StrCpy(param1 
            (if (> paramTotal 4)
                param5
            )(else
                ""
            )
)
        (if (> paramTotal 2)
            = temp0 param3
            (if (> paramTotal 3)
                = temp1 param4
            )
        )
        (send dialog:
            add((send ((DEdit:new())):
                    text(param1)
                    max(param2)
                    setSize()
                    moveTo(+ temp0 4 + temp1 4)
                    yourself()
                )
)
            setSize()
        )
    )


    (method (addIcon param1 param2 param3 param4 param5)
        (var temp0, temp1)
        (if (not dialog)
            = dialog (Dialog:new())
        )
        (if (> paramTotal 3)
            = temp0 param4
            = temp1 param5
        )(else
            = temp1 0
            = temp0 temp1
        )
        (if (IsObject(param1))
            (send dialog:
                add((send param1:
                        setSize()
                        moveTo(+ temp0 4 + temp1 4)
                        yourself()
                    )
)
                setSize()
            )
        )(else
            (send dialog:
                add((send ((DIcon:new())):
                        view(param1)
                        loop(param2)
                        cel(param3)
                        setSize()
                        moveTo(+ temp0 4 + temp1 4)
                        yourself()
                    )
)
                setSize()
            )
        )
    )


    (method (addText theTheGModNum)
        (var theTheTheGModNum, theTheTheGModNum_2, theTheTheGModNum_3, temp3, theTheTheGModNum_4, theTheTheGModNum_5, theGModNum, temp7, temp8)
        (if (not dialog)
            = dialog (Dialog:new())
        )
        (if (== font -1)
            = font gFont
        )
        (if (> paramTotal 3)
            = theTheTheGModNum theTheGModNum[0]
            = theTheTheGModNum_2 theTheGModNum[1]
            = theTheTheGModNum_3 theTheGModNum[2]
            = temp3 
                (if (theTheGModNum[3])
                    theTheGModNum[3]
                )(else
                    1
                )
            = theTheTheGModNum_4 0
            = theTheTheGModNum_5 0
            = theGModNum gModNum
            (if (>= paramTotal 5)
                = theTheTheGModNum_4 theTheGModNum[4]
                (if (>= paramTotal 6)
                    = theTheTheGModNum_5 theTheGModNum[5]
                    (if (>= paramTotal 7)
                        = theGModNum theTheGModNum[6]
                    )
                )
            )
            = temp8 Message(msgSIZE theGModNum theTheTheGModNum theTheTheGModNum_2 theTheTheGModNum_3 temp3)
            (if (temp8)
                = temp7 Memory(memALLOC_CRIT (= temp8 Message(msgSIZE theGModNum theTheTheGModNum theTheTheGModNum_2 theTheTheGModNum_3 temp3)))
                (if (Message(msgGET theGModNum theTheTheGModNum theTheTheGModNum_2 theTheTheGModNum_3 temp3 temp7))
                    (send dialog:
                        add((send ((DText:new())):
                                text(temp7)
                                font(font)
                                mode(mode)
                                setSize(width)
                                moveTo(+ 4 theTheTheGModNum_4 + 4 theTheTheGModNum_5)
                                yourself()
                            )
)
                        setSize()
                    )
                )
            )
        )(else
            = theTheTheGModNum_4 0
            = theTheTheGModNum_5 0
            (if (>= paramTotal 2)
                = theTheTheGModNum_4 theTheGModNum[1]
                (if (>= paramTotal 3)
                    = theTheTheGModNum_5 theTheGModNum[2]
                )
            )
            = temp7 Memory(memALLOC_CRIT (+ StrLen(theTheGModNum[0]) 1))
            StrCpy(temp7 theTheGModNum[0])
            (send dialog:
                add((send ((DText:new())):
                        text(temp7)
                        font(font)
                        mode(mode)
                        setSize(width)
                        moveTo(+ 4 theTheTheGModNum_4 + 4 theTheTheGModNum_5)
                        yourself()
                    )
)
                setSize()
            )
        )
    )


    (method (addTextF param1)
        (var temp0, temp1)
        = temp0 GetTotalLength(rest param1)
        = temp1 Memory(memALLOC_CRIT temp0)
        Format(temp1 rest param1)
        (self:addText(temp1))
        Memory(memFREE temp1)
    )


    (method (addTitle param1)
        (var temp0, temp1, temp2, temp3, temp4, temp5)
        (if (> paramTotal 1)
            = temp0 param1[0]
            = temp1 param1[1]
            = temp2 param1[2]
            = temp3 param1[3]
            = temp4 param1[4]
            = temp5 Message(msgSIZE temp4 temp0 temp1 temp2 temp3)
            (if (temp5)
                = title Memory(memALLOC_CRIT (= temp5 Message(msgSIZE temp4 temp0 temp1 temp2 temp3)))
                Message(msgGET temp4 temp0 temp1 temp2 temp3 title)
            )
        )(else
            = title Memory(memALLOC_CRIT (+ StrLen(param1[0]) 1))
            StrCpy(title param1[0])
        )
    )


    (method (posn theX theY)
        = x theX
        = y theY
    )


    (method (handleEvent pEvent)
        (if ((send dialog:handleEvent(pEvent)))
            (send dialog:dispose())
        )
    )


    (method (cue)
        (var theCaller)
        = theCaller caller
        = dialog 0
        (if (window)
            (send window:dispose())
        )
        (self:dispose())
        (if (theCaller)
            (send theCaller:cue())
        )
    )

)
