/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 door.sc
 Contains the Door class giving the ability to use doors which ego can use 
 to go from room to room. 
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script DOOR_SCRIPT)
/******************************************************************************/
(use "main")
(use "controls")
(use "cycle")
(use "feature")
/******************************************************************************/
(class Door of Prop
	(properties
		y 0
		x 0
		z 0
		heading 0
		yStep 2
		view 0
		loop 0
		cel 0
		priority 0
		underBits 0
		signal 0
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		lsTop 0
		lsLeft 0
		lsBottom 0
		lsRight 0
		brTop 0
		brLeft 0
		brBottom 0
		brRight 0
		cycleSpeed 1
		script 0
		cycler 0
		timer 0
		entranceTo 0
		locked 0
		openSnd 0
		closeSnd 0
		doorState 0
		doorCtrl 2
		roomCtrl 4
		doorBlock $4000
		code 0
		illegalBits 0
		force 0
		notify 0
	)
	(method (init)
		(if(gPreviousRoomNumber and (== gPreviousRoomNumber entranceTo))
			= doorState DOOR_OPEN
		)
		(if(== doorState DOOR_NULL)
			= cel 0
			(send gEgo:observeControl(doorBlock))
		)(else
			= cel (- NumCels(self) 1)
			= locked 0
			(send gEgo:ignoreControl(doorBlock))
		)
		(super:init())
		(self:
			stopUpd()
			ignoreActors()
		)
	)
	(method (doit)
		(super:doit())
		(if(<> doorState DOOR_OPEN)
		    (send gEgo:observeControl(doorBlock))
 		)(else
		    (send gEgo:ignoreControl(doorBlock))
		)
		(if(roomCtrl and entranceTo and ( & (send gEgo:onControl()) roomCtrl ))
			(send gRoom:newRoom(entranceTo))
		)
	)
	(method (cue)
		(if(== doorState DOOR_CLOSED)
			= doorState DOOR_NULL
		)(else
			= doorState DOOR_OPEN
		)
		(if(== doorState DOOR_OPEN)
		    (send gEgo:ignoreControl(doorBlock))
		)(else
		    (send gEgo:observeControl(doorBlock))
		)
		(self:stopUpd())
		(if(notify)
			(send notify:cue())
			= notify 0
 		)
	)
	(method (open)
		(if( (not force) and (<> (send gEgo:onControl(USE_POINT)) doorCtrl))
			PrintNotCloseEnough()
		)(else
			(if(locked)
				Print("You can't. It's locked.")
			)(else
				(if(== doorState DOOR_CUED or == doorState DOOR_OPEN)
					PrintItIs()
				)(else
					= doorState DOOR_CUED
		    		(self:setCycle(End self))
					(if(openSnd)
						(send openSnd:play())
					)
		    	)
		    )
		)
  
	)
	(method (close)
		(if( (not force) and (<> (send gEgo:onControl(USE_POINT)) doorCtrl) )
			PrintNotCloseEnough()
		)(else
			(if(locked)
				Print("You can't. It's locked.")
			)(else
				(if((== doorState DOOR_CLOSED) or (== doorState DOOR_CUED))
					PrintItIs()
				)(else
					(if( & (send gEgo:onControl()) doorBlock )
						Print("Please move out of the doorway first.")
					)(else
						= doorState DOOR_CLOSED
						(self:setCycle(Beg self))
						(if(closeSnd)
							(send closeSnd:play())
						)
		    		)
		    	)
		    )
		)
	)
)
/******************************************************************************/
