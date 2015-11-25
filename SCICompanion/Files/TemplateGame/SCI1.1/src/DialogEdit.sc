/*
	This script contains Sierra's dialog editing functionality and its related classes.
	It can be invoked by entering debug
	mode (ALT-d) then pressing ALT-d again.
*/
(version 2)
(include "sci.sh")
(include "game.sh")
(use "Main")
(use "Controls")
(use "Print")
(use "PolygonEdit")
(use "SysWindow")
(use "File")
(use "Object")
(script DIALOGEDIT_SCRIPT)

(local
    theWindow
    local1
    local2[40]
    local42[20]
    local62[28] = ("DIALOG" 0 0 "About" 0 0 "Item" 0 0 "Edit" 0 0 "Look" 0 0 "Del" 0 0 "Win" 0 0 "Help" 0 0 "eXit" 120 )
    local90[22] = ("EDITING" 0 0 "Text" 0 0 "Font" 0 0 "Value" 0 0 "Position" 0 0 "Help" 0 0 "Menu" )
    local112[25] = ("EDITING" 0 0 "Text" 0 0 "Font" 0 0 "Just" 0 0 "Width" 0 0 "Position" 0 0 "Help" 0 0 "Menu" )
    local137[22] = ("EDITING" 0 0 "Font" 0 0 "Length" 0 0 "Position" 0 0 "Text" 0 0 "Help" 0 0 "Menu" )
    local159[22] = ("EDITING" 0 0 "View" 0 0 "Loop" 0 0 "Cel" 0 0 "Position" 0 0 "Help" 0 0 "Menu" )
    local181[19] = ("EDITING" 0 0 "Width" 0 0 "Length" 0 0 "Position" 0 0 "Help" 0 0 "Menu" )
    local200[22] = ("WINDOW" 0 0 "Create" 0 0 "Position" 0 0 "Delete" 0 0 "Title" 0 0 "Help" 0 0 "Menu" )

)
(procedure (localproc_229e param1 param2)
    (if (> StrLen(param1) param2)
        StrAt(param1 param2 0)
        StrAt(param1 --param2 46)
        StrAt(param1 --param2 46)
        StrAt(param1 --param2 46)
    )
    return param1
)


(procedure (localproc_22dc param1)
    (var temp0, temp1, temp2, temp3, temp4, temp5[10], temp15[10], temp25[10], temp35[10], temp45[12])
    = temp0 (send param1:noun)
    = temp1 (send param1:verb)
    = temp2 (send param1:case)
    = temp3 (send param1:seq)
    = temp4 (send param1:modNum)
    Format(@temp5 "%d" temp0)
    Format(@temp15 "%d" temp1)
    Format(@temp25 "%d" temp2)
    Format(@temp35 "%d" temp3)
    Format(@temp45 "%d" temp4)
    return 
        (if ((Print:
            addTitle(@local42)
            font(0)
            addText("Enter new message parameters:")
            addText("Noun" 5 25)
            addText("Verb" 85 25)
            addText("Case" 5 39)
            addText("Seq" 85 39)
            addText("Module" 47 53)
            addEdit(@temp5 4 45 25 @temp5)
            addEdit(@temp15 4 125 25 @temp15)
            addEdit(@temp25 4 45 39 @temp25)
            addEdit(@temp35 4 125 39 @temp35)
            addEdit(@temp45 5 101 53 @temp45)
            addButton(1 "   OK   " 18 67)
            addButton(0 "Cancel" 91 67)
            init()
        )
)
            = temp0 ReadNumber(@temp5)
            = temp1 ReadNumber(@temp15)
            = temp2 ReadNumber(@temp25)
            = temp3 ReadNumber(@temp35)
            = temp4 ReadNumber(@temp45)
            (if (not Message(msgGET temp4 temp0 temp1 temp2 temp3))
                Prints("Can't find message!")
                return 0
            )(else
                (if (not Message(msgSIZE temp4 temp0 temp1 temp2 temp3))
                    Prints("Message contains no text!")
                    return 0
                )(else
                    (send param1:
                        noun(temp0)
                        verb(temp1)
                        case(temp2)
                        seq(temp3)
                        modNum(temp4)
                    )
                    return 1
                )
            )
        )(else
            return 0
        )
)


(procedure (localproc_24db param1)
    (Print:
        addTitle(@local42)
        font(0)
        width(50)
        addText("What kind of text?")
        addButton(1 " Literal " 60 0)
        addButton(2 "MSG file" 60 14)
        addButton(0 "  Cancel  " 60 28)
        first(
            (if ((send param1:seq))
                2
            )(else
                1
            )
)
        init()
    )
)


(instance mainMenu of ClickMenu
    (properties)

    (method (init)
        (super:init(@local62))
    )

)
(instance editBMenu of ClickMenu
    (properties)

    (method (init)
        (super:init(@local90))
    )

)
(instance editTMenu of ClickMenu
    (properties)

    (method (init)
        (super:init(@local112))
    )

)
(instance editEMenu of ClickMenu
    (properties)

    (method (init)
        (super:init(@local137))
    )

)
(instance editIMenu of ClickMenu
    (properties)

    (method (init)
        (super:init(@local159))
    )

)
(instance editSMenu of ClickMenu
    (properties)

    (method (init)
        (super:init(@local181))
    )

)
(instance editWMenu of ClickMenu
    (properties)

    (method (init)
        (super:init(@local200))
    )
)

