/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 dpath.sc
 Contains a motion class for actors allowing you to define paths which they
 will travel.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script DPATH_SCRIPT)
/******************************************************************************/
(use "cycle")
(use "obj")
/******************************************************************************/
(class DPath of Motion
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
		points 0
		value 0
	)
	(method (init theClient pPoints)
		(var i)
		(if(not points)
			= points (List:new())
		)
		(if(paramTotal)
			= client theClient
			= i 0
		    (while(<= i (- paramTotal 3))
		    	(send points:add(pPoints[i] pPoints[++i]))
		    	++i
		    )
  			(if(<= i (- paramTotal 2))
		    	= caller pPoints[i]
		    )
		)
  		(if(not (send points:contains($8000)))
		    (send points:add($8000))
		)
		(self:setTarget())
		(super:init())
	)
	(method (dispose)
		(if(IsObject(points))
			(send points:dispose())
		)
		(super:dispose())
	)
	(method (moveDone)
		(if(== (send points:at(value)) $8000)
			return(super:moveDone())
		)(else
			return(self:init())
		)
	)
	(method (setTarget)
		(if(<> (send points:at(value)) $8000)
		    = x (send points:at(value))
		    = y (send points:at(++value))
		   	++value
		)
	)
)
/******************************************************************************/
