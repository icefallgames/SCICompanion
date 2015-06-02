(version 2)
(include "sci.sh")
(include "Verbs.sh")
(include "0.shm")
(exports
    0 rm100
)
(use "Main")
(use "Controls")
(use "Print")
(use "Cycle")
(use "Game")
(use "Feature")
(use "Obj")
(script 100)

(instance myDialog of Dialog
    (properties)

    (method (handleEvent pEvent)
        Palette(palANIMATE 64 254 -1)
        (super:handleEvent(rest pEvent))
    )

)
(instance public rm100 of Rm
    (properties
        picture 300
    )

    (method (init)
        Palette(palSET_FROM_RESOURCE 999 2)
        (if (gDialog)
            (send gDialog:dispose())
        )
        (super:init())
        (send gOldMH:addToFront(self))
        (send gOldKH:addToFront(self))
        (send gGame:setCursor(996 1))
        (send gSq5IconBar:
            hide()
            disable()
        )
        (send gUser:canInput(TRUE))
        (self:setScript(rmScript))
        
 /*       
        FormatPrint(
    "Free Heap: %u Bytes\nLargest ptr: %u Bytes\nFreeHunk: %u KBytes\nLargest hunk: %u Bytes"
    MemoryInfo(miFREEHEAP)
    MemoryInfo(miLARGESTPTR)
    (>> MemoryInfo(miFREEHUNK) 6)
    MemoryInfo(miLARGESTHUNK)
				   )
	*/			   
				   
				   
    )

    (method (dispose param1)
        (send gSq5IconBar:
            hide()
            enable()
        )
        = gCursor 999
        Palette(palSET_FROM_RESOURCE 999 2)
        (send gGame:setCursor(996 1))
        (send gOldKH:delete(self))
        (send gOldMH:delete(self))
        (super:dispose(rest param1))
    )

    (method (handleEvent pEvent)
        (if ((<> (rmScript:state) 4) and (& (send pEvent:type) (| evVERB (| (| evMOUSEBUTTON evMOUSERELEASE) evKEYBOARD))) )
        	// Skip to state 4 if the keyboard or mouse is used
            (rmScript:changeState(4))
            (send pEvent:claimed(1))
            return 
        )(else
            (super:handleEvent(pEvent))
        )
    )

)
(instance rmScript of Script
    (properties)

    (method (doit param1)
        Palette(palANIMATE 64 254 -1)
        (super:doit(rest param1))
    )

    (method (changeState newState)
        (var temp0, temp1[10])
        (switch (= state newState)
            (case 0
                (send gSq5Music1:
                    number(1001)
                    loop(1)
                    play()
                )
                = seconds 4
            )
            (case 1
                (if (== (send gSq5Music1:prevSignal) 20)
                	// sparkle removed
                )(else
                    --state
                    = cycles 1
                )
            )
            (case 2
                (if (== (send gSq5Music1:prevSignal) 30)
                	// sparkle removed
                )(else
                    --state
                    = cycles 1
                )
            )
            (case 3
                = seconds 3
            )
            (case 4
                (send gSq5Music1:stop())
                = cycles 5
            )
            (case 5
                = seconds 0
                = gCursor 999
                (send gGame:setCursor(999 1))
                (Print:
                    dialog(myDialog)
                    font(gFont)
                    width(150)
                    mode(1)
                    addText(N_TITLEMENU V_LOOK 0 4 0 0 0)
                    addText(N_TITLEMENU V_LOOK 0 5 0 10 0)
                    addColorButton(0 N_TITLEMENU V_LOOK 0 1 0 40 0 0 15 23 5 5 5)
                    addColorButton(1 N_TITLEMENU V_LOOK 0 2 0 50 0 0 15 23 5 5 5)
                )
                
                (= temp0 (Print:
                        addColorButton(2 N_TITLEMENU V_LOOK 0 3 0 70 0 0 15 23 5 5 5)
                        init()
								 ))
                
                (switch (temp0)
                    (case 0
                        (send global2:newRoom(110))
                    )
                    (case 1
                        (send gGame:restore())
                        (self:changeState(state))
                    )
                    (case 2
                    	(= gQuitGame TRUE)
                    )
                )
            )
        )
    )
)