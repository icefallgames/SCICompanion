// This script contains code that can be run when debugging your game starting in a particular room.
(version 2)
(include "sci.sh")
(include "game.sh")
(exports
	0 DebugRoomInit
)
(use "Main")
/******************************************************************************/
(script DEBUGROOM_SCRIPT)

/*
	Modify this function to set up inventory items or flags as necessary.
	
	:param number roomNumber: The room that the game is starting in when being debugged.
	
	Example::
	
		(case 150 // The room number
			// The ego must have the thing if he's in this room.
			(send gEgo:set(INV_THETHING))
		)
	
	See also: :doc:`/debugging`
*/
(procedure public (DebugRoomInit roomNumber)
	(switch (roomNumber)
		//(case 100
		//    (send gEgo:get(INV_THETHING))
		//)
	)
)