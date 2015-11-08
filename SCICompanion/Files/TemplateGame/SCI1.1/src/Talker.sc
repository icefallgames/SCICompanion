(version 2)
(include "sci.sh")
(use "Main")
(use "Print")
(use "Sync")
(use "RandCycle")
(use "Cycle")
(use "Actor")
(use "Obj")
(script 928)

/*
	Blink is a cycler used with :class:`Talker` to make eyes blink randomly.
*/
(class Blink of Cycle
    (properties
        client 0
        caller 0
        cycleDir 1
        cycleCnt 0
        completed 0
        waitCount 0
        lastCount 0
        waitMin 0
        waitMax 0
    )

    (method (init param1 param2)
        (if (paramTotal)
            = waitMin (/ param2 2)
            = waitMax (+ param2 waitMin)
            (super:init(param1))
        )(else
            (super:init())
        )
    )


    (method (doit)
        (var blinkNextCel)
        (if (waitCount)
            (if (> (- gLastTicks waitCount) 0)
                = waitCount 0
                (self:init())
            )
        )(else
            = blinkNextCel (self:nextCel())
            (if ((> blinkNextCel (send client:lastCel())) or (< blinkNextCel 0))
                = cycleDir neg cycleDir
                (self:cycleDone())
            )(else
                (send client:cel(blinkNextCel))
            )
        )
    )


    (method (cycleDone)
        (if (== cycleDir -1)
            (self:init())
        )(else
            = waitCount (+ Random(waitMin waitMax) gLastTicks)
        )
    )
)

