/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 Follow.sc
 A motion class which allows actors to follow other actors.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script FOLLOW_SCRIPT)
/******************************************************************************/
(use "cycle")
/******************************************************************************/
(class Follow of Motion
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
		who 0
		distance 20
	)
	(method (init theClient theWho theDistance)
		(if(>= paramTotal 1)
			= client theClient
			(if(>= paramTotal 2)
				= who theWho
				(if(>= paramTotal 3)
					= distance theDistance
				)
		    )
 		)
 		(if(> (send client:distanceTo(who)) distance)
 			(super:
 				init( client (send who:x) (send who:y) )
 			)
		)
	)
	(method (doit)
		(var angle)
		(if(> (send client:distanceTo(who)) distance)
			(super:doit())
			(if(== {b-moveCnt} 0)
				(super:
					init( client (send who:x) (send who:y) )
				)
		    )
		)(else
			= xLast (send client:x)
			= yLast (send client:y)
			
			= angle GetAngle(xLast yLast (send who:x) (send who:y))

			(if( (send client:looper) )
				(send client:
					doit( client angle)
					(send client:looper())
				)
			)(else
				DirLoop(client angle)
		    )
		)
	)
	(method (moveDone))
	(method (setTarget sendParams)
		(if(paramTotal)
			(super:setTarget(rest sendParams))
		)(else
			(if( not (self:onTarget()) )
				(super:setTarget( (send who:x) (send who:y) ) )
		    )
		)
	)
	(method (onTarget)
		return(<= (send client:distanceTo(who)) distance)
	)
)
/******************************************************************************/
