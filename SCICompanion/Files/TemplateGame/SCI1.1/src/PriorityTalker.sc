(version 2)
(include "sci.sh")
(include "game.sh")
(use "Main")
(use "Talker")
(script PRITALKER_SCRIPT)

/*
	A PriorityTalker is similar to a :class:`Talker`, except that when the PriorityTalker is drawn, it obeys
	the priority value it is given, allowing it to appear partially behind other objects in the scene.
	This could be useful, for instance, if the talker was an image on a tv screen in a game.
*/
(class PriorityTalker of Talker
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
        priority 15
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
        priBits 0
    )

    (method (dispose param1)
        (if (mouth and underBits)
            (send mouth:cel(0))
            DrawCel((send mouth:view) (send mouth:loop) 0 + (send mouth:nsLeft) nsLeft + (send mouth:nsTop) nsTop priority)
        )
        (if (mouth and (send mouth:cycler))
            (if ((send ((send mouth:cycler)):respondsTo(#cue)))
                (send ((send mouth:cycler)):cue())
            )
            (send mouth:setCycle(0))
        )
        (if (not paramTotal or param1)
            (if (eyes and underBits)
                (send eyes:
                    setCycle(0)
                    cel(0)
                )
                DrawCel((send eyes:view) (send eyes:loop) 0 + (send eyes:nsLeft) nsLeft + (send eyes:nsTop) nsTop priority)
            )
            (self:hide())
        )
        (super:dispose(param1))
    )


    (method (hide)
        Graph(grRESTORE_BOX underBits)
        = underBits 0
        Graph(grRESTORE_BOX priBits)
        = priBits 0
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
        (if (not priBits)
            = priBits Graph(grSAVE_BOX nsTop nsLeft nsBottom nsRight 2)
        )
        = temp0 PicNotValid()
        PicNotValid(1)
        (if (bust)
            DrawCel((send bust:view) (send bust:loop) (send bust:cel) + (send bust:nsLeft) nsLeft + (send bust:nsTop) nsTop priority)
        )
        (if (eyes)
            DrawCel((send eyes:view) (send eyes:loop) (send eyes:cel) + (send eyes:nsLeft) nsLeft + (send eyes:nsTop) nsTop priority)
        )
        (if (mouth)
            DrawCel((send mouth:view) (send mouth:loop) (send mouth:cel) + (send mouth:nsLeft) nsLeft + (send mouth:nsTop) nsTop priority)
        )
        DrawCel(view loop cel nsLeft nsTop priority)
        Graph(grUPDATE_BOX nsTop nsLeft nsBottom nsRight 1)
        PicNotValid(temp0)
    )


    (method (cycle param1)
        (var temp0, temp1[100])
        (if (param1 and (send param1:cycler))
            = temp0 (send param1:cel)
            (send ((send param1:cycler)):doit())
            (if (<> temp0 (send param1:cel))
                DrawCel((send param1:view) (send param1:loop) (send param1:cel) + (send param1:nsLeft) nsLeft + (send param1:nsTop) nsTop priority)
                (send param1:nsRight((+ (send param1:nsLeft) CelWide((send param1:view) (send param1:loop) (send param1:cel)))))
                (send param1:nsBottom((+ (send param1:nsTop) CelHigh((send param1:view) (send param1:loop) (send param1:cel)))))
                Graph(grUPDATE_BOX + (send param1:nsTop) nsTop + (send param1:nsLeft) nsLeft + (send param1:nsBottom) nsTop + (send param1:nsRight) nsLeft 1)
            )
        )
    )

)
