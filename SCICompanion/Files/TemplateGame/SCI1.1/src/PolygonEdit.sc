/*
	This script contains the Sierra polygon editor. It's interesting from an historical purpose,
	but SCI Companion contains a much-improved polygon editor. It can be activated from the debug
	functionality (ALT-d) by pressing ALT-b.
*/
(version 2)
(include "sci.sh")
(include "game.sh")
(use "Main")
(use "Print")
(use "Polygon")
(use "SysWindow")
(use "File")
(use "Object")
(script POLYGONEDIT_SCRIPT)


(local
    local0
    theMainWin
    local2[40]
    theLsTop
    theLsLeft
    theLsBottom
    theLsRight
    local46[25] = ("EDITING" 0 0 "About" 0 0 "Map" 0 0 "Create" 0 0 "Type" 0 0 "Undo" 0 0 "Help" 0 0 "eXit" 120 )
    local71[22] = ("CREATING" 0 0 "About" 0 0 "Map" 0 0 "Done" 0 0 "Undo" 0 0 "Help" 0 0 "eXit" 120 )

)
(procedure (localproc_179a theTheTheLsLeft)
    (var temp0, theTheLsLeft, theTheLsTop)
    = theLsBottom 0
    = theLsRight theLsBottom
    = theLsTop 32767
    = theLsLeft theLsTop
    = temp0 0
    (while (< temp0 paramTotal)
        = theTheLsLeft theTheTheLsLeft[temp0]
        = theTheLsTop theTheTheLsLeft[(+ temp0 1)]
        (if (< theTheLsTop theLsTop)
            = theLsTop theTheLsTop
        )
        (if (> theTheLsTop theLsBottom)
            = theLsBottom theTheLsTop
        )
        (if (< theTheLsLeft theLsLeft)
            = theLsLeft theTheLsLeft
        )
        (if (> theTheLsLeft theLsRight)
            = theLsRight theTheLsLeft
        )
        = temp0 (+ temp0 2)
    )
    = theLsLeft (- theLsLeft 2)
    = theLsTop (- theLsTop 2)
    = theLsRight (+ theLsRight 2)
    = theLsBottom (+ theLsBottom 2)
)


(procedure (localproc_1819 param1 param2 param3 param4)
    return + (* (+ (/ param1 2) 1) (+ (/ param3 2) 1)) (* (+ (/ param2 2) 1) (+ (/ param4 2) 1))
)


(procedure (localproc_1844 param1 param2 param3 param4 param5 param6)
    (var temp0)
    return 
        (if ((<= 0 localproc_1819((- param3 param1) (- param4 param2) (- param5 param1) (- param6 param2))) and (<= 0 localproc_1819((- param1 param3) (- param2 param4) (- param5 param3) (- param6 param4))))
            = temp0 GetDistance(param1 param2 param3 param4)
            return 
                (if (temp0)
                    / Abs(localproc_1819((- param4 param2) (- param1 param3) (- param5 param1) (- param6 param2))) temp0
                )(else
                    0
                )
        )(else
            return Min(GetDistance(param5 param6 param1 param2) GetDistance(param5 param6 param3 param4))
        )
)


(procedure (localproc_18f1 param1 param2)
    (Print:
        width(240)
        font(999)
        mode(param2)
        addText(param1)
        addTitle("Polygon Editor 1.11")
        init()
    )
)

// Nodoc
(class ClickMenu of Object
    (properties
        text 0
        array 0
    )

    (method (init theArray)
        (var temp0, temp1, temp2, temp3, temp4[4], temp8[40], temp48, temp49)
        = text Memory(memALLOC_NONCRIT 81)
        Memory(memPOKE text 0)
        = temp1 (= array theArray)
        = temp48 0
        = temp0 0
        (while (= temp2 Memory(memPEEK temp1))
            StrCpy(@temp8 temp2)
            (if (not temp0)
                StrCat(@temp8 ": ")
            )
            StrCat(@temp8 " ")
            StrCat(text @temp8)
            TextSize(@temp4 @temp8 0 0)
            = temp48 (+ temp48 temp4[3])
            Memory(memPOKE + temp1 4 temp48)
            (if (not Memory(memPEEK + temp1 2))
                = temp49 StrAt(temp2 0)
                (if ((<= 65 temp49) and (<= (= temp49 StrAt(temp2 0)) 90))
                    = temp49 (+ temp49 32)
                )
                Memory(memPOKE + temp1 2 temp49)
            )
            ++temp0
            = temp1 (+ temp1 6)
        )
        DrawStatus(text)
    )


    (method (dispose)
        Memory(memFREE text)
        (super:dispose())
    )


    (method (handleEvent pEvent)
        (var theArray, temp1)
        (if (<> (send pEvent:type) evMOUSEBUTTON)
            return 0
        )
        (if (>= (send pEvent:y) 10)
            return 0
        )
        = theArray array
        = temp1 0
        (while (Memory(memPEEK theArray))
            (if ((< (send pEvent:x) Memory(memPEEK (+ theArray 4))) and temp1)
                (send pEvent:
                    type(4)
                    message(Memory(memPEEK + theArray 2))
                )
                return 0
            )
            ++temp1
            = theArray (+ theArray 6)
        )
        return (send pEvent:claimed(TRUE))
    )

)
(instance editMenu of ClickMenu
    (properties)

    (method (init)
        (super:init(@local46))
    )

)
(instance addMenu of ClickMenu
    (properties)

    (method (init)
        (super:init(@local71))
    )
)

