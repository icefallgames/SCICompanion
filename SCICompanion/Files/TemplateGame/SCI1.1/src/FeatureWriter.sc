/*
	This script contains Sierra's FeatureWriter functionality and related classes. It can be invoked by entering debug
	mode (ALT-d) then pressing ALT-w.
*/
(version 2)
(include "sci.sh")
(include "game.sh")
(use "Main")
(use "Controls")
(use "Print")
(use "DialogControls")
(use "Feature")
(use "SysWindow")
(use "File")
(use "Actor")
(use "Obj")
(script FEATUREWRITER_SCRIPT)


(local
    local0[50]
    local50[30]
    local80[100]
    local180[100]
    local280[100]
    local380 =     " WALK            "
    local381[6] = (" LOOK            " " DO              " " TALK            " " ASK             " " HELP            " "" )
    local387
    local388 =     1
    local389
    local390
    local391
    theWindow
    newFile

)
(procedure (localproc_0286 param1)
    (send param1:sightAngle(GetNumber("sight angle?" 40)))
)


(procedure (localproc_029b param1)
    (var newEvent, temp1, temp2, newEventY, newEventX, newEventY_2, newEventX_2)
    TextPrint("Click left mouse button on top left corner")
    (while (<> (send ((= newEvent (Event:new()))):type) 1)
        (send newEvent:dispose())
    )
    GlobalToLocal(newEvent)
    = newEventY (send newEvent:y)
    = newEventX (send newEvent:x)
    (send newEvent:dispose())
    TextPrint("Click left mouse button on bottom right corner")
    (while (<> (send ((= newEvent (Event:new()))):type) 1)
        (send newEvent:dispose())
    )
    GlobalToLocal(newEvent)
    = newEventY_2 (send newEvent:y)
    = newEventX_2 (send newEvent:x)
    (send newEvent:dispose())
    = temp1 (+ (/ (- newEventX_2 newEventX) 2) newEventX)
    = temp2 (+ (/ (- newEventY_2 newEventY) 2) newEventY)
    (send param1:
        x(temp1)
        y(temp2)
        nsLeft(newEventX)
        nsTop(newEventY)
        nsBottom(newEventY_2)
        nsRight(newEventX_2)
    )
    (if (local388)
        Graph(grDRAW_LINE newEventY newEventX newEventY newEventX_2 1 0)
        Graph(grDRAW_LINE newEventY_2 newEventX newEventY_2 newEventX_2 1 0)
        Graph(grDRAW_LINE newEventY newEventX newEventY_2 newEventX 1 0)
        Graph(grDRAW_LINE newEventY newEventX_2 newEventY_2 newEventX_2 1 0)
        Graph(grUPDATE_BOX newEventY newEventX + newEventY_2 1 + newEventX_2 1 1)
    )
)


