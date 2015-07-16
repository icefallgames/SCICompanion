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

(procedure public (TextPrint theText)
    (Print:
        addText(rest theText)
        init()
    )
)

(procedure public (FormatPrint theText)
    (Print:
        addTextF(rest theText)
        init()
    )
)

(procedure public (EditPrint theText theMax theMessage theFont)
    (if (
        (Print:
            font(
                (if (> paramTotal 3)
                    theFont
                )(else
                    gFont
                )
            )
            addText(
                (if ((> paramTotal 2) and theMessage)
                    theMessage
                )(else
                    ""
                )
            )
            addEdit(theText theMax 0 12 theText)
            init()
            )
        )
        StrLen(theText)
    )
)

(procedure public (GetTotalLength theText param2)
    (var temp0, temp1, temp2, temp3)
    = temp1 StrLen(theText)
    = temp0 temp1
    = temp2 0
    = temp3 0
    (while (< temp3 temp1)
        (if (== StrAt(theText temp3) 37)	// %
            (switch (StrAt(theText ++temp3))
                (case KEY_d
                    = temp0 (+ temp0 5)
                )
                (case KEY_x
                    = temp0 (+ temp0 4)
                )
                (case KEY_s
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

    (method (init theCaller theText)
        = caller 0
        (if (paramTotal)
            = caller theCaller
        )
        (if (> paramTotal 1)
            (self:addText(rest theText))
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
        (send dialog:open( (if (title) nwTITLE)(else nwNORMAL) 15))
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

    // addButton (theValue noun verb cond seq x y modNum)
    // addButton (theValue theText x y)
    (method (addButton theValue params)
        (var noun, verb, cond, seq, x, y, modNum, text, temp8)
        (if (not dialog)
            = dialog (Dialog:new())
        )
        (if (== font -1)
            = font gFont
        )
        (if (> paramTotal 4)
            = noun params[0]
            = verb params[1]
            = cond params[2]
            = seq 
                (if (params[3])
                    params[3]
                )(else
                    1
                )
            = x 0
            = y 0
            = modNum gModNum
            (if (> paramTotal 5)
                = x params[4]
                (if (> paramTotal 6)
                    = y params[5]
                    (if (> paramTotal 7)
                        = modNum params[6]
                    )
                )
            )
            = temp8 Message(msgSIZE modNum noun verb cond seq)
            (if (temp8)
                = text Memory(memALLOC_CRIT (= temp8 Message(msgSIZE modNum noun verb cond seq)))
                (if (not Message(msgGET modNum noun verb cond seq text))
                    = text 0
                )
            )
        )(else
            = x 0
            = y 0
            (if (> paramTotal 2)
                = x params[1]
                (if (> paramTotal 3)
                    = y params[2]
                )
            )
            = text Memory(memALLOC_CRIT (+ StrLen(params[0]) 1))
            StrCpy(text params[0])
        )
        (if (text)
            (send dialog:
                add((send ((DButton:new())):
                        value(theValue)
                        font(font)
                        text(text)
                        setSize()
                        moveTo((+ 4 x) (+ 4 y))
                        yourself()
                    )
                     )
                setSize()
            )
        )
    )

    // addColorButton(theValue noun verb cond seq x y modNum)
    // addColorButton(theValue noun verb cond seq x y modNum normalForeColor highlightForeColor selectedForeColor normalBackColor highlightBackColor selectedBackColor)
    (method (addColorButton theValue params)
        (var noun, verb, cond, seq, x, y, modNum, temp7, temp8, normalForeColor, normalBackColor, highlightForeColor, highlightBackColor, selectedForeColor, selectedBackColor)
        (if (not dialog)
            = dialog (Dialog:new())
        )
        (if (== font -1)
            = font gFont
        )
        = normalForeColor 0
        = highlightForeColor 15
        = selectedForeColor 31
        = normalBackColor 5
        = highlightBackColor 5
        = selectedBackColor 5
        (if (< Abs(params[0]) 1000)
            = noun params[0]
            = verb params[1]
            = cond params[2]
            = seq 
                (if (params[3])
                    params[3]
                )(else
                    1
                )
            = x 0
            = y 0
            = modNum gModNum
            (if (> paramTotal 5)
                = x params[4]
                (if (> paramTotal 6)
                    = y params[5]
                    (if (> paramTotal 7)
                        = modNum params[6]
                        (if (> paramTotal 8)
                            = normalForeColor params[7]
                            (if (> paramTotal 9)
                                = highlightForeColor params[8]
                                (if (> paramTotal 10)
                                    = selectedForeColor params[9]
                                    (if (> paramTotal 11)
                                        = normalBackColor params[10]
                                        (if (> paramTotal 12)
                                            = highlightBackColor params[11]
                                            (if (> paramTotal 13)
                                                = selectedBackColor params[12]
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    )
                )
            )
            = temp8 Message(msgSIZE modNum noun verb cond seq)
            (if (temp8)
                = temp7 Memory(memALLOC_CRIT (= temp8 Message(msgSIZE modNum noun verb cond seq)))
                (if (not Message(msgGET modNum noun verb cond seq temp7))
                    = temp7 0
                )
            )
        )(else
            = x 0
            = y 0
            (if (> paramTotal 2)
                = x params[1]
                (if (> paramTotal 3)
                    = y params[2]
                    (if (> paramTotal 4)
                        = normalForeColor params[3]
                        (if (> paramTotal 5)
                            = highlightForeColor params[4]
                            (if (> paramTotal 6)
                                = selectedForeColor params[5]
                                (if (> paramTotal 7)
                                    = normalBackColor params[6]
                                    (if (> paramTotal 8)
                                        = highlightBackColor params[7]
                                        (if (> paramTotal 9)
                                            = selectedBackColor params[8]
                                        )
                                    )
                                )
                            )
                        )
                    )
                )
            )
            = temp7 Memory(memALLOC_CRIT (+ StrLen(params[0]) 1))
            StrCpy(temp7 params[0])
        )
        (if (temp7)
            (send dialog:
                add((send ((DColorButton:new())):
                        value(theValue)
                        font(font)
                        text(temp7)
                        mode(mode)
                        nfc(normalForeColor)
                        nbc(normalBackColor)
                        sfc(selectedForeColor)
                        sbc(selectedBackColor)
                        hfc(highlightForeColor)
                        hbc(highlightBackColor)
                        setSize(width)
                        moveTo((+ 4 x) (+ 4 y))
                        yourself()
                    )
                )
                setSize()
            )
        )
    )

	// addEdit(theBuffer maxLength [x] [y] [initialText])
    (method (addEdit theBuffer maxLength x y initialText)
        (var theX, theY)
        (if (not dialog)
            = dialog (Dialog:new())
        )
        StrCpy(theBuffer 
            (if (> paramTotal 4)
                initialText
            )(else
                ""
            )
        )
        (if (> paramTotal 2)
            = theX x
            (if (> paramTotal 3)
                = theY y
            )
        )
        (send dialog:
            add((send ((DEdit:new())):
                    text(theBuffer)
                    max(maxLength)
                    setSize()
                    moveTo((+ theX 4) (+ theY 4))
                    yourself()
                )
            )
            setSize()
        )
    )

    (method (addIcon theView theLoop theCel theX theY)
        (var temp0, temp1)
        (if (not dialog)
            = dialog (Dialog:new())
        )
        (if (> paramTotal 3)
            = temp0 theX
            = temp1 theY
        )(else
            = temp1 0
            = temp0 temp1
        )
        (if (IsObject(theView))
            (send dialog:
                add((send theView:
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
                        view(theView)
                        loop(theLoop)
                        cel(theCel)
                        setSize()
                        moveTo(+ temp0 4 + temp1 4)
                        yourself()
                    )
                )
                setSize()
            )
        )
    )

    // addText (noun verb cond seq x y modNum)
    // addText (theText x y)
    (method (addText params)
        (var noun, verb, cond, seq, x, y, modNum, text, temp8)
        (if (not dialog)
            = dialog (Dialog:new())
        )
        (if (== font -1)
            = font gFont
        )
        (if (> paramTotal 3)
            = noun params[0]
            = verb params[1]
            = cond params[2]
            = seq 
                (if (params[3])
                    params[3]
                )(else
                    1
                )
            = x 0
            = y 0
            = modNum gModNum
            (if (>= paramTotal 5)
                = x params[4]
                (if (>= paramTotal 6)
                    = y params[5]
                    (if (>= paramTotal 7)
                        = modNum params[6]
                    )
                )
            )
            = temp8 Message(msgSIZE modNum noun verb cond seq)
            (if (temp8)
                = text Memory(memALLOC_CRIT (= temp8 Message(msgSIZE modNum noun verb cond seq)))
                (if (Message(msgGET modNum noun verb cond seq text))
                    (send dialog:
                        add((send ((DText:new())):
                                text(text)
                                font(font)
                                mode(mode)
                                setSize(width)
                                moveTo(+ 4 x + 4 y)
                                yourself()
                            )
                        )
                        setSize()
                    )
                )
            )
        )(else
            = x 0
            = y 0
            (if (>= paramTotal 2)
                = x params[1]
                (if (>= paramTotal 3)
                    = y params[2]
                )
            )
            = text Memory(memALLOC_CRIT (+ StrLen(params[0]) 1))
            StrCpy(text params[0])
            (send dialog:
                add((send ((DText:new())):
                        text(text)
                        font(font)
                        mode(mode)
                        setSize(width)
                        moveTo(+ 4 x + 4 y)
                        yourself()
                    )
                )
                setSize()
            )
        )
    )

    (method (addTextF params)
        (var temp0, temp1)
        = temp0 GetTotalLength(rest params)
        = temp1 Memory(memALLOC_CRIT temp0)
        Format(temp1 rest params)
        (self:addText(temp1))
        Memory(memFREE temp1)
    )

    // addTitle (noun verb cond seq modNum)
    // addTitle (theText)
    (method (addTitle params)
        (var noun, verb, cond, seq, modNum, temp5)
        (if (> paramTotal 1)
            = noun params[0]
            = verb params[1]
            = cond params[2]
            = seq params[3]
            = modNum params[4]
            = temp5 Message(msgSIZE modNum noun verb cond seq)
            (if (temp5)
                = title Memory(memALLOC_CRIT (= temp5 Message(msgSIZE modNum noun verb cond seq)))
                Message(msgGET modNum noun verb cond seq title)
            )
        )(else
            = title Memory(memALLOC_CRIT (+ StrLen(params[0]) 1))
            StrCpy(title params[0])
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
