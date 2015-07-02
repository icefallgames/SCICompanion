(version 2)
(include "sci.sh")
(include "game.sh")
(include "0.shm")
(exports
    0 SQ5
    1 Btest
    2 Bset
    3 Bclear
    4 RestorePreviousHandsOn
    5 IsObjectOnControl
    6 SetUpEgo
    8 proc0_8
    9 Die
    10 AddToScore
    11 HideStatus
    12 DebugPrint
    13 AddPolygonsToRoom
    14 CreateNewPolygon
)
(use "ColorInit")
(use "Smopper")
(use "SQEgo")
(use "ScrollableInventory")
(use "ScrollInsetWindow")
(use "DColorButton")
(use "SpeakWindow")
(use "Print")
(use "DIcon")
(use "Messager")
(use "Blink")
(use "PseudoMouse")
(use "Scaler")
(use "BorderWindow")
(use "IconI")
(use "RandCycle")
(use "PolyPath")
(use "Polygon")
(use "StopWalk")
(use "Timer")
(use "Grooper")
(use "Sound")
(use "Game")
(use "User")
(use "Obj")
(use "File")
(script 0)

(define STARTING_ROOM 100)

(local
    gEgo
    gGame
    gRoom
    global3		// Unused
    gQuitGame = FALSE
    gOldCast
    gRegions
    gTimers
    gSounds
    gSq5Inv
    gOldATPs
    gModNum				// Brian called this gRoomNumberExit. It's what gets set to move to the next room, which will then be gRoomNumber
    					// It's used everywhere in place of gRoomNumber here though.
    gPreviousRoomNumber
    gRoomNumber
    gDebugOnNextRoom
    gScore
    gMaxScore
    global17
    gNewSet
    gCursorNumber
    gCursor =     999
    gInvisibleCursor =     997
    gFont =     1
    gSmallFont =     4
    gPEvent
    gDialog
    gBigFont =     1
    gVersion
    gSaveDir
    gPicAngle
    gOldFeatures
    global34
    gPicNumber =     -1
    gCastMotionCue
    gSq5Win
    global39	// Unused	(foreground text?)
    global40	// Unused	(window background color?)
    gOldPort
    global42[21] // debug filename
    gGameControls
    gFeatureInit
    gDoVerbCode
    gApproachCode
    global67 =     1		// Has something to do with default Motion for ego (0: MoveTo, 1: PolyPath, 2: PolyPath...)
    gSq5IconBar
    gPEventX
    gPEventY
    gOldKH
    gOldMH
    gOldDH
    gPseudoMouse
    gTheDoits
    gEatTheMice =     60
    gUser
    global81				// Something to do with Sync
    gNewSync
    global83				// Something to do with audio narration
    gNewEventHandler
    gFont_2
    global86				// Something to do with time (ticks per frame?)
    gLastTicks
    gSQ5Narrator
    global90 =     $0001	// Seems to be a flag for talker: 0x1 (text) and 0x2 (audio)
    gTestMessager
    gPrintEventHandler
    gOldWH
    gTextReadSpeed =     2
    gAltPolyList
    gColorDepth
    gPolyphony
    gStopGroop
    global107
    gGSq5IconBarCurIcon
    gGUserCanControl
    gGUserCanInput
    gCheckedIcons
    gState
    gNewSpeakWindow
    gSq5Win_2
    gDeathReason
    gSq5Music1
    gDongle = 1234		// This variable CAN'T MOVE
    gSq5Music2
    gCurrentTalkerNumber
    gGEgoMoveSpeed
    gColorWindowForeground
    gColorWindowBackground
    gLowlightColor
    gDefaultEgoView =     1
    gRegister
    gFlags[14]		// Start of bit set. Room for 14x16 = 224 flags.
    gEdgeDistance = 10	// Margin around screen to make it easier to walk the ego to the edge
    gDebugOut
)

(procedure public (Btest param1)
    return & gFlags[(/ param1 16)] (>> $8000 (% param1 16))
)

(procedure public (Bset param1)
    (var temp0)
    = temp0 Btest(param1)
    = gFlags[(/ param1 16)] (| gFlags[(/ param1 16)] (>> $8000 (% param1 16)))
    return temp0
)

(procedure public (Bclear param1)
    (var temp0)
    = temp0 Btest(param1)
    = gFlags[(/ param1 16)] (& gFlags[(/ param1 16)] bnot (>> $8000 (% param1 16)))
    return temp0
)

(procedure public (RestorePreviousHandsOn)
    (var temp0)
    (send gUser:
        canControl(gGUserCanControl)
        canInput(gGUserCanInput)
    )
    = temp0 0
    (while (< temp0 8)
        (if (& gCheckedIcons (>> $8000 temp0))
            (send gSq5IconBar:disable(temp0))
        )
        ++temp0
    )
)