(procedure (localproc_03c6 param1)
    (var newEvent)
    (send param1:
        view(GetNumber("View?" (send gRoom:curPic)))
        loop(GetNumber("Loop?" 0))
        cel(GetNumber("Cel?" 0))
        signal(16400)
        priority(15)
        init()
    )
    (if ((send param1:respondsTo(#illegalBits)))
        (send param1:illegalBits(0))
    )
    (while (<> (send ((= newEvent (Event:new()))):type) 1)
        GlobalToLocal(newEvent)
        (send param1:posn((send newEvent:x) (send newEvent:y)))
        Animate((send gOldCast:elements) 0)
        (send newEvent:dispose())
    )
    (send newEvent:dispose())
)


(procedure (localproc_046c param1)
    (var newEvent, newEventX, newEventY, temp3[10], temp13)
    (if ((Print:
        addText("Where should the approach point be?")
        addButton(1 "Select with mouse" 0 20)
        addButton(0 "Default to x, y" 0 34)
        init()
    )
)
        (while (<> (send ((= newEvent (Event:new()))):type) 1)
            (send newEvent:dispose())
        )
        GlobalToLocal(newEvent)
        = newEventX (send newEvent:x)
        = newEventY (send newEvent:y)
        (send param1:
            approachX(newEventY)
            approachY(newEventY)
        )
        (send newEvent:dispose())
        Graph(grDRAW_LINE - newEventY 1 - newEventX 1 - newEventY 1 + newEventX 1 7)
        Graph(grDRAW_LINE newEventY - newEventX 1 newEventY + newEventX 1 7)
        Graph(grDRAW_LINE + newEventY 1 - newEventX 1 + newEventY 1 + newEventX 1 7)
        Graph(grDRAW_LINE newEventY newEventX newEventY newEventX 0)
        Graph(grUPDATE_BOX - newEventY 1 - newEventX 1 + newEventY 2 + newEventX 2 1)
    )(else
        (send param1:
            approachX((send param1:x))
            approachY((send param1:y))
        )
    )
    = temp3 0
    = temp13 (Print:
            addText("How far away must ego" 0 1)
            addText("be before he tries to approach?" 0 12)
            addEdit(@temp3 5 -50 13)
            addButton(1 "Select with mouse" 0 32)
            addButton(0 "Always approach" 0 45)
            init()
        )
    (if (temp3)
        (send param1:approachDist(ReadNumber(@temp3)))
    )(else
        (if (not temp13)
            (send param1:approachDist(0))
        )(else
            (while (<> (send ((= newEvent (Event:new()))):type) 1)
                (send newEvent:dispose())
            )
            GlobalToLocal(newEvent)
            = newEventX (send newEvent:x)
            = newEventY (send newEvent:y)
            (send param1:approachDist(GetDistance((send param1:x) (send param1:y) newEventX newEventY)))
            (send newEvent:dispose())
            Graph(grDRAW_LINE - newEventY 1 - newEventX 1 - newEventY 1 + newEventX 1 28)
            Graph(grDRAW_LINE newEventY - newEventX 1 newEventY + newEventX 1 28)
            Graph(grDRAW_LINE + newEventY 1 - newEventX 1 + newEventY 1 + newEventX 1 28)
            Graph(grDRAW_LINE newEventY newEventX newEventY newEventX 52)
            Graph(grUPDATE_BOX - newEventY 1 - newEventX 1 + newEventY 2 + newEventX 2 1)
        )
    )
)


(procedure (localproc_06e6)
    = local391 (Print:
            addText("doVerb method?")
            addButton(1 "YES" 0 12)
            addButton(0 "NO" 50 12)
            init()
        )
)


(procedure (localproc_0714 param1)
    (send newFile:
        name(@gDebugFilename)
        writeString(param1)
        close()
    )
)

/*
	This is Sierra's FeatureWriter debug functionality. It is included for historical purposes and is not essential to the template game.

	Example usage::
	
		(FeatureWriter:doit())
*/
(class FeatureWriter
    (properties)

    (method (doit)
        (var temp0[15], temp15, newEvent)
        (send gGame:setCursor(999))
        = theWindow gWindow
        = gWindow wfWin
        (if (not local389)
            = temp0 0
            Format(@temp0 "%d.fea" (send gRoom:curPic))
            (if (not EditPrint(@temp0 30 "Enter path and filename"))
                return 
            )(else
                Format(@gDebugFilename @temp0)
                = local388 (Print:
                        addText("Outline Features?")
                        addTitle("Feature Write V1.0")
                        addButton(1 "YES" 0 12)
                        addButton(0 "NO" 50 12)
                        init()
                    )
                = local387 (Print:
                        addText("Display code to screen? (but not doVerb)")
                        addTitle("Feature Write V1.0")
                        addButton(0 "NO" 0 18)
                        addButton(1 "YES" 50 18)
                        init()
                    )
                = local389 1
            )
        )
        = local390 (Print:
                addText("Class?")
                addTitle("Feature Writer V1.0")
                addButton(Feature "Feature" 0 12)
                addButton(View "View" 73 12)
                addButton(Prop "Prop" 113 12)
                addButton(Actor "Actor" 153 12)
                init()
            )
        (if (not local390)
            return 
        )
        = temp15 (send local390:new())
        = local0 0
        EditPrint(@local0 30 "Name?")
        = local50 0
        EditPrint(@local50 16 "Noun?")
        localproc_0286(temp15)
        (if (== local390 Feature)
            localproc_029b(temp15)
        )(else
            localproc_03c6(temp15)
        )
        localproc_046c(temp15)
        (if ((Print:
            addText("Z property")
            addTitle("Feature Writer V1.0")
            addButton(0 "NO" 0 12)
            addButton(1 "YES" 50 12)
            init()
        )
)
            (Print:
                addText("Click mouse on object's projection")
                addText("onto the ground" 0 12)
                init()
            )
            (while (<> (send ((= newEvent (Event:new()))):type) 1)
                (send newEvent:dispose())
            )
            GlobalToLocal(newEvent)
            (send temp15:z((- (send newEvent:y) (send temp15:y))))
            (send temp15:y((send newEvent:y)))
            (send newEvent:dispose())
        )
        localproc_06e6()
        (FeatureSaver:doit(temp15))
        = gWindow theWindow
    )
)

// Nodoc
(class FeatureSaver
    (properties)

    (method (doit param1)
        (var temp0[400], temp400[40], temp440[50], temp490, temp491, temp492[60], temp552[40], temp592[12], temp604, temp605, temp606, temp607[20], newFeature, temp628)
        (if (FileIO(fiEXISTS @gDebugFilename))
            Format(@temp492 "The file '%s' already exists" @gDebugFilename)
            = temp491 (Print:
                    addText(@temp492)
                    addButton(1 "Replace" 0 20)
                    addButton(2 "Append" 73 20)
                    addButton(0 "Cancel" 133 20)
                    init()
                )
            (if (not temp491)
                return 0
            )
        )
        = temp490 
            (if (== temp491 1)
                2
            )(else
                0
            )
        = newFile (File:new())
        (if (not (send newFile:
            name(@gDebugFilename)
            open(temp490)
        )
			)
            Format(@temp0 "Error opening '%s'" @gDebugFilename)
            TextPrint(@temp0)
            (send newFile:dispose())
            return 0
        )
        = temp0 0
        (if ((send param1:isMemberOf(Feature)))
            Format(@temp400 " \t\tnsLeft\t\t\t%d
\n\t\tnsTop\t\t\t\t%d
\n\t\tnsBottom\t\t\t%d
\n\t\tnsRight\t\t\t%d
\n" (send param1:nsLeft) (send param1:nsTop) (send param1:nsBottom) (send param1:nsRight))
            Format(@temp592 "..\msg\%d.shm" (send gRoom:curPic))
            (shmFile:name(@temp592))
            (if (not (shmFile:open(fOPENFAIL)))
                = temp604 0
            )(else
                = temp605 0
                (while (<= temp605 StrLen(@local50))
                    (if ((< 96 StrAt(@local50 temp605)) and (< StrAt(@local50 temp605) 123))
                        = temp628 (- StrAt(@local50 temp605) 32)
                        StrAt(@local50 temp605 temp628)
                    )
                    ++temp605
                )
                = temp604 0
                (while (<> FileIO(fiREAD_STRING @temp552 80 (shmFile:handle)) -1)
                    (if (not StrCmp(@temp552 "(define" 6))
                        = temp605 0
                        (while (<= temp605 40)
                            = temp552[temp605] temp552[(+ temp605 4)]
                            ++temp605
                        )
                        (if (not StrCmp(@temp552 @local50 StrLen(@local50)))
                            = temp606 0
                            = temp605 (+ (/ StrLen(@local50) 2) 1)
                            (while (< temp605 20)
                                = temp607[temp606] temp552[temp605]
                                ++temp606
                                ++temp605
                            )
                            break
                        )
                    )
                    (if (not StrCmp(@temp552 "; CASES"))
                        break
                    )
                )
                (shmFile:close())
            )
            = newFeature (Feature:new())
            (send newFeature:
                init()
                setName(@local0)
                nsLeft((send param1:nsLeft))
                nsTop((send param1:nsTop))
                nsBottom((send param1:nsBottom))
                nsRight((send param1:nsRight))
                x((send param1:x))
                y((send param1:y))
                z((send param1:z))
                heading((send param1:heading))
                sightAngle((send param1:sightAngle))
                approachX((send param1:approachX))
                approachY((send param1:approachY))
                noun(temp604)
            )
        )(else
            Format(@temp400 " \t\tview\t\t\t%d
\n\t\tloop\t\t\t%d
\n\t\tcel\t\t\t%d
\n" (send param1:view) (send param1:loop) (send param1:cel))
        )
        Format(@temp440 " \t\tapproachX\t\t%d
\n\t\tapproachY\t\t%d
\n\t\tapproachDist\t%d
\n\t\t_approachVerbs\t$%x
\n" (send param1:approachX) (send param1:approachY) (send param1:approachDist) (send param1:_approachVerbs))
        Format(@temp0 " 
\n(instance %s of %s
\n\t(properties
\n\t\tx\t\t\t\t\t%d
\n\t\ty\t\t\t\t\t%d
\n\t\tz\t\t\t\t\t%d
\n\t\theading\t\t\t%d
\n%s \t\tsightAngle\t\t%d
\n%s \t\tnoun\t\t\t\t%s
\n\t)
\n" @local0 (send ((send param1:{-super-})):name) (send param1:x) (send param1:y) (send param1:z) (send param1:heading) @temp400 (send param1:sightAngle) @temp440 @local50)
        (if (local387)
            (Print:
                font(999)
                addText(@temp0)
                addTitle("Feature Writer V1.0")
                init()
            )
        )
        localproc_0714(@temp0)
        (if (local391)
            Format(@temp0 " \t(method (doVerb theVerb)
\n\t\t(switch theVerb
\n")
            localproc_0714(@temp0)
            (if (local180[0])
                Format(@temp0 " \t\t\t(LOOK
\n\t\t\t)
\n" @local180)
                localproc_0714(@temp0)
            )
            (if (local80[0])
                Format(@temp0 " \t\t\t(DO
\n\t\t\t)
\n" @local80)
                localproc_0714(@temp0)
            )
            (if (local280[0])
                Format(@temp0 " \t\t\t(TALK
\n\t\t\t)
\n" @local280)
                localproc_0714(@temp0)
            )
            Format(@temp0 " \t\t\t(else
\n\t\t\t\t(super doVerb: theVerb)
\n\t\t\t)
\n\t\t)
\n\t)
\n")
            localproc_0714(@temp0)
        )
        StrCpy(@temp0 ")
\n")
        localproc_0714(@temp0)
        (if ((send param1:isMemberOf(Feature)))
            (send param1:dispose())
        )(else
            (send param1:addToPic())
        )
        (send newFile:
            close()
            dispose()
        )
        DisposeScript(FILE_SCRIPT)
        return DisposeScript(FEATUREWRITER_SCRIPT)
    )


    (method (writeList param1)
        (send param1:eachElementDo(#perform self))
        (FeatureWriter:doit())
        DisposeScript(FEATUREWRITER_SCRIPT)
    )

)
(instance selectorI of DSelector
    (properties
        x 18
    )

    (method (handleEvent pEvent)
        (var pEventType, pEventMessage)
        (super:handleEvent(pEvent))
        = pEventType (send pEvent:type)
        = pEventMessage (send pEvent:message)
        (if (((== pEventType evMOUSEBUTTON) and (send pEvent:claimed)) or ((== pEventType evKEYBOARD) and (== pEventMessage KEY_SPACE)))
            (if (== StrAt(cursor 0) 62)
                StrAt(cursor 0 32)
            )(else
                StrAt(cursor 0 62)
            )
            (self:draw())
            (send pEvent:claimed(TRUE))
        )
        (send pEvent:claimed)
    )

)
(instance clearBut of DButton
    (properties
        state $0001
        text "Clear"
    )

    (method (doit)
        (var temp0)
        = temp0 0
        (while (< temp0 6)
            StrAt(local380 * temp0 18 32)
            ++temp0
        )
        (selectorI:draw())
    )

)
(instance allBut of DButton
    (properties
        state $0001
        value 2
        text "  All  "
    )

    (method (doit)
        (var temp0)
        = temp0 0
        (while (< temp0 6)
            StrAt(local380 * temp0 18 62)
            ++temp0
        )
        (selectorI:draw())
    )

)
(instance doneBut of DButton
    (properties
        value 1
        text " Done "
    )
)
(instance wfWin of Window
    (properties)
)
(instance shmFile of File
    (properties)
)
