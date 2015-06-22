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
(use "IconI")
(use "GameControls")
(use "Obj")
(script 24)



(class SQ5ControlItem of ControlIcon
    (properties
        view -1
        loop -1
        cel -1
        nsLeft 0
        nsTop -1
        nsRight 0
        nsBottom 0
        state $0000
        cursor -1
        type evVERB
        message -1
        modifiers $0000
        signal $0001
        maskView 0
        maskLoop 0
        maskCel 0
        highlightColor 0
        lowlightColor 0
        noun 0
        modNum 0
        helpVerb 0
        theObj 0
        selector 0
    )

    (method (select param1)
        (super:select(rest param1))
        (self:doit())
    )

)
(instance gameControls of GameControls
    (properties)

    (method (dispatchEvent param1)
        (var gSq5WinEraseOnly, temp1, temp2, temp3[50], temp53, temp54)
        = temp53 (send param1:type)
        = temp54 (send param1:message)
        return 
            (if (& temp53 evHELP)
                = temp1 (self:firstTrue(#onMe param1))
                (if (temp1 and (send ((= temp1 (self:firstTrue(#onMe param1)))):helpVerb))
                    = temp2 GetPort()
                    (if ((send gSq5Win:respondsTo(#eraseOnly)))
                        = gSq5WinEraseOnly (send gSq5Win:eraseOnly)
                        (send gSq5Win:eraseOnly(1))
                        (Print:
                            font(gFont)
                            width(250)
                            addText((send temp1:noun) (send temp1:helpVerb) 0 1 0 0 (send temp1:modNum))
                            init()
                        )
                        (send gSq5Win:eraseOnly(gSq5WinEraseOnly))
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
            add(detailSlider (volumeSlider:
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
 iconOk (iconAbout:
                    theObj(gGame)
                    selector(#showAbout)
                    yourself()
                )
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
        DrawCel(995 9 0 + 5 CelWide(995 1 1) + 38 CelHigh(995 0 1) temp51)
        Graph(grUPDATE_BOX 12 1 15 + 151 CelWide(995 0 1) 1)
        Message(msgGET 0 18 0 1 1 @temp1)
        Format(@temp26 "%d %s %d" gScore @temp1 gMaxScore)
        Display(@temp26 dsFONT gFont dsCOORD (+ (+ 5 CelWide(995 1 1)) 6) (+ (+ 38 CelHigh(995 0 1)) 15))
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
(instance iconSave of SQ5ControlItem
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
(instance iconRestore of SQ5ControlItem
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
(instance iconRestart of SQ5ControlItem
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
(instance iconQuit of SQ5ControlItem
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
(instance iconAbout of SQ5ControlItem
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
(instance iconHelp of IconI
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
(instance iconOk of IconI
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
