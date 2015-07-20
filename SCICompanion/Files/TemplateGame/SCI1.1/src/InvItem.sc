(version 2)
(include "sci.sh")
(include "game.sh")
(include "15.shm")
(exports
    0 invCode
    1 invWin
)
(use "Main")
(use "ScrollableInventory")
(use "ScrollInsetWindow")
(use "Print")
(use "IconI")
(use "InvI")
(use "Obj")
(script INVENTORY_SCRIPT)

(class InvItem of InvI
    (properties
        view 0
        loop 0
        cel 0
        nsLeft 0
        nsTop 0
        nsRight 0
        nsBottom 0
        state $0000
        cursor 999
        type evVERB
        message 0
        modifiers $0000
        signal $0000
        maskView 0
        maskLoop 0
        maskCel 0
        highlightColor 0
        lowlightColor 0
        noun 0
        modNum 0
        helpVerb 0
        owner 0
        script 0
        value 0
    )

    (method (doVerb theVerb)
        (var temp0, temp1)
        (if (not modNum)
            = modNum gModNum
        )
        (switch (theVerb)
            (case V_LOOK
                (if (Message(msgSIZE modNum noun V_LOOK 0 1))
                    = temp1 CelWide(view loop cel)
                    = temp0 GetPort()
                    (Print:
                        addIcon(view loop cel 0 0)
                        addText(noun V_LOOK 0 1 (+ temp1 4) 0 modNum)
                        init()
                    )
                    SetPort(temp0)
                )
            )
            (case V_DO
                (if (Message(msgSIZE modNum noun V_DO 0 1))
                    = temp0 GetPort()
                    (Print:
                        addText(noun V_DO 0 0 0 0 modNum)
                        init()
                    )
                    SetPort(temp0)
                )(else
                    = temp0 GetPort()
                    (Print:
                        addText(0 V_DO 0 0 0 0 modNum)
                        init()
                    )
                    SetPort(temp0)
                )
            )
            (default 
                (if (Message(msgSIZE modNum noun theVerb 0 1))
                    = temp0 GetPort()
                    (Print:
                        addText(noun theVerb 0 0 0 0 modNum)
                        init()
                    )
                    SetPort(temp0)
                )(else
                    = temp0 GetPort()
                    (Print:
                        addText(0 7 0 0 0 0 modNum)
                        init()
                    )
                    SetPort(temp0)
                )
            )
        )
    )

)
(instance templateInventory of ScrollableInventory
    (properties)

    (method (init param1)
        (super:init(rest param1))
        (invWin:
            color(gColorWindowForeground)
            back(3)
            topBordColor(5)
            lftBordColor(4)
            rgtBordColor(2)
            botBordColor(1)
            insideColor(2)
            topBordColor2(1)
            lftBordColor2(1)
            botBordColor2(6)
            rgtBordColor2(6)
        )
        (self:
        	// Add inventory items here.
            // add(Money AThing AnotherThing)
            eachElementDo(#lowlightColor 2)
            add(invLook invSelect invHelp invUp invDown ok)
        )
        (self:
            state(2048)
            upIcon(invUp)
            downIcon(invDown)
            window(invWin)
            helpIconItem(invHelp)
            selectIcon(invSelect)
            okButton(ok)
            numCols(5)
            scrollAmount(5)
            dispAmount(10)
            empty(13)
            normalHeading(15)
            eachElementDo(#highlightColor 0)
            eachElementDo(#modNum INVENTORY_SCRIPT)
            eachElementDo(#init)
        )
    )

)
(instance public invCode of Code
    (properties)

    (method (init)
        = gInv templateInventory
        (send gInv:init())
    )

)
(instance public invWin of ScrollInsetWindow
    (properties
        priority -1
        topBordHgt 30
        botBordHgt 5
    )

    (method (open param1)
        (invLook:nsLeft((- (/ (- (self:right) (self:left)) 2) 84)))
        (invLook:nsTop(2))
        (super:open(rest param1))
    )

)
(instance invUp of IconI
    (properties
        view 991
        loop 5
        cel 0
        cursor 999
        maskView 991
        maskLoop 5
        maskCel 2
        lowlightColor 5
        noun N_UPICON
        helpVerb V_HELP
    )

    (method (select param1)
        (if ((super:select(rest param1)))
            (send gInv:scroll(-1))
        )
        return 0
    )

)
(instance invDown of IconI
    (properties
        view 991
        loop 6
        cel 0
        cursor 999
        maskView 991
        maskLoop 6
        maskCel 2
        lowlightColor 5
        noun N_DOWNICON
        helpVerb V_HELP
    )

    (method (select param1)
        (if ((super:select(rest param1)))
            (send gInv:scroll(1))
        )
        return 0
    )

)
(instance ok of IconI
    (properties
        view 991
        loop 3
        cel 0
        cursor 999
        signal $0043
        lowlightColor 5
        noun N_CLOSEBUTTON
        helpVerb V_HELP
    )
)
(instance invLook of IconI
    (properties
        view 991
        loop 2
        cel 0
        cursor 981
        message 1
        signal $0081
        lowlightColor 5
        noun N_DESCICON
        helpVerb V_HELP
    )
)
(instance invHand of IconI
    (properties
        view 991
        loop 0
        cel 0
        cursor 982
        message 4
        lowlightColor 5
        noun N_DOICON
        helpVerb V_HELP
    )
)
(instance invHelp of IconI
    (properties
        view 991
        loop 1
        cel 0
        cursor 989
        message 5
        lowlightColor 5
        noun N_HELPICON
        helpVerb V_HELP
    )
)
(instance invSelect of IconI
    (properties
        view 991
        loop 4
        cel 0
        cursor 999
        lowlightColor 5
        noun N_SELECTICON
        helpVerb V_HELP
    )
)

// Declare inventory items here.
/*
(instance Money of InvItem
    (properties
        view 900
        loop 1
        cursor 900
        message V_PAY
        signal $0002
        noun N_MONEY
    )
)*/