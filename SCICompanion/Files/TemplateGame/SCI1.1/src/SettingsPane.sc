/*
	This script contains the settings dialog and the controls that are used in the settings dialog.
*/
(version 2)
(include "sci.sh")
(include "game.sh")
(include "0.shm")
(exports
    0 gameControlCode
    1 gcWin
)
(use "Main")
(use "Print")
(use "Slider")
(use "BorderWindow")
(use "IconItem")
(use "GameControls")
(use "Object")
(script GAMECONTROLS_SCRIPT)

(instance gameControls of GameControls
    (properties)

    (method (dispatchEvent param1)
        (var winEraseOnly, temp1, temp2, temp3[50], temp53, temp54)
        = temp53 (send param1:type)
        = temp54 (send param1:message)
        return 
            (if (& temp53 evHELP)
                = temp1 (self:firstTrue(#onMe param1))
                (if (temp1 and (send ((= temp1 (self:firstTrue(#onMe param1)))):helpVerb))
                    = temp2 GetPort()
                    (if ((send gWindow:respondsTo(#eraseOnly)))
                        = winEraseOnly (send gWindow:eraseOnly)
                        (send gWindow:eraseOnly(1))
                        (Print:
                            font(gFont)
                            width(250)
                            addText((send temp1:noun) (send temp1:helpVerb) 0 1 0 0 (send temp1:modNum))
                            init()
                        )
                        (send gWindow:eraseOnly(winEraseOnly))
                    )(else
                        (Print:
                            font(gFont)
                            width(250)
                            addText((send temp1:noun) (send temp1:helpVerb) 0 1 0 0 (send temp1:modNum))
                            init()
                        )
                    )
                    SetPort(temp2)
                )
                (if (helpIconItem)
                    (send helpIconItem:signal((& (send helpIconItem:signal) $ffef)))
                )
                (send gGame:setCursor(999))
                return 0
            )(else
                (if (& temp53 $0040)
                    (switch (temp54)
                        (case 5
                            (if (IsObject(highlightedIcon) and (send highlightedIcon:respondsTo(#retreat)))
                                (send highlightedIcon:retreat())
                                return 0
                            )(else
                                (if (not IsObject(highlightedIcon) or (& (send highlightedIcon:signal) $0100))
                                    (self:advance())
                                    return 0
                                )
                            )
                        )
                        (case 1
                            (if (IsObject(highlightedIcon) and (send highlightedIcon:respondsTo(#advance)))
                                (send highlightedIcon:advance())
                                return 0
                            )(else
                                (if (not IsObject(highlightedIcon) or (& (send highlightedIcon:signal) $0100))
                                    (self:retreat())
                                    return 0
                                )
                            )
                        )
                        (default 
                            (super:dispatchEvent(param1))
                        )
                    )
                )(else
                    (super:dispatchEvent(param1))
                )
            )
    )
)

(instance public gameControlCode of Code
    (properties)

    (method (doit)
        = gGameControls gameControls
        (send gGameControls:
            add(detailSlider
            	(volumeSlider:
                    theObj(gGame)
                    selector(#masterVolume)
                    yourself()
                )
 				(speedSlider:
                    theObj(gEgo)
                    selector(#setSpeed)
                    yourself()
                )
 				(iconSave:
                    theObj(gGame)
                    selector(#save)
                    yourself()
                )
 				(iconRestore:
                    theObj(gGame)
                    selector(#restore)
                    yourself()
                )
 				(iconRestart:
                    theObj(gGame)
                    selector(#restart)
                    yourself()
                )
 				(iconQuit:
                    theObj(gGame)
                    selector(#quitGame)
                    yourself()
                )
 				iconOk
 				(iconAbout:
                    theObj(gGame)
                    selector(#showAbout)
                    yourself()
                )
                iconMessageMode
 				iconHelp)
            eachElementDo(#highlightColor 0)
            eachElementDo(#lowlightColor 4)
            eachElementDo(#modNum 0)
            eachElementDo(#helpVerb V_HELP)
            helpIconItem(iconHelp)
            window(gcWin)
            curIcon(iconSave)
            state(2048)
            show()
        )
    )

)
(instance public gcWin of BorderWindow
    (properties)

    (method (open)
        (var temp0, temp1[25], temp26[25], temp51)
        = temp51 -1
        (self:
            top(/ (- 200 (+ CelHigh(995 1 1) 6)) 2)
            left(/ (- 320 (+ 151 CelWide(995 0 1))) 2)
            bottom(+ (+ CelHigh(995 1 1) 6) (/ (- 200 (+ CelHigh(995 1 1) 6)) 2))
            right(+ (+ 151 CelWide(995 0 1)) (/ (- 320 (+ 151 CelWide(995 0 1))) 2))
            priority(temp51)
        )
        (super:open())
        DrawCel(995 0 5 + (/ (- (- (+ 151 CelWide(995 0 1)) (+ 4 CelWide(995 1 1))) CelWide(995 0 5)) 2) (+ 4 CelWide(995 1 1)) 3 temp51)
        DrawCel(995 1 1 4 3 temp51)
        DrawCel(995 1 0 94 38 temp51)
        DrawCel(995 1 0 135 38 temp51)
        DrawCel(995 0 4 63 (- 37 (+ CelHigh(995 0 4) 3)) temp51)
        DrawCel(995 0 3 101 (- 37 (+ CelHigh(995 0 4) 3)) temp51)
        DrawCel(995 0 2 146 (- 37 (+ CelHigh(995 0 4) 3)) temp51)
        Graph(grUPDATE_BOX 12 1 15 (+ 151 CelWide(995 0 1)) 1)
        SetPort(0)
    )
)

(instance detailSlider of Slider
    (properties
        view 995
        loop 0
        cel 1
        nsLeft 139
        nsTop 73
        signal $0080
        noun N_DETAILBUTTON
        helpVerb V_HELP
        sliderView 995
        bottomValue 1
        topValue 3
    )

    (method (doit param1)
        (if (paramTotal)
            (send gGame:detailLevel(param1))
        )
        (send gGame:detailLevel())
    )
)

(instance volumeSlider of Slider
    (properties
        view 995
        loop 0
        cel 1
        nsLeft 179
        nsTop 73
        signal $0080
        noun N_VOLUME
        helpVerb V_HELP
        sliderView 995
        topValue 15
    )
)

(instance speedSlider of Slider
    (properties
        view 995
        loop 0
        cel 1
        nsLeft 219
        nsTop 73
        signal $0080
        noun N_SPEED
        helpVerb V_HELP
        sliderView 995
        bottomValue 15
    )

    (method (doit param1)
        (if (paramTotal)
            (send gEgo:setSpeed(param1))
        )
        return gGEgoMoveSpeed
    )


    (method (show param1)
        (if (not (send gUser:controls))
            = signal 132
        )(else
            = signal 128
        )
        (super:show(rest param1))
    )


    (method (mask)
    )


    (method (move param1)
        (if ((send gUser:controls))
            (super:move(rest param1))
        )
    )
)

(instance iconSave of ControlIcon
    (properties
        view 995
        loop 2
        cel 0
        nsLeft 80
        nsTop 42
        message 8
        signal $01c3
        noun N_SAVE
        helpVerb V_HELP
    )
)

(instance iconRestore of ControlIcon
    (properties
        view 995
        loop 3
        cel 0
        nsLeft 80
        nsTop 62
        message 8
        signal $01c3
        noun N_RESTORE
        helpVerb V_HELP
    )
)

(instance iconRestart of ControlIcon
    (properties
        view 995
        loop 4
        cel 0
        nsLeft 80
        nsTop 82
        message 8
        signal $01c3
        noun N_RESTART
        helpVerb V_HELP
    )
)

(instance iconQuit of ControlIcon
    (properties
        view 995
        loop 5
        cel 0
        nsLeft 80
        nsTop 102
        message 8
        signal $01c3
        noun N_QUITMENU
        helpVerb V_HELP
    )
)

(instance iconAbout of ControlIcon
    (properties
        view 995
        loop 6
        cel 0
        nsLeft 80
        nsTop 142
        message 8
        signal $01c3
        noun N_GAMEINFO
        helpVerb V_HELP
    )
)

(instance iconHelp of IconItem
    (properties
        view 995
        loop 7
        cel 0
        nsLeft 106
        nsTop 142
        cursor 989
        message V_HELP
        signal $0183
        noun N_HELPICON
        helpVerb V_HELP
    )
)

(instance iconOk of IconItem
    (properties
        view 995
        loop 8
        cel 0
        nsLeft 80
        nsTop 122
        cursor 989
        message 8
        signal $01c3
        noun N_RETURNTOGAME
        helpVerb V_HELP
    )
)

(instance iconMessageMode of IconItem
    (properties
        view 995
        loop 9
        cel 0
        nsLeft 137
        nsTop 143
        cursor 989
        message 8
        signal $0183
        noun N_MSGMODE
        helpVerb V_HELP
    )
    
    (method (doit)
        (switch (global90)
            (case 1
                = global90 2
            )
            (case 2
                = global90 3
            )
            (case 3
                = global90 1
            )
        )
        (self:show())
    )

    (method (show param1)
        (switch (global90)
            (case 1
                = global83 0
                DrawCel(995 10 0 188 141 -1)
            )
            (case 2
                = global83 1
                DrawCel(995 10 1 188 141 -1)
            )
            (case 3
                = global83 1
                DrawCel(995 10 2 188 141 -1)
            )
        )
        Graph(grUPDATE_BOX 141 188 (+ 141 CelHigh(995 10))( + 188 CelWide(995 10)) 1)
        (super:show(rest param1))
    )    
)