(procedure public (IsObjectOnControl param1 param2)
    return 
        (if (& (send param1:onControl(1)) param2)
            return 1
        )(else
            0
        )
)

(procedure public (SetUpEgo param1 param2)
    (if ((> paramTotal 0) and (<> param1 -1))
        (send gEgo:view(param1))
        (if ((> paramTotal 1) and (<> param2 -1))
            (send gEgo:loop(param2))
        )
    )(else
        (send gEgo:view(gDefaultEgoView))
        (if ((> paramTotal 1) and (<> param2 -1))
            (send gEgo:loop(param2))
        )
    )
    (if ((send gEgo:looper))
        (send ((send gEgo:looper)):dispose())
    )
    (send gEgo:
        setStep(5 2)
        illegalBits(0)
        ignoreActors(0)
        setSpeed(gGEgoMoveSpeed)
        //signal(| (send gEgo:signal) $1000)
        heading(
            (switch ((send gEgo:loop))
                (case 0
                    90
                )
                (case 1
                    270
                )
                (case 2
                    180
                )
                (case 3
                    0
                )
                (case 4
                    135
                )
                (case 5
                    225
                )
                (case 6
                    45
                )
                (case 7
                    315
                )
			)
			   )
    )
    (send gEgo:
        setLoop(-1)
        setLoop(stopGroop)
        setPri(-1)
        setMotion(NULL)
        view(13)
        state(| (send gEgo:state) $0002)
    )
)

(procedure public (proc0_8 param1 param2 param3 param4)
    (var temp0, temp1, temp2, temp3, temp4)
    = temp3 0
    = temp4 0
    (if (IsObject(param2))
        = temp1 (send param2:x)
        = temp2 (send param2:y)
        (if (> paramTotal 2)
            (if (IsObject(param3))
                = temp3 param3
            )(else
                = temp4 param3
            )
            (if (== paramTotal 4)
                = temp3 param4
            )
        )
    )(else
        = temp1 param2
        = temp2 param3
        (if (== paramTotal 4)
            = temp3 param4
        )
    )
    (if (temp4)
        proc0_8(param2 param1)
    )
    = temp0 GetAngle((send param1:x) (send param1:y) temp1 temp2)
    (send param1:setHeading(temp0 
        (if (IsObject(temp3))
            temp3
        )(else
            0
        )
						   ))
)


(procedure public (Die param1)
    (if (not paramTotal)
        = gDeathReason 1
    )(else
        = gDeathReason param1
    )
    (send gRoom:newRoom(DEATH_SCRIPT))
)

(procedure public (AddToScore param1 param2)
    (if (not Btest(param1))
        = gScore (+ gScore param2)
        (sq5StatusLineCode:doit())
        Bset(param1)
        (rm0Sound:
            priority(15)
            number(1000)
            loop(1)
            flags(1)
            play()
        )
    )
)

(procedure public (HideStatus)
    (var temp0)
    = temp0 GetPort()
    SetPort(-1)
    Graph(grFILL_BOX 0 0 10 320 VISUAL 0 -1 -1)
    Graph(grUPDATE_BOX 0 0 10 320 VISUAL)
    SetPort(temp0)
)

(procedure public (DebugPrint params)
	(if (gDebugOut)
		(send gDebugOut:debugPrint(rest params))
	)
)

(procedure (CreateNewPolygonHelper polyBuffer nextPoly)
	(var newPoly, pointCount)
	(= newPoly (Polygon:new()))
	(= pointCount (Memory(memPEEK (+ polyBuffer 2))))
	(send newPoly:
		dynamic(FALSE)
		type(Memory(memPEEK polyBuffer))
		size(pointCount)
		// Use the points directly from the buffer:
		points(+ polyBuffer 4)
	)
	
	// Tell the caller the position of the next poly, if they care:
	(if (> paramTotal 1)
		Memory(memPOKE nextPoly (+ polyBuffer (+ 4 (* 4 pointCount))))
	)
	return newPoly
)

(procedure public (AddPolygonsToRoom polyBuffer)
	(var polyCount)
	(if (< polyBuffer 100)
		TextPrint("polyBuffer is not a pointer. Polygon ignored.")
	)(else
		(= polyCount Memory(memPEEK polyBuffer))
		(+= polyBuffer 2)
		(while (polyCount)
			(send gRoom:addObstacle(CreateNewPolygonHelper(polyBuffer @polyBuffer)))
			--polyCount
		)
	 )
)

