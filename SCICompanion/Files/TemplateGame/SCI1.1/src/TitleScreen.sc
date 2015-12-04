(version 2)
(include "sci.sh")
(include "Verbs.sh")
(include "0.shm")
(include "game.sh")
(exports
    0 rm100
)
(use "Main")
(use "Controls")
(use "Print")
(use "Cycle")
(use "Game")
(use "Actor")
(use "System")
(script TITLEROOM_SCRIPT)

(instance myDialog of Dialog
    (properties)
)
(instance public rm100 of Room
    (properties
        picture 100
    )

    (method (init)
    	// Set port to the entire screen, since our title image is 200px high.
    	SetPort(0 0 200 320 0 0)
        (if (gDialog)
            (send gDialog:dispose())
        )
        (super:init())
        (send gOldMH:addToFront(self))
        (send gOldKH:addToFront(self))
        (send gGame:setCursor(996 1))
        (send gIconBar:
            hide()
            disable()
        )
        (send gUser:canInput(TRUE))
        (self:setScript(rmScript))
    )

    (method (dispose param1)
    	// Restore the port to standard size.
    	SetPort(0 0 190 320 10 0)
        (send gIconBar:
            hide()
            enable()
        )
        = gNormalCursor 999
        (send gGame:setCursor(996 1))
        (send gOldKH:delete(self))
        (send gOldMH:delete(self))
        (super:dispose(rest param1))
    )

    (method (handleEvent pEvent)
        (if ((<> (rmScript:state) 1) and (& (send pEvent:type) (| evVERB (| (| evMOUSEBUTTON evMOUSERELEASE) evKEYBOARD))) )
        	// Skip to state 4 if the keyboard or mouse is used
            (rmScript:changeState(1))
            (send pEvent:claimed(TRUE))
            return 
        )(else
            (super:handleEvent(pEvent))
        )
    )

)
(instance rmScript of Script
    (properties)

    (method (changeState newState)
        (var temp0, temp1[10])
        (switch (= state newState)
            (case 0
            	// Wait 4 seconds before going to the next state.
                = seconds 4
            )
            (case 1
                = seconds 0
                = gNormalCursor 999
                (send gGame:setCursor(999 1))
                (Print:
                    dialog(myDialog)
                    font(gFont)
                    width(150)
                    mode(alCENTER)
                    addText(N_TITLEMENU V_LOOK 0 4 0 0 0)
                    addText(N_TITLEMENU V_LOOK 0 5 0 10 0)
                    addColorButton(0 N_TITLEMENU V_LOOK 0 1 0 20 0 0 11 23 5 5 5)
                    addColorButton(1 N_TITLEMENU V_LOOK 0 2 0 30 0 0 11 23 5 5 5)
                )
                
                (= temp0 (Print:
                        addColorButton(2 N_TITLEMENU V_LOOK 0 3 0 40 0 0 11 23 5 5 5)
                        init()
								 ))
                
                (switch (temp0)
                    (case 0
                        (send gRoom:newRoom(110))
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