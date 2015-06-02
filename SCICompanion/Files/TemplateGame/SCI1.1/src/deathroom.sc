(version 2)
(include "sci.sh")
(exports
    0 deathRoom
)
(use "Main")
(use "Osc")
(use "ForwardCounter")
(use "Cycle")
(use "Game")
(use "Feature")
(use "Obj")
(script 20)


(local
    local0[200]
    local200[28] = (58 73 87 103 128 142 136 130 128 132 139 147 160 173 184 191 193 189 189 185 176 165 154 144 121 97 105 120 )
    local228[28] = (90 100 104 106 104 101 106 110 112 116 121 127 129 131 129 126 124 125 129 133 138 145 153 161 168 170 176 185 )
    local256[28] = (0 1 2 3 4 5 6 7 8 9 10 11 12 0 1 2 3 3 4 5 6 2 3 4 3 1 0 1 )
    local284
    local285
    local286[52] = (8 7 7 7 3 3 2 2 3 1 3 2 2 3 3 3 7 2 2 4 2 2 2 2 1 6 6 0 3 1 1 8 1 1 5 2 7 8 8 2 1 6 7 7 7 8 8 8 7 1 3 7 )

)
(procedure (localproc_006e)
    (iWannaQuit:init())
    (iWannaRestore:init())
    (iWannaRestart:init())
    (send gUser:
        canControl(1)
        canInput(1)
    )
    (send gSq5IconBar:enable())
    (send gSq5IconBar:enable(2))
    (send gSq5IconBar:select((send gSq5IconBar:at(2))))
    (send gGame:setCursor(999))
)


