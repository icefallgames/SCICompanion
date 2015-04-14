/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 wander.sc
 Contains a movement class to allow actors to randomly wander around the screen.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script WANDER_SCRIPT)
/******************************************************************************/
(use "cycle")
/******************************************************************************/
(class Wander of Motion
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
		distance 30
	)
	(method (init theClient theDistance)
		(if(>= paramTotal 1)
			= client theClient
			(if(>= paramTotal 2)
				= distance theDistance
			)
		)
		(self:setTarget())
		(super:init(client))
	)
	(method (doit)
		(super:doit())
		(if((send client:isStopped))
			(self:moveDone())
		)
	)
	(method (moveDone)
		(self:init())
	)
	(method (setTarget)
		(var dist2)
		
		= dist2 (* distance 2)
		= x (+ (send client:x) (- distance Random(0 dist2)) )
		= y (+ (send client:y) (- distance Random(0 dist2)) )
	)
	(method (onTarget)
		return(FALSE)
	)
)
/******************************************************************************/
