(version 2)
(include "sci.sh")
(use "Main")
(use "Obj")
(script 933)


/*
	Code that lets the player move the mouse cursor with the joystick or keyboard.
*/
(class PseudoMouse of Code
    (properties
        cursorInc 2
        minInc 2
        maxInc 20
        prevDir 0
        joyInc 5
    )

    (method (doit)
        (var gPEventX, gPEventY)
        = gPEventX (send gPEvent:x)
        = gPEventY (send gPEvent:y)
        (switch (prevDir)
            (case 1
                = gPEventY (- gPEventY cursorInc)
            )
            (case 2
                = gPEventX (+ gPEventX cursorInc)
                = gPEventY (- gPEventY cursorInc)
            )
            (case 3
                = gPEventX (+ gPEventX cursorInc)
            )
            (case 4
                = gPEventX (+ gPEventX cursorInc)
                = gPEventY (+ gPEventY cursorInc)
            )
            (case 5
                = gPEventY (+ gPEventY cursorInc)
            )
            (case 6
                = gPEventX (- gPEventX cursorInc)
                = gPEventY (+ gPEventY cursorInc)
            )
            (case 7
                = gPEventX (- gPEventX cursorInc)
            )
            (case 8
                = gPEventX (- gPEventX cursorInc)
                = gPEventY (- gPEventY cursorInc)
            )
        )
        (send gGame:setCursor(gCursorNumber 1 gPEventX gPEventY))
    )


    (method (handleEvent pEvent)
        (var pEventType, thePrevDir, pEventModifiers)
        = pEventType (send pEvent:type)
        = thePrevDir (send pEvent:message)
        = pEventModifiers (send pEvent:modifiers)
        (if (& pEventType evJOYSTICK)
            = prevDir thePrevDir
            = cursorInc 
                (if (& pEventType evKEYBOARD)
                    (if (& pEventModifiers emSHIFT)
                        minInc
                    )(else
                        maxInc
                    )
                )(else
                    joyInc
                )
            (if (& pEventType evKEYBOARD)
                (if (prevDir)
                    (self:doit())
                )(else
                    (send pEvent:claimed(FALSE))
                    return 
                )
            )(else
                (if (prevDir)
                    (self:start())
                )(else
                    (self:stop())
                )
            )
            (send pEvent:claimed(TRUE))
            return 
        )
    )


    (method (start thePrevDir)
        (if (paramTotal)
            = prevDir thePrevDir
        )
        (send gTheDoits:add(self))
    )


    (method (stop)
        = prevDir 0
        (send gTheDoits:delete(self))
    )

)
