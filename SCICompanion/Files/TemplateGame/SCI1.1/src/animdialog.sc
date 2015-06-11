(version 2)
(include "sci.sh")
(use "Main")
(use "Controls")
(use "Print")
(use "Blink")
(use "RandCycle")
(use "Obj")
(script 30)



(class AnimDialog of Dialog
    (properties
        elements 0
        size 0
        text 0
        font 0
        window 0
        theItem 0
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        time 0
        caller 0
        seconds 0
        lastSeconds 0
        eatTheMice 0
        lastTicks 0
    )

    (method (doit param1)
        (var temp0, temp1, temp2)
        = gLastTicks (+ global86 GetTime())
        = temp2 0
        (self:eachElementDo(#init))
        (if (theItem)
            (send theItem:select(0))
        )
        = theItem 
            (if (paramTotal and param1)
                param1
            )(else
                (self:firstTrue(#checkState 1))
            )
        (if (theItem)
            (send theItem:select(1))
        )
        (if (not theItem)
            = eatTheMice gEatTheMice
            = lastTicks GetTime()
        )(else
            = eatTheMice 0
        )
        = temp1 0
        (while (not temp1)
            (if (IsObject(gNewEventHandler))
                (send gNewEventHandler:eachElementDo(#doit))
            )(else
                (if (IsObject(gTheDoits))
                    (send gTheDoits:eachElementDo(#doit))
                )
            )
            = gLastTicks (+ global86 GetTime())
            (self:eachElementDo(#cycle))
            = temp0 (send ((Event:new())):localize())
            (if (eatTheMice)
                --eatTheMice
                (if (== (send temp0:type) 1)
                    (send temp0:type(0))
                )
                (while (== lastTicks GetTime())
                )
                = lastTicks GetTime()
            )
            (self:eachElementDo(#perform checkHiliteCode self temp0))
            = temp1 (self:handleEvent(temp0))
            (send temp0:dispose())
            (if ((self:check()))
                break
            )
            (if (== temp1 -2)
                break
            )
            Wait(1)
        )
        return temp1
    )

)
(instance checkHiliteCode of Code
    (properties)

    (method (doit param1 param2 param3)
        (if (((& (send param1:state) $0001) and (send param1:check(param3))) and not (& (send param1:state) $0008))
            (send ((send param2:theItem)):select(0))
            (send param2:theItem(param1))
            (send param1:select(1))
        )
    )

)
(class ChoiceNarrator of Narrator
    (properties
        x -1
        y -1
        z 0
        heading 0
        noun 0
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
        disposeWhenDone 1
        ticks 0
        talkWidth 0
        keepWindow 0
        modeless 0
        font 0
        cueVal 0
        initialized 0
        showTitle 0
        color 0
        back 5
        curVolume 0
        saveCursor 0
        whichSelect 0
        normal 0
        curNoun 0
        curVerb 0
        curCase 0
    )

    (method (display theText param2)
        (var theTalkWidth, newGSq5Win, temp2, temp3, temp4, temp5, newPrint)
        (if (normal)
            (super:display(theText rest param2))
        )(else
            (if (> (+ x talkWidth) 318)
                = theTalkWidth (- 318 x)
            )(else
                = theTalkWidth talkWidth
            )
            = newGSq5Win (send gSq5Win:new())
            (send newGSq5Win:
                color(color)
                back(back)
            )
            (if (not HaveMouse() and (<> gCursorNumber 996))
                = saveCursor gCursorNumber
                (send gGame:setCursor(996))
            )(else
                = saveCursor 0
            )
            (if (showTitle)
                (Print:addTitle(name))
            )
            = newPrint (Print:new())
            (send newPrint:
                dialog((AnimDialog:new()))
                window(newGSq5Win)
                posn(x y)
                font(font)
                width(theTalkWidth)
                addText(theText)
                modeless(0)
            )
            (if (not normal)
                = temp2 NodeValue((send ((send newPrint:dialog)):last()))
                = temp3 (+ (+ (- (send temp2:nsBottom) (send temp2:nsTop)) 4) y)
                = temp4 2
                (while (= temp5 Message(msgSIZE modNum curNoun curVerb curCase temp4))
                    (send newPrint:addColorButton((- temp4 1) curNoun curVerb curCase temp4 x temp3 modNum))
                    = temp2 NodeValue((send ((send newPrint:dialog)):last()))
                    = temp3 (+ temp3 (+ (- (send temp2:nsBottom) (send temp2:nsTop)) 4))
                    ++temp4
                )
                = whichSelect (send newPrint:init())
                (self:dispose(1))
            )(else
                (send newPrint:init())
            )
        )
    )

)
(class ChoiceTalker of Talker
    (properties
        x -1
        y -1
        z 0
        heading 0
        noun 0
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
        disposeWhenDone 1
        ticks 0
        talkWidth 318
        keepWindow 0
        modeless 0
        font 0
        cueVal 0
        initialized 0
        showTitle 0
        color 0
        back 5
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
        verb 0
        case 0
        whichSelect 0
        normal 0
        curNoun 0
        curVerb 0
        curCase 0
    )

    (method (show param1)
        (var temp0, temp1)
        (if (normal)
            (super:show(rest param1))
        )(else
            = temp1 GetPort()
            SetPort(0)
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
            SetPort(temp1)
            PicNotValid(temp0)
        )
    )


    (method (say param1 param2 param3)
        return 
            (if (normal)
                (super:say(param1 param2 rest param3))
            )(else
                (if ((> view 0) and not underBits)
                    (self:init())
                )
                (if (gSq5IconBar)
                    (send gSq5IconBar:disable())
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
                (if (IsObject(gNewEventHandler))
                    (send gNewEventHandler:add(self))
                )(else
                    = gNewEventHandler (EventHandler:new())
                    (send gNewEventHandler:
                        name("fastCast")
                        add(self)
                    )
                )
                (if (& global90 $0002)
                    (self:startAudio())
                )
                (if (& global90 $0001)
                    (self:startText(param1))
                )
                = ticks (+ (+ ticks 60) gLastTicks)
                return 1
            )
    )


    (method (startText param1 param2)
        (var temp0)
        return 
            (if (normal)
                (super:startText(param1 rest param2))
            )(else
                (if (not viewInPrint)
                    (self:show())
                )
                (if (not & global90 $0002)
                    = temp0 StrLen(param1)
                    = ticks Max(240 (* 8 temp0))
                )
                (if (mouth)
                    (send mouth:setCycle(RandCycle (* 4 temp0) 0 1))
                )
                (if (eyes and not (send eyes:cycler))
                    (send eyes:setCycle(Blink blinkSpeed))
                )
                (if (gDialog)
                    (send gDialog:dispose())
                )
                (self:display(param1))
                return temp0
            )
    )


    (method (display theText param2)
        (var temp0, theTalkWidth, temp2, newGSq5Win, temp4, temp5, temp6, temp7, newPrint)
        (if (normal)
            (super:display(theText rest param2))
        )(else
            = newGSq5Win (send gSq5Win:new())
            (send newGSq5Win:
                color(color)
                back(back)
            )
            (if (not HaveMouse() and (<> gCursorNumber 996))
                = saveCursor gCursorNumber
                (send gGame:setCursor(996))
            )(else
                = saveCursor 0
            )
            = newPrint (Print:new())
            (if (viewInPrint)
                = temp0 
                    (if (useFrame)
                        loop
                    )(else
                        (send bust:loop)
                    )
                (if (showTitle)
                    (send newPrint:addTitle(name))
                )
                (send newPrint:
                    window(newGSq5Win)
                    dialog((AnimDialog:new()))
                    posn(x y)
                    modeless(0)
                    font(font)
                    addIcon(view temp0 cel 0 0)
                    addText(theText + x textX)
                    width(theTalkWidth)
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
                    (send newPrint:addTitle(name))
                )
                (send newPrint:
                    window(newGSq5Win)
                    dialog((AnimDialog:new()))
                    posn(+ x textX + y textY)
                    modeless(0)
                    font(font)
                    addText(theText 4)
                    width(theTalkWidth)
                )
            )
            (if (not normal)
                = temp4 NodeValue((send ((send newPrint:dialog)):last()))
                = temp5 (+ (- (send temp4:nsBottom) (send temp4:nsTop)) 4)
                = temp6 2
                (while (= temp7 Message(msgSIZE modNum curNoun curVerb curCase temp6))
                    (send newPrint:addColorButton((- temp6 1) curNoun curVerb curCase temp6 4 temp5 modNum))
                    = temp4 NodeValue((send ((send newPrint:dialog)):last()))
                    = temp5 (+ temp5 (+ (- (send temp4:nsBottom) (send temp4:nsTop)) 4))
                    ++temp6
                )
                = whichSelect (send newPrint:init())
                (self:dispose(1))
            )(else
                (send newPrint:
                    width(theTalkWidth)
                    init()
                )
            )
        )
    )


    (method (cycle param1 param2)
        (var temp0, temp1[100], temp101)
        (if (normal)
            (super:cycle(param1 rest param2))
        )(else
            (if (param1 and (send param1:cycler))
                = temp101 GetPort()
                SetPort(0)
                = temp0 (send param1:cel)
                (send ((send param1:cycler)):doit())
                (if (<> temp0 (send param1:cel))
                    DrawCel((send param1:view) (send param1:loop) (send param1:cel) + (send param1:nsLeft) nsLeft + (send param1:nsTop) nsTop -1)
                    (send param1:nsRight((+ (send param1:nsLeft) CelWide((send param1:view) (send param1:loop) (send param1:cel)))))
                    (send param1:nsBottom((+ (send param1:nsTop) CelHigh((send param1:view) (send param1:loop) (send param1:cel)))))
                    Graph(grUPDATE_BOX + (send param1:nsTop) nsTop + (send param1:nsLeft) nsLeft + (send param1:nsBottom) nsTop + (send param1:nsRight) nsLeft 1)
                )
                SetPort(temp101)
            )
        )
    )

)
