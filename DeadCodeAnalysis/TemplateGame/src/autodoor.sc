/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 autodoor.sc
 Contains an extension of the Door class for simpler doors.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script AUTODOOR_SCRIPT)
/******************************************************************************/
(use "main")
(use "door")
(use "cycle")
/******************************************************************************/
(class AutoDoor of Door
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
		(super:init())
	)
	(method (doit)
		(super:doit())
		(if(code)
			(if(send code:doit(self))
		    	(self:open())
			)(else
				(self:close())
			)
		)(else
			(if( & (send gEgo:onControl) doorCtrl)
				(self:open())
			)(else
				(self:close())
		    )
		)
	)
	(method (open)
		(if( (not locked) and (<> doorState 1) and (<> doorState 2))
			= doorState 1
			(self:setCycle(End self))
			(if(openSnd)
				(send openSnd:play())
			)
		)
	)
	(method (close)
		(if((<> doorState 3) and (<> doorState 0))
			= doorState 3
			(self:setCycle(Beg self))
			(if(closeSnd)
				(send closeSnd:play())
			)
		)
	)
)
/******************************************************************************/