(instance public deathRoom of Rm
    (properties
        picture 200
    )

    (method (init)
        Palette(palSET_INTENSITY 0 255 100)
        PalVary(pvUNINIT)
        (send gGame:handsOff())
        (super:init())
        (send gSq5Music1:
            number(45)
            loop(-1)
            play()
        )
        (send gSq5Music2:stop())
        (switch (local286[gDeathReason])
            (case 0
                (send global2:setScript(sRogBeatUp))
            )
            (case 1
                (send global2:setScript(sRogPuked))
            )
            (case 2
                (send global2:setScript(sRogBurned))
            )
            (case 3
                (send global2:setScript(sHeadExplodes))
            )
            (case 4
                (send global2:setScript(sAccordion))
            )
            (case 5
                (send global2:setScript(sFlattened))
            )
            (case 6
                (send global2:setScript(sFly))
            )
            (case 7
                (send global2:setScript(sGeneric))
            )
            (case 8
                (send global2:setScript(sPixelOut))
            )
        )
    )


    (method (doVerb theVerb param2)
        (switch (theVerb)
            (case 4
            )
            (default 
                (super:doVerb(theVerb rest param2))
            )
        )
    )

)
(instance sRogBeatUp of Script
    (properties)

    (method (changeState newState)
        (switch (= state newState)
            (case 0
                (send gGame:handsOff())
                (if (< gDeathReason 36)
                    Message(msgGET 20 2 0 0 gDeathReason @local0)
                )(else
                    Message(msgGET 20 1 0 0 - gDeathReason 35 @local0)
                )
                Display(@local0 100 143 68 102 0 103 5 105 1605 106 140 101 1)
                (rogPart1:
                    view(2000)
                    loop(0)
                    cel(0)
                    x(24)
                    y(79)
                    init()
                )
                (rogPart2:
                    view(2000)
                    loop(1)
                    cel(0)
                    x(67)
                    y(80)
                    setPri(14)
                    init()
                    setCycle(ForwardCounter 2 self)
                )
            )
            (case 1
                (rogPart2:
                    loop(2)
                    cel(0)
                    setCycle(End self)
                )
            )
            (case 2
                (rogPart2:
                    setLoop(-1)
                    setLoop(2)
                    cel((rogPart2:lastCel()))
                    setStep(2 10)
                    moveSpeed(0)
                    setCycle(0)
                    setMotion(MoveTo 67 200 self)
                )
            )
            (case 3
                (rogPart2:hide())
                localproc_006e()
                (self:dispose())
            )
        )
    )

)
(instance sRogPuked of Script
    (properties)

    (method (changeState newState)
        (switch (= state newState)
            (case 0
                (send gGame:handsOff())
                (if (< gDeathReason 36)
                    Message(msgGET 20 2 0 0 gDeathReason @local0)
                )(else
                    Message(msgGET 20 1 0 0 - gDeathReason 35 @local0)
                )
                Display(@local0 100 143 68 102 0 103 5 105 1605 106 140 101 1)
                (rogPart1:
                    view(2001)
                    loop(0)
                    cel(0)
                    x(24)
                    y(79)
                    init()
                )
                (rogPart2:
                    view(2001)
                    loop(1)
                    cel(0)
                    x(67)
                    y(80)
                    setPri(14)
                    init()
                    setCycle(End self)
                )
                (rogPart3:
                    view(2001)
                    loop(2)
                    cel(0)
                    x(44)
                    y(128)
                    setPri(14)
                    init()
                    setCycle(End self)
                )
            )
            (case 1
                localproc_006e()
                (self:dispose())
            )
        )
    )

)
(instance sRogBurned of Script
    (properties)

    (method (changeState newState)
        (switch (= state newState)
            (case 0
                (send gGame:handsOff())
                (if (< gDeathReason 36)
                    Message(msgGET 20 2 0 0 gDeathReason @local0)
                )(else
                    Message(msgGET 20 1 0 0 - gDeathReason 35 @local0)
                )
                Display(@local0 100 143 68 102 0 103 5 105 1605 106 140 101 1)
                (rogPart1:
                    view(2002)
                    loop(0)
                    cel(0)
                    x(24)
                    y(79)
                    init()
                )
                = seconds 2
            )
            (case 1
                Palette(palSET_FROM_RESOURCE 200 2)
                = ticks 15
            )
            (case 2
                (rogPart1:
                    view(2002)
                    loop(0)
                    cel(1)
                    x(40)
                    y(73)
                )
                = ticks 15
            )
            (case 3
                Palette(palSET_FROM_RESOURCE 202 2)
                = local284 0
                = ticks 30
            )
            (case 4
                (rogPart2:
                    view(2002)
                    loop(1)
                    cel(local256[local284])
                    x(local200[local284])
                    y(local228[local284])
                    setPri(14)
                    init()
                )
                = ticks 5
            )
            (case 5
                (rogPart2:
                    cel(local256[local284])
                    x(local200[
                        (if ((< 12 ++local284) and (< ++local284 21))
                            (rogPart2:loop(2))
                        )(else
                            (rogPart2:loop(1))
                        )
])
                    y(local228[local284])
                )
                (if (< local284 27)
                    --state
                )
                = ticks 5
            )
            (case 6
                (rogPart2:hide())
                localproc_006e()
                (self:dispose())
            )
        )
    )

)
(instance sHeadExplodes of Script
    (properties)

    (method (changeState newState)
        (switch (= state newState)
            (case 0
                (send gGame:handsOff())
                (if (< gDeathReason 36)
                    Message(msgGET 20 2 0 0 gDeathReason @local0)
                )(else
                    Message(msgGET 20 1 0 0 - gDeathReason 35 @local0)
                )
                Display(@local0 100 143 68 102 0 103 5 105 1605 106 140 101 1)
                (rogPart1:
                    view(2003)
                    loop(0)
                    cel(0)
                    x(24)
                    y(79)
                    init()
                )
                (rogPart2:
                    view(2003)
                    loop(1)
                    cel(0)
                    x(67)
                    y(80)
                    init()
                    setPri(14)
                    setCycle(End self)
                )
            )
            (case 1
                (rogPart2:
                    loop(2)
                    cel(0)
                    setCycle(End self)
                )
            )
            (case 2
                (if (== gDeathReason 28)
                    localproc_006e()
                    (self:dispose())
                )(else
                    (rogPart2:
                        loop(3)
                        cel(0)
                    )
                    = seconds 2
                )
            )
            (case 3
                (rogPart2:dispose())
                localproc_006e()
                (self:dispose())
            )
        )
    )

)
(instance sAccordion of Script
    (properties)

    (method (changeState newState)
        (switch (= state newState)
            (case 0
                (send gGame:handsOff())
                (if (< gDeathReason 36)
                    Message(msgGET 20 2 0 0 gDeathReason @local0)
                )(else
                    Message(msgGET 20 1 0 0 - gDeathReason 35 @local0)
                )
                Display(@local0 100 143 68 102 0 103 5 105 1605 106 140 101 1)
                (rogPart1:
                    view(2004)
                    loop(0)
                    cel(0)
                    x(24)
                    y(79)
                    init()
                    setCycle(Osc 3 self)
                )
            )
            (case 1
                (rogPart1:setCycle(End self))
            )
            (case 2
                localproc_006e()
                (self:dispose())
            )
        )
    )

)
(instance sFlattened of Script
    (properties)

    (method (changeState newState)
        (switch (= state newState)
            (case 0
                (send gGame:handsOff())
                (if (< gDeathReason 36)
                    Message(msgGET 20 2 0 0 gDeathReason @local0)
                )(else
                    Message(msgGET 20 1 0 0 - gDeathReason 35 @local0)
                )
                Display(@local0 100 143 68 102 0 103 5 105 1605 106 140 101 1)
                (rogPart1:
                    view(2005)
                    loop(0)
                    cel(0)
                    x(24)
                    y(79)
                    init()
                )
                (rogPart2:
                    view(2005)
                    loop(1)
                    cel(0)
                    x(67)
                    y(80)
                    setPri(14)
                    init()
                    setCycle(End self)
                )
            )
            (case 1
                = seconds 2
            )
            (case 2
                localproc_006e()
                (self:dispose())
            )
        )
    )

)
(instance sFly of Script
    (properties)

    (method (changeState newState)
        (switch (= state newState)
            (case 0
                (send gGame:handsOff())
                (if (< gDeathReason 36)
                    Message(msgGET 20 2 0 0 gDeathReason @local0)
                )(else
                    Message(msgGET 20 1 0 0 - gDeathReason 35 @local0)
                )
                Display(@local0 100 160 68 102 0 103 5 105 1605 106 120 101 1)
                (rogPart1:
                    view(2006)
                    loop(0)
                    cel(0)
                    x(20)
                    y(63)
                    init()
                )
                = seconds 2
            )
            (case 1
                localproc_006e()
                (self:dispose())
            )
        )
    )

)
(instance sGeneric of Script
    (properties)

    (method (changeState newState)
        (switch (= state newState)
            (case 0
                (send gGame:handsOff())
                (if (< gDeathReason 36)
                    Message(msgGET 20 2 0 0 gDeathReason @local0)
                )(else
                    Message(msgGET 20 1 0 0 - gDeathReason 35 @local0)
                )
                Display(@local0 100 143 68 102 0 103 5 105 1605 106 140 101 1)
                (rogPart1:
                    view(2001)
                    loop(0)
                    cel(0)
                    x(24)
                    y(79)
                    init()
                )
                (rogPart2:
                    view(2001)
                    loop(1)
                    cel(0)
                    x(67)
                    y(80)
                    setPri(14)
                    init()
                )
                (rogPart3:
                    view(2001)
                    loop(2)
                    cel(0)
                    x(44)
                    y(128)
                    setPri(14)
                    init()
                )
                = seconds 2
            )
            (case 1
                localproc_006e()
                (self:dispose())
            )
        )
    )

)
(instance sPixelOut of Script
    (properties)

    (method (changeState newState)
        (switch (= state newState)
            (case 0
                (send gGame:handsOff())
                (rogPart1:
                    view(2001)
                    loop(0)
                    cel(0)
                    x(24)
                    y(79)
                    init()
                    addToPic()
                )
                (rogPart2:
                    view(2001)
                    loop(1)
                    cel(0)
                    x(67)
                    y(80)
                    setPri(14)
                    init()
                    addToPic()
                )
                (rogPart3:
                    view(2001)
                    loop(2)
                    cel(0)
                    x(44)
                    y(128)
                    setPri(14)
                    init()
                    addToPic()
                )
                = seconds 2
            )
            (case 1
                DrawPic(200 dpOPEN_PIXELATION)
                = seconds 2
            )
            (case 2
                (if (< gDeathReason 36)
                    Message(msgGET 20 2 0 0 gDeathReason @local0)
                )(else
                    Message(msgGET 20 1 0 0 - gDeathReason 35 @local0)
                )
                Display(@local0 100 143 68 102 0 103 5 105 1605 106 140 101 1)
                localproc_006e()
                (self:dispose())
            )
        )
    )

)
(instance rogPart1 of Prop
    (properties
        signal $4000
    )

    (method (doVerb theVerb param2)
        (switch (theVerb)
            (case 4
            )
            (default 
                (super:doVerb(theVerb rest param2))
            )
        )
    )

)
(instance rogPart2 of Actor
    (properties
        signal $4000
    )

    (method (doVerb theVerb param2)
        (switch (theVerb)
            (case 4
            )
            (default 
                (super:doVerb(theVerb rest param2))
            )
        )
    )

)
(instance rogPart3 of Prop
    (properties
        signal $4000
    )

    (method (doVerb theVerb param2)
        (switch (theVerb)
            (case 4
            )
            (default 
                (super:doVerb(theVerb rest param2))
            )
        )
    )

)
(instance iWannaRestart of View
    (properties
        x 50
        y 170
        view 2099
        loop 1
    )

    (method (doVerb theVerb param2)
        (switch (theVerb)
            (case 4
                (self:cel(1))
                (send gGame:restart())
            )
            (default 
                (super:doVerb(theVerb rest param2))
            )
        )
    )

)
(instance iWannaRestore of View
    (properties
        x 150
        y 170
        view 2099
    )

    (method (doVerb theVerb param2)
        (switch (theVerb)
            (case 4
                (self:cel(1))
                (send gGame:restore())
            )
            (default 
                (super:doVerb(theVerb rest param2))
            )
        )
    )

)
(instance iWannaQuit of View
    (properties
        x 250
        y 170
        view 2099
        loop 2
    )

    (method (doVerb theVerb param2)
        (switch (theVerb)
            (case 4
                (self:cel(1))
                = gQuitGame TRUE
            )
            (default 
                (super:doVerb(theVerb rest param2))
            )
        )
    )

)
