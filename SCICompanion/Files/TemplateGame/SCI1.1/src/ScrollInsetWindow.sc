(version 2)
(include "sci.sh")
(use "BorderWindow")
(script 27)



(procedure (localproc_0135 param1 param2 param3 param4 param5 param6 param7 param8 param9 param10 param11 param12 param13)
    (var temp0, temp1)
    = temp0 GetPort()
    SetPort(0)
    Graph(grFILL_BOX param1 param2 + param3 1 + param4 1 param13 param5 param12)
    = param1 (- param1 param10)
    = param2 (- param2 param10)
    = param4 (+ param4 param10)
    = param3 (+ param3 param10)
    Graph(grFILL_BOX param1 param2 + param1 param10 param4 param13 param6 param12)
    Graph(grFILL_BOX - param3 param10 param2 param3 param4 param13 param8 param12)
    = temp1 0
    (while (< temp1 param10)
        Graph(grDRAW_LINE + param1 temp1 + param2 temp1 - param3 (+ temp1 1) + param2 temp1 param7 param12 -1)
        Graph(grDRAW_LINE + param1 temp1 - param4 (+ temp1 1) - param3 (+ temp1 1) - param4 (+ temp1 1) param9 param12 -1)
        ++temp1
    )
    (if (param11)
        Graph(grFILL_BOX + param1 param11 param4 + param3 param11 + param4 param11 param13 0 param12)
        Graph(grFILL_BOX param3 + param2 param11 + param3 param11 param4 param13 0 param12)
    )
    SetPort(temp0)
)


(class ScrollInsetWindow of BorderWindow
    (properties
        top 0
        left 0
        bottom 0
        right 0
        color 0
        back 5
        priority 15
        window 0
        type $0000
        title 0
        brTop 0
        brLeft 0
        brBottom 190
        brRight 320
        lsTop 0
        lsLeft 0
        lsBottom 0
        lsRight 0
        eraseOnly 0
        topBordColor 5
        lftBordColor 4
        rgtBordColor 2
        botBordColor 1
        bevelWid 3
        shadowWid 2
        ck 3
        insideColor 2
        topBordColor2 0
        lftBordColor2 1
        botBordColor2 5
        rgtBordColor2 4
        topBordHgt 10
        botBordHgt 24
        sideBordWid 2
        shadWid 0
        bevWid 2
        xOffset 0
        yOffset 0
    )

    (method (open param1)
        (var temp0, temp1, theTop, theLeft, theBottom, theRight, temp6)
        (if (paramTotal and param1)
            = temp6 1
        )(else
            = temp6 0
        )
        = temp0 1
        (if (<> priority -1)
            = temp0 (| temp0 $0002)
        )
        = theTop top
        = theLeft left
        = theBottom bottom
        = theRight right
        = top (- top (+ bevelWid topBordHgt))
        = left (- left (+ bevelWid sideBordWid))
        = bottom (+ bottom (+ bevelWid botBordHgt))
        = right (+ right (+ bevelWid sideBordWid))
        = xOffset (+ bevelWid sideBordWid)
        = yOffset (+ bevelWid topBordHgt)
        (if (not temp6)
            (super:open())
        )
        localproc_0135(theTop theLeft theBottom theRight insideColor topBordColor2 lftBordColor2 botBordColor2 rgtBordColor2 bevWid shadWid priority temp0)
        = temp1 GetPort()
        SetPort(0)
        Graph(grUPDATE_BOX - theTop bevWid - theLeft bevWid + theBottom bevWid + theRight bevWid 1)
        SetPort(temp1)
    )

)
