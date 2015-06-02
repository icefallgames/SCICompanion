/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 titlescreen.sc
 Contains the title screen room.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script TITLESCREEN_SCRIPT)
/******************************************************************************/
(use "main")
(use "game")
(use "menubar")
(use "obj")
(use "cycle")
(use "user")
(use "controls")
(use "feature")
/******************************************************************************/
(instance public TitleScreen of Rm
	(properties
		picture scriptNumber
	)
	(method (init)
        // Set up the title screen
		ProgramControl() 
		= gProgramControl FALSE
		(send gGame:setSpeed(1))
		(SL:disable())
		(TheMenuBar:hide())
		
		(super:init())
		(self:setScript(RoomScript))
		
		(send gEgo:
			init()
			hide()
		)	
  
        /****************************************
         * Set up the room's music to play here *
         ****************************************/ /*
		(send gTheMusic:
			prevSignal(0)
			stop()
			number(scriptNumber)
			loop(-1)
			play()
		)       */

        /**************************************************
         * Add the rest of your initialization stuff here *
         **************************************************/
		Display(
			"Intro/Opening Screen"
			dsCOORD 90 80
			dsCOLOUR clWHITE
			dsBACKGROUND clTRANSPARENT
		)
	)
)
/******************************************************************************/
(instance RoomScript of Script
	(properties)     /*
    (method (changeState newState)
        = state newState
        /************************************
         * Add the state related stuff here *
         ************************************/
    //)
	(method (handleEvent pEvent)
        (super:handleEvent(pEvent))
        (if( not (send pEvent:claimed) )
			(if( (== (send pEvent:type) evKEYBOARD) and (== (send pEvent:message) $3C00) )
				ToggleSound()
			)(else
                /******************************************
                 * If the title screen has music, fade it *
                 ******************************************/
				//(send gTheMusic:fade())
    
                // End the title screen, start the game
				(send gRoom:newRoom(INITROOMS_SCRIPT))
		    )
		)
	)
)
/******************************************************************************/
