// This script contains various classes that customize the way a window is drawn.
(version 2)
(include "sci.sh")
(use "SysWindow")
(script 936)

(procedure (drawWindow top left bottom right backgroundColor topBordColor lftBordColor botBordColor rgtBordColor bevelWid shadowWidth priority screens)
    (var oldPort, bevelWidCounter)
    = oldPort GetPort()
    SetPort(0)
    Graph(grFILL_BOX top left (+ bottom 1) (+ right 1) screens backgroundColor priority)
    = top (- top bevelWid)
    = left (- left bevelWid)
    = right (+ right bevelWid)
    = bottom (+ bottom bevelWid)
    Graph(grFILL_BOX top left (+ top bevelWid) right screens topBordColor priority)
    Graph(grFILL_BOX (- bottom bevelWid) left bottom right screens botBordColor priority)
    = bevelWidCounter 0    
    (while (< bevelWidCounter bevelWid)			
        Graph(grDRAW_LINE (+ top bevelWidCounter) (+ left bevelWidCounter) (- bottom (+ bevelWidCounter 1)) (+ left bevelWidCounter) lftBordColor priority -1)
        Graph(grDRAW_LINE (+ top bevelWidCounter) (- right (+ bevelWidCounter 1)) (- bottom (+ bevelWidCounter 1)) (- right (+ bevelWidCounter 1)) rgtBordColor priority -1)
        ++bevelWidCounter
    )
    (if (shadowWidth)
        Graph(grFILL_BOX (+ top shadowWidth) right (+ bottom shadowWidth) (+ right shadowWidth) screens 0 priority)
        Graph(grFILL_BOX bottom (+ left shadowWidth) (+ bottom shadowWidth) right screens 0 priority)
    )
    SetPort(oldPort)
)

/*
	Used for windows with borders. The border colors, bevel width and shadow width can be specified.
*/
(class BorderWindow of SysWindow
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
        topBordColor 7
        lftBordColor 6
        rgtBordColor 4
        botBordColor 3
        bevelWid 3
        shadowWid 2
    )

    (method (dispose)
        (super:dispose())
        SetPort(0)
    )

    (method (open)
        (var oldPort, screens)
        SetPort(0)
        = screens VISUAL
        (if (<> priority -1)
            = screens (| screens PRIORITY)
        )
        = lsTop (- top bevelWid)
        = lsLeft (- left bevelWid)
        = lsRight (+ (+ right bevelWid) shadowWid)
        = lsBottom (+ (+ bottom bevelWid) shadowWid)
        = type 128
        (super:open())
        drawWindow(top left bottom right back topBordColor lftBordColor botBordColor rgtBordColor bevelWid shadowWid priority screens)
        = oldPort GetPort()
        SetPort(0)
        Graph(grUPDATE_BOX lsTop lsLeft lsBottom lsRight VISUAL)
        SetPort(oldPort)
    )
)

/*
	Inset window extends :class:`BorderWindow` by providing an inner border in addition to and outer border.
*/
(class InsetWindow of BorderWindow
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


    (method (open)
        (var screens, oldPort, theTop, theLeft, theBottom, theRight)
        = screens VISUAL
        (if (<> priority -1)
            = screens (| screens PRIORITY)
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
        (super:open())
        drawWindow(theTop theLeft theBottom theRight insideColor topBordColor2 lftBordColor2 botBordColor2 rgtBordColor2 bevWid shadWid priority screens)
        = oldPort GetPort()
        SetPort(0)
        Graph(grUPDATE_BOX (- theTop bevWid) (- theLeft bevWid) (+ theBottom bevWid) (+ theRight bevWid) VISUAL)
        SetPort(oldPort)
    )

)
