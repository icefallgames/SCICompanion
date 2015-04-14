/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 jump.sc
 Contains a motion class to allows actors to jump.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script JUMP_SCRIPT)
/******************************************************************************/
(use "main")
(use "cycle")
/******************************************************************************/
(class Jump of Motion
	(properties
		client 0
		caller 0
		x 20000
		y 20000
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
		gx 0
		gy 3
		xStep 20000
		yStep 0
		signal 0
		illegalBits 0
		waitApogeeX 1
		waitApogeeY 1
	)
	(method (init theClient theCaller)
		(var theHeading)
		= client theClient
		(if(== paramTotal 2)
			= caller theCaller
		)
		= illegalBits (send client:illegalBits)
		= signal (send client:signal)
		(send client:
			illegalBits(0)
			setPri()
		)
		(if(== xStep 20000)
			= theHeading (send client:heading)
			(if( (> theHeading 330) or (> theHeading 30) or ((< 150 theHeading) and (< theHeading 210)) )
		    	= xStep 0
		    )(else
		    	(if(< theHeading 180)
		    		= xStep (send client:xStep)
				)(else
					= xStep (neg (send client:xStep))	
				)
			)
  		)
  		(if((not waitApogeeX) or (>= (* xStep gx) 0))
  			= waitApogeeX 0
  		)
  		(if((not waitApogeeY) or (>= (* yStep gy) 0))
  			= waitApogeeY 0
  		)
  		(self:setTest())
	)
	(method (doit)
		(var theXStep, theYStep)
		= xLast (send client:x)
		= yLast (send client:y)
		(send client:
			x(+ xLast xStep)
			x(+ yLast yStep)
		)
		= theXStep xStep
		= theYStep yStep
		= xStep (+ xStep gx)
		= yStep (+ yStep gy)
		
		(if((not waitApogeeX) and (<> x 20000) and (<= 0 (* dx (- (send client:x) x)) ))
			(send client:x(x))
			return((self:moveDone()))
  		)
		(if((not waitApogeeY) and (<> y 20000) and (<= 0 (* dy (- (send client:y) y)) ))
			(send client:y(y))
			return((self:moveDone()))
  		)
		(if(<= (* theXStep xStep) 0)
			= waitApogeeX 0
			(self:setTest())
		)
		(if(<= (* theYStep yStep) 0)
			= waitApogeeY 0
			(self:setTest())
		)
	)
	(method (moveDone)
		(send client:
			illegalBits(illegalBits)
			signal(signal)
		)
		(if(caller)
			= gCastMotionCue	TRUE
			= completed 		TRUE
		)
	)
	(method (motionCue)
		(send client:mover(NULL))
		(if(completed and IsObject(caller))
			(send caller:cue())
		)
		(self:dispose())
	)
	(method (setTest)
		(if( (> (send client:x) x) or ( (== (send client:x) x) or (> xStep 0) ))
			= dx -1
		)(else
			= dx 1
		)
		(if(> (send client:y) y)
			= dx -1
		)(else
			(if( (== (send client:y) y) or (> xStep 0))
		    	= dx 1
			)
		)
		(if( (> (send client:y) y) or ( (== (send client:y) y) or (> yStep 0) ))
			= dy -1
		)(else
			= dy 1
		)
	)
)
/******************************************************************************/
(class JumpTo of Jump
	(properties
		client 0
		caller 0
		x 20000
		y 20000
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
		gx 0
		gy 3
		xStep 20000
		yStep 0
		signal 0
		illegalBits 0
		waitApogeeX 1
		waitApogeeY 1
	)
	(method (init theClient theX theY theCaller)
		(var dx, dy)
		= client theClient
		= x theX
		= y theY
		
		(if((== x (send theClient:x)) and (== y (send theClient:y)))
			= illegalBits (send client:illegalBits)
			= signal (send client:signal)
			(self:moveDone())
  		)
  		= dx (- x (send theClient:x))
  		= dy (- y (send theClient:y))
  		SetJump(self dx dy gy)
  		(if( (not dx) )
  			= x 20000
  		)
  		(if( (not dy) )
  			= y 20000
  		)
  		(switch(paramTotal)
  			(case 3
  				(super:init(theClient))
			)
			(case 4
  				(super:init(theClient theCaller))
		    )
		)
	)
	(method (moveDone)
		(if(<> x 20000)
			(send client:x(x))
		)
		(if(<> y 20000)
			(send client:y(y))
		)
		(super:moveDone())
	)
)
/******************************************************************************/
