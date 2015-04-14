/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 rm001.sc
 Contains the first room of your game. 
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script 1)
/******************************************************************************/
(use "main")
(use "controls")
(use "cycle")
(use "game")
(use "feature")
(use "obj")
(use "inv")
(use "door")
(use "jump")
(use "dpath")
/******************************************************************************/
(instance public rm001 of Rm
	(properties
		picture scriptNumber
		// Set up the rooms to go to/come from here
		north 0
		east 0
		south 0
		west 0
	)
	(method (init)
		// same in every script, starts things up
  		(super:init())
  		(self:setScript(RoomScript))
  		
  		// Check which room ego came from and position it
  		(switch(gPreviousRoomNumber)
            /******************************************************
             * Put the cases here for the rooms ego can come from *
             ******************************************************/ /*
            (case north
  				(send gEgo:
  					posn(210 110)
  					loop(2)
  				)
  			)*/
            // Set up ego's position if it hasn't come from any room
  			(default
  				(send gEgo:
  					posn(150 130)
  					loop(1)
  				)
  			)
  		)
		
		// Set up the ego
		SetUpEgo()		
		(send gEgo:init())

        /****************************************
         * Set up the room's music to play here *
         ****************************************/ /*
		(send gTheMusic:
			prevSignal(0)
			stop()
			number(scriptNumber)
			loop(-1)
			play()
		)*/

        /**************************************************
         * Add the rest of your initialization stuff here *
         **************************************************/
  )
)
/******************************************************************************/
(instance RoomScript of Script
	(properties)
	(method (handleEvent pEvent)
        (super:handleEvent(pEvent))

        /*****************************************
         * Handle the possible said phrases here *
         *****************************************/
        (if(Said('look'))
            Print("You are in an empty room")
        )
 	)
)
/******************************************************************************/
