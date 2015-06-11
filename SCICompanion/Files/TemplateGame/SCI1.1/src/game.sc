(version 2)
(include "sci.sh")
(use "Main")
(use "Print")
(use "Polygon")
(use "Sound")
(use "SRDialog")
(use "Cycle")
(use "InvI")
(use "User")
(use "Obj")
(script 994)



(procedure (localproc_0e32 param1)
    (var temp0, temp1[40], temp41[40], temp81, temp82[40], temp122[10], temp132[5])
    = temp81 Memory(memALLOC_CRIT 150)
    = temp0 1
    DeviceInfo(0 gSaveDir @temp1)
    DeviceInfo(1 @temp41)
    (if (DeviceInfo(3 @temp41) and (DeviceInfo(2 @temp1 @temp41) or not DeviceInfo(6 (send gGame:name))))
        Message(msgGET 994 6 0 0 1 @temp82)
        Message(msgGET 994 7 0 0 1 @temp122)
        Message(msgGET 994 8 0 0 1 @temp132)
        Format(temp81 @temp82 
            (if (param1)
                @temp122
            )(else
                @temp132
            )
 @temp1)
        Load(rsFONT gFont)
        DeviceInfo(4)
        Message(msgGET 994 2 0 0 1 @temp82)
        Message(msgGET 994 4 0 0 1 @temp122)
        Message(msgGET 994 5 0 0 1 @temp132)
        = temp0 
            (if (param1)
                (Print:
                    font(0)
                    addText(temp81)
                    addButton(1 @temp82 0 40)
                    addButton(0 @temp122 30 40)
                    addButton(2 @temp132)
                    init()
                )
            )(else
                (Print:
                    font(0)
                    addText(temp81)
                    addButton(1 @temp82 0 40)
                    init()
                )
            )
        (if (== temp0 2)
            = temp0 proc990_0(gSaveDir)
        )
    )
    Memory(memFREE temp81)
    return temp0
)