// Nodoc
(class DlgWindow of SysWindow
    (properties
        top 0
        left 0
        bottom 0
        right 0
        color 0
        back 15
        priority 15
        window 0
        type $0000
        title 0
        brTop 0
        brLeft 0
        brBottom 190
        brRight 320
        lsTop 0
        lsLeft 0
        lsBottom 0
        lsRight 0
        eraseOnly 0
        noun 0
        verb 0
        cond 0
        seq 0
        modNum 0
    )

    (method (dispose)
        (super:dispose())
        = local1 0
    )


    (method (open param1)
        (var temp0)
        = type 
            (if (title)
                4
            )(else
                0
            )
        = temp0 GetPort()
        (super:open(rest param1))
        SetPort(temp0)
        = local1 1
    )


    (method (create)
        (var temp0, temp1, temp2, temp3, temp4, temp5)
        = temp0 190
        = temp1 320
        = temp2 0
        = temp3 0
        = temp4 0
        (while (< temp4 (DialogEditor:size))
            = temp5 (DialogEditor:at(temp4))
            = temp0 Min((send temp5:nsTop) temp0)
            = temp1 Min((send temp5:nsLeft) temp1)
            = temp2 Max((send temp5:nsBottom) temp2)
            = temp3 Max((send temp5:nsRight) temp3)
            ++temp4
        )
        (DialogEditor:eachElementDo(#hide))
        (self:dispose())
        = top (- temp0 4)
        = bottom (+ temp2 4)
        = left (- temp1 4)
        = right (+ temp3 4)
        (self:open())
        (DialogEditor:eachElementDo(#draw))
    )


    (method (moveTo theLeft theTop)
        (var temp0, temp1, temp2, temp3)
        = temp0 0
        (while (< temp0 (DialogEditor:size))
            = temp1 (DialogEditor:at(temp0))
            = temp2 (- (send temp1:nsLeft) left)
            = temp3 (- (send temp1:nsTop) top)
            (send temp1:
                hide()
                moveTo(+ theLeft temp2 + theTop temp3)
            )
            ++temp0
        )
        = left theLeft
        = top theTop
        (self:create())
    )


    (method (editMsg)
        (if (localproc_22dc(self))
            (if (title)
                Memory(memFREE title)
            )
            = title Memory(memALLOC_CRIT Message(msgSIZE modNum noun verb cond seq))
            Message(msgGET modNum noun verb cond seq title)
        )
        (self:create())
    )


    (method (editPosn)
        (var newEvent, newEventX, temp2, temp3, temp4)
        (if (local1)
            Prints("Click to where the top left of the window should be")
            (while (<> (send ((= newEvent (Event:new()))):type) 1)
                (send newEvent:dispose())
            )
            = newEventX (send newEvent:x)
            = temp2 (- (send newEvent:y) 10)
            (send newEvent:dispose())
            = temp3 (- bottom top)
            = temp4 (- right left)
            = newEventX Max(0 Min(newEventX (- 320 temp4)))
            = temp2 Max(0 Min(temp2 (- 190 temp3)))
            (self:moveTo(newEventX temp2))
        )(else
            Prints("No window to position!")
        )
    )


    (method (editTitle)
        (switch (localproc_24db(self))
            (case 0
                return 
            )
            (case 2
                (self:editMsg())
                return 
            )
        )
        (if (seq)
            Memory(memFREE title)
            = noun (= verb (= cond (= seq (= modNum (= title 0)))))
        )
        (if (not title)
            = title Memory(memALLOC_CRIT 50)
            StrCpy(title "title")
        )
        (Print:
            addTitle(@local42)
            addText("Enter new title:")
            addEdit(title 50 0 12 title)
            init()
        )
        (self:create())
    )
)

// Nodoc
(class _DItem of Control
    (properties
        type $0000
        state $0000
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        key 0
        said 0
        value 0
        underBits 0
    )

    (method (dispose param1)
        (self:hide())
        (super:dispose(rest param1))
    )


    (method (select param1)
        (self:hide())
        (if (param1)
            = state (| state csSELECTED)
        )(else
            = state (& state (bnot csSELECTED))
        )
        (self:draw())
    )


    (method (handleEvent pEvent)
        (var temp0)
        = temp0 0
        (send pEvent:claimed(TRUE))
        = temp0 (self:track(pEvent))
    )


    (method (track param1)
        (var temp0, temp1, temp2[40])
        (if (== (send param1:type) 1)
            (self:hide())
            (do 
                = param1 (Event:new(-32768))
                (send param1:localize())
                = temp0 (send param1:x)
                = temp1 (send param1:y)
                DrawStatus(Format(@temp2 "DRAGGING: %d, %d" temp0 temp1))
                (self:moveTo(temp0 temp1))
                (send param1:dispose())
            ) while (not not MouseStillDown())
            DrawStatus(" " 0 0)
            DrawStatus(0)
            (if ((DialogEditor:curMenu))
                (send ((DialogEditor:curMenu)):init())
            )
            DrawPic((send gRoom:picture) 100)
            (if (local1)
                (DlgWindow:create())
            )(else
                (DialogEditor:eachElementDo(#draw))
            )
        )
    )


    (method (draw)
        (var temp0, temp1, temp2, temp3)
        = temp0 (- nsTop 1)
        = temp1 (- nsLeft 1)
        = temp2 (+ nsBottom 1)
        = temp3 (+ nsRight 1)
        (if (underBits)
            UnLoad(133 underBits)
            = underBits 0
        )
        = underBits Graph(grSAVE_BOX temp0 temp1 temp2 temp3 1)
        DrawControl(self)
    )


    (method (hide)
        (var temp0, temp1, temp2, temp3)
        (if (underBits)
            Graph(grRESTORE_BOX underBits)
            = temp0 (- nsTop 1)
            = temp1 (- nsLeft 1)
            = temp2 (+ nsBottom 1)
            = temp3 (+ nsRight 1)
            = underBits 0
            Graph(grREDRAW_BOX temp0 temp1 temp2 temp3)
        )
    )


    (method (editPosn)
        (var temp0[25], temp25[5], temp30, temp31, temp32)
        Format(@temp0 "%d" nsLeft)
        Format(@temp25 "%d" nsTop)
        = temp30 (Print:
                addTitle(@local42)
                addText("Enter new position:")
                addText("x = " 0 12)
                addText("y = " 65 12)
                addEdit(@temp0 5 25 12 @temp0)
                addEdit(@temp25 6 90 12 @temp25)
                font(0)
                addButton(0 " Cancel " 35 26)
                init()
            )
        (if (temp30)
            = temp31 Max(4 ReadNumber(@temp0))
            = temp32 Max(4 ReadNumber(@temp25))
            (self:
                hide()
                moveTo(temp31 temp32)
                draw()
            )
        )
    )
)

// Nodoc
(class _DText of _DItem
    (properties
        type $0002
        state $0000
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        key 0
        said 0
        value 0
        underBits 0
        text 0
        font 0
        mode 0
        width 0
        noun 0
        verb 0
        cond 0
        seq 0
        modNum 0
    )

    (method (setSize param1)
        (var temp0[4])
        TextSize(@temp0[0] text font 
            (if (paramTotal)
                param1
            )(else
                width
            )
)
        --nsLeft
        --nsTop
        = nsBottom (+ (+ nsTop temp0[2]) 1)
        = nsRight (+ (+ nsLeft temp0[3]) 1)
    )


    (method (showHelp)
        (Print:
            font(gFont)
            width(250)
            addText("Text Menu:\n\n  Text - Change the text\n  Font - Change the font of the text\n  Just - Change justification mode\n  Position - Change the position of the text\n  Menu - Return to the Main Menu\n")
            init()
        )
    )


    (method (editFont)
        (var temp0[25], temp25)
        = temp25 (Print:
                addTitle(@local42)
                font(0)
                width(90)
                addText("Enter new font number:")
                addEdit(@temp0 6 0 24)
                addButton(0 " System " 100 0)
                addButton(gFont "  User  " 100 14)
                addButton(gSmallFont "  Small  " 100 28)
                addButton(gBigFont "   Big   " 100 42)
                addButton(-1 " Cancel " 100 56)
                init()
            )
        (if (temp0)
            = temp25 ReadNumber(@temp0)
        )
        (if (<> temp25 -1)
            (self:
                hide()
                font(temp25)
                setSize()
                draw()
            )
        )
    )


    (method (editJust)
        (var temp0[25], temp25)
        = temp0 0
        = temp25 (Print:
                addTitle(@local42)
                font(0)
                width(100)
                addText("Enter justification mode:")
                addButton(1 "   Left   " 100 0)
                addButton(2 " Center " 100 14)
                addButton(3 "  Right  " 100 28)
                addButton(0 " Cancel " 100 42)
                init()
            )
        (if (temp0)
            = temp25 ReadNumber(@temp0)
        )
        (if (temp25)
            (self:
                hide()
                mode(
                    (switch (temp25)
                        (case 1
                            0
                        )
                        (case 2
                            1
                        )
                        (case 3
                            -1
                        )
                    )
)
                setSize()
                draw()
            )
        )
    )


    (method (editMsg)
        (if (localproc_22dc(self))
            Memory(memFREE text)
            = text Memory(memALLOC_CRIT Message(msgSIZE modNum noun verb cond seq))
            Message(msgGET modNum noun verb cond seq text)
        )
        (self:
            hide()
            setSize()
            draw()
        )
    )


    (method (editText)
        (switch (localproc_24db(self))
            (case 0
                return 
            )
            (case 2
                (self:editMsg())
                return 
            )
        )
        (if (seq)
            Memory(memFREE text)
            = text Memory(memALLOC_CRIT 100)
            StrCpy(text "text")
            = noun (= verb (= cond (= seq (= modNum 0))))
        )
        (Print:
            addTitle(@local42)
            addText("Enter new text:")
            addEdit(text 50 0 12 text)
            init()
        )
        (self:
            hide()
            setSize()
            draw()
        )
    )


    (method (editWidth)
        (var temp0[25], temp25)
        Format(@temp0 "%d" width)
        = temp25 (Print:
                addTitle(@local42)
                addText("Enter new width:")
                addEdit(@temp0 6 0 12 @temp0)
                font(0)
                addButton(0 " Cancel " 0 26)
                init()
            )
        (if (temp0)
            = temp25 ReadNumber(@temp0)
        )
        (if (temp25)
            (self:
                hide()
                width(temp25)
                setSize()
                draw()
            )
        )
    )
)

// Nodoc
(class _DIcon of _DItem
    (properties
        type $0004
        state $0000
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        key 0
        said 0
        value 0
        underBits 0
        view 0
        loop 0
        cel 0
    )

    (method (setSize)
        = nsRight (+ nsLeft CelWide(view loop cel))
        = nsBottom (+ nsTop CelHigh(view loop cel))
    )


    (method (showHelp)
        (Print:
            font(gFont)
            width(250)
            addText("Icon Menu:\n\n  View - Change the view of the icon\n  Loop - Change the loop of the icon\n  Cel - Change the cel of the icon\n  Position - Change the position of the icon\n  Menu - Return to the Main Menu\n")
            init()
        )
    )


    (method (editView)
        (var temp0[25], theView)
        Format(@temp0 "%d" view)
        = theView (Print:
                addTitle(@local42)
                addText("Enter new view number:")
                addEdit(@temp0 5 0 12 @temp0)
                font(0)
                addButton(0 " Cancel " 0 26)
                init()
            )
        (if (temp0)
            = theView ReadNumber(@temp0)
        )
        (if (theView)
            = view theView
        )
        (self:
            hide()
            setSize()
            draw()
        )
    )


    (method (editLoop)
        (var temp0[25], theLoop)
        Format(@temp0 "%d" loop)
        = theLoop (Print:
                addTitle(@local42)
                addText("Enter new loop number:")
                addEdit(@temp0 5 0 12 @temp0)
                font(0)
                addButton(0 " Cancel " 0 26)
                init()
            )
        (if (temp0)
            = theLoop ReadNumber(@temp0)
        )
        (if (theLoop)
            = loop theLoop
        )
        (self:
            hide()
            setSize()
            draw()
        )
    )


    (method (editCel)
        (var temp0[25], theCel)
        Format(@temp0 "%d" cel)
        = theCel (Print:
                addTitle(@local42)
                addText("Enter new cel number:")
                addEdit(@temp0 5 0 12 @temp0)
                font(0)
                addButton(0 " Cancel " 0 26)
                init()
            )
        (if (temp0)
            = theCel ReadNumber(@temp0)
        )
        (if (theCel)
            = cel theCel
        )
        (self:
            hide()
            setSize()
            draw()
        )
    )
)

// Nodoc
(class _DButton of _DItem
    (properties
        type $0001
        state $0001
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        key 0
        said 0
        value 0
        underBits 0
        text 0
        font 0
        noun 0
        verb 0
        cond 0
        seq 0
        modNum 0
    )

    (method (setSize)
        (var temp0[4])
        TextSize(@temp0[0] text font 0 0)
        = temp0[2] (+ temp0[2] 2)
        = temp0[3] (+ temp0[3] 2)
        = nsBottom (+ nsTop temp0[2])
        = temp0[3] (* (/ (+ temp0[3] 15) 16) 16)
        = nsRight (+ temp0[3] nsLeft)
    )


    (method (showHelp)
        (Print:
            font(gFont)
            width(250)
            addText("Button Menu:\n\n  Text - Change the button text\n  Font - Change the font of the button text\n  Value - Change the return value of the button\n  Position - Change the position of the button\n  Menu - Return to the Main Menu\n")
            init()
        )
    )


    (method (editFont)
        (var temp0[25], temp25)
        = temp25 (Print:
                addTitle(@local42)
                font(0)
                width(90)
                addText("Enter new font number:")
                addEdit(@temp0 6 0 24)
                addButton(0 " System " 100 0)
                addButton(gFont "  User  " 100 14)
                addButton(gSmallFont "  Small  " 100 28)
                addButton(gBigFont "   Big   " 100 42)
                addButton(-1 " Cancel " 100 56)
                init()
            )
        (if (temp0)
            = temp25 ReadNumber(@temp0)
        )
        (if (<> temp25 -1)
            (self:
                hide()
                font(temp25)
                setSize()
                draw()
            )
        )
    )


    (method (editMsg)
        (if (localproc_22dc(self))
            Memory(memFREE text)
            = text Memory(memALLOC_CRIT Message(msgSIZE modNum noun verb cond seq))
            Message(msgGET modNum noun verb cond seq text)
        )
        (self:
            hide()
            setSize()
            draw()
        )
    )


    (method (editText)
        (switch (localproc_24db(self))
            (case 0
                return 
            )
            (case 2
                (self:editMsg())
                return 
            )
        )
        (if (seq)
            Memory(memFREE text)
            = text Memory(memALLOC_CRIT 50)
            StrCpy(text "button")
            = noun (= verb (= cond (= seq (= modNum 0))))
        )
        (Print:
            addTitle(@local42)
            addText("Enter new text:")
            addEdit(text 50 0 12 text)
            init()
        )
        (self:
            hide()
            setSize()
            draw()
        )
    )


    (method (editValue)
        (var temp0[25], theValue)
        Format(@temp0 "%d" value)
        = theValue (Print:
                addTitle(@local42)
                addText("Enter new value:")
                addEdit(@temp0 6 0 12 @temp0)
                font(0)
                addButton(0 " Cancel " 0 26)
                init()
            )
        (if (temp0)
            = theValue ReadNumber(@temp0)
        )
        (if (theValue)
            = value theValue
        )
    )
)

// Nodoc
(class _DEdit of _DItem
    (properties
        type $0003
        state $0001
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        key 0
        said 0
        value 0
        underBits 0
        text 0
        font 0
        max 0
        cursor 0
    )

    (method (setSize)
        (var temp0[4])
        TextSize(@temp0[0] "M" font 0 0)
        = nsBottom (+ nsTop temp0[2])
        = nsRight (+ nsLeft (/ (* (* temp0[3] max) 3) 4))
        = cursor StrLen(text)
    )


    (method (showHelp)
        (Print:
            font(gFont)
            width(250)
            addText("Edit Menu:\n\n  Font - Change the font of the edit text\n  Length - Change the maximum length of input\n  Position - Change the position of the edit\n  Text - Change the default edit text\n  Menu - Return to the Main Menu\n")
            init()
        )
    )


    (method (editFont)
        (var temp0[25], temp25)
        = temp25 (Print:
                addTitle(@local42)
                font(0)
                width(90)
                addText("Enter new font number:")
                addEdit(@temp0 6 0 24)
                addButton(0 " System " 100 0)
                addButton(gFont "  User  " 100 14)
                addButton(gSmallFont "  Small  " 100 28)
                addButton(gBigFont "   Big   " 100 42)
                addButton(-1 " Cancel " 100 56)
                init()
            )
        (if (temp0)
            = temp25 ReadNumber(@temp0)
        )
        (if (<> temp25 -1)
            (self:
                hide()
                font(temp25)
                setSize()
                draw()
            )
        )
    )


    (method (editLength)
        (var temp0[25], temp25)
        Format(@temp0 "%d" max)
        = temp25 (Print:
                addTitle(@local42)
                addText("Enter new maximum length:")
                addEdit(@temp0 5 0 12 @temp0)
                font(0)
                addButton(0 " Cancel " 0 26)
                init()
            )
        (if (temp0)
            = temp25 ReadNumber(@temp0)
        )
        (if (temp25)
            (self:
                max(temp25)
                hide()
                setSize()
                draw()
            )
        )
    )


    (method (editText)
        (Print:
            addTitle(@local42)
            addText("Enter new default text:")
            addEdit(text 25 0 12 text)
            init()
        )
        (self:
            hide()
            setSize()
            draw()
        )
    )
)

// Nodoc
(class _DSelector of _DItem
    (properties
        type $0006
        state $0000
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        key 0
        said 0
        value 0
        underBits 0
        font 0
        x 20
        y 6
    )

    (method (setSize)
        (var temp0[4])
        TextSize(@temp0[0] "M" font 0 0)
        = nsBottom (+ (+ nsTop 20) (* temp0[2] y))
        = nsRight (+ nsLeft (/ (* (* temp0[3] x) 3) 4))
    )


    (method (showHelp)
        (Print:
            font(gFont)
            width(250)
            addText("Selector Menu:\n\n  Width - Change the selector width (in chars)\n  Length - Change number of selector lines\n  Position - Change the position of the selector\n  Menu - Return to the Main Menu\n")
            init()
        )
    )


    (method (editLength)
        (var temp0[25], temp25)
        Format(@temp0 "%d" y)
        = temp25 (Print:
                addTitle(@local42)
                addText("Enter new length:")
                addEdit(@temp0 5 0 12 @temp0)
                font(0)
                addButton(0 " Cancel " 0 26)
                init()
            )
        (if (temp0)
            = temp25 ReadNumber(@temp0)
        )
        (if (temp25)
            (self:
                y(temp25)
                hide()
                setSize()
                draw()
            )
        )
    )


    (method (editWidth)
        (var temp0[25], temp25)
        Format(@temp0 "%d" x)
        = temp25 (Print:
                addTitle(@local42)
                addText("Enter new width:")
                addEdit(@temp0 5 0 12 @temp0)
                font(0)
                addButton(0 " Cancel " 0 26)
                init()
            )
        (if (temp0)
            = temp25 ReadNumber(@temp0)
        )
        (if (temp25)
            (self:
                x(temp25)
                hide()
                setSize()
                draw()
            )
        )
    )

)

/*
	Invokes the in-game Dialog Editor. This can be considered debug functionality, and is not essential to the template game.
	
	Example usage::
	
		(DialogEditor:doit())
*/
(class DialogEditor of List
    (properties
        elements 0
        size 0
        state $0000
        curItem 0
        curMenu 0
    )

    (method (init)
        = theWindow gWindow
        StrCpy(@local42 "DialogEditor  v1.1")
        = gWindow SysWindow
        (send gWindow:
            color(gColorWindowForeground)
            back(255)
        )
        (send gGame:setCursor(999))
        (self:changeState(0))
    )


    (method (doit)
        (var newEvent, temp1[100])
        (self:init())
        (while (TRUE)
            = newEvent (Event:new())
            (if (not (send curMenu:handleEvent(newEvent)))
                GlobalToLocal(newEvent)
                (if ((self:handleEvent(newEvent)))
                    break
                )
            )
            (send newEvent:dispose())
        )
        (send newEvent:dispose())
        (self:dispose())
    )


    (method (dispose)
        (mainMenu:dispose())
        (DlgWindow:dispose())
        = gWindow theWindow
        (send gGame:setCursor((send ((send gIconBar:curIcon)):cursor)))
        DrawStatus(" " 0 0)
        DrawStatus(0)
        (super:dispose())
        DrawPic((send gRoom:picture) 100)
        DisposeScript(DIALOGEDIT_SCRIPT)
    )


    (method (handleEvent pEvent)
        (var temp0, theCurItem, temp2[506], temp508[30], temp538[30], temp568[30], temp598[30], temp628[30], temp658[20], temp678[30], temp708[30], temp738[20], temp758[30], temp788[30])
        (switch ((send pEvent:type))
            (case evNULL
            )
            (case evMOUSEBUTTON
                = theCurItem (self:firstTrue(#check pEvent))
                (if (theCurItem)
                    (send pEvent:claimed(TRUE))
                    (if (== (send pEvent:modifiers) emSHIFT)
                        (send theCurItem:handleEvent(pEvent))
                    )(else
                        (if (curItem)
                            (send curItem:select(0))
                        )
                        (send (= curItem theCurItem):select(1))
                        (self:changeState(0))
                    )
                )
            )
            (case evMOUSERELEASE
            )
            (case evKEYBOARD
                (switch ((send pEvent:message))
                    (case 63
                        (send pEvent:message(104))
                    )
                    (case KEY_PAUSE
                        (send pEvent:message(120))
                    )
                )
                (switch ((send pEvent:message))
                    (case KEY_TAB
                        (send curItem:select(0))
                        = temp0 (DialogEditor:indexOf(curItem))
                        (if (== ++temp0 (DialogEditor:size))
                            = curItem (DialogEditor:at(0))
                        )(else
                            = curItem (DialogEditor:at(temp0))
                        )
                        (send curItem:select(1))
                        (DialogEditor:changeState(0))
                    )
                    (case KEY_SHIFTTAB
                        (send curItem:select(0))
                        = temp0 (DialogEditor:indexOf(curItem))
                        (if (== temp0 0)
                            = curItem (DialogEditor:at((- (DialogEditor:size) 1)))
                        )(else
                            = curItem (DialogEditor:at(--temp0))
                        )
                    )
                    (case KEY_SPACE
                    )
                    (case JOY_UPLEFT
                    )
                    (case KEY_a
                        (Print:
                            addTitle(@local42)
                            mode(1)
                            width(140)
                            addText("by Brian K. Hughes\n17 July, 1992")
                            init()
                        )
                    )
                    (case KEY_c
                        (switch (state)
                            (case 4
                                (send curItem:editCel())
                            )
                            (case 6
                                (if (size)
                                    (DlgWindow:create())
                                )(else
                                    Prints("Can't create window: no items!")
                                )
                            )
                        )
                    )
                    (case KEY_d
                        (switch (state)
                            (case 0
                                (self:delItem())
                            )
                            (case 6
                                (if (local1)
                                    (DlgWindow:dispose())
                                    (self:eachElementDo(#draw))
                                )(else
                                    Prints("No window to delete!")
                                )
                            )
                        )
                    )
                    (case KEY_e
                        (if (size)
                            (self:changeState(
                                (switch ((send curItem:{-super-}))
                                    (case _DButton
                                        1
                                    )
                                    (case _DText
                                        2
                                    )
                                    (case _DEdit
                                        3
                                    )
                                    (case _DIcon
                                        4
                                    )
                                    (case _DSelector
                                        5
                                    )
                                )
))
                        )(else
                            Prints("No items to edit!")
                        )
                    )
                    (case KEY_f
                        (if (IsOneOf(state 2 1 3))
                            (send curItem:editFont())
                        )
                    )
                    (case KEY_h
                        (switch (state)
                            (case 0
                                (Print:
                                    font(gFont)
                                    width(250)
                                    addText("Main Menu:\n\n  About - About the DialogEditor\n  Item - Create a new item\n  Edit - Edit properties of the current item\n  Look - View properties of the current item\n  Del - Delete the current item\n  Win - Create a window background\n  Help - You're here!\n  eXit - Exit the DialogEditor (& maybe save)\n")
                                    init()
                                )
                            )
                            (case 6
                                (Print:
                                    font(gFont)
                                    width(250)
                                    addText("Window Menu:\n\n  Create - Draw the window to the correct size\n  Position - Move the window and all items\n  Delete - Remove the window\n  Menu - Return to the Main Menu")
                                    init()
                                )
                            )
                            (default 
                                (send curItem:showHelp())
                            )
                        )
                    )
                    (case KEY_i
                        = theCurItem 0
                        (switch ((Print:
                            addTitle(@local42)
                            width(65)
                            addText("Select the kind of item you want to add to the dialog:")
                            font(0)
                            addButton(1 "  Button  " 80 0)
                            addButton(2 "   Text   " 80 14)
                            addButton(3 "   Edit   " 80 28)
                            addButton(4 "   Icon   " 80 42)
                            addButton(6 " Selector " 80 56)
                            addButton(0 "  Cancel  " 80 70)
                            init()
                        )
								)
                            (case 1
                                = temp0 Memory(memALLOC_CRIT 50)
                                StrCpy(temp0 "button")
                                = theCurItem (send ((_DButton:new())):
                                        text(temp0)
                                        yourself()
                                    )
                            )
                            (case 2
                                = temp0 Memory(memALLOC_CRIT 100)
                                StrCpy(temp0 "text")
                                = theCurItem (send ((_DText:new())):
                                        text(temp0)
                                        yourself()
                                    )
                            )
                            (case 3
                                = temp0 Memory(memALLOC_CRIT 50)
                                StrCpy(temp0 "")
                                = theCurItem (send ((_DEdit:new())):
                                        text(temp0)
                                        max(5)
                                        yourself()
                                    )
                            )
                            (case 4
                                = theCurItem (send ((_DIcon:new())):
                                        view(0)
                                        loop(0)
                                        cel(0)
                                        yourself()
                                    )
                            )
                            (case 6
                                = theCurItem (send ((_DSelector:new())):
                                        x(10)
                                        y(1)
                                        yourself()
                                    )
                            )
                        )
                        (if (theCurItem)
                            (if (curItem)
                                (send curItem:select(0))
                            )
                            = curItem theCurItem
                            (if (local1)
                                (DlgWindow:dispose())
                            )
                            (self:
                                addToEnd((send curItem:
                                        setSize()
                                        moveTo(4 4)
                                        yourself()
                                    )
)
                                eachElementDo(#draw)
                            )
                            (send curItem:select(1))
                        )
                    )
                    (case KEY_j
                        (if (== state 2)
                            (send curItem:editJust())
                        )
                    )
                    (case KEY_l
                        (switch (state)
                            (case 0
                                (if (curItem)
                                    Format(@temp508 "  x:       %d" (send curItem:nsLeft))
                                    Format(@temp538 "  y:       %d" (send curItem:nsTop))
                                    (switch ((send curItem:{-super-}))
                                        (case _DText
                                            StrCpy(@temp788 "Text Item")
                                            StrCpy(@temp2 (send curItem:text))
                                            Format(@temp568 "  text:   %s" localproc_229e(@temp2 15))
                                            Format(@temp598 "  font:   %d" (send curItem:font))
                                            Format(@temp628 "  width:  %d" (send curItem:width))
                                            Format(@temp658 "  noun:   %d" (send curItem:noun))
                                            Format(@temp678 "  verb:   %d" (send curItem:verb))
                                            Format(@temp708 "  case:   %d" (send curItem:case))
                                            Format(@temp738 "  seq:    %d" (send curItem:seq))
                                            Format(@temp758 "  modNum: %d" (send curItem:modNum))
                                        )
                                        (case _DButton
                                            StrCpy(@temp788 "Button Item")
                                            StrCpy(@temp2 (send curItem:text))
                                            Format(@temp568 "  text:   %s" localproc_229e(@temp2 15))
                                            Format(@temp598 "  font:   %d" (send curItem:font))
                                            Format(@temp628 "  value:  %d" (send curItem:value))
                                            Format(@temp658 "  noun:   %d" (send curItem:noun))
                                            Format(@temp678 "  verb:   %d" (send curItem:verb))
                                            Format(@temp708 "  case:   %d" (send curItem:case))
                                            Format(@temp738 "  seq:    %d" (send curItem:seq))
                                            Format(@temp758 "  modNum: %d" (send curItem:modNum))
                                        )
                                        (case _DIcon
                                            StrCpy(@temp788 "Icon Item")
                                            Format(@temp568 "  view:   %d" (send curItem:view))
                                            Format(@temp598 "  loop:   %d" (send curItem:loop))
                                            Format(@temp628 "  cel:    %d" (send curItem:cel))
                                            = temp758 0
                                            = temp738 temp758
                                            = temp708 temp738
                                            = temp678 temp708
                                            = temp658 temp678
                                        )
                                        (case _DEdit
                                            StrCpy(@temp788 "Edit Item")
                                            StrCpy(@temp2 (send curItem:text))
                                            Format(@temp568 "  text:   %s" localproc_229e(@temp2 15))
                                            Format(@temp598 "  font:   %d" (send curItem:font))
                                            Format(@temp628 "  max:    %d" (send curItem:max))
                                            = temp758 0
                                            = temp738 temp758
                                            = temp708 temp738
                                            = temp678 temp708
                                            = temp658 temp678
                                        )
                                        (case _DSelector
                                            StrCpy(@temp788 "Selector Item")
                                            Format(@temp568 "  width:  %d" (send curItem:x))
                                            Format(@temp598 "  length: %d" (send curItem:y))
                                            = temp758 0
                                            = temp738 temp758
                                            = temp708 temp738
                                            = temp678 temp708
                                            = temp658 temp678
                                            = temp628 temp658
                                        )
                                    )
                                    (Print:
                                        addTitle(@temp788)
                                        addText(@temp508)
                                        addText(@temp538 0 12)
                                        addText(@temp568 0 24)
                                        addText(@temp598 0 36)
                                        addText(@temp628 0 48)
                                        addText(@temp658 0 60)
                                        addText(@temp678 0 72)
                                        addText(@temp708 0 84)
                                        addText(@temp738 0 96)
                                        addText(@temp758 0 108)
                                        init()
                                    )
                                    (if (local1)
                                        (DlgWindow:create())
                                    )
                                )(else
                                    Prints("No item to look at!")
                                )
                            )
                            (case 4
                                (send curItem:editLoop())
                            )
                            (case 3
                                (send curItem:editLength())
                            )
                            (case 5
                                (send curItem:editLength())
                            )
                        )
                    )
                    (case KEY_m
                        (self:changeState(0))
                    )
                    (case KEY_p
                        (switch (state)
                            (case 6
                                (DlgWindow:editPosn())
                            )
                            (default 
                                (send curItem:editPosn())
                            )
                        )
                    )
                    (case KEY_t
                        (switch (state)
                            (case 6
                                (DlgWindow:editTitle())
                            )
                            (default 
                                (send curItem:editText())
                            )
                        )
                    )
                    (case KEY_v
                        (switch (state)
                            (case 4
                                (send curItem:editView())
                            )
                            (case 1
                                (send curItem:editValue())
                            )
                        )
                    )
                    (case KEY_w
                        (switch (state)
                            (case 0
                                (self:changeState(6))
                            )
                            (case 5
                                (send curItem:editWidth())
                            )
                            (case 2
                                (send curItem:editWidth())
                            )
                        )
                    )
                    (case KEY_x
                        return (self:exit())
                    )
                )
            )
        )
        return 0
    )


    (method (delItem)
        (var temp0[45])
        (if (size)
            (if ((Print:
                addText("Delete current item?")
                font(0)
                addButton(1 "Yes" 0 12)
                addButton(0 "No" 40 12)
                init()
            )
)
                (self:delete(curItem))
                (send curItem:dispose())
                = curItem 0
                (if (size)
                    = curItem (self:at(0))
                )
            )
        )(else
            Prints("Nothing to delete!")
        )
    )


    (method (exit)
        (var temp0[100], temp100, newFile, openMode)
        (if (not local1)
            (DlgWindow:create())
        )
        (if (not local2)
            Format(@local2 "%d.dlg" gModNum)
        )
        = temp100 (Print:
                addTitle(@local42)
                addText("Save to: ")
                addEdit(@local2 25 60 0 @local2)
                font(0)
                addButton(1 "  Save  " 10 12)
                addButton(2 " Abandon " 80 12)
                addButton(0 " Cancel " 151 12)
                init()
            )
        (if (not temp100)
            return 0
        )
        (if (== temp100 2)
            return 1
        )
        (if (FileIO(fiEXISTS @local2))
            Format(@temp0 "The file '%s' already exists.  Now what?" @local2)
            = temp100 (Print:
                    addTitle(@local42)
                    addText(@temp0)
                    font(0)
                    addButton(1 " Replace " 0 24)
                    addButton(2 " Append " 70 24)
                    addButton(0 " Cancel " 125 24)
                    init()
                )
            (if (not temp100)
                return 0
            )
        )
        = openMode 
            (if (== temp100 1)
                fCREATE
            )(else
                fOPENCREATE
            )
        = newFile (File:new())
        (if (not (send newFile:
            name(@local2)
            open(openMode)
				 )
			)
            Printf("Error opening '%s'" (send newFile:name))
            (send newFile:dispose())
            return 0
        )
        (send newFile:
            writeString("\t\t; DialogEditor v1.0
\n")
            writeString("\t\t; by Brian K. Hughes
\n")
            writeString("\t\t(Print
\n")
        )
        (if (local1)
            Format(@temp0 "\t\t\tposn:\t\t\t%d %d,
\n" (DlgWindow:left) (DlgWindow:top))
            (send newFile:writeString(@temp0))
            (if ((DlgWindow:title))
                (if ((DlgWindow:seq))
                    Format(@temp0 "\t\t\taddTitle:\t%d %d %d %d %d,
\n" (DlgWindow:noun) (DlgWindow:verb) (DlgWindow:cond) (DlgWindow:seq) (DlgWindow:modNum))
                )(else
                    Format(@temp0 "\t\t\taddTitle:\t{%s},
\n" (DlgWindow:title))
                )
                (send newFile:writeString(@temp0))
            )
        )
        (self:writeFile(newFile))
        (if ((Print:
            addTitle(@local42)
            addText("This dialog should be...")
            font(0)
            addButton(0 "   Modal   " 0 24)
            addButton(1 " Modeless " 0 38)
            init()
        )
)
            (send newFile:writeString("\t\t\tmodeless:\tTRUE,
\n"))
        )
        (send newFile:
            writeString("\t\t\tinit:
\n")
            writeString("\t\t)
\n")
        )
        (send newFile:dispose())
        return 1
    )


    (method (writeFile param1)
        (var temp0, temp1, temp2[100], temp102, temp103, temp104)
        = temp102 -1
        = temp103 0
        = temp104 0
        = temp0 0
        (while (< temp0 size)
            (switch ((send (= temp1 (self:at(temp0))):{-super-}))
                (case _DButton
                    (if (<> (send temp1:font) temp102)
                        = temp102 (send temp1:font)
                        Format(@temp2 "\t\t\tfont:\t\t\t%d,
\n" temp102)
                        (send param1:writeString(@temp2))
                    )
                    (if ((send temp1:seq))
                        Format(@temp2 "\t\t\taddButton:\t%d %d %d %d %d %d %d %d, 
\n" (send temp1:value) (send temp1:noun) (send temp1:verb) (send temp1:case) (send temp1:seq) - (- (send temp1:nsLeft) (DlgWindow:left)) 4 - (- (send temp1:nsTop) (DlgWindow:top)) 4 (send temp1:modNum))
                    )(else
                        Format(@temp2 "\t\t\taddButton:\t%d {%s} %d %d,
\n" (send temp1:value) (send temp1:text) - (- (send temp1:nsLeft) (DlgWindow:left)) 4 - (- (send temp1:nsTop) (DlgWindow:top)) 4)
                    )
                    (send param1:writeString(@temp2))
                )
                (case _DText
                    (if (<> (send temp1:font) temp102)
                        = temp102 (send temp1:font)
                        Format(@temp2 "\t\t\tfont:\t\t\t%d,
\n" temp102)
                        (send param1:writeString(@temp2))
                    )
                    (if (<> (send temp1:mode) temp103)
                        = temp103 (send temp1:mode)
                        Format(@temp2 "\t\t\tmode:\t\t\t%s,
\n" 
                            (switch ((send temp1:mode))
                                (case 0
                                    "teJustLeft"
                                )
                                (case -1
                                    "teJustRight"
                                )
                                (case 1
                                    "teJustCenter"
                                )
                            )
)
                        (send param1:writeString(@temp2))
                    )
                    (if (<> (send temp1:width) temp104)
                        = temp104 (send temp1:width)
                        Format(@temp2 "\t\t\twidth:\t\t%d, 
\n" temp104)
                        (send param1:writeString(@temp2))
                    )
                    (if ((send temp1:seq))
                        Format(@temp2 "\t\t\taddText:\t\t%d %d %d %d %d %d %d, 
\n" (send temp1:noun) (send temp1:verb) (send temp1:case) (send temp1:seq) - (- (send temp1:nsLeft) (DlgWindow:left)) 4 - (- (send temp1:nsTop) (DlgWindow:top)) 4 (send temp1:modNum))
                    )(else
                        Format(@temp2 "\t\t\taddText:\t\t{%s} %d %d,
\n" (send temp1:text) - (- (send temp1:nsLeft) (DlgWindow:left)) 4 - (- (send temp1:nsTop) (DlgWindow:top)) 4)
                    )
                    (send param1:writeString(@temp2))
                )
                (case _DEdit
                    (if (<> (send temp1:font) temp102)
                        = temp102 (send temp1:font)
                        Format(@temp2 "\t\t\tfont:\t\t\t%d,
\n" temp102)
                        (send param1:writeString(@temp2))
                    )
                    Format(@temp2 "\t\t\taddEdit:\t\t@str %d %d %d {%s},
\n" (send temp1:max) - (- (send temp1:nsLeft) (DlgWindow:left)) 4 - (- (send temp1:nsTop) (DlgWindow:top)) 4 (send temp1:text))
                    (send param1:writeString(@temp2))
                )
                (case _DIcon
                    Format(@temp2 "\t\t\taddIcon:\t\t%d %d %d %d %d,
\n" (send temp1:view) (send temp1:loop) (send temp1:cel) - (- (send temp1:nsLeft) (DlgWindow:left)) 4 - (- (send temp1:nsTop) (DlgWindow:top)) 4)
                    (send param1:writeString(@temp2))
                )
            )
            ++temp0
        )
    )


    (method (changeState newState)
        (if (curMenu)
            (send curMenu:dispose())
        )
        = curMenu 
            (switch ((= state newState))
                (case 0
                    mainMenu
                )
                (case 1
                    editBMenu
                )
                (case 2
                    editTMenu
                )
                (case 3
                    editEMenu
                )
                (case 4
                    editIMenu
                )
                (case 5
                    editSMenu
                )
                (case 6
                    editWMenu
                )
                (default 
                    0
                )
            )
        (if (curMenu)
            (send curMenu:init())
        )
    )

)
