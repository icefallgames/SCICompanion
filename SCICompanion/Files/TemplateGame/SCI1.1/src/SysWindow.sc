(version 2)
(include "sci.sh")
(use "Object")
(script 981)


// The SysWindow class is a basic window class.
(class SysWindow of Object
    (properties
        top 0
        left 0
        bottom 0
        right 0
        color 0
        back 15
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
    )

    (method (dispose)
        (if (window)
            DisposeWindow(window eraseOnly)
            = window 0
        )
        (super:dispose())
    )


    (method (open)
        = window NewWindow(top left bottom right lsTop lsLeft lsBottom lsRight title type priority color back)
    )
)

// The Window class is an enhanced window class.
(class Window of SysWindow
    (properties
        top 0
        left 0
        bottom 0
        right 0
        color 0
        back 15
        priority -1
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
        underBits 0
    )

    (method (doit)
    )


    (method (dispose)
        (self:restore())
        (if (window)
            DisposeWindow(window)
            = window 0
        )
        (super:dispose())
    )


    (method (handleEvent)
        return 0
    )


    (method (setMapSet)
        (var temp0)
        = temp0 0
        (if (<> -1 color)
            = temp0 (| temp0 $0001)
        )
        (if (<> -1 priority)
            = temp0 (| temp0 $0002)
        )
        return temp0
    )


    (method (move param1 param2)
        = left (+ left param1)
        = right (+ right param2)
        = right (+ right param1)
        = bottom (+ bottom param2)
    )


    (method (moveTo param1 param2)
        (self:move((- param1 left) (- param2 top)))
    )


    (method (draw theColor thePriority)
        (if (>= paramTotal 1)
            = color theColor
        )
        (if (>= paramTotal 2)
            = priority thePriority
        )
        Graph(grFILL_BOX top left bottom right (self:setMapSet()) color priority)
    )


    (method (save)
        = underBits Graph(grSAVE_BOX top left bottom right (self:setMapSet()))
    )


    (method (restore)
        (if (underBits)
            Graph(grRESTORE_BOX underBits)
        )
    )


    (method (inset param1 param2)
        = top (+ top param2)
        = left (+ left param1)
        = bottom (- bottom param2)
        = right (- right param1)
    )


    (method (show)
        Graph(grUPDATE_BOX top left bottom right (self:setMapSet()))
    )


    (method (erase)
        (self:draw(back -1))
    )


    (method (center)
        (self:moveTo((/ (- (- brRight left) (- right left)) 2) (/ (- (- brBottom top) (- bottom top)) 2)))
    )

)