(instance cast of EventHandler
    (properties)
)
(instance features of EventHandler
    (properties)
)
(instance theDoits of EventHandler
    (properties)
)
(class Sounds of EventHandler
    (properties
        elements 0
        size 0
    )

    (method (pause param1)
        (self:eachElementDo(#perform mayPause 
            (if (paramTotal)
                param1
            )(else
                1
            )
))
    )

)
(instance mayPause of Code
    (properties)

    (method (doit param1 param2)
        (if (not & (send param1:flags) $0001)
            (send param1:pause(param2))
        )
    )

)
(instance regions of EventHandler
    (properties)
)
(instance addToPics of EventHandler
    (properties)

    (method (doit)
        (self:eachElementDo(#perform aTOC))
        AddToPic(elements)
    )

)
(instance timers of Set
    (properties)
)
(instance mouseDownHandler of EventHandler
    (properties)
)
(instance keyDownHandler of EventHandler
    (properties)
)
(instance directionHandler of EventHandler
    (properties)
)
(instance walkHandler of EventHandler
    (properties)
)
(class Cue of Obj
    (properties
        cuee 0
        cuer 0
        register 0
    )

    (method (doit)
        (send gNewSet:delete(self))
        (if ((send gNewSet:isEmpty()))
            (send gNewSet:dispose())
            = gNewSet 0
        )
        (send cuee:cue(register cuer))
        (self:dispose())
    )

)
(instance aTOC of Code
    (properties)

    (method (doit param1)
        (var temp0, temp1)
        (if (not & (send param1:signal) $4000)
            = temp0 (+ (send gEgo:xStep) (/ CelWide((send gEgo:view) 2 0) 2))
            = temp1 (* (send gEgo:yStep) 2)
            (send gRoom:addObstacle((send ((Polygon:new())):
                    init((- (send param1:brLeft) temp0) (- CoordPri(1 CoordPri((send param1:y))) temp1) (+ (send param1:brRight) temp0) (- CoordPri(1 CoordPri((send param1:y))) temp1) (+ (send param1:brRight) temp0) (+ (send param1:y) temp1) (- (send param1:brLeft) temp0) (+ (send param1:y) temp1))
                    yourself()
                )
))
        )
    )

)
(class Game of Obj
    (properties
        script 0
        printLang 1
        _detailLevel 3
        panelObj 0
        panelSelector 0
        handsOffCode 0
        handsOnCode 0
    )

    (method (init)
        = gOldCast cast
        (send gOldCast:add())
        = gOldFeatures features
        (send gOldFeatures:add())
        = gSounds Sounds
        (send gSounds:add())
        = gRegions regions
        (send gRegions:add())
        = gOldATPs addToPics
        (send gOldATPs:add())
        = gTimers timers
        (send gTimers:add())
        = gTheDoits theDoits
        (send gTheDoits:add())
        = gOldMH mouseDownHandler
        (send gOldMH:add())
        = gOldKH keyDownHandler
        (send gOldKH:add())
        = gOldDH directionHandler
        (send gOldDH:add())
        = gOldWH walkHandler
        (send gOldWH:add())
        = gNewEventHandler 0
        = gSaveDir GetSaveDir()
        (Inv:init())
        (if (not gUser)
            = gUser User
        )
        (send gUser:init())
    )


    (method (doit)
        (var newEvent, thePanelObj, thePanelSelector)
        (if (panelObj)
            = thePanelObj panelObj
            = thePanelSelector panelSelector
            = panelObj (= panelSelector 0)
            proc999_7(thePanelObj thePanelSelector)
        )
        = gLastTicks (+ global86 GetTime())
        (if (gNewEventHandler)
            (while (gNewEventHandler)
                (send gNewEventHandler:eachElementDo(#doit))
                = newEvent (Event:new())
               
                (if ((send newEvent:type) and gNewEventHandler)
                    (send gNewEventHandler:firstTrue(#handleEvent newEvent))
                )
                (send newEvent:dispose())
                = gLastTicks (+ global86 GetTime())
                (send gSounds:eachElementDo(#check))
            )
        )
        (if (gPrintEventHandler)
            (send gPrintEventHandler:eachElementDo(#doit))
            (if (not gDialog)
                = newEvent (Event:new())
                (if ((send newEvent:type) and gPrintEventHandler)
                    (send gPrintEventHandler:firstTrue(#handleEvent newEvent))
                )
                (send newEvent:dispose())
                = gLastTicks (+ global86 GetTime())
                return 
            )
        )
        (send gSounds:eachElementDo(#check))
        (send gTimers:eachElementDo(#doit))
        (if (gDialog and (send gDialog:check()))
            (send gDialog:dispose())
        )
        Animate((send gOldCast:elements) 1)
        (if (gCastMotionCue)
            = gCastMotionCue FALSE
            (send gOldCast:eachElementDo(#motionCue))
        )
        (if (gNewSet)
            (send gNewSet:eachElementDo(#doit))
        )
        (if (script)
            (send script:doit())
        )
        (send gRegions:eachElementDo(#doit))
        (if (gNewEventHandler)
            return 
        )
        (if (== gRoomNumber gModNum)
            (send gUser:doit())
        )
        (send gTheDoits:doit())
        (if (<> gRoomNumber gModNum)
            (self:newRoom(gRoomNumber))
        )
        (send gTimers:eachElementDo(#delete))
        GameIsRestarting(0)
    )


    (method (play)
        = gGame self
        = gSaveDir GetSaveDir()
        (self:
            setCursor(gInvisibleCursor 1)
            init()
        )
        (self:setCursor(gCursor 1))
        (while (not gQuitGame)
            (self:doit())
        )
    )


    (method (replay)
        (var temp0)
        (if (gPEvent)
            (send gPEvent:dispose())
        )
        (if (gDialog)
            (send gDialog:dispose())
        )
        (send gOldCast:eachElementDo(#perform RU))
        (send gGame:setCursor(gInvisibleCursor 1))
        = temp0 
            (if (not IsOneOf((send gRoom:style) -1 11 12 13 14))
                (send gRoom:style)
            )(else
                100
            )
        DrawPic((send gRoom:curPic) temp0 dpCLEAR)
        (if (<> gPicNumber -1)
            DrawPic(gPicNumber 100 dpNO_CLEAR)
        )
        (send gOldATPs:doit())
        (if (not (send gUser:canControl()) and not (send gUser:canInput()))
            (send gGame:setCursor(gInvisibleCursor))
        )(else
            (if (gSq5IconBar and (send gSq5IconBar:curIcon))
                (send gGame:setCursor((send ((send gSq5IconBar:curIcon)):cursor)))
            )(else
                (send gGame:setCursor(gCursor))
            )
        )
        (SL:doit())
        DoSound(sndRESTORE)
        (send gSounds:pause(0))
        = global86 (- gLastTicks GetTime())
        (while (not gQuitGame)
            (self:doit())
        )
    )


    (method (newRoom newRoomNumber)
        (var temp0[5], temp5)
        (send gOldATPs:
            eachElementDo(#dispose)
            eachElementDo(#delete)
            release()
        )
        (send gOldFeatures:
            eachElementDo(#perform fDC)
            release()
        )
        (send gOldCast:
            eachElementDo(#dispose)
            eachElementDo(#delete)
        )
        (send gTimers:eachElementDo(#delete))
        (send gRegions:
            eachElementDo(#perform DNKR)
            release()
        )
        (send gTheDoits:release())
        Animate(0)
        = gPreviousRoomNumber gModNum
        = gModNum newRoomNumber
        = gRoomNumber newRoomNumber
        FlushResources(newRoomNumber)
        (self:startRoom(gModNum))
        (while ((send (= temp5 (Event:new(3))):type))
            (send temp5:dispose())
        )
        (send temp5:dispose())
    )


    (method (startRoom param1)
        (if (gDebugOnNextRoom)
            SetDebug()
        )
        = gRoom ScriptID(param1)
        (send gRegions:addToFront(gRoom))
        (send gRoom:init())
    )


    (method (restart)
        (if (gDialog)
            (send gDialog:dispose())
        )
        RestartGame()
    )


    (method (restore param1)
        (var temp0[20], temp20, temp21, temp22[100], temp122[5], temp127[100])
        (if (not ValidPath(gSaveDir))
            Message(msgGET 994 9 0 0 1 @temp22)
            Format(@temp127 @temp22 gSaveDir)
            (Print:
                font(0)
                addText(@temp127)
                init()
            )
            proc990_0(gSaveDir)
        )
        Load(rsFONT gSmallFont)
        ScriptID(990)
        = temp21 (self:setCursor(gCursor))
        (send gSounds:pause(1))
        (if (localproc_0e32(1))
            (if (gDialog)
                (send gDialog:dispose())
            )
            = temp20 (Restore:doit(rest param1))
            (if (<> temp20 -1)
                (self:setCursor(gInvisibleCursor 1))
                (if (CheckSaveGame(name temp20 gVersion))
                    RestoreGame(name temp20 gVersion)
                )(else
                    Message(msgGET 994 3 0 0 1 @temp22)
                    Message(msgGET 994 2 0 0 1 @temp122)
                    (Print:
                        font(0)
                        addText(@temp22)
                        addButton(1 @temp122 0 40)
                        init()
                    )
                    (self:setCursor(temp21 HaveMouse()))
                )
            )
            localproc_0e32(0)
        )
        (send gSounds:pause(0))
    )


    (method (save)
        (var temp0[20], temp20, temp21, temp22[100], temp122[5], temp127[100])
        (if (not ValidPath(gSaveDir))
            Message(msgGET 994 9 0 0 1 @temp22)
            Format(@temp127 @temp22 gSaveDir)
            (Print:
                font(0)
                addText(@temp127)
                init()
            )
            proc990_0(gSaveDir)
        )
        Load(rsFONT gSmallFont)
        ScriptID(990)
        = temp21 (self:setCursor(gCursor))
        (send gSounds:pause(1))
        (if (localproc_0e32(1))
            (if (gDialog)
                (send gDialog:dispose())
            )
            = temp20 (Save:doit(@temp0))
            (if (<> temp20 -1)
                = temp21 (self:setCursor(gInvisibleCursor 1))
                (if (not SaveGame(name temp20 @temp0 gVersion))
                    Message(msgGET 994 1 0 0 1 @temp22)
                    Message(msgGET 994 2 0 0 1 @temp122)
                    (Print:
                        font(0)
                        addText(@temp22)
                        addButton(1 @temp122 0 40)
                        init()
                    )
                )
                (self:setCursor(temp21 HaveMouse()))
            )
            localproc_0e32(0)
        )
        (send gSounds:pause(0))
    )


    (method (changeScore param1)
        = gScore (+ gScore param1)
        (SL:doit())
    )


    (method (handleEvent pEvent)
        (if ((send pEvent:claimed))
            1
        )(else
            (if (script and (send script:handleEvent(pEvent)))
                1
            )(else
                (if (& (send pEvent:type) evVERB)
                    (self:pragmaFail())
                )
            )
        )
        (send pEvent:claimed)
    )


    (method (showMem)
        (var temp0[100])
        Format(@temp0 "Free Heap: %u Bytes\nLargest ptr: %u Bytes\nFreeHunk: %u KBytes\nLargest hunk: %u Bytes" MemoryInfo(1) MemoryInfo(0) >> MemoryInfo(3) $0006 MemoryInfo(2))
        (Print:
            addText(@temp0)
            init()
        )
    )


    (method (setCursor cursorNumber param2 param3 param4 param5 param6)
        (var theGCursorNumber)
        = theGCursorNumber gCursorNumber
        (if (IsObject(cursorNumber))
            = gCursorNumber cursorNumber
            (send cursorNumber:init())
        )(else
            SetCursor(cursorNumber 0 0)
        )
        (if (> paramTotal 1)
            SetCursor(param2)
            (if (> paramTotal 2)
                SetCursor(param3 param4)
                (if (> paramTotal 4)
                    SetCursor(cursorNumber 0 0 param5 param6)
                )
            )
        )
        return theGCursorNumber
    )


    (method (notify)
    )


    (method (setScript theScript sendParams)
        (if (script)
            (send script:dispose())
        )
        (if (theScript)
            (send theScript:init(self rest sendParams))
        )
    )


    (method (cue)
        (if (script)
            (send script:cue())
        )
    )


    (method (quitGame param1)
        (if (not paramTotal or param1)
            = gQuitGame TRUE
        )
    )


    (method (masterVolume param1)
        (if (paramTotal)
            DoSound(sndMASTER_VOLUME param1)
        )(else
            DoSound(sndMASTER_VOLUME)
        )
    )


    (method (detailLevel the_detailLevel)
        (if (paramTotal)
            = _detailLevel the_detailLevel
            (send gOldCast:eachElementDo(#checkDetail))
        )
        return _detailLevel
    )


    (method (pragmaFail)
    )


    (method (handsOff param1)
        (if (handsOffCode)
            (send handsOffCode:doit(rest param1))
        )(else
            (User:
                canControl(0)
                canInput(0)
            )
            (if (IsObject(gEgo))
                (send gEgo:setMotion(0))
            )
        )
    )


    (method (handsOn param1)
        (if (handsOnCode)
            (send handsOnCode:doit(rest param1))
        )(else
            (User:
                canControl(1)
                canInput(1)
            )
        )
    )

)
(class Rgn of Obj
    (properties
        script 0
        number 0
        modNum -1
        noun 0
        timer 0
        keep 0
        initialized 0
    )

    (method (init)
        (if (not initialized)
            = initialized 1
            (if (not (send gRegions:contains(self)))
                (send gRegions:addToEnd(self))
            )
            (super:init())
        )
    )


    (method (doit)
        (if (script)
            (send script:doit())
        )
    )


    (method (dispose)
        (send gRegions:delete(self))
        (if (IsObject(script))
            (send script:dispose())
        )
        (if (IsObject(timer))
            (send timer:
                dispose()
                delete()
            )
        )
        (send gSounds:eachElementDo(#clean self))
        DisposeScript(number)
    )


    (method (handleEvent pEvent)
        (if ((send pEvent:claimed))
            1
        )(else
            (if (& (send pEvent:type) evJOYSTICK)
                0
            )(else
                (if (not 
                (if (script and ((send script:handleEvent(pEvent)) or 1))
                    (send pEvent:claimed)
                )
)
                    (send pEvent:claimed((self:doVerb((send pEvent:message)))))
                )
            )
        )
        (send pEvent:claimed)
    )


    (method (doVerb theVerb)
        (var temp0)
        (if (== modNum -1)
            = modNum gModNum
        )
        
        return 
            (if (Message(msgGET modNum noun theVerb 0 1))
                (send gTestMessager:say(noun theVerb 0 0 0 modNum))
                1
            )(else
                0
            )
    )


    (method (setScript theScript sendParams)
        (if (IsObject(script))
            (send script:dispose())
        )
        (if (theScript)
            (send theScript:init(self rest sendParams))
        )
    )


    (method (cue)
        (if (script)
            (send script:cue())
        )
    )


    (method (newRoom)
    )


    (method (notify)
    )

)
(class Rm of Rgn
    (properties
        script 0
        number 0
        modNum -1
        noun 0
        timer 0
        keep 0
        initialized 0
        picture 0
        style $ffff
        horizon 0
        controls 0
        north 0
        east 0
        south 0
        west 0
        curPic 0
        picAngle 0
        vanishingX 160
        vanishingY 0
        obstacles 0
        inset 0
    )

    (method (init)
        = number gModNum
        = gPicAngle picAngle
        (if (picture)
            (self:drawPic(picture))
        )
        (self:reflectPosn((send gUser:alterEgo) (send ((send gUser:alterEgo)):edgeHit)))
        (send ((send gUser:alterEgo)):edgeHit(0))
    )


    (method (doit)
        (var temp0)
        (if (script)
            (send script:doit())
        )
        = temp0 (self:edgeToRoom((send ((send gUser:alterEgo)):edgeHit)))
        (if (temp0)
            (self:newRoom((= temp0 (self:edgeToRoom((send ((send gUser:alterEgo)):edgeHit))))))
        )
    )


    (method (dispose)
        (if (obstacles)
            (send obstacles:dispose())
        )
        (super:dispose())
    )


    (method (handleEvent pEvent)
    	
        (if (inset and (send inset:handleEvent(pEvent)))
        )(else
            (super:handleEvent(pEvent))
        )
        (send pEvent:claimed)
    )


    (method (newRoom newRoomNumber)
        (send gRegions:
            delete(self)
            eachElementDo(#newRoom newRoomNumber)
            addToFront(self)
        )
        = gRoomNumber newRoomNumber
        (super:newRoom(newRoomNumber))
    )


    (method (setRegions scriptNumbers)
        (var temp0, theScriptNumbers, temp2)
        = temp0 0
        (while (< temp0 paramTotal)
            = theScriptNumbers scriptNumbers[temp0]
            = temp2 ScriptID(theScriptNumbers)
            (send temp2:number(theScriptNumbers))
            (send gRegions:add(temp2))
            (if (not (send temp2:initialized))
                (send temp2:init())
            )
            ++temp0
        )
    )


    (method (drawPic picNumber picAnimation)
        (if (gOldATPs)
            (send gOldATPs:
                eachElementDo(#dispose)
                release()
            )
        )
        = curPic picNumber
        = gPicNumber -1
        DrawPic(picNumber 
            (if (== paramTotal 2)
                picAnimation
            )(else
                (if (<> style -1)
                    style
                )(else
                    100
                )
            )
 dpCLEAR)
    )


    (method (overlay picNumber picAnimation)
        = gPicNumber picNumber
        DrawPic(picNumber 
            (if (== paramTotal 2)
                picAnimation
            )(else
                (if (<> style -1)
                    style
                )(else
                    100
                )
            )
 dpNO_CLEAR)
    )


    (method (addObstacle param1 param2)
        (if (not IsObject(obstacles))
            = obstacles (List:new())
        )
        (send obstacles:add(param1 rest param2))
    )


    (method (reflectPosn param1 param2)
        (switch (param2)
            (case 1
                (send param1:y(188))
            )
            (case 4
                (send param1:x((- 319 (send param1:xStep))))
            )
            (case 3
                (send param1:y((+ horizon (send param1:yStep))))
            )
            (case 2
                (send param1:x(1))
            )
        )
    )


    (method (edgeToRoom param1)
        (switch (param1)
            (case 1
                north
            )
            (case 2
                east
            )
            (case 3
                south
            )
            (case 4
                west
            )
        )
    )


    (method (roomToEdge param1)
        (switch (param1)
            (case north
                1
            )
            (case south
                3
            )
            (case east
                2
            )
            (case west
                4
            )
        )
    )


    (method (setInset param1 param2 param3)
        (if (inset)
            (send inset:dispose())
        )
        (if (paramTotal and param1)
            (send param1:init(
                (if (>= paramTotal 2)
                    param2
                )(else
                    0
                )
 self 
                (if (>= paramTotal 3)
                    param3
                )(else
                    0
                )
))
        )
    )

)
(class SL of Obj
    (properties
        state $0000
        code 0
    )

    (method (doit)
        (var temp0)
        (if (code)
            = temp0 Memory(memALLOC_CRIT 150)
            (send code:doit(temp0))
            DrawStatus(
                (if (state)
                    temp0
                )(else
                    0
                )
)
            Memory(memFREE temp0)
        )
    )


    (method (enable)
        = state 1
        (self:doit())
    )


    (method (disable)
        = state 0
        (self:doit())
    )

)
(instance RU of Code
    (properties)

    (method (doit param1)
        (var temp0)
        (if ((send param1:underBits))
            = temp0 (send param1:signal)
            = temp0 (| temp0 $0001)
            = temp0 (& temp0 $fffb)
            (send param1:
                underBits(0)
                signal(temp0)
            )
        )
    )

)
(instance DNKR of Code
    (properties)

    (method (doit param1)
        (if (not (send param1:keep))
            (send param1:dispose())
        )
    )

)
(instance fDC of Code
    (properties)

    (method (doit param1)
        (if ((send param1:respondsTo(#delete)))
            (send param1:
                signal(& (send param1:signal) $ffdf)
                dispose()
                delete()
            )
        )(else
            (send param1:dispose())
        )
    )

)
