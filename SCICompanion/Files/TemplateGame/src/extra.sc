/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 extra.sc
 Contains a class for extras in your game (similar to actors).
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script EXTRA_SCRIPT)
/******************************************************************************/
(use "cycle")
(use "feature")
/******************************************************************************/
(class Extra of Prop
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
		cycleType 0
		hesitation 0
		pauseCel 0
		minPause 10
		maxPause 30
		minCycles 8
		maxCycles 20
		counter 0
		state $FFFF
		cycles 0
	)
	(method (init)
		= cel GetCel()
		(self:changeState(0))
		(super:init())
	)
	(method (doit)
		(if((== cycleType 1) and (== cel pauseCel) and (<> pauseCel 0))
			= cycles (+ hesitation 1)
		)
		(if((cycles) and (not --cycles))
			(self:cue())
		)
		(super:doit())
	)
	(method (cue)
		(if(& signal 5)
			return
		)
		(self:changeState( + state 1 ))
	)
	(method (stopExtra)
		(self:
			setCel(GetCel())
			stopUpd()
		)
	)
	(method (startExtra) 
		(self:changeState(1))
	)
	(method (changeState newState)
		= state newState
		(switch(state)
			(case 0
				(if(== counter 0)
		    		= cycles Random(minPause maxPause)
					(if(<> cycleType 0)
		    			= counter (- Random(minCycles maxCycles) 1)
		    		)
				)(else
					--counter
					(self:cue())
				)
			)
  			(case 1
  				(if(== cycleType 0)
		    		(self:setCycle(Fwd))
		    		= cycles Random(minCycles maxCycles)
				)(else
					(self:setCycle(End self))
		    	)
			)
  			(case 2
  				(if(== cycleType 2)
		    		= cycles hesitation
		    	)(else
  					(self:cue())
				)
			)
  			(case 3
  				(if(== cycleType 2)
		    		(self:setCycle(Beg self))
		    	)(else
  					(self:cue())
				)
			)
  			(case 4
  				(self:setCel(GetCel()))
  				(self:changeState(0))
			)
		)
	)
)
/******************************************************************************/
(procedure (GetCel) of Extra
	(switch(pauseCel)
		(case -1
			return(Random(0 (self:lastCel)))
		)
  		(case -2
  			return((self:lastCel))
		)
  		(default
		    (if(== pauseCel (== cycleType 0))	
		    	return(pauseCel)
			)
  		)
  	)
  	return(0)
)
/******************************************************************************/