/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 dcicon.sc
 Contains an extension of the DIcon class from controls.sc which allows 
 animation.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script DCICON_SCRIPT)
/******************************************************************************/
(use "controls")
(use "cycle")
/******************************************************************************/
(class DCIcon of DIcon
	(properties
		type ctlICON
		state 0
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		key 0
		said 0
		value 0
		view 0
		loop 0
		cel 0
		cycler 0
		cycleSpeed 6
		signal 0
	)
	(method (init)
		= cycler (Fwd:new())
		(send cycler:init(self))
	)
	(method (dispose)
		(if(cycler)
			(send cycler:dispose())
		)
		(super:dispose())
	)
	(method (cycle)
		(var theCel)
		(if(cycler)
			= theCel cel
			(send cycler:doit())
			(if(<> cel theCel)
				(self:draw())
		    )
		)
	)
	(method (lastCel)
		return(- NumCels(self) 1)
	)
)
/******************************************************************************/