/*
	Narrator is responsible for displaying messages and message sequences, and controlling the look
	of the text dialogs. A default Narrator
	(templateNarrator in main.sc) is defined as part of the template game, but you can create your
	own Narrator instances to define different text styles or colors for messages.
	
	If you add a new Narrator instance, you must assign it some talker number, and then add code in the
	findTalker method of testMessage in Main.sc to direct that talker number to the right script and export.
*/
(class Narrator of Prop
    (properties
        x -1
        y -1
        z 0
        heading 0
        noun 0
        _case 0
        modNum -1
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        sightAngle $6789
        actions 0
        onMeCheck omcDISABLE
        state $0000
        approachX 0
        approachY 0
        approachDist 0
        _approachVerbs 0
        yStep 2
        view -1
        loop 0
        cel 0
        priority 0
        underBits 0
        signal $0000
        lsTop 0
        lsLeft 0
        lsBottom 0
        lsRight 0
        brTop 0
        brLeft 0
        brBottom 0
        brRight 0
        scaleSignal $0000
        scaleX 128
        scaleY 128
        maxScale 128
        cycleSpeed 6
        script 0
        cycler 0
        timer 0
        detailLevel 0
        scaler 0
        caller 0
        disposeWhenDone 2
        ticks 0
        talkWidth 0
        keepWindow 0
        modeless 0
        font 0
        cueVal 0
        initialized 0
        showTitle 0
        color 0
        back 7
        curVolume 0
        saveCursor 0
    )

    (method (init)
    	(var theCurVolume_2, temp1, theCurVolume)
    	
        (if (((& global90 $0002) and not modeless) or not HaveMouse())
            = saveCursor gCursorNumber
            (send gGame:setCursor(gInvisibleCursor 1))
        )
        
        = gLastTicks (+ global86 GetTime())
        
        = initialized 1
    )


    (method (doit)
        (if ((<> ticks -1) and (> (- gLastTicks ticks) 0))
            (if (
            (if (& global90 $0002)
                (== DoAudio(audPOSITION) -1)
            )(else
                1
            )
 			and (not keepWindow or (& global90 $0002)))
                (self:dispose(disposeWhenDone))
                return 0
            )
        )
        return 1
    )

    (method (dispose theDisposeWhenDone)
        = ticks -1
       
        (if (not paramTotal or (== theDisposeWhenDone 1))
            (if (modeless)
                (send gOldKH:delete(self))
                (send gOldMH:delete(self))
                (send gTheDoits:delete(self))
            )(else
                (if (gNewEventHandler and (send gNewEventHandler:contains(self)))
                    (send gNewEventHandler:delete(self))
                    (if ((send gNewEventHandler:isEmpty()))
                        (send gNewEventHandler:dispose())
                        = gNewEventHandler 0
                    )
                )
            )
            (if (& global90 $0002)
                DoAudio(audSTOP)
            )
            = modNum -1
            = initialized 0
        )
        (if (gDialog)
            (send gDialog:dispose())
        )
        (if (saveCursor)
            (if (((& global90 $0002) and not modeless) or not HaveMouse())
                (send gGame:setCursor(saveCursor))
			)
        )(else
            = saveCursor 0
        )
        (if (caller)
            (send caller:cue(cueVal))
        )
        = cueVal 0
        DisposeClone(self)
    )


    (method (handleEvent pEvent)
        return 
            (if ((send pEvent:claimed))
            )(else
                (if (== ticks -1)
                    return 0
                )(else
                    (if (not cueVal)
                        (switch ((send pEvent:type))
                            (case 256
                                = cueVal 0
                            )
                            (case evMOUSEBUTTON
                                = cueVal (& (send pEvent:modifiers) emSHIFT)
                            )
                            (case evKEYBOARD
                                = cueVal (== (send pEvent:message) KEY_ESCAPE)
                            )
                        )
                    )
                    (if ((& (send pEvent:type) $4101) or ((& (send pEvent:type) evKEYBOARD) and IsOneOf((send pEvent:message) 13 27)))
                        (send pEvent:claimed(1))
                        (self:dispose(disposeWhenDone))
                    )
                )
            )
    )


    (method (say param1 param2)
        (if (gIconBar)
            (send gIconBar:disable())
        )
        (if (not initialized)
            (self:init())
        )
        = caller 
            (if ((> paramTotal 1) and param2)
                param2
            )(else
                0
            )
        (if (& global90 $0001)
            (self:startText(param1))
        )
        (if (& global90 $0002)
            (self:startAudio(param1))
        )
        (if (modeless)
            (send gOldMH:addToFront(self))
            (send gOldKH:addToFront(self))
            (send gTheDoits:add(self))
        )(else
            (if (IsObject(gNewEventHandler))
                (send gNewEventHandler:add(self))
            )(else
                = gNewEventHandler (EventHandler:new())
                (send gNewEventHandler:
                    name("fastCast")
                    add(self)
                )
            )
        )
        = ticks (+ (+ ticks 60) gLastTicks)
        return 1
    )


    (method (startText param1)
        (var temp0)
        // No need to check this. If we did the check, then if there's no audio, the ticks would be 0
        // startAudio is always called after this, and it sets ticks.
        //(if (not & global90 $0002)
            = temp0 StrLen(param1)
            = ticks Max(240 (* (* gTextReadSpeed 2) temp0))
        //)
        (if (gDialog)
            (send gDialog:dispose())
        )
        (self:display(param1))
        return temp0
    )


    (method (display theText)
        (var theTalkWidth, newGSq5Win, textBuffer[500])
        (if (> (+ x talkWidth) 318)
            = theTalkWidth (- 318 x)
        )(else
            = theTalkWidth talkWidth
        )
        = newGSq5Win (send gWindow:new())
        (send newGSq5Win:
            color(color)
            back(back)
        )
        (if (showTitle)
            (Print:addTitle(name))
        )
        (Print:
            window(newGSq5Win)
            posn(x y)
            font(font)
            width(theTalkWidth)
            modeless(1)
		)
		(if (& global90 $0002)
            Message(msgGET GetValueAt(theText 0) GetValueAt(theText 1) GetValueAt(theText 2) GetValueAt(theText 3) GetValueAt(theText 4) @textBuffer)
            (Print:addText(@textBuffer))
		)
		(else
			(Print:addText(theText))
		)
		(Print:            
            init()
        )
    )


    (method (startAudio param1)
        (var temp0, temp1, temp2, temp3, temp4)
        = temp0 GetValueAt(param1 0)
        = temp1 GetValueAt(param1 1)
        = temp2 GetValueAt(param1 2)
        = temp3 GetValueAt(param1 3)
        = temp4 GetValueAt(param1 4)
        (if (ResCheck(rsAUDIO36 temp0 temp1 temp2 temp3 temp4))
        	= ticks DoAudio(audPLAY temp0 temp1 temp2 temp3 temp4)
		)
    )
)

