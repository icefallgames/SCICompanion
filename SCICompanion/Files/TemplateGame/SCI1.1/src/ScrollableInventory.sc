(version 2)
(include "sci.sh")
(use "Main")
(use "Print")
(use "InventoryItem")
(use "IconItem")
(use "SysWindow")
(use "Object")
(script 26)


(local
    local0
)

(procedure (localproc_0814 param1 param2 param3)
    (var temp0, temp1, temp2)
    = temp2 (+ (/ (- (send param1:nsRight) (send param1:nsLeft)) 2) (send param1:nsLeft))
    = temp1 param2
    (while (>= Abs((- temp1 param3)) 4)
        = temp0 (self:firstTrue(#onMe (send ((send ((send gUser:curEvent)):new())):
                    x(temp2)
                    y(temp1)
                    yourself()
                )
							   )
				)
        (if (temp0)
            return 
        )
        (if (< param2 param3)
            = temp1 (+ temp1 4)
        )(else
            = temp1 (- temp1 4)
        )
    )
)

/*
	Base class for :class:`ScrollableInventory`.
*/
(class InventoryBase of IconBar
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
        normalHeading "You are carrying:"
        heading 0
        empty "nothing!"
        iconBarInvItem 0
        okButton 0
        selectIcon 0
    )

    (method (init)
        = heading normalHeading
    )


    (method (doit)
        (var temp0, temp1, temp2, temp3, temp4, temp5[2], temp7, temp8, temp9, temp10[50], temp60)
        (asm
code_088d:  pushi   #type
            pushi   0
            pushi   #new
            pushi   0
            pushi   #curEvent
            pushi   0
            lag     gUser
            send    4
            send    4
            sat     temp1
            send    4
            bnt     code_08a5
            jmp     code_088d
code_08a5:  pTos    state
            ldi     32
            and     
            bnt     code_0d0f
            pushi   #new
            pushi   0
            pushi   #curEvent
            pushi   0
            lag     gUser
            send    4
            send    4
            sat     temp1
            pushi   #x
            pushi   0
            send    4
            sag     gPEventX
            pushi   #y
            pushi   0
            lat     temp1
            send    4
            sag     gPEventY
            pushi   #type
            pushi   0
            lat     temp1
            send    4
            sat     temp2
            pushi   #message
            pushi   0
            lat     temp1
            send    4
            sat     temp3
            pushi   #modifiers
            pushi   0
            lat     temp1
            send    4
            sat     temp4
            ldi     0
            sat     temp9
            pushi   #localize
            pushi   0
            lat     temp1
            send    4
            pToa    curIcon
            bnt     code_0956
            lat     temp4
            not     
            bnt     code_0956
            pTos    curIcon
            pToa    selectIcon
            ne?     
            bnt     code_0956
            lst     temp2
            ldi     1
            eq?     
            bt      code_092d
            lst     temp2
            ldi     4
            eq?     
            bnt     code_091f
            lst     temp3
            ldi     13
            eq?     
            bnt     code_091f
            ldi     1
            sat     temp9
            bt      code_092d
code_091f:  lst     temp2
            ldi     256
            eq?     
            bnt     code_0956
            ldi     1
            sat     temp9
            bnt     code_0956
code_092d:  pTos    curIcon
            pToa    helpIconItem
            ne?     
            bt      code_0941
            pushi   #signal
            pushi   0
            pToa    helpIconItem
            send    4
            push    
            ldi     16
            and     
            bnt     code_0956
code_0941:  pushi   #type
            pushi   1
            pushi   16384
            pushi   37
            pushi   1
            pushi   #message
            pushi   0
            pToa    curIcon
            send    4
            push    
            lat     temp1
            send    12
code_0956:  pushi   1
            lst     temp1
            callk   MapKeyToDir, 2
            pushi   #type
            pushi   0
            lat     temp1
            send    4
            sat     temp2
            pushi   #message
            pushi   0
            lat     temp1
            send    4
            sat     temp3
            lag     gNewSet
            bnt     code_0998
            pushi   #eachElementDo
            pushi   1
            pushi   57
            send    6
code_0998:  lag     gNewEventHandler
            bnt     code_09a7
            pushi   #handleEvent
            pushi   1
            lst     temp1
            send    6
            jmp     code_08a5
code_09a7:  lst     temp2
            ldi     1
            eq?     
            bnt     code_09c3
            lat     temp4
            bnt     code_09c3
            pushi   #advanceCurIcon
            pushi   0
            self    4
            pushi   #claimed
            pushi   1
            pushi   1
            lat     temp1
            send    6
            jmp     code_08a5
code_09c3:  lst     temp2
            ldi     0
            eq?     
            bnt     code_09e9
            pushi   #firstTrue
            pushi   2
            pushi   226
            lst     temp1
            self    8
            sat     temp0
            bnt     code_09e9
            push    
            pToa    highlightedIcon
            ne?     
            bnt     code_09e9
            pushi   #highlight
            pushi   1
            lst     temp0
            self    6
            jmp     code_08a5
code_09e9:  lst     temp2
            ldi     1
            eq?     
            bt      code_0a07
            lst     temp2
            ldi     4
            eq?     
            bnt     code_09fe
            lst     temp3
            ldi     13
            eq?     
            bt      code_0a07
code_09fe:  lst     temp2
            ldi     256
            eq?     
            bnt     code_0a94
code_0a07:  pushi   1
            pTos    highlightedIcon
            callk   IsObject, 2
            bnt     code_08a5
            pushi   184
            pushi   #view
            pTos    highlightedIcon
            lst     temp2
            ldi     1
            eq?     
            push    
            self    8
            bnt     code_08a5
            pTos    highlightedIcon
            pToa    okButton
            eq?     
            bnt     code_0a2e
            jmp     code_0d0f
            jmp     code_08a5
code_0a2e:  pTos    highlightedIcon
            pToa    helpIconItem
            eq?     
            bnt     code_0a7d
            pushi   #cursor
            pushi   0
            pToa    highlightedIcon
            send    4
            push    
            ldi     65535
            ne?     
            bnt     code_0a53
            pushi   #setCursor
            pushi   1
            pushi   #cursor
            pushi   0
            pToa    helpIconItem
            send    4
            push    
            lag     gGame
            send    6
code_0a53:  pTos    state
            ldi     2048
            and     
            bnt     code_0a64
            pushi   #noClickHelp
            pushi   0
            self    4
            jmp     code_08a5
code_0a64:  pToa    helpIconItem
            bnt     code_08a5
            pushi   14
            pushi   #x
            pushi   #signal
            pushi   0
            send    4
            push    
            ldi     16
            or      
            push    
            pToa    helpIconItem
            send    6
            jmp     code_08a5
code_0a7d:  pToa    highlightedIcon
            aTop    curIcon
            pushi   #setCursor
            pushi   1
            pushi   #cursor
            pushi   0
            pToa    curIcon
            send    4
            push    
            lag     gGame
            send    6
            jmp     code_08a5
code_0a94:  lst     temp2
            ldi     64
            and     
            bnt     code_0b46
            lst     temp3
            dup     
            ldi     3
            eq?     
            bnt     code_0aad
            pushi   #advance
            pushi   0
            self    4
            jmp     code_0b42
code_0aad:  dup     
            ldi     7
            eq?     
            bnt     code_0abc
            pushi   #retreat
            pushi   0
            self    4
            jmp     code_0b42
code_0abc:  dup     
            ldi     1
            eq?     
            bnt     code_0af2
            pToa    highlightedIcon
            bnt     code_0ae9
            pushi   3
            push    
            pushi   #nsTop
            pushi   0
            send    4
            push    
            ldi     1
            sub     
            push    
            pushi   0
            call    localproc_0814, 6
            sat     temp0
            bnt     code_0ae9
            pushi   #highlight
            pushi   2
            lst     temp0
            pushi   1
            self    8
            jmp     code_0b42
code_0ae9:  pushi   #retreat
            pushi   0
            self    4
            jmp     code_0b42
code_0af2:  dup     
            ldi     5
            eq?     
            bnt     code_0b2f
            pToa    highlightedIcon
            bnt     code_0b27
            pushi   3
            push    
            pushi   #nsBottom
            pushi   0
            send    4
            push    
            ldi     1
            add     
            push    
            pushi   #bottom
            pushi   0
            pToa    window
            send    4
            push    
            call    localproc_0814, 6
            sat     temp0
            bnt     code_0b27
            pushi   #highlight
            pushi   2
            lst     temp0
            pushi   1
            self    8
            jmp     code_0b42
code_0b27:  pushi   #advance
            pushi   0
            self    4
            jmp     code_0b42
code_0b2f:  dup     
            ldi     0
            eq?     
            bnt     code_0b42
            lst     temp2
            ldi     4
            and     
            bnt     code_0b42
            pushi   #advanceCurIcon
            pushi   0
            self    4
code_0b42:  toss    
            jmp     code_08a5
code_0b46:  lst     temp2
            ldi     4
            eq?     
            bnt     code_0b7a
            lst     temp3
            dup     
            ldi     9
            eq?     
            bnt     code_0b5e
            pushi   #advance
            pushi   0
            self    4
            jmp     code_0b76
code_0b5e:  dup     
            ldi     3840
            eq?     
            bnt     code_0b6d
            pushi   #retreat
            pushi   0
            self    4
            jmp     code_0b76
code_0b6d:  dup     
            ldi     27
            eq?     
            bnt     code_0b76
            jmp     code_0d0f
code_0b76:  toss    
            jmp     code_08a5
code_0b7a:  lst     temp2
            ldi     16384
            and     
            bnt     code_08a5
            pushi   #firstTrue
            pushi   2
            pushi   226
            lst     temp1
            self    8
            sat     temp0
            bnt     code_08a5
            lst     temp2
            ldi     8192
            and     
            bnt     code_0c36
            lat     temp0
            bnt     code_0c15
            pushi   #noun
            pushi   0
            send    4
            bnt     code_0c15
            pushi   7
            pushi   0
            pushi   #modNum
            pushi   0
            lat     temp0
            send    4
            push    
            pushi   #noun
            pushi   0
            lat     temp0
            send    4
            push    
            pushi   #helpVerb
            pushi   0
            lat     temp0
            send    4
            push    
            pushi   0
            pushi   1
            lea     @temp10
            push    
            callk   Message, 14
            bnt     code_0c15
            pushi   #respondsTo
            pushi   1
            pushi   244
            lag     gWindow
            send    6
            bnt     code_0c0a
            pushi   #eraseOnly
            pushi   0
            lag     gWindow
            send    4
            sat     temp7
            pushi   #eraseOnly
            pushi   1
            pushi   1
            lag     gWindow
            send    6
            pushi   1
            lea     @temp10
            push    
            calle   TextPrint, 2
            pushi   #eraseOnly
            pushi   1
            lst     temp7
            lag     gWindow
            send    6
            jmp     code_0c15
code_0c0a:  pushi   1
            lea     @temp10
            push    
            calle   TextPrint, 2
code_0c15:  pushi   14
            pushi   #x
            pushi   #signal
            pushi   0
            pToa    helpIconItem
            send    4
            push    
            ldi     65519
            and     
            push    
            pToa    helpIconItem
            send    6
            pushi   #setCursor
            pushi   1
            pushi   999
            lag     gGame
            send    6
            jmp     code_08a5
code_0c36:  lst     temp0
            pToa    okButton
            eq?     
            bnt     code_0c43
            jmp     code_0d0f
            jmp     code_08a5
code_0c43:  pushi   #isKindOf
            pushi   1
            class   InventoryItem
            push    
            lat     temp0
            send    6
            not     
            bnt     code_0ca3
            pushi   #select
            pushi   2
            lst     temp0
            lat     temp9
            not     
            push    
            self    8
            bnt     code_08a5
            lat     temp0
            aTop    curIcon
            pushi   #setCursor
            pushi   1
            pushi   #cursor
            pushi   0
            pToa    curIcon
            send    4
            push    
            lag     gGame
            send    6
            lst     temp0
            pToa    helpIconItem
            eq?     
            bnt     code_08a5
            pTos    state
            ldi     2048
            and     
            bnt     code_0c8d
            pushi   #noClickHelp
            pushi   0
            self    4
            jmp     code_08a5
code_0c8d:  pushi   14
            pushi   #x
            pushi   #signal
            pushi   0
            pToa    helpIconItem
            send    4
            push    
            ldi     16
            or      
            push    
            pToa    helpIconItem
            send    6
            jmp     code_08a5
code_0ca3:  pToa    curIcon
            bnt     code_08a5
            pushi   #respondsTo
            pushi   1
            pushi   244
            lag     gWindow
            send    6
            bnt     code_0cc7
            pushi   #eraseOnly
            pushi   0
            lag     gWindow
            send    4
            sat     temp7
            pushi   #eraseOnly
            pushi   1
            pushi   1
            lag     gWindow
            send    6
code_0cc7:  pushi   #isKindOf
            pushi   1
            class   InventoryItem
            push    
            pToa    curIcon
            send    6
            bnt     code_0ce5
            pushi   #doVerb
            pushi   1
            pushi   #message
            pushi   0
            pToa    curIcon
            send    4
            push    
            lat     temp0
            send    6
            jmp     code_0cf5
code_0ce5:  pushi   #doVerb
            pushi   1
            pushi   #message
            pushi   0
            lat     temp1
            send    4
            push    
            lat     temp0
            send    6
code_0cf5:  pushi   #respondsTo
            pushi   1
            pushi   244
            lag     gWindow
            send    6
            bnt     code_08a5
            pushi   #eraseOnly
            pushi   1
            lst     temp7
            lag     gWindow
            send    6
            jmp     code_08a5
code_0d0f:  pushi   #hide
            pushi   0
            self    4
            ret     
        )
    )


    (method (showSelf param1)
        (send gSounds:pause())
        (if (gPseudoMouse and (send gPseudoMouse:respondsTo(#stop)))
            (send gPseudoMouse:stop())
        )
        (if ((send gIconBar:height))
            (send gIconBar:hide())
        )
        (if (not window)
            = window (SysWindow:new())
        )
        (if ((send window:window))
            (send window:dispose())
            = window 0
        )
        (if (not okButton)
            = okButton NodeValue((self:first()))
        )
        = curIcon 0
        (if ((self:show(
            (if (paramTotal)
                param1
            )(else
                gEgo
            )
			)))
            (self:doit())
        )
    )


    (method (show param1)
        (var temp0, temp1)
        (send gGame:setCursor(
            (if (curIcon)
                (send curIcon:cursor)
            )(else
                (send selectIcon:cursor)
            )
		))
        = temp0 PicNotValid()
        PicNotValid(0)
        = state (| state $0020)
        = temp1 (self:drawInvWindow(
	                (if (paramTotal)
	                    param1
	                )(else
	                    gEgo
					)
 					(send gIconBar:curIcon)))
        (if (not temp1)
            = state (& state $ffdf)
        )
        PicNotValid(temp0)
        return temp1
    )


    (method (hide)
        (var temp0)
        (if (& state $0020)
            (send gSounds:pause(FALSE))
            = state (& state $ffdf)
        )
        (if (window)
            (send window:dispose())
        )
        (if (IsObject(curIcon) and (send curIcon:isKindOf(InventoryItem)))
            (if (not (send gIconBar:curInvIcon))
                (send gIconBar:enable((send gIconBar:useIconItem)))
            )
            (send gIconBar:
                curIcon((send ((send gIconBar:useIconItem)):
	                        cursor((send curIcon:cursor))
	                        yourself()
						)
					   )
                curInvIcon(curIcon)
            )
            = temp0 (send ((send gIconBar:curIcon)):cursor)
            (if (temp0)
                (send gGame:setCursor((= temp0 (send ((send gIconBar:curIcon)):cursor))))
            )
        )
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
        = temp3 (+ temp1 temp2)
        (while (TRUE)
            = temp0 (self:at(
	                    (if (<= temp3 size)
	                        temp3
	                    )(else
	                        (% temp3 (- size 1))
	                    )
					))
            (if (not IsObject(temp0))
                = temp0 NodeValue((self:first()))
            )
            (if (not & (send temp0:signal) icDISABLED)
                break
            )
            ++temp3
        )
        (self:highlight(temp0 1))
    )


    (method (retreat param1)
        (var temp0, temp1, temp2, temp3)
        = temp1 
            (if (paramTotal)
                param1
            )(else
                1
            )
        = temp2 (self:indexOf(highlightedIcon))
        = temp3 (- temp2 temp1)
        (while (TRUE)
            = temp0 (self:at(temp3))
            (if (not IsObject(temp0))
                = temp0 NodeValue((self:last()))
            )
            (if (not & (send temp0:signal) icDISABLED)
                break
            )
            --temp3
        )
        (self:highlight(temp0 1))
    )


    (method (ownedBy param1)
        (self:firstTrue(#ownedBy param1))
    )


    (method (drawInvWindow param1 param2)
        (var temp0, temp1, temp2, temp3, temp4, temp5, temp6, temp7, invFirst, temp9, temp10, temp11, temp12, temp13, temp14, temp15, temp16, temp17, temp18, temp19, temp20, invWindow, temp22[50])
        = temp5 0
        = temp4 temp5
        = temp3 temp4
        = temp2 temp3
        = temp1 temp2
        = temp0 temp1
        = invFirst (self:first())
        (while (invFirst)
            = temp9 NodeValue(invFirst)
            (if ((send temp9:isKindOf(InventoryItem)))
                (if ((send temp9:ownedBy(param1)))
                    (send temp9:signal((& (send temp9:signal) (bnot icDISABLED))))
                    ++temp0
                    = temp6 CelWide((send temp9:view) (send temp9:loop) (send temp9:cel))
                    (if (> temp6 temp2)
                        = temp2 temp6
                    )
                    = temp7 CelHigh((send temp9:view) (send temp9:loop) (send temp9:cel))
                    (if (> temp7 temp1)
                        = temp1 temp7
                    )
                )(else
                    (send temp9:signal((| (send temp9:signal) icDISABLED)))
                )
            )(else
                ++temp3
                = temp5 (+ temp5 CelWide((send temp9:view) (send temp9:loop) (send temp9:cel)))
                = temp7 CelHigh((send temp9:view) (send temp9:loop) (send temp9:cel))
                (if (> temp7 temp4)
                    = temp4 temp7
                )
            )
            = invFirst (self:next(invFirst))
        )
        (if (not temp0)
            (Print:
                addTextF("%s %s" normalHeading empty)
                init()
            )
            return 0
        )
        = temp16 Sqrt(temp0)
        (if (> (* temp16 (= temp16 Sqrt(temp0))) temp0)
            --temp16
        )
        (if (> temp16 3)
            = temp16 3
        )
        = local0 (/ temp0 temp16)
        (if (< (* temp16 local0) temp0)
            ++local0
        )
        = temp10 Max((+ 4 temp5) (* local0 (+ 4 temp2)))
        = temp11 (* temp16 (+ 4 temp1))
        = temp12 (/ (- 190 temp11) 2)
        = temp13 (/ (- 320 temp10) 2)
        = temp14 (+ temp12 temp11)
        = temp15 (+ temp13 temp10)
        = invWindow (self:window)
        (if (invWindow)
            (send (= invWindow (self:window)):
                top(temp12)
                left(temp13)
                right(temp15)
                bottom(temp14)
                open()
            )
        )
        = temp20 local0
        (if (temp0)
            = temp18 (+ 2 
                (if ((send invWindow:respondsTo(#yOffset)))
                    (send invWindow:yOffset)
                )(else
                    0
                )
)
            = temp17 (+ 4 
                (if ((send invWindow:respondsTo(#xOffset)))
                    (send invWindow:xOffset)
                )(else
                    0
                )
)
            = temp19 temp17
            = invFirst (self:first())
            (while (invFirst)
                = temp9 NodeValue(invFirst)
                (if (not (& (send temp9:signal) icDISABLED) and (send temp9:isKindOf(InventoryItem)))
                    (if (not & (send temp9:signal) $0080)
                        = temp6 CelWide((send temp9:view) (send temp9:loop) (send temp9:cel))
                        = temp7 CelHigh((send temp9:view) (send temp9:loop) (send temp9:cel))
                        (send temp9:
                            nsLeft(+ temp17 (/ (- temp2 temp7) 2))
                            nsTop(+ temp18 (/ (- temp1 temp7) 2))
                        )
                        (send temp9:
                            nsRight(+ (send temp9:nsLeft) temp6)
                            nsBottom(+ (send temp9:nsTop) temp7)
                        )
                        (if (--temp20)
                            = temp17 (+ temp17 temp2)
                        )(else
                            = temp20 local0
                            = temp18 (+ temp18 temp1)
                            = temp17 temp19
                        )
                    )(else
                        = temp17 (send temp9:nsLeft)
                        = temp18 (send temp9:nsTop)
                    )
                    (send temp9:show())
                    (if (== temp9 param2)
                        (send temp9:highlight())
                    )
                )
                = invFirst (self:next(invFirst))
            )
        )
        = temp17 (/ (- (- (send invWindow:right) (send invWindow:left)) temp5) 2)
        = temp11 (- (send invWindow:bottom) (send invWindow:top))
        = temp18 32767
        = invFirst (self:first())
        (while (invFirst)
            = temp9 NodeValue(invFirst)
            (if (not (send temp9:isKindOf(InventoryItem)))
                (send temp9:nsTop(0))
                = temp6 CelWide((send temp9:view) (send temp9:loop) (send temp9:cel))
                = temp7 CelHigh((send temp9:view) (send temp9:loop) (send temp9:cel))
                (if (not & (send temp9:signal) $0080)
                    (if (== temp18 32767)
                        = temp18 (- temp11 temp7)
                    )
                    (send temp9:
                        nsLeft(temp17)
                        nsTop(temp18)
                        nsBottom(temp11)
                        nsRight(+ temp17 temp6)
                    )
                )
                = temp17 (+ (send temp9:nsLeft) temp6)
                = temp18 (send temp9:nsTop)
                (send temp9:
                    signal(& (send temp9:signal) (bnot icDISABLED))
                    show()
                )
            )
            = invFirst (self:next(invFirst))
        )
        return 1
    )

)


/*
	This class is used for the default inventory management UI of the template game. It lets
	the user choose and manipulate inventory items.
*/
(class ScrollableInventory of InventoryBase
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
            (if ((IsObject(temp0) and not (& (send temp0:signal) icDISABLED)) and ((> (send temp0:nsLeft) -1) or not (send temp0:isKindOf(InventoryItem))))
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
            class   InventoryItem
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
            (if ((send temp9:isKindOf(InventoryItem)))
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
                (if (not (send temp9:isKindOf(InventoryItem)))
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