// nodoc
(class Class_943_3
    (properties
    	x 0
        y 0
        underBits 0
    )

    (method (new)
        Clone(self)
    )


    (method (yourself)
        return self
    )


    (method (dispose)
        (if (underBits)
            UnLoad(133 underBits)
            = underBits 0
        )
        DisposeClone(self)
    )


    (method (draw param1 param2)
        (var temp0, temp1)
        (if (== local0 4)
            = temp0 -1
            = temp1 
                (if (== param2 1)
                    3
                )(else
                    4
                )
        )(else
            = temp0 0
            = temp1 -1
        )
        Graph(grDRAW_LINE y x (send param1:y) (send param1:x) temp0 -1 temp1)
    )


    (method (save param1)
        localproc_179a(x y (send param1:x) (send param1:y))
        (if (underBits)
            UnLoad(133 underBits)
        )
        = underBits Graph(grSAVE_BOX theLsTop theLsLeft theLsBottom theLsRight local0)
    )


    (method (restore)
        (if (underBits)
            Graph(grRESTORE_BOX underBits)
            = underBits 0
        )
    )
)

// nodoc
(class _EditablePolygon of List
    (properties
        elements 0
        size 0
        curNode 0
        curPt 0
        closed 0
        type $0002
        srcList 0
        closestPt 0
        lsTop 0
        lsLeft 0
        lsBottom 0
        lsRight 0
    )

    (method (add param1 param2 param3)
        (super:add((= curPt (send ((Class_943_3:new())):
                    x(param1)
                    y(param2)
                    yourself()
                )
)))
        (self:setCur(FindKey(elements curPt) param3))
    )


    (method (next param1)
        (var temp0)
        = temp0 (super:next(param1))
        (if (closed and not temp0)
            return (super:first())
        )
        return temp0
    )


    (method (prev param1)
        (var temp0)
        = temp0 (super:prev(param1))
        (if (closed and not temp0)
            return (super:last())
        )
        return temp0
    )


    (method (draw)
        (self:eachLineDo(#draw srcList))
    )


    (method (advance)
        (var temp0)
        (self:setCur((self:next(curNode))))
    )


    (method (retreat)
        (var temp0)
        (self:setCur((self:prev(curNode))))
    )


    (method (setCur theCurNode param2)
        (if (= curNode theCurNode)
            = curPt NodeValue((= curNode theCurNode))
            (if ((< paramTotal 2) or param2)
                (send gGame:setCursor(999 1 (send curPt:x) (send curPt:y)))
            )
        )
    )


    (method (setCurClosest param1)
        (self:setCur(FindKey(elements closestPt) param1))
    )


    (method (insertPt param1 param2)
        (var temp0)
        = temp0 (send ((Class_943_3:new())):
                x(param1)
                y(param2)
                yourself()
            )
        (self:addAfter(closestPt temp0))
        (self:setCur(FindKey(elements temp0)))
    )


    (method (deletePt)
        (var temp0)
        = temp0 (self:prev(curNode))
        (if (== curNode temp0)
            = temp0 0
        )
        (self:delete(curPt))
        (send curPt:dispose())
        (self:setCur(temp0))
    )


    (method (movePt param1 param2)
        (send curPt:
            x(param1)
            y(param2)
        )
    )


    (method (undo param1)
        (var temp0, temp1, temp2, temp3)
        (self:
            eachElementDo(#dispose)
            release()
        )
        = closed Memory(memPEEK param1)
        = param1 (+ param1 2)
        = temp1 Memory(memPEEK param1)
        = param1 (+ param1 2)
        = temp3 Memory(memPEEK param1)
        = param1 (+ param1 2)
        = temp2 0
        (while (< temp2 temp1)
            = param1 (+ param1 2)
            (self:add(Memory(memPEEK param1) Memory(memPEEK param1) 0))
            ++temp2
            = param1 (+ param1 2)
        )
        (self:setCur(FindKey(elements (self:at(temp3))) 0))
    )


    (method (saveForUndo)
        (var temp0, temp1, _EditablePolygonFirst, temp3)
        = temp0 Memory(memALLOC_NONCRIT (* 2 (+ (* 2 size) 3)))
        = temp1 temp0
        Memory(memPOKE temp1 closed)
        = temp1 (+ temp1 2)
        Memory(memPOKE temp1 size)
        = temp1 (+ temp1 2)
        Memory(memPOKE temp1 (self:indexOf(curPt)))
        = temp1 (+ temp1 2)
        = _EditablePolygonFirst (self:first())
        (while (_EditablePolygonFirst)
            = temp3 NodeValue(_EditablePolygonFirst)
            Memory(memPOKE temp1 (send temp3:x))
            = temp1 (+ temp1 2)
            Memory(memPOKE temp1 (send temp3:y))
            = _EditablePolygonFirst NextNode(_EditablePolygonFirst)
            = temp1 (+ temp1 2)
        )
        return temp0
    )


    (method (getDistToLine param1 param2 param3)
        (var _EditablePolygonLast, _EditablePolygonFirst, theClosestPt, temp3, temp4, temp5, the_EditablePolygonFirst)
        (if (< size 2)
            return (self:getDistToPt(param1 param2 rest param3))
        )
        = temp4 32767
        = _EditablePolygonFirst (self:first())
        = _EditablePolygonLast (self:last())
        (while (1)
            = theClosestPt NodeValue(_EditablePolygonFirst)
            = the_EditablePolygonFirst (self:next(_EditablePolygonFirst))
            = temp5 NodeValue(the_EditablePolygonFirst)
            = temp3 localproc_1844((send theClosestPt:x) (send theClosestPt:y) (send temp5:x) (send temp5:y) param1 param2)
            (if (< temp3 temp4)
                = temp4 temp3
                = closestPt theClosestPt
            )
            (if (== _EditablePolygonFirst _EditablePolygonLast)
                break
            )
            = _EditablePolygonFirst the_EditablePolygonFirst
        )
        return temp4
    )


    (method (getDistToPt param1 param2)
        (var _EditablePolygonLast, _EditablePolygonFirst, theClosestPt, temp3, temp4)
        = temp4 32767
        = _EditablePolygonFirst (self:first())
        = _EditablePolygonLast (self:last())
        (while (1)
            = theClosestPt NodeValue(_EditablePolygonFirst)
            = temp3 GetDistance(param1 param2 (send theClosestPt:x) (send theClosestPt:y))
            (if (< temp3 temp4)
                = temp4 temp3
                = closestPt theClosestPt
            )
            (if (== _EditablePolygonFirst _EditablePolygonLast)
                break
            )
            = _EditablePolygonFirst (self:next(_EditablePolygonFirst))
        )
        return temp4
    )


    (method (startRedraw)
        (var temp0, temp1, temp2, theCurPt, theCurPt_2)
        = temp0 (self:next(curNode))
        (if (temp0)
            = theCurPt NodeValue((= temp0 (self:next(curNode))))
        )(else
            = theCurPt curPt
        )
        = temp1 (self:prev(curNode))
        (if (temp1)
            = theCurPt_2 NodeValue((= temp1 (self:prev(curNode))))
        )(else
            = theCurPt_2 curPt
        )
        localproc_179a((send theCurPt_2:x) (send theCurPt_2:y) (send curPt:x) (send curPt:y) (send theCurPt:x) (send theCurPt:y))
        = lsTop theLsTop
        = lsLeft theLsLeft
        = lsBottom theLsBottom
        = lsRight theLsRight
    )


    (method (endRedraw)
        localproc_179a((send curPt:x) (send curPt:y) lsLeft lsTop lsRight lsBottom)
        Graph(grUPDATE_BOX theLsTop theLsLeft theLsBottom theLsRight local0)
    )


    (method (restore)
        (self:eachLineDo(#restore))
    )


    (method (save)
        (self:eachLineDo(#save))
    )


    (method (eachLineDo param1 param2)
        (var _EditablePolygonFirst, the_EditablePolygonFirst, temp2, temp3, _EditablePolygonLast)
        = _EditablePolygonFirst (self:first())
        = _EditablePolygonLast (self:last())
        (while ((<> _EditablePolygonFirst _EditablePolygonLast) or closed)
            = the_EditablePolygonFirst (self:next(_EditablePolygonFirst))
            = temp2 NodeValue(_EditablePolygonFirst)
            = temp3 NodeValue(the_EditablePolygonFirst)
            (send temp2:param1(temp3 rest param2))
            (if (== _EditablePolygonFirst _EditablePolygonLast)
                break
            )
            = _EditablePolygonFirst the_EditablePolygonFirst
        )
    )


    (method (writeObstacle)
        (var temp0, temp1, temp2, temp3, temp4)
        = temp1 Memory(memALLOC_NONCRIT (* size 4))
        = temp2 FirstNode(elements)
        = temp0 temp1
        (while (temp2)
            = temp3 NodeValue(temp2)
            Memory(memPOKE temp0 (send temp3:x))
            Memory(memPOKE + temp0 2 (send temp3:y))
            = temp2 NextNode(temp2)
            = temp0 (+ temp0 4)
        )
        (if (== srcList 1)
            (send gAltPolyList:add((send ((Polygon:new())):
                    type(type)
                    points(temp1)
                    size(size)
                    dynamic(1)
                    yourself()
                )
))
        )(else
            (send gRoom:addObstacle((send ((Polygon:new())):
                    type(type)
                    points(temp1)
                    size(size)
                    dynamic(1)
                    yourself()
                )
))
        )
    )


    (method (writeFile theFile param2)
        (var temp0, temp1, temp2, temp3, temp4[10], temp14)
        (if (== param2 srcList)
            (send theFile:writeString("\t\t\t((Polygon new:)
\n" "\t\t\t\ttype:\t\t" 
                (switch (type)
                    (case 0
                        "PTotalAccess"
                    )
                    (case 1
                        "PNearestAccess"
                    )
                    (case 2
                        "PBarredAccess"
                    )
                    (case 3
                        "PContainedAccess"
                    )
                )
 ",
\n"))
            (send theFile:writeString("\t\t\t\tinit:\t\t"))
            = temp14 1
            = temp0 17
            = temp3 FirstNode(elements)
            (while (temp3)
                = temp2 NodeValue(temp3)
                Format(@temp4 943 0 (send temp2:x) (send temp2:y))
                = temp1 (+ StrLen(@temp4) 1)
                = temp0 (+ temp0 temp1)
                (if (>= temp0 80)
                    (send theFile:writeString("
\n\t\t\t\t\t\t"))
                    = temp14 1
                    = temp0 (+ 17 temp1)
                )
                (if (not temp14)
                    (send theFile:writeString(" "))
                )
                (send theFile:writeString(@temp4))
                = temp14 0
                = temp3 NextNode(temp3)
            )
            (send theFile:writeString(",
\n"))
            (send theFile:writeString("\t\t\t\tyourself:
\n" "\t\t\t)
\n"))
        )
    )


    (method (getAccessType)
        (var temp0)
        = temp0 (Print:
                addTitle("Polygon access type")
                addButton(1 "Total" 0 0)
                addButton(2 " Near " 60 0)
                addButton(3 " Barred " 120 0)
                addButton(4 " Container " 195 0)
                first(type)
                init()
            )
        (if (temp0)
            = type (- (= temp0 (Print:
                        addTitle("Polygon access type")
                        addButton(1 "Total" 0 0)
                        addButton(2 " Near " 60 0)
                        addButton(3 " Barred " 120 0)
                        addButton(4 " Container " 195 0)
                        first(type)
                        init()
                    )
) 1)
        )
    )


    (method (check)
        (var temp0, temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9, temp10, temp11, temp12, temp13, temp14, temp15, temp16, temp17[40])
        (asm
            pushi   #first
            pushi   0
            self    4
            sat     temp0
code_050c:  lat     temp0
            bnt     code_0575
            pushi   1
            push    
            callk   NodeValue, 2
            sat     temp2
            pushi   1
            lst     temp0
            callk   NextNode, 2
            sat     temp15
code_0520:  lat     temp15
            bnt     code_056b
            pushi   1
            push    
            callk   NodeValue, 2
            sat     temp16
            pushi   #x
            pushi   0
            lat     temp2
            send    4
            push    
            pushi   #x
            pushi   0
            lat     temp16
            send    4
            eq?     
            bnt     code_0561
            pushi   #y
            pushi   0
            lat     temp2
            send    4
            push    
            pushi   #y
            pushi   0
            lat     temp16
            send    4
            eq?     
            bnt     code_0561
            pushi   1
            lst     temp15
            callk   PrevNode, 2
            sat     temp15
            pushi   #delete
            pushi   1
            lst     temp16
            self    6
            pushi   #dispose
            pushi   0
            lat     temp16
            send    4
code_0561:  pushi   1
            lst     temp15
            callk   NextNode, 2
            sat     temp15
            jmp     code_0520
code_056b:  pushi   1
            lst     temp0
            callk   NextNode, 2
            sat     temp0
            jmp     code_050c
code_0575:  ldi     0
            sat     temp4
            ldi     0
            sat     temp9
            ldi     0
            sat     temp7
            ldi     1
            sat     temp8
            pushi   #first
            pushi   0
            self    4
            sat     temp6
            sat     temp0
code_058e:  ldi     1
            bnt     code_061d
            pushi   1
            lst     temp0
            callk   NodeValue, 2
            sat     temp2
            pushi   #next
            pushi   1
            lst     temp0
            self    6
            sat     temp1
            pushi   1
            push    
            callk   NodeValue, 2
            sat     temp3
            pushi   4
            pushi   #x
            pushi   0
            lat     temp2
            send    4
            push    
            pushi   #y
            pushi   0
            lat     temp2
            send    4
            push    
            pushi   #x
            pushi   0
            lat     temp3
            send    4
            push    
            pushi   #y
            pushi   0
            lat     temp3
            send    4
            push    
            callk   ATan, 8
            sat     temp4
            lat     temp8
            not     
            bnt     code_0602
            lst     temp4
            lat     temp9
            sub     
            sat     temp5
            push    
            ldi     180
            gt?     
            bnt     code_05eb
            lst     temp5
            ldi     360
            sub     
            sat     temp5
            jmp     code_05fb
code_05eb:  lst     temp5
            ldi     65356
            lt?     
            bnt     code_05fb
            lst     temp5
            ldi     360
            add     
            sat     temp5
code_05fb:  lst     temp7
            lat     temp5
            add     
            sat     temp7
code_0602:  lat     temp4
            sat     temp9
            lst     temp0
            lat     temp6
            eq?     
            bnt     code_0612
            lat     temp8
            not     
            bnt     code_0612
code_0612:  ldi     0
            sat     temp8
            lat     temp1
            sat     temp0
            jmp     code_058e
code_061d:  pTos    type
            ldi     3
            eq?     
            bnt     code_0629
            lat     temp7
            neg     
            sat     temp7
code_0629:  lst     temp7
            ldi     65176
            eq?     
            bnt     code_06b2
            pushi   #first
            pushi   0
            self    4
            sat     temp0
            pushi   #last
            pushi   0
            self    4
            sat     temp10
code_0640:  lst     temp0
            lat     temp10
            ne?     
            bnt     code_06d6
            lst     temp0
            pushi   1
            lst     temp10
            callk   NextNode, 2
            ne?     
            bnt     code_06d6
            pushi   1
            lst     temp0
            callk   NodeValue, 2
            sat     temp2
            pushi   1
            lst     temp10
            callk   NodeValue, 2
            sat     temp11
            pushi   #x
            pushi   0
            lat     temp2
            send    4
            sat     temp12
            pushi   #y
            pushi   0
            lat     temp2
            send    4
            sat     temp13
            pushi   #x
            pushi   1
            pushi   #x
            pushi   0
            lat     temp11
            send    4
            push    
            lat     temp2
            send    6
            pushi   #y
            pushi   1
            pushi   #y
            pushi   0
            lat     temp11
            send    4
            push    
            lat     temp2
            send    6
            pushi   #x
            pushi   1
            lst     temp12
            lat     temp11
            send    6
            pushi   #y
            pushi   1
            lst     temp13
            lat     temp11
            send    6
            pushi   1
            lst     temp0
            callk   NextNode, 2
            sat     temp0
            pushi   1
            lst     temp10
            callk   PrevNode, 2
            sat     temp10
            jmp     code_0640
            jmp     code_06d6
code_06b2:  lst     temp7
            ldi     360
            ne?     
            bnt     code_06d6
            pushi   5
            lea     @temp17
            push    
            pushi   943
            pushi   1
            pTos    name
            lst     temp7
            callk   Format, 10
            pushi   1
            lea     @temp17
            push    
            calle   Prints, 2
code_06d6:  ret     
        )
    )

)
(instance readObstacle of Code
    (properties)

    (method (doit param1 param2)
        (var temp0, temp1, polyEditAdd)
        = polyEditAdd (PolyEdit:add())
        = temp0 0
        = temp1 (send param1:points)
        (while (< temp0 (send param1:size))
            (send polyEditAdd:
                add(Memory(memPEEK temp1) Memory(memPEEK + temp1 2) 0)
                type((send param1:type))
                srcList(param2)
            )
            ++temp0
            = temp1 (+ temp1 4)
        )
        (send polyEditAdd:closed(1))
    )
)

/*
	This is an in-game polygon editor. It is not essential for the template
	game, but it is included for historical purposes.

	Usage::

		(PolyEdit:doit())
*/
(class PolyEdit of List
    (properties
        elements 0
        size 0
        curPolygon 0
        x 0
        y 0
        state $0000
        isMouseDown 0
        curMenu 0
        undoPrvPoly 0
        undoPoly 0
        undoPolyBuf 0
        undoX 0
        undoY 0
        undoState 0
    )

    (method (init)
        DrawPic((send gRoom:curPic) 100 dpCLEAR)
        (if (<> gPicNumber -1)
            DrawPic(gPicNumber 100 dpNO_CLEAR)
        )
        (send gAddToPics:doit())
        (send gCast:eachElementDo(#stopUpd))
        Animate((send gCast:elements) 0)
        = theMainWin gWindow
        = gWindow SysWindow
        = local0 1
        (send gGame:setCursor(999 1))
        (self:readObstacles())
        (self:changeState(
            (if (size)
                1
            )(else
                0
            )
))
        (self:draw())
    )


    (method (doit)
        (var newEvent, temp1[100])
        (self:init())
        (while (TRUE)
            = newEvent (Event:new())
            (if (not 
            (if (curMenu)
                (send curMenu:handleEvent(newEvent))
            )
)
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
        (self:writeObstacles())
        (if (curMenu)
            (send curMenu:dispose())
            = curMenu 0
        )
        (if (undoPolyBuf)
            Memory(memFREE undoPolyBuf)
            = undoPolyBuf 0
        )
        DrawStatus(" " 0 0)
        DrawStatus(0)
        (send gCast:eachElementDo(#startUpd))
        Animate((send gCast:elements) 0)
        (self:eachElementDo(#draw))
        (if ((Print:
            addText("Erase polygon outlines?")
            addButton(1 "Yes " 30 12)
            addButton(0 " No " 85 12)
            init()
        )
)
            DrawPic((send gRoom:curPic) 100 dpCLEAR)
            (if (<> gPicNumber -1)
                DrawPic(gPicNumber 100 dpNO_CLEAR)
            )
            (send gAddToPics:doit())
        )
        = gWindow theMainWin
        DisposeScript(FILE_SCRIPT)
        (super:dispose())
        DisposeScript(POLYGONEDIT_SCRIPT)
    )


    (method (add)
        (super:add((= curPolygon (_EditablePolygon:new()))))
        return curPolygon
    )


    (method (delete param1)
        (var theCurPolygon)
        = theCurPolygon curPolygon
        (self:advanceRetreat(128 65))
        (if (== curPolygon theCurPolygon)
            = curPolygon 0
        )
        (super:delete(theCurPolygon rest param1))
        (send theCurPolygon:dispose())
    )


    (method (handleEvent pEvent)
        (var pEventModifiers, theX, theY, temp3[20])
        = theX x
        = theY y
        = x (send pEvent:x)
        = y (send pEvent:y)
        (switch ((send pEvent:type))
            (case evNULL
                (if (curPolygon)
                    (if ((isMouseDown and not IsOneOf(state 0 2)) and (> (+ Abs((- theX x)) Abs((- theY y))) 1))
                        (if (<> state 3)
                            (self:saveForUndo())
                        )
                        (self:changeState(2))
                    )
                    (if (IsOneOf(state 2 0) and ((<> theX x) or (<> theY y)))
                        (self:movePt(x y))
                    )
                    (if (== state 2)
                        DrawStatus(Format(@temp3 943 2 x y))
                    )
                )
            )
            (case evMOUSEBUTTON
                = pEventModifiers (send pEvent:modifiers)
                = isMouseDown 1
                (if (& pEventModifiers emCTRL)
                    (if (== state 0)
                        (self:finishAdding())
                        = isMouseDown 0
                    )(else
                        (self:insertPt())
                    )
                )(else
                    (if (& pEventModifiers emSHIFT)
                        (if (<> state 0)
                            (self:deletePt())
                        )
                        = isMouseDown 0
                    )(else
                        (if (== state 0)
                            (self:addPt())
                        )(else
                            (self:selectPt())
                        )
                    )
                )
            )
            (case evMOUSERELEASE
                = isMouseDown 0
                (if (IsOneOf(state 2 3))
                    (self:changeState(1))
                )
            )
            (case evKEYBOARD
                (switch ((send pEvent:message))
                    (case 63
                        (send pEvent:message(104))
                    )
                    (case KEY_PAUSE
                        (send pEvent:message(120))
                    )
                    (case KEY_F2
                        (send pEvent:message(12032))
                    )
                    (case KEY_F4
                        (send pEvent:message(11776))
                    )
                )
                (switch ((send pEvent:message))
                    (case KEY_TAB
                        (if ((== state 1) and curPolygon)
                            (self:advanceRetreat(65 124))
                        )
                    )
                    (case KEY_SHIFTTAB
                        (if ((== state 1) and curPolygon)
                            (self:advanceRetreat(128 127))
                        )
                    )
                    (case KEY_SPACE
                        (if ((== state 1) and curPolygon)
                            (send curPolygon:advance())
                        )
                    )
                    (case JOY_UPLEFT
                        (if ((== state 1) and curPolygon)
                            (send curPolygon:retreat())
                        )
                    )
                    (case KEY_c
                        (if (== state 1)
                            (self:changeState(0))
                            = curPolygon 0
                        )
                    )
                    (case 116
                        (if (curPolygon and (== state 1))
                            (send curPolygon:getAccessType())
                        )
                    )
                    (case 100
                        (if (== state 1)
                            (if (curPolygon)
                                (self:deletePt())
                            )
                        )(else
                            (if (== state 0)
                                (self:finishAdding())
                            )
                        )
                    )
                    (case 104
                        (switch (state)
                            (case 0
                                localproc_18f1("           CREATING POLYGON
\n
\nClick to create each corner of the polygon, then choose Done from the menu to finish.  You can also press Esc or Ctrl-click to finish.
\n
\nTo UNDO a corner, choose Undo.
\n
\nTo change MAP displayed (visual or control), choose Map.
\n
\nTo EXIT the Polygon Editor, choose eXit or press Ctrl-S." 0)
                            )
                            (case 1
                                localproc_18f1("             EDITING POLYGON
\n
\nTo MOVE a corner, click on it and drag it to the new position.
\nTo INSERT a new corner, Ctrl-click to create it, then drag it to the correct position.
\nTo DELETE a corner, Shift-click on it.
\nTo UNDO an action, choose Undo from the menu.
\nTo CREATE a new polygon, choose Create.
\nTo change a polygon's TYPE (Total, Near or Barred), choose Type.
\nTo change MAP displayed (visual or control), choose Map.
\nTo EXIT the Polygon Editor, choose eXit or press Ctrl-S.
\n
\nIn addition to using the mouse, you can use Space and BackSpace to select corners and Tab and BackTab to select polygons. " 0)
                            )
                        )
                    )
                    (case 117
                        (self:undo())
                    )
                    (case 109
                        (self:showMap(-1))
                    )
                    (case 12032
                        (self:showMap(1))
                    )
                    (case 11776
                        (self:showMap(4))
                    )
                    (case 97
                        localproc_18f1(" by
\n
\nMark Wilden
\n
\nOriginal program by Chad Bye " 1)
                    )
                    (case 114
                        (if (== state 1)
                            (self:draw())
                        )
                    )
                    (case 120
                        return (self:exit())
                    )
                    (case KEY_ESCAPE
                        (if (== state 0)
                            (self:finishAdding())
                        )
                    )
                )
            )
        )
        return 0
    )


    (method (changeState newState)
        (if (curMenu)
            (send curMenu:dispose())
        )
        = curMenu 
            (switch ((= state newState))
                (case 0
                    addMenu
                )
                (case 1
                    editMenu
                )
                (case 2
                    0
                )
                (default 
                    0
                )
            )
        (if (curMenu)
            (send curMenu:init())
        )
    )


    (method (draw)
        (self:eachElementDo(#restore))
        (self:eachElementDo(#save))
        (self:eachElementDo(#draw))
        Graph(grUPDATE_BOX 0 0 190 320 local0)
    )


    (method (select param1 param2)
        (var temp0, theCurPolygon, temp2, theTheCurPolygon, temp4)
        = temp0 32767
        = theCurPolygon 0
        = temp4 FirstNode(elements)
        (while (temp4)
            = theTheCurPolygon NodeValue(temp4)
            = temp2 (send theTheCurPolygon:param1(x y))
            (if (< temp2 temp0)
                = temp0 temp2
                = theCurPolygon theTheCurPolygon
            )
            = temp4 NextNode(temp4)
        )
        (send (= curPolygon theCurPolygon):setCurClosest(param2))
    )


    (method (selectPt)
        (var newEvent)
        (self:select(483 1))
        = newEvent (Event:new())
        GlobalToLocal(newEvent)
        = x (send newEvent:x)
        = y (send newEvent:y)
        (send newEvent:dispose())
    )


    (method (addPt)
        (self:saveForUndo())
        (if (not curPolygon)
            (self:add())
            (send curPolygon:add(x y 0))
        )
        (send curPolygon:add(x y))
    )


    (method (finishAdding)
        (var polyEditFirst)
        (if (curPolygon)
            (send curPolygon:closed(1))
            (if (> (send curPolygon:size) 1)
                (send curPolygon:
                    deletePt((send curPolygon:last()))
                    advance()
                )
            )
            (self:draw())
            (send curPolygon:getAccessType())
        )(else
            = polyEditFirst (self:first())
            (if (not polyEditFirst)
                = curPolygon 0
            )(else
                = curPolygon NodeValue(polyEditFirst)
                (self:draw())
            )
        )
        (if (curPolygon)
            (self:changeState(1))
        )
    )


    (method (movePt param1 param2)
        (send curPolygon:startRedraw())
        (self:eachElementDo(#restore))
        (send curPolygon:movePt(param1 param2))
        (self:eachElementDo(#save))
        (self:eachElementDo(#draw))
        (send curPolygon:endRedraw())
    )


    (method (insertPt)
        (self:eachElementDo(#restore))
        (self:select(482 0))
        (self:saveForUndo())
        (send curPolygon:insertPt(x y))
        (self:changeState(3))
        (self:eachElementDo(#save))
        (self:eachElementDo(#draw))
        Graph(grUPDATE_BOX 0 0 190 320 local0)
    )


    (method (deletePt)
        (var temp0)
        (self:eachElementDo(#restore))
        (self:select(483 0))
        (self:saveForUndo())
        (send curPolygon:deletePt())
        (if (not (send curPolygon:size))
            (self:delete(curPolygon))
            (if (not size)
                (self:changeState(0))
            )
        )
        (self:eachElementDo(#save))
        (self:eachElementDo(#draw))
        Graph(grUPDATE_BOX 0 0 190 320 local0)
    )


    (method (undo)
        (var temp0, theUndoPoly, theUndoPrvPoly, theUndoPolyBuf, theUndoX, theUndoY, theUndoState)
        (if (not undoPoly)
            Prints("Nothing to undo")
            return 
        )
        = theUndoPoly undoPoly
        = theUndoPrvPoly undoPrvPoly
        = theUndoPolyBuf undoPolyBuf
        = theUndoX undoX
        = theUndoY undoY
        = theUndoState undoState
        (self:saveForUndo(0))
        (self:eachElementDo(#restore))
        (if (= curPolygon theUndoPoly)
            (if (not (self:contains(curPolygon)))
                = curPolygon (self:add())
                (if (theUndoPrvPoly)
                    (self:addAfter(theUndoPrvPoly curPolygon))
                )(else
                    (self:addToFront(curPolygon))
                )
            )
            (send curPolygon:undo(theUndoPolyBuf))
        )(else
            = curPolygon (self:add())
        )
        Memory(memFREE theUndoPolyBuf)
        = x theUndoX
        = y theUndoY
        (self:changeState(theUndoState))
        (self:eachElementDo(#save))
        (self:eachElementDo(#draw))
        Graph(grUPDATE_BOX 0 0 190 320 local0)
        (send gGame:setCursor(999 1 x y))
    )


    (method (saveForUndo param1)
    	(var tempNode, thePrevNode)
        (if (= undoPoly curPolygon)
            = undoPrvPoly (self:prev((= undoPoly curPolygon)))
            
            (if ((not paramTotal or param1) and undoPolyBuf)
                Memory(memFREE undoPolyBuf)
            )
            = undoPolyBuf (send curPolygon:saveForUndo())
        )
        = undoX x
        = undoY y
        = undoState state
    )


    (method (advanceRetreat param1 param2)
        (var temp0, temp1)
        = temp1 FindKey(elements curPolygon)
        = temp0 (self:param1(temp1))
        (if (not temp0 and not (= temp0 (self:param2(temp1))))
            = temp0 temp1
        )
        = curPolygon NodeValue(temp0)
        (send curPolygon:setCur((send curPolygon:curNode)))
    )


    (method (readObstacles)
        (if ((send gRoom:obstacles))
            (send ((send gRoom:obstacles)):eachElementDo(#perform readObstacle 0))
        )
        (if (gAltPolyList)
            (send gAltPolyList:eachElementDo(#perform readObstacle 1))
        )
    )


    (method (writeObstacles)
        (if ((send gRoom:obstacles))
            (send ((send gRoom:obstacles)):
                eachElementDo(#dispose)
                release()
            )
        )
        (self:eachElementDo(#writeObstacle))
    )


    (method (showMap param1)
        (if (== param1 -1)
            (if (== local0 1)
                = param1 4
            )(else
                = param1 1
            )
        )
        (if (<> local0 param1)
            (self:eachElementDo(#restore))
            = local0 param1
            (self:eachElementDo(#save))
            (self:eachElementDo(#draw))
            Graph(grUPDATE_BOX 0 0 190 320 local0)
        )
    )


    (method (exit)
        (var temp0[100], temp100, newFile, openMode)
        (if (== state 0)
            (self:finishAdding())
        )
        (self:showMap(1))
        (if (not curPolygon)
            return 1
        )
        
        // This uses Atan, which Scumm doesn't support.
        // (self:eachElementDo(#check))
        
        
        (if (not local2)
            Format(@local2 943 3 (send gRoom:curPic))
        )
        = temp100 (Print:
                addTitle("Save Polygons")
                addText("File:")
                addEdit(@local2 25 50 0 @local2)
                addButton(1 " Save " 5 12)
                addButton(2 "Abandon" 70 12)
                addButton(0 "Cancel" 150 12)
                init()
            )
        (if (not temp100)
            return 0
        )
        (if (== temp100 2)
            return 1
        )
        (if (FileIO(fiEXISTS @local2))
            Format(@temp0 943 4 @local2)
            = temp100 (Print:
                    width(210)
                    addText(@temp0)
                    addButton(1 "Replace" 5 12)
                    addButton(2 "Append" 85 12)
                    addButton(0 "Cancel" 150 12)
                    init()
                )
            (if (not temp100)
                return 0
            )
        )
        = openMode 
            (if (== temp100 1)
                fCREATE		// Replace
            )(else
                fOPENCREATE	// Append
            )
        = newFile (File:new())
        (if (not (send newFile:
            name(@local2)
            open(openMode)
				 )
			)
            Format(@temp0 943 5 (send newFile:name))
            Prints(@temp0)
            (send newFile:dispose())
            return 0
        )
        (send newFile:writeString(Format(@temp0 943 6 "Polygon Editor 1.11")))
        (send newFile:writeString(Format(@temp0 943 7 "Dynamic Obstacles" (send gRoom:curPic))))
        (send newFile:writeString("\t\t(curRoom addObstacle:
\n"))
        (self:eachElementDo(#writeFile newFile 0))
        (send newFile:writeString("\t\t)
\n
\n"))
        (send newFile:writeString("\t\t(altPolyList add:
\n"))
        (self:eachElementDo(#writeFile newFile 1))
        (send newFile:writeString("\t\t)
\n"))
        (send newFile:dispose())
        return 1
    )

)
