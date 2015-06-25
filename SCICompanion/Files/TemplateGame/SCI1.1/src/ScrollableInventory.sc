(version 2)
(include "sci.sh")
(use "Main")
(use "Print")
(use "InvI")
(use "Obj")
(script 26)



(class ScrollableInventory of Inv
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
        normalHeading -1
        heading 0
        empty -1
        iconBarInvItem 0
        okButton 0
        selectIcon 0
        curPos 0
        dispAmount 12
        items 0
        numCols 6
        numRows 2
        scrollAmount 6
        firstThru 1
        upIcon 0
        downIcon 0
    )

    (method (dispose param1)
        (if (IsObject(items))
            (send items:dispose())
            = items 0
        )
        (super:dispose(rest param1))
    )


    (method (hide param1)
        (if (IsObject(items))
            (send items:dispose())
            = items 0
        )
        = firstThru 1
        (super:hide(rest param1))
    )


    (method (advance param1)
        (var temp0, temp1, temp2, temp3)
        = temp1 
            (if (paramTotal)
                param1
            )(else
                1
            )
        = temp2 (self:indexOf(highlightedIcon))
        = temp3 (% (+ temp1 temp2) size)
        (while (TRUE)
            = temp0 (self:at(temp3))
            (if ((IsObject(temp0) and not (& (send temp0:signal) icDISABLED)) and ((> (send temp0:nsLeft) -1) or not (send temp0:isKindOf(InvI))))
                break
            )
            = temp3 (% (+ temp3 1) size)
        )
        (self:highlight(temp0 1))
    )


    (method (retreat param1)
        (var temp0, temp1, temp2, temp3)
        (asm
            lap     paramTotal
            bnt     code_00f4
            lap     param1
            jmp     code_00f6
code_00f4:  ldi     1
code_00f6:  sat     temp1
            pushi   #indexOf
            pushi   1
            pTos    highlightedIcon
            self    6
            sat     temp2
            push    
            lat     temp1
            sub     
            sat     temp3
            push    
            ldi     0
            lt?     
            bnt     code_0115
            pTos    size
            ldi     1
            sub     
            sat     temp3
code_0115:  pushi   #at
            pushi   1
            lst     temp3
            self    6
            sat     temp0
            pushi   1
            push    
            callk   IsObject, 2
            bnt     code_0151
            pushi   #signal
            pushi   0
            lat     temp0
            send    4
            push    
            ldi     4
            and     
            not     
            bnt     code_0151
            pushi   #nsLeft
            pushi   0
            lat     temp0
            send    4
            push    
            ldi     65535
            gt?     
            bt      code_014d
            pushi   #isKindOf
            pushi   1
            class   InvI
            push    
            lat     temp0
            send    6
            not     
            bnt     code_0151
code_014d:  jmp     code_0162
            jmp     code_0115
code_0151:  -at     temp3
            push    
            ldi     0
            lt?     
            bnt     code_0115
            pTos    size
            ldi     1
            sub     
            sat     temp3
            jmp     code_0115
code_0162:  pushi   #highlight
            pushi   2
            lst     temp0
            pushi   1
            self    8
            ret     
        )
    )


    (method (drawInvWindow param1 param2)
        (var theTheCurPos, theTheTheCurPos, theTheTheTheCurPos, theTheTheTheTheCurPos, theTheTheTheTheCurPos_2_2, theTheTheTheTheTheCurPos_2_2, theTheTheTheTheCurPos_2, theTheTheTheCurPos_2, scrollableInventoryFirst, temp9, temp10, temp11, temp12, temp13, temp14, temp15, temp16, temp17, temp18, theNumCols, theCurPos, scrollableInventoryWindow)
        = theTheTheTheTheTheCurPos_2_2 0
        = theTheTheTheTheCurPos_2_2 theTheTheTheTheTheCurPos_2_2
        = theTheTheTheTheCurPos theTheTheTheTheCurPos_2_2
        = theTheTheTheCurPos theTheTheTheTheCurPos
        = theTheTheCurPos theTheTheTheCurPos
        = theTheCurPos theTheTheCurPos
        = theCurPos theTheCurPos
        (if (firstThru)
            (if (IsObject(items))
                (send items:dispose())
                = items 0
            )
            = items (Set:new())
        )
        = scrollableInventoryFirst (self:first())
        (while (scrollableInventoryFirst)
            = temp9 NodeValue(scrollableInventoryFirst)
            (if ((send temp9:isKindOf(InvI)))
                (if ((send temp9:ownedBy(param1)))
                    (send temp9:signal((& (send temp9:signal) $fffb)))
                    (send items:add(temp9))
                    (send temp9:
                        nsLeft(-5)
                        nsRight(-5)
                        nsTop(-5)
                        nsBottom(-5)
                    )
                    = theTheTheTheTheCurPos_2 CelWide((send temp9:view) (send temp9:loop) (send temp9:cel))
                    (if (> theTheTheTheTheCurPos_2 theTheTheTheCurPos)
                        = theTheTheTheCurPos theTheTheTheTheCurPos_2
                    )
                    = theTheTheTheCurPos_2 CelHigh((send temp9:view) (send temp9:loop) (send temp9:cel))
                    (if (> theTheTheTheCurPos_2 theTheTheCurPos)
                        = theTheTheCurPos theTheTheTheCurPos_2
                    )
                )(else
                    (send temp9:signal((| (send temp9:signal) icDISABLED)))
                )
            )(else
                ++theTheTheTheTheCurPos
                = theTheTheTheTheTheCurPos_2_2 (+ theTheTheTheTheTheCurPos_2_2 CelWide((send temp9:view) (send temp9:loop) (send temp9:cel)))
                = theTheTheTheCurPos_2 CelHigh((send temp9:view) (send temp9:loop) (send temp9:cel))
                (if (> theTheTheTheCurPos_2 theTheTheTheTheCurPos_2_2)
                    = theTheTheTheTheCurPos_2_2 theTheTheTheCurPos_2
                )
            )
            = scrollableInventoryFirst (self:next(scrollableInventoryFirst))
        )
        (if (not (send items:size))
            (if ((<= 0 normalHeading) and (<= 0 empty))
                (Print:
                    addText(empty 0 0 0 0 0 normalHeading)
                    init()
                )
            )(else
                TextPrint("You'll get nothing and like it!")
            )
            (if (IsObject(items))
                (send items:dispose())
            )
            return 0
        )
        = theTheCurPos 
            (if (< (send items:size) dispAmount)
                (send items:size)
            )(else
                dispAmount
            )
        = temp10 Max((+ 4 theTheTheTheTheTheCurPos_2_2) (* numCols (+ 4 theTheTheTheCurPos)))
        = temp11 (* numRows (+ 4 theTheTheCurPos))
        = temp12 (/ (- 190 temp11) 2)
        = temp13 (/ (- 320 temp10) 2)
        = temp14 (+ temp12 temp11)
        = temp15 (+ temp13 temp10)
        = scrollableInventoryWindow (self:window)
        (if (scrollableInventoryWindow)
            (send scrollableInventoryWindow:
                top(temp12)
                left(temp13)
                right(temp15)
                bottom(temp14)
                open(not firstThru)
            )
        )
        = theNumCols numCols
        (if (theTheCurPos)
            = temp17 (+ 2 
                (if ((send scrollableInventoryWindow:respondsTo(#yOffset)))
                    (send scrollableInventoryWindow:yOffset)
                )(else
                    0
                )
)
            = temp16 (+ 4 
                (if ((send scrollableInventoryWindow:respondsTo(#xOffset)))
                    (send scrollableInventoryWindow:xOffset)
                )(else
                    0
                )
)
            = temp18 temp16
            = theCurPos curPos
            (while ((< theCurPos (+ curPos dispAmount)) and (< theCurPos (send items:size)))
                = temp9 (send items:at(theCurPos))
                (if (not & (send temp9:signal) $0080)
                    = theTheTheTheTheCurPos_2 CelWide((send temp9:view) (send temp9:loop) (send temp9:cel))
                    = theTheTheTheCurPos_2 CelHigh((send temp9:view) (send temp9:loop) (send temp9:cel))
                    (send temp9:
                        nsLeft(+ temp16 (/ (- theTheTheTheCurPos theTheTheTheCurPos_2) 2))
                        nsTop(+ temp17 (/ (- theTheTheCurPos theTheTheTheCurPos_2) 2))
                    )
                    (send temp9:
                        nsRight(+ (send temp9:nsLeft) theTheTheTheTheCurPos_2)
                        nsBottom(+ (send temp9:nsTop) theTheTheTheCurPos_2)
                    )
                    (if (--theNumCols)
                        = temp16 (+ temp16 theTheTheTheCurPos)
                    )(else
                        = theNumCols numCols
                        = temp17 (+ temp17 theTheTheCurPos)
                        = temp16 temp18
                    )
                )(else
                    = temp16 (send temp9:nsLeft)
                    = temp17 (send temp9:nsTop)
                )
                (send temp9:show())
                (if (== temp9 param2)
                    (send temp9:highlight())
                )
                ++theCurPos
            )
        )
        = temp16 (/ (- (- (send scrollableInventoryWindow:right) (send scrollableInventoryWindow:left)) theTheTheTheTheTheCurPos_2_2) 2)
        = temp11 (- (send scrollableInventoryWindow:bottom) (send scrollableInventoryWindow:top))
        = temp17 32767
        (if (firstThru)
            = scrollableInventoryFirst (self:first())
            (while (scrollableInventoryFirst)
                = temp9 NodeValue(scrollableInventoryFirst)
                (if (not (send temp9:isKindOf(InvI)))
                    = theTheTheTheTheCurPos_2 CelWide((send temp9:view) (send temp9:loop) (send temp9:cel))
                    = theTheTheTheCurPos_2 CelHigh((send temp9:view) (send temp9:loop) (send temp9:cel))
                    (if (not & (send temp9:signal) $0080)
                        (if (== temp17 32767)
                            = temp17 (- temp11 theTheTheTheCurPos_2)
                        )
                        (send temp9:
                            nsLeft(temp16)
                            nsTop(temp17)
                            nsBottom(+ temp17 theTheTheTheCurPos_2)
                            nsRight(+ temp16 theTheTheTheTheCurPos_2)
                        )
                    )
                    = temp16 (+ (send temp9:nsLeft) theTheTheTheTheCurPos_2)
                    = temp17 (send temp9:nsTop)
                    (send temp9:signal((& (send temp9:signal) $fffb)))
                    (send temp9:show())
                )
                = scrollableInventoryFirst (self:next(scrollableInventoryFirst))
            )
        )
        (if (not curPos)
            (send upIcon:signal((| (send upIcon:signal) icDISABLED)))
        )(else
            (send upIcon:signal((& (send upIcon:signal) (bnot icDISABLED))))
        )
        (if (>= curPos (- (send items:size) dispAmount))
            (send downIcon:signal((| (send downIcon:signal) icDISABLED)))
        )(else
            (send downIcon:signal((& (send downIcon:signal) (bnot icDISABLED))))
        )
        (send upIcon:show())
        (send downIcon:show())
        return 1
    )


    (method (scroll param1)
        (if (paramTotal and (> 0 param1))
            (if (< (= curPos (- curPos scrollAmount)) 0)
                = curPos 0
            )
        )(else
            (if (> (= curPos (+ curPos scrollAmount)) (- size dispAmount))
                = curPos (- size dispAmount)
            )
        )
        = firstThru 0
        (send selectIcon:select())
        (self:show(gEgo))
    )

)
