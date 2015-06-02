/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 cycle.sc
 Contains classes for views, props, acts and their descendants which handles 
 animation cycling.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script CYCLE_SCRIPT)
/******************************************************************************/
(use "main")
(use "obj")
/******************************************************************************/
(class Cycle of Obj
	(properties
		client 0
		caller 0
		cycleDir cdFORWARD
		cycleCnt 0
		completed 0
	)
	(method (init newClient)
		(if(paramTotal)
			= client newClient
		)
		= cycleCnt 0
		= completed FALSE
	)
	(method (nextCel)
		++cycleCnt
		(if(<= cycleCnt (send client:cycleSpeed))
			return (send client:cel)
		)(else
			= cycleCnt 0
			(if(& (send client:signal) $1000)
				return (send client:cel)
			)(else
				return (+ (send client:cel) cycleDir)
		    )
		)
	)
	(method (cycleDone))
	(method (motionCue)
		(send client:cycler(NULL))
		(if(completed and IsObject(caller))
			(send caller:cue())
		)
		(self:dispose())
	)
)
/******************************************************************************/
(class Fwd of Cycle
	(properties
		client 0
		caller 0
		cycleDir cdFORWARD
		cycleCnt 0
		completed 0
	)
	(method (doit)
		(var theCel)
		= theCel (self:nextCel())
		(if(> theCel (send client:lastCel))
			(self:cycleDone())
		)(else
			(send client:cel(theCel))
		)
	)
	(method (cycleDone)
		(send client:cel(0))
	)
)
/******************************************************************************/
(class Walk of Fwd
	(properties
		client 0
		caller 0
		cycleDir cdFORWARD
		cycleCnt 0
		completed 0
	)
	(method (doit)
		(if(not (send client:isStopped))
			(super:doit())
		)
	)
)
/******************************************************************************/
(class CT of Cycle
	(properties
		client 0
		caller 0
		cycleDir cdFORWARD
		cycleCnt 0
		completed 0
		endCel 0
	)
	(method (init theClient theEndCel theCycleDir theCaller)
		(var theLastCel)
		(super:init(theClient))
		= cycleDir theCycleDir
		(if(== paramTotal 4)
			= caller theCaller
		)
		= theLastCel (send client:lastCel)
		(if(> theEndCel theLastCel)
		    = endCel theLastCel
		)(else
		    = endCel theEndCel
		)
	)
	(method (doit)		
		(var theNextCel, theLastCel)
		= theLastCel (send client:lastCel)
		(if(> endCel theLastCel)
			= endCel theLastCel
		)
		= theNextCel (self:nextCel)
		(if(> theNextCel theLastCel)
		    (send client:cel(0))
		)(else
		    (if(< theNextCel 0)
		    	(send client:cel(theLastCel))
		    )(else
		    	(send client:cel(theNextCel))
		    )
  		)
  		(if((== cycleCnt 0) and (== endCel (send client:cel)))
		    (self:cycleDone())
		)
	)
	(method (cycleDone)
		= completed TRUE
		(if(caller)
			= gCastMotionCue TRUE
		)(else
			(self:motionCue())
		)
	)
)
/******************************************************************************/
(class End of CT
	(properties
		client 0
		caller 0
		cycleDir cdFORWARD
		cycleCnt 0
		completed 0
		endCel 0
	)
	(method (init theClient theCaller)
		(if(== paramTotal 2)
			(super:init(theClient (send theClient:lastCel) cdFORWARD theCaller))
		)(else
			(super:init(theClient (send theClient:lastCel) cdFORWARD NULL))
		)
	)
)
/******************************************************************************/
(class Beg of CT
	(properties
		client 0
		caller 0
		cycleDir cdFORWARD
		cycleCnt 0
		completed 0
		endCel 0
	)
	(method(init theClient theCaller)
		(if(== paramTotal 2)
		    (super:init(theClient 0 cdBACKWARD theCaller))
		)(else
		    (super:init(theClient 0 cdBACKWARD NULL))
		)
	)
)
/******************************************************************************/
(class Motion of Obj
	(properties
		client 0
		caller 0
		x 0
		y 0
		dx 0
		dy 0
		{b-moveCnt} 0
		{b-i1} 0
		{b-i2} 0
		{b-di} 0
		{b-xAxis} 0
		{b-incr} 0
		completed 0
		xLast 0
		yLast 0
	)
	(method (init theClient theX theY theCaller)
		(var theCycler, theLooper)
		(if(>= paramTotal 1)
			= client theClient
			(if(>= paramTotal 2)
				= x theX
				(if(>= paramTotal 3)
					= y theY
					(if(>= paramTotal 4)
						= caller theCaller
		    		)
		    	)
		    )
		)
		= completed FALSE
		= {b-moveCnt} 0
		= xLast 0
		= yLast 0
		= theCycler (send client:cycler)
		(if(theCycler)
			(send theCycler:cycleCnt(0))
		)
		(send client:heading( GetAngle((send client:x) (send client:y) x y) ))
		(if((send client:looper))
			= theLooper (send client:looper)
			(send theLooper:
				doit(client (send client:heading))
			)
		)(else
			DirLoop(client (send client:heading))
		)
		InitBresen(self)
	)
	(method (doit)
        (var s[40],t)
		(if(== {b-moveCnt} (send client:moveSpeed))
			= xLast (send client:x)
			= yLast (send client:y)
		)
        DoBresen(self)
        Animate()
		
		(if( (== x (send client:x)) and (== y (send client:y)) )
			(self:moveDone())
		)
	)
	(method (moveDone)
		= completed TRUE
		(if(caller)
			= gCastMotionCue TRUE
		)(else
			(self:motionCue())
		)
	)
	(method (setTarget newX newY)
		(if(paramTotal)
			= x newX
			= y newY
		)
	)
	(method (onTarget)
		(if( (== (send client:x) x) and (== (send client:y) y) )
			return(TRUE)
		)
        return(FALSE)
	)
	(method (motionCue)
		(send client:mover(NULL))
		(if(completed and IsObject(caller))
			(send caller:cue())
		)
		(self:dispose())
	)
)
/******************************************************************************/
(class MoveTo of Motion
	(properties
		client 0
		caller 0
		x 0
		y 0
		dx 0
		dy 0
		{b-moveCnt} 0
		{b-i1} 0
		{b-i2} 0
		{b-di} 0
		{b-xAxis} 0
		{b-incr} 0
		completed 0
		xLast 0
		yLast 0
	)
	(method (init params)
		(super:init(rest params))
	)
	(method (onTarget)
		(if((<= Abs(- (send client:x) x) (send client:xStep)) and
		    (<= Abs(- (send client:y) y) (send client:yStep)))
		    	return(TRUE)
		)
	)
)
/******************************************************************************/