(procedure public (CreateNewPolygon polyBuffer nextPolyOptional)
	(var polyCount)
	(if (< polyBuffer 100)
		TextPrint("polyBuffer is not a pointer. Polygon ignored.")
		return NULL
	)(else
		(= polyCount Memory(memPEEK polyBuffer))
		(+= polyBuffer 2)
		return CreateNewPolygonHelper(polyBuffer nextPolyOptional)
	 )
)

(instance rm0Sound of Sound
    (properties
        priority 15
    )
)
(instance sq5Music1 of Sound
    (properties
        flags $0001
    )
)
(instance sq5Music2 of Sound
    (properties
        flags $0001
    )
)
(instance stopGroop of Grooper
    (properties)
)
(instance egoStopWalk of FiddleStopWalk
    (properties)
)
(instance ego of SQEgo
    (properties)
)
(instance sq5StatusLineCode of Code
    (properties)

    (method (doit)
        (var temp0[50], temp50[50], temp100)
        = temp100 GetPort()
        SetPort(-1)
        Graph(grFILL_BOX 0 0 10 320 VISUAL 5 -1 -1)
        Graph(grUPDATE_BOX 0 0 10 320 VISUAL)
        Message(msgGET 0 N_TITLEBAR 0 0 1 @temp0)
        Format(@temp50 "%s %d" @temp0 gScore)
        Display(@temp50 dsCOORD 4 1 dsFONT gFont dsCOLOR 6)
        Display(@temp50 dsCOORD 6 3 dsFONT gFont dsCOLOR 4)
        Display(@temp50 dsCOORD 5 2 dsFONT gFont dsCOLOR 0)
        Graph(grDRAW_LINE 0 0 0 319 7 -1 -1)
        Graph(grDRAW_LINE 0 0 9 0 6 -1 -1)
        Graph(grDRAW_LINE 9 0 9 319 4 -1 -1)
        Graph(grDRAW_LINE 0 319 9 319 3 -1 -1)
        Graph(grUPDATE_BOX 0 0 10 319 VISUAL)
        SetPort(temp100)
    )

)
(instance sq5IconBar of IconBar
    (properties)

    (method (show)
        (if (IsObject(curInvIcon))
            (send curInvIcon:loop(2))
        )
        (super:show())
        (if (IsObject(curInvIcon))
            (send curInvIcon:loop(1))
        )
    )

    (method (hide param1)
        (super:hide(rest param1))
        (send gGame:setCursor(gCursorNumber 1))
    )

    (method (noClickHelp)
        (var temp0, temp1, temp2, temp3, gSq5WinEraseOnly)
        = temp2 0
        = temp1 temp2
        = temp3 GetPort()
        = gSq5WinEraseOnly (send gSq5Win:eraseOnly)
        (send gSq5Win:eraseOnly(1))
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
                    Animate((send gOldCast:elements) 0)
                    SetPort(temp3)
                )
            )(else
                (if (gDialog)
                    (send gDialog:dispose())
                    Animate((send gOldCast:elements) 0)
                )(else
                    = temp1 0
                )
            )
            (send temp0:dispose())
        )
        (send gSq5Win:eraseOnly(gSq5WinEraseOnly))
        (send gGame:setCursor(999 1))
        (if (gDialog)
            (send gDialog:dispose())
            Animate((send gOldCast:elements) 0)
        )
        SetPort(temp3)
        (if (not (send helpIconItem:onMe(temp0)))
            (self:dispatchEvent(temp0))
        )
    )
)