/*
	Talker is similar to :class:`Narrator`, but includes support for showing a bust, eyes and mouth
	while the messages are shown or spoken.
	The mouth can also be driven by lip-sync data, if present.
	
	If you add a new Talker instance, you must assign it some talker number, and then add code in the
	findTalker method of testMessage in Main.sc to direct that talker number to the right script and export.
*/
(class Talker of Narrator
    (properties
        x -1
        y -1
        z 0
        heading 0
        noun 0
        _case 0
        modNum -1
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        sightAngle $6789
        actions 0
        onMeCheck omcDISABLE
        state $0000
        approachX 0
        approachY 0
        approachDist 0
        _approachVerbs 0
        yStep 2
        view -1
        loop 0
        cel 0
        priority 0
        underBits 0
        signal $0000
        lsTop 0
        lsLeft 0
        lsBottom 0
        lsRight 0
        brTop 0
        brLeft 0
        brBottom 0
        brRight 0
        scaleSignal $0000
        scaleX 128
        scaleY 128
        maxScale 128
        cycleSpeed 6
        script 0
        cycler 0
        timer 0
        detailLevel 0
        scaler 0
        caller 0
        disposeWhenDone 2
        ticks 0
        talkWidth 318
        keepWindow 0
        modeless 0
        font 0
        cueVal 0
        initialized 0
        showTitle 0
        color 0
        back 7
        curVolume 0
        saveCursor 0
        bust 0
        eyes 0
        mouth 0
        viewInPrint 0
        textX 0
        textY 0
        useFrame 0
        blinkSpeed 100
    )

	/*
	.. function:: init([theBust theEyes theMouth])
	
		Override this method in your Talker instance, and call (super:init(bust eyes mouth)) with
		the appropriate Props for theBust, theEyes and theMouth. You will need to
		position them at the correct spot.
		
		:param heapPtr theBust: A :class:`Prop` instance for the overall face (bust) of the talker.
		:param heapPtr theEyes: A :class:`Prop` instance for the eyes.
		:param heapPtr theMouth: A :class:`Prop' instance for the talker's mouth.
	*/
    (method (init theBust theEyes theMouth)
   	
        (if (paramTotal)
            = bust theBust
            (if (> paramTotal 1)
                = eyes theEyes
                (if (> paramTotal 2)
                    = mouth theMouth
                )
            )
        )
        (self:setSize())
        (super:init())
    )


    (method (doit)
        (if ((super:doit()) and mouth)
            (self:cycle(mouth))
        )
        (if (eyes)
            (self:cycle(eyes))
        )
    )


    (method (dispose param1)
        (if (mouth and underBits)
            (send mouth:cel(0))
            DrawCel((send mouth:view) (send mouth:loop) 0 + (send mouth:nsLeft) nsLeft + (send mouth:nsTop) nsTop -1)
        )
        (if (mouth and (send mouth:cycler))
            (if ((send ((send mouth:cycler)):respondsTo(#cue)))
                (send ((send mouth:cycler)):cue())
            )
            (send mouth:setCycle(0))
        )
        (if (not paramTotal or (== param1 1))
            (if (eyes and underBits)
                (send eyes:
                    setCycle(0)
                    cel(0)
                )
                DrawCel((send eyes:view) (send eyes:loop) 0 + (send eyes:nsLeft) nsLeft + (send eyes:nsTop) nsTop -1)
            )
            (self:hide())
        )
        (super:dispose(param1))
    )


    (method (hide)
        Graph(grRESTORE_BOX underBits)
        = underBits 0
        Graph(grREDRAW_BOX nsTop nsLeft nsBottom nsRight)
        (if (gIconBar)
            (send gIconBar:enable())
        )
    )


    (method (show)
        (var temp0)
        (if (not underBits)
            = underBits Graph(grSAVE_BOX nsTop nsLeft nsBottom nsRight 1)
        )
        = temp0 PicNotValid()
        PicNotValid(1)
        (if (bust)
            DrawCel((send bust:view) (send bust:loop) (send bust:cel) + (send bust:nsLeft) nsLeft + (send bust:nsTop) nsTop -1)
        )
        (if (eyes)
            DrawCel((send eyes:view) (send eyes:loop) (send eyes:cel) + (send eyes:nsLeft) nsLeft + (send eyes:nsTop) nsTop -1)
        )
        (if (mouth)
            DrawCel((send mouth:view) (send mouth:loop) (send mouth:cel) + (send mouth:nsLeft) nsLeft + (send mouth:nsTop) nsTop -1)
        )
        DrawCel(view loop cel nsLeft nsTop -1)
        Graph(grUPDATE_BOX nsTop nsLeft nsBottom nsRight 1)
        PicNotValid(temp0)
    )


    (method (say param1)
        (if ((> view 0) and not underBits)
            (self:init())
        )
        (super:say(rest param1))
    )


    (method (startText param1)
        (var temp0)
        (if (not viewInPrint)
            (self:show())
        )
        = temp0 (super:startText(rest param1))
        DebugPrint("starting mouth")
        (if (mouth)
        	DebugPrint("yup, have mouth, setting rand cycle")
            (send mouth:setCycle(RandCycle (* 4 temp0) 0 1))
        )
        (if (eyes and not (send eyes:cycler))
            (send eyes:setCycle(Blink blinkSpeed))
        )
    )


    (method (display theText)
        (var temp0, theTalkWidth, temp2, newGSq5Win, textBuffer[500])
        = newGSq5Win (send gWindow:new())
        (send newGSq5Win:
            color(color)
            back(back)
        )
        (if (viewInPrint)
            = temp0 
                (if (useFrame)
                    loop
                )(else
                    (send bust:loop)
                )
            (if (showTitle)
                (Print:addTitle(name))
            )
            (Print:
                window(newGSq5Win)
                posn(x y)
                modeless(1)
                font(font)
                addText(theText)
                addIcon(view temp0 cel 0 0)
                init()
            )
        )(else
            (if (not + textX textY)
                = textX (+ (- nsRight nsLeft) 5)
            )
            = temp2 (+ nsLeft textX)
            (if (> (+ temp2 talkWidth) 318)
                = theTalkWidth (- 318 temp2)
            )(else
                = theTalkWidth talkWidth
            )
            (if (showTitle)
                (Print:addTitle(name))
            )
            (Print:
                window(newGSq5Win)
                posn(+ x textX + y textY)
                modeless(1)
                font(font)
                width(theTalkWidth)
            )
			(if (& global90 $0002)
	            Message(msgGET GetValueAt(theText 0) GetValueAt(theText 1) GetValueAt(theText 2) GetValueAt(theText 3) GetValueAt(theText 4) @textBuffer)
	            (Print:addText(@textBuffer))
			)
			(else
				(Print:addText(theText))
			)
			(Print:init())
            
        )
    )


    (method (startAudio param1)
        (var temp0, temp1, temp2, temp3, temp4, temp5)
        (self:show())
        (if (mouth)
            = temp0 GetValueAt(param1 0)
            = temp1 GetValueAt(param1 1)
            = temp2 GetValueAt(param1 2)
            = temp3 GetValueAt(param1 3)
            = temp4 GetValueAt(param1 4)
            
            (if (ResCheck(rsSYNC36 temp0 temp1 temp2 temp3 temp4))
            	DebugPrint("yup, have mouth, setting sync 36 cycle")
                (send mouth:setCycle(MouthSync temp0 temp1 temp2 temp3 temp4))
                = temp5 (super:startAudio(param1))
            )(else
                = temp5 (super:startAudio(param1))
                DebugPrint("yup, have mouth, setting rand")
                (send mouth:setCycle(RandCycle temp5 0))
            )
        )
        (else
        	= temp5 (super:startAudio(param1))
		)
        (if (eyes and not (send eyes:cycler))
            (send eyes:setCycle(Blink blinkSpeed))
        )
    )


    (method (cycle param1)
        (var temp0, temp1[100])
        (if (param1 and (send param1:cycler))
            = temp0 (send param1:cel)
            (send ((send param1:cycler)):doit())
            (if (<> temp0 (send param1:cel))
                DrawCel((send param1:view) (send param1:loop) (send param1:cel) + (send param1:nsLeft) nsLeft + (send param1:nsTop) nsTop -1)
                (send param1:nsRight((+ (send param1:nsLeft) CelWide((send param1:view) (send param1:loop) (send param1:cel)))))
                (send param1:nsBottom((+ (send param1:nsTop) CelHigh((send param1:view) (send param1:loop) (send param1:cel)))))
                Graph(grUPDATE_BOX + (send param1:nsTop) nsTop + (send param1:nsLeft) nsLeft + (send param1:nsBottom) nsTop + (send param1:nsRight) nsLeft 1)
            )
        )
    )


    (method (setSize)
        = nsLeft x
        = nsTop y
        = nsRight (+ nsLeft Max(
                (if (view)
                    CelWide(view loop cel)
                )(else
                    0
                )
 
                (if (IsObject(bust))
                    (+ (send bust:nsLeft) CelWide((send bust:view) (send bust:loop) (send bust:cel)))
                )(else
                    0
                )
 
                (if (IsObject(eyes))
                    (+ (send eyes:nsLeft) CelWide((send eyes:view) (send eyes:loop) (send eyes:cel)))
                )(else
                    0
                )
 
                (if (IsObject(mouth))
                    (+ (send mouth:nsLeft) CelWide((send mouth:view) (send mouth:loop) (send mouth:cel)))
                )(else
                    0
                )
				))
        = nsBottom (+ nsTop Max(
                (if (view)
                    CelHigh(view loop cel)
                )(else
                    0
                )
 
                (if (IsObject(bust))
                    (+ (send bust:nsTop) CelHigh((send bust:view) (send bust:loop) (send bust:cel)))
                )(else
                    0
                )
 
                (if (IsObject(eyes))
                    (+ (send eyes:nsTop) CelHigh((send eyes:view) (send eyes:loop) (send eyes:cel)))
                )(else
                    0
                )
 
                (if (IsObject(mouth))
                    (+ (send mouth:nsTop) CelHigh((send mouth:view) (send mouth:loop) (send mouth:cel)))
                )(else
                    0
                )
				))
    )
)