(class public SQ5 of Game
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
        (var temp0[7], temp7)
        
        (send (ScriptID(INVENTORY_SCRIPT 0)):init())
        (super:init())
        = gEgo ego
        (User:
            alterEgo(gEgo)
            canControl(0)
            canInput(0)
        )
        = global90 $0001
        = global34 1
        = gPolyphony DoSound(sndGET_POLYPHONY)
        = gMaxScore 5000
        = gFont 1605
        = gGEgoMoveSpeed 6
        = gEatTheMice 30
        = gTextReadSpeed 2
        = gColorDepth Graph(grGET_COLOURS)
        = gStopGroop stopGroop
        = gPseudoMouse PseudoMouse
        (send gEgo:setLoop(gStopGroop))
        (if (== gColorDepth 256)
            Bset(0)
        )
        // The position of these font resource numbers correspond to font codes used in messages:
        TextFonts(1605 1605 1605 1605 1605 2106)
        // These correspond to color codes used in messages (values into global palette):
        TextColors(0 15 26 31 34 52 63)
        = gVersion "x.yyy.zzz"
        = temp7 FileIO(fiOPEN "version" fOPENFAIL)
        FileIO(fiREAD_STRING gVersion 11 temp7)
        FileIO(fiCLOSE temp7)
        ColorInit()
        DisposeScript(COLORINIT_SCRIPT)
        = gSQ5Narrator sQ5Narrator
        = gSq5Win sq5Win
        = gSq5Win_2 sq5Win
        = gTestMessager testMessager
        = gNewSpeakWindow (SpeakWindow:new())
        (send gSq5Win:
            color(gColorWindowForeground)
            back(gColorWindowBackground)
        )
        (send gGame:
            setCursor(gCursorNumber TRUE 304 172)
            detailLevel(3)
        )
        = gSq5Music1 sq5Music1
        (send gSq5Music1:
            //number(1)
            owner(self)
            flags(1)
            init()
        )
        = gSq5Music2 sq5Music2
        (send gSq5Music2:
            //number(1)
            owner(self)
            flags(1)
            init()
        )
        = gSq5IconBar sq5IconBar
        (send gSq5IconBar:
        	// These correspond to ICONINDEX_*** in game.sh
            add(icon0 icon1 icon2 icon3 icon4 icon6 icon7 icon8 icon9)
            eachElementDo(#init)
            eachElementDo(#highlightColor 0)
            eachElementDo(#lowlightColor 5)
            curIcon(icon0)
            useIconItem(icon6)
            helpIconItem(icon9)
            walkIconItem(icon0)
            disable(ICONINDEX_CURITEM)
            state(3072)
            disable()
        )
        = gCursor 999
        = gInvisibleCursor 996
        = gDoVerbCode lb2DoVerbCode
        = gFeatureInit lb2FtrInit
        = gApproachCode lb2ApproachCode
    )


    (method (doit param1)
        (if (GameIsRestarting())
            (if (IsOneOf(gModNum TITLEROOM_SCRIPT))
                HideStatus()
            )(else
                (sq5StatusLineCode:doit())
            )
            = gColorDepth Graph(grGET_COLOURS)
            (if (== gColorDepth 256)
                Bset(0)
            )
        )
        (super:doit(rest param1))
    )


    (method (play)
    	(var deleteMe, debugRoom, theStartRoom)
        = gGame self
        = gSaveDir GetSaveDir()
        
        (if (not GameIsRestarting())
            GetCWD(gSaveDir)
        )
        (self:
            setCursor(gInvisibleCursor 1)
            init()
        )

		(= theStartRoom STARTING_ROOM)
		(if (not GameIsRestarting())
	        (if (FileIO(fiEXISTS "sdebug.txt"))
	        	= gDebugOut ScriptID(DEBUGOUT_SCRIPT 0)
	        	DebugPrint("Debugger enabled")
	        	= debugRoom (send gDebugOut:init("sdebug.txt"))
	        	(if (<> debugRoom -1)
	        		(= theStartRoom debugRoom)
	        		(send gGame:handsOn())
	        		DebugPrint("Starting in room %d" theStartRoom)
				)
			)
		)
		
        (self:newRoom(theStartRoom))
        
        (while (not gQuitGame)
            (self:doit())
        )
    )


    (method (startRoom param1)
        (var temp0[4])
        (if (IsOneOf(param1 TITLEROOM_SCRIPT))
            HideStatus()
        )(else
            (sq5StatusLineCode:doit())
        )
        (if (gPseudoMouse)
            (send gPseudoMouse:stop())
        )
        (send (ScriptID(DISPOSECODE_SCRIPT)):doit(param1))
        
        (super:startRoom(param1))
    )


    (method (restart param1)
        (var temp0, temp1)
        = temp1 (send ((send gSq5IconBar:curIcon)):cursor)
        (send gGame:setCursor(999))
        = temp0 (Print:
                font(gFont)
                width(75)
                window(gSq5Win)
                mode(1)
                addText(N_RESTART V_LOOK 0 1 0 0 0)
                addColorButton(1 N_RESTART V_LOOK 0 2 0 40 0)
                addColorButton(0 N_RESTART V_LOOK 0 3 0 50 0)
                init()
            )
        (if (temp0)
            (super:restart(rest param1))
        )(else
            (send gGame:setCursor(temp1))
        )
    )


    (method (restore param1)
        (var temp0[2])
        (super:restore(rest param1))
        (send gGame:setCursor((send ((send gSq5IconBar:curIcon)):cursor)))
    )


    (method (save param1)
        (super:save(rest param1))
        (send gGame:setCursor((send ((send gSq5IconBar:curIcon)):cursor)))
    )


    (method (handleEvent pEvent)
        (var theGCursorNumber)
        
        
        (super:handleEvent(pEvent))
        (if ((send pEvent:claimed))
            return 1
        )
        return 
            (switch ((send pEvent:type))
                (case evKEYBOARD
                    (switch ((send pEvent:message))
                        (case KEY_TAB
                            (if (not & (send ((send gSq5IconBar:at(6))):signal) icDISABLED)
                                (if (gNewEventHandler)
                                    return gNewEventHandler
                                )
                                = theGCursorNumber gCursorNumber
                                (send gSq5Inv:showSelf(gEgo))
                                (send gGame:setCursor(theGCursorNumber 1))
                                (send pEvent:claimed(1))
                            )
                        )
                        (case KEY_CONTROL
                            (if (not & (send ((send gSq5IconBar:at(7))):signal) icDISABLED)
                                (send gGame:quitGame())
                                (send pEvent:claimed(1))
                            )
                        )
                        (case JOY_RIGHT
                            (if (not & (send ((send gSq5IconBar:at(7))):signal) icDISABLED)
                                = theGCursorNumber (send ((send gSq5IconBar:curIcon)):cursor)
                                (send (ScriptID(24 0)):doit())
                                (send gGameControls:dispose())
                                (send gGame:setCursor(theGCursorNumber 1))
                            )
                        )
                        (case KEY_F2
                            (if ((send gGame:masterVolume()))
                                (send gGame:masterVolume(0))
                            )(else
                                (if (> gPolyphony 1)
                                    (send gGame:masterVolume(15))
                                )(else
                                    (send gGame:masterVolume(1))
                                )
                            )
                            (send pEvent:claimed(1))
                        )
                        (case KEY_F5
                            (if (not & (send ((send gSq5IconBar:at(7))):signal) icDISABLED)
                                (if (gNewEventHandler)
                                    return gNewEventHandler
                                )
                                = theGCursorNumber gCursorNumber
                                (send gGame:save())
                                (send gGame:setCursor(theGCursorNumber 1))
                                (send pEvent:claimed(1))
                            )
                        )
                        (case KEY_F7
                            (if (not & (send ((send gSq5IconBar:at(7))):signal) icDISABLED)
                                (if (gNewEventHandler)
                                    return gNewEventHandler
                                )
                                = theGCursorNumber gCursorNumber
                                (send gGame:restore())
                                (send gGame:setCursor(theGCursorNumber 1))
                                (send pEvent:claimed(1))
                            )
                        )
                        (case KEY_EXECUTE
                            (if ((send gUser:controls))
                                = gGEgoMoveSpeed (send gEgo:moveSpeed)
                                = gGEgoMoveSpeed Max(0 --gGEgoMoveSpeed)
                                (send gEgo:setSpeed(gGEgoMoveSpeed))
                            )
                        )
                        (case KEY_SUBTRACT
                            (if ((send gUser:controls))
                                = gGEgoMoveSpeed (send gEgo:moveSpeed)
                                (send gEgo:setSpeed(++gGEgoMoveSpeed))
                            )
                        )
                        (case 61
                            (if ((send gUser:controls))
                                (send gEgo:setSpeed(6))
                            )
                        )
                        (case KEY_ALT_v
                            (Print:
                                addText("Version number:" 0 0)
                                addText(gVersion 0 14)
                                init()
                            )
                        )
                        (case KEY_ALT_d
                        	// Script-base debugger
                        	(send (ScriptID(10 0)):init())
                        )
                        (default 
                            (send pEvent:claimed(0))
                        )
                    )
                )
            )
    )


    (method (setCursor cursorNumber param2 param3 param4)
        (var theGCursorNumber)
        = theGCursorNumber gCursorNumber
        (if (paramTotal)
            (if (IsObject(cursorNumber))
                = gCursorNumber cursorNumber
                (send gCursorNumber:init())
            )(else
                = gCursorNumber cursorNumber
                SetCursor(gCursorNumber 0 0)
            )
        )
        (if ((> paramTotal 1) and not param2)
            SetCursor(996 0 0)
        )
        (if (> paramTotal 2)
            SetCursor(param3 param4)
        )
        return theGCursorNumber
    )


    (method (quitGame param1)
        (var temp0, temp1)
        = temp1 (send ((send gSq5IconBar:curIcon)):cursor)
        (send gGame:setCursor(999))
        = temp0 (Print:
                font(gFont)
                width(75)
                mode(1)
                addText(N_QUITMENU V_LOOK 0 1 0 0 0)
                addColorButton(1 N_QUITMENU V_LOOK 0 2 0 25 0)
                addColorButton(0 N_QUITMENU V_LOOK 0 3 0 35 0)
                init()
            )
        (if (temp0)
            (Print:
                addText(19 1 0 4 0 0 0)
                init()
            )
            (super:quitGame(rest param1))
        )(else
            (send gGame:setCursor(temp1))
        )
    )


    (method (pragmaFail)
        (if ((User:canControl()))
            (switch ((send ((send gUser:curEvent)):message))
                (case V_DO
                    (send gTestMessager:say(0 V_DO 0 Random(1 2) 0 0))
                )
                (case V_TALK
                    (send gTestMessager:say(0 V_TALK 0 Random(1 2) 0 0))
                )
                (default 
                    (if (not IsOneOf((send ((send gUser:curEvent)):message) V_LOOK))
                        (send gTestMessager:say(0 V_COMBINE 0 Random(2 3) 0 0))
                    )
                )
            )
        )
    )


    (method (handsOff)
        (if (not gGSq5IconBarCurIcon)
            = gGSq5IconBarCurIcon (send gSq5IconBar:curIcon)
        )
        = gGUserCanControl (send gUser:canControl())
        = gGUserCanInput (send gUser:canInput())
        (send gUser:
            canControl(0)
            canInput(0)
        )
        (send gEgo:setMotion(0))
        = gCheckedIcons 0
        (send gSq5IconBar:eachElementDo(#perform checkIcon))
        (send gSq5IconBar:curIcon((send gSq5IconBar:at(7))))
        (send gSq5IconBar:disable())
        (send gSq5IconBar:disable(
        		ICONINDEX_WALK
        		ICONINDEX_LOOK
        		ICONINDEX_DO
        		ICONINDEX_TALK
        		ICONINDEX_CUSTOM
        		ICONINDEX_CURITEM
        		ICONINDEX_INVENTORY
        		ICONINDEX_SETTINGS)
        		)
        (send gGame:setCursor(996))
    )


    (method (handsOn fRestore)
        (send gSq5IconBar:enable())
        (send gUser:
            canControl(1)
            canInput(1)
        )
        (send gSq5IconBar:enable(
        		ICONINDEX_WALK
        		ICONINDEX_LOOK
        		ICONINDEX_DO
        		ICONINDEX_TALK
        		// ICONINDEX_CUSTOM // see below
        		ICONINDEX_CURITEM
        		ICONINDEX_INVENTORY
        		ICONINDEX_SETTINGS)
        		)
        		
        (send gSq5IconBar:disable(
        		// See above
        		ICONINDEX_CUSTOM)
		)
        (if (paramTotal and fRestore)
            RestorePreviousHandsOn()
        )
        (if (not (send gSq5IconBar:curInvIcon))
            (send gSq5IconBar:disable(ICONINDEX_CURITEM))
        )
        (if (gGSq5IconBarCurIcon)
            (send gSq5IconBar:curIcon(gGSq5IconBarCurIcon))
            (send gGame:setCursor((send gGSq5IconBarCurIcon:cursor)))
            = gGSq5IconBarCurIcon 0
            (if ((== (send gSq5IconBar:curIcon) (send gSq5IconBar:at(5))) and not (send gSq5IconBar:curInvIcon))
                (send gSq5IconBar:advanceCurIcon())
            )
        )
        (send gGame:setCursor((send ((send gSq5IconBar:curIcon)):cursor) 1))
        = gCursorNumber (send ((send gSq5IconBar:curIcon)):cursor)
    )


    (method (showAbout)
        (send (ScriptID(ABOUT_SCRIPT 0)):doit())
        DisposeScript(ABOUT_SCRIPT)
    )


    (method (showControls)
        (var temp0)
        = temp0 (send ((send gSq5IconBar:curIcon)):cursor)
        (send (ScriptID(GAMECONTROLS_SCRIPT 0)):doit())
        (send gGameControls:dispose())
        (send gGame:setCursor(temp0 1))
    )

)

(instance icon0 of IconI
    (properties
        view 990
        loop 0
        cel 0
        cursor 980
        type $5000
        message V_WALK
        signal $0041
        maskView 990
        maskLoop 13
        noun N_MOVEICON
        helpVerb V_HELP
    )

    (method (init)
        = lowlightColor gLowlightColor
        (super:init())
    )

    (method (select param1)
        (var temp0)
        return 
            (if ((super:select(rest param1)))
                (send gSq5IconBar:hide())
                return 1
            )(else
                return 0
            )
    )

)
(instance icon1 of IconI
    (properties
        view 990
        loop 1
        cel 0
        cursor 981
        message V_LOOK
        signal $0041
        maskView 990
        maskLoop 13
        noun N_EXAMINEICON
        helpVerb V_HELP
    )

    (method (init)
        = lowlightColor gLowlightColor
        (super:init())
    )

)
(instance icon2 of IconI
    (properties
        view 990
        loop 2
        cel 0
        cursor 982
        message V_DO
        signal $0041
        maskView 990
        maskLoop 13
        noun N_DOICON
        helpVerb V_HELP
    )

    (method (init)
        = lowlightColor gLowlightColor
        (super:init())
    )

)
(instance icon3 of IconI
    (properties
        view 990
        loop 3
        cel 0
        cursor 983
        message V_TALK
        signal $0041
        maskView 990
        maskLoop 13
        maskCel 4
        noun N_TALKICON
        helpVerb V_HELP
    )

    (method (init)
        = lowlightColor gLowlightColor
        (super:init())
    )

)

// Use this icon for whatever action you want
(instance icon4 of IconI
    (properties
        view 990
        loop 10			// This is currently a loop with "empty" cels
        cel 0
        cursor 999		// The cursor view associated with your action.
        message 0		// The verb associated with this action.
        signal $0041
        maskView 990
        maskLoop 13
        maskCel 4
        noun 0			// The noun for your button
        helpVerb V_HELP
    )

    (method (init)
        = lowlightColor gLowlightColor
        (super:init())
    )

)
(instance icon6 of IconI
    (properties
        view 990
        loop 4
        cel 0
        cursor 999
        message 0
        signal $0041
        maskView 990
        maskLoop 13
        maskCel 4
        noun N_INVENTORYICON
        helpVerb V_HELP
    )

    (method (init)
        = lowlightColor gLowlightColor
        (super:init())
    )


    (method (select param1)
        (var newEvent, temp1, gSq5IconBarCurInvIcon, temp3, temp4)
        return 
            (if (& signal icDISABLED)
                0
            )(else
                (if ((paramTotal and param1) and (& signal notUpd))
                    = gSq5IconBarCurInvIcon (send gSq5IconBar:curInvIcon)
                    (if (gSq5IconBarCurInvIcon)
                        = temp3 (+ (/ (- (- nsRight nsLeft) CelWide((send gSq5IconBarCurInvIcon:view) 2 (send gSq5IconBarCurInvIcon:cel))) 2) nsLeft)
                        = temp4 (+ (+ (send gSq5IconBar:y) (/ (- (- nsBottom nsTop) CelHigh((send gSq5IconBarCurInvIcon:view) 2 (send gSq5IconBarCurInvIcon:cel))) 2)) nsTop)
                    )
                    = temp1 1
                    DrawCel(view loop temp1 nsLeft nsTop -1)
                    = gSq5IconBarCurInvIcon (send gSq5IconBar:curInvIcon)
                    (if (gSq5IconBarCurInvIcon)
                        DrawCel((send (= gSq5IconBarCurInvIcon (send gSq5IconBar:curInvIcon)):view) 2 (send gSq5IconBarCurInvIcon:cel) temp3 temp4 -1)
                    )
                    Graph(grUPDATE_BOX nsTop nsLeft nsBottom nsRight 1)
                    (while (<> (send ((= newEvent (Event:new()))):type) 2)
                        (send newEvent:localize())
                        (if ((self:onMe(newEvent)))
                            (if (not temp1)
                                = temp1 1
                                DrawCel(view loop temp1 nsLeft nsTop -1)
                                = gSq5IconBarCurInvIcon (send gSq5IconBar:curInvIcon)
                                (if (gSq5IconBarCurInvIcon)
                                    DrawCel((send (= gSq5IconBarCurInvIcon (send gSq5IconBar:curInvIcon)):view) 2 (send gSq5IconBarCurInvIcon:cel) temp3 temp4 -1)
                                )
                                Graph(grUPDATE_BOX nsTop nsLeft nsBottom nsRight 1)
                            )
                        )(else
                            (if (temp1)
                                = temp1 0
                                DrawCel(view loop temp1 nsLeft nsTop -1)
                                = gSq5IconBarCurInvIcon (send gSq5IconBar:curInvIcon)
                                (if (gSq5IconBarCurInvIcon)
                                    DrawCel((send (= gSq5IconBarCurInvIcon (send gSq5IconBar:curInvIcon)):view) 2 (send gSq5IconBarCurInvIcon:cel) temp3 temp4 -1)
                                )
                                Graph(grUPDATE_BOX nsTop nsLeft nsBottom nsRight 1)
                            )
                        )
                        (send newEvent:dispose())
                    )
                    (send newEvent:dispose())
                    (if (== temp1 1)
                        DrawCel(view loop 0 nsLeft nsTop -1)
                        = gSq5IconBarCurInvIcon (send gSq5IconBar:curInvIcon)
                        (if (gSq5IconBarCurInvIcon)
                            DrawCel((send (= gSq5IconBarCurInvIcon (send gSq5IconBar:curInvIcon)):view) 2 (send gSq5IconBarCurInvIcon:cel) temp3 temp4 -1)
                        )
                        Graph(grUPDATE_BOX nsTop nsLeft nsBottom nsRight 1)
                    )
                    temp1
                )(else
                    1
                )
            )
    )

)
(instance icon7 of IconI
    (properties
        view 990
        loop 5
        cel 0
        cursor 999
        type $0000
        message 0
        signal $0043
        maskView 990
        maskLoop 13
        noun N_SELECTINVICON2
        helpVerb V_HELP
    )

    (method (init)
        = lowlightColor gLowlightColor
        (super:init())
    )


    (method (select param1)
        (var theGCursorNumber)
        return 
            (if ((super:select(rest param1)))
                (send gSq5IconBar:hide())
                = theGCursorNumber gCursorNumber
                (send gSq5Inv:showSelf(gEgo))
                (send gGame:setCursor(theGCursorNumber 1))
                return 1
            )(else
                return 0
            )
    )

)
(instance icon8 of IconI
    (properties
        view 990
        loop 7
        cel 0
        cursor 999
        message V_COMBINE
        signal $0043
        maskView 990
        maskLoop 13
        noun N_SETTINGSICON
        helpVerb V_HELP
    )

    (method (init)
        = lowlightColor gLowlightColor
        (super:init())
    )


    (method (select param1)
        return 
            (if ((super:select(rest param1)))
                (send gSq5IconBar:hide())
                (send gGame:showControls())
                return 1
            )(else
                return 0
            )
    )

)
(instance icon9 of IconI
    (properties
        view 990
        loop 9
        cel 0
        cursor 989
        type evHELP
        message V_HELP
        signal $0003
        maskView 990
        maskLoop 13
        noun N_HELPICON
        helpVerb V_HELP
    )

    (method (init)
        = lowlightColor gLowlightColor
        (if (gDialog)
            (send gDialog:dispose())
        )
        (super:init())
    )

)
(instance checkIcon of Code
    (properties)

    (method (doit param1)
        (if ((send param1:isKindOf(IconI)) and (& (send param1:signal) $0004))
            = gCheckedIcons (| gCheckedIcons (>> $8000 (send gSq5IconBar:indexOf(param1))))
        )
    )

)
(instance lb2DoVerbCode of Code
    (properties)

    (method (doit theVerb param2)
        (if ((User:canControl()))
            (if (== param2 gEgo)
                (if (Message(msgSIZE 0 N_EGO theVerb 0 1))
                    (send gTestMessager:say(N_EGO theVerb 0 0 0 0))
                )(else
                    (send gTestMessager:say(N_EGO 0 0 Random(1 2) 0 0))
                )
            )(else
                (switch (theVerb)
                    (case V_DO
                        (send gTestMessager:say(0 V_DO 0 Random(1 2) 0 0))
                    )
                    (case V_TALK
                        (send gTestMessager:say(0 V_TALK 0 Random(1 2) 0 0))
                    )
                    (default 
                        (if (not IsOneOf(theVerb V_LOOK))
                            (send gTestMessager:say(0 V_COMBINE 0 Random(2 3) 0 0))
                        )
                    )
                )
            )
        )
    )
)

(instance lb2FtrInit of Code
    (properties)

    (method (doit param1)
        (if (== (send param1:sightAngle) $6789)
            (send param1:sightAngle(90))
        )
        (if (== (send param1:actions) $6789)
            (send param1:actions(0))
        )
        (if (not (send param1:approachX) and not (send param1:approachY))
            (send param1:
                approachX((send param1:x))
                approachY((send param1:y))
            )
        )
    )
)

// This converts verbs into a bit flag mask
(instance lb2ApproachCode of Code
    (properties)

    (method (doit param1)
        (switch (param1)
            (case V_LOOK
                1
            )
            (case V_TALK
                2
            )
            (case V_WALK
                4
            )
            (case V_DO
                8
            )
            (case 31
                16
            )
            (case 29
                64
            )
            (case 25
                128
            )
            (default 
                $8000
            )
        )
    )
)

(instance sq5Win of BorderWindow
    (properties)
)
(instance sQ5Narrator of Narrator
    (properties)

    (method (init param1)
        = font gFont
        (self:back(gColorWindowBackground))
        (super:init(rest param1))
    )

)
(instance testMessager of Messager
    (properties)

    (method (findTalker talkerNumber)
        (var temp0)
        = gCurrentTalkerNumber talkerNumber
        = temp0 
            (switch (talkerNumber)
                (case NARRATOR
                    gSQ5Narrator
                )
                // Add more cases here for different narrators
                // (case 8
                    //ScriptID(109 7)
                //)
            )
            
        (if (temp0)
            return temp0
        )(else
            return (super:findTalker(talkerNumber))
        )
    )

)
