/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 timer.sc
 Contains the timer class for setting up your events.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script TIMER_SCRIPT)
/******************************************************************************/
(use "main")
(use "obj")
/******************************************************************************/
(class Timer of Obj
	(properties
		cycleCnt	-1
		seconds		-1
		lastTime	-1
		client		0
	)
	(method (new)
		(if(== self Timer)
		    return( (super:new()) )
		)(else
			return(self)
		)
	)
	(method (init theClient)
		(var hTimer)
		= client theClient
		(send gTimers:add(self))
		(if( (send theClient:respondsTo(#timer)) )
			= hTimer (send theClient:timer)
			(if( IsObject( hTimer ) )
				(send hTimer:dispose())
			)
			(send theClient:timer(self))
		)
	)
	(method (doit)
		(var theTime)
		(if(<> cycleCnt -1)
			(if(not --cycleCnt)
				CueClient()
			)
		)(else
  			= theTime GetTime(gtTIME_OF_DAY)
  			(if(<> lastTime theTime)
				= lastTime theTime
				(if(not --seconds)
					CueClient()
				)
			)
		)
	)
	(method (dispose)
		(if( IsObject(client) and (send client:respondsTo(#timer)) )
			(send client:timer(NULL))
		)
		= client NULL
	)
	(method (set theClient sec min hour)
		(var hObj, cycleCounter, speed)
		= speed gSpeed
		(if(== speed 0)
			= speed 1
		)
		= cycleCounter (/ (* sec 60) speed)
		(if(> paramTotal 2)
			= cycleCounter (+ cycleCounter (/ (* min 3600) speed) )
		)
		(if(> paramTotal 3)		    
		    = cycleCounter (+ cycleCounter (* (/ (* hour 3600) speed) 60))
		)
		// Check if it's a class or instance
		// If it's an instance, use it, otherwise, create an instance
		(if( & objectInfo $8000 )
		    = hObj (self:new())
		)(else
		    = hObj self
		)
		(send hObj:
			init(theClient)
			cycleCnt(cycleCounter)
		)
		return(hObj)
	)
	(method (setCycle theClient cycleCounter)
		(var hObj)
		(if( & objectInfo $8000)
			= hObj (self:new())
		)(else
			= hObj self
		)
		(send hObj:
			init(theClient)
			cycleCnt(cycleCounter)
		)
		return(hObj)
	)
	(method (setReal theClient milliSec sec min)
		(var hTimer, theSeconds)
		= theSeconds milliSec
		(if(> paramTotal 2)
			= theSeconds (+ theSeconds (* sec 60) )  // one sec
		)
		(if(> paramTotal 3)
		    = theSeconds (+ theSeconds (* min 3600)) // one min
		)
		(if( & objectInfo $8000 )
		    = hTimer (self:new())
		)(else
		    = hTimer self
		) 
		(send hTimer:
			init(theClient)
			seconds(theSeconds)
		)
		return(hTimer)
	)
	(method (delete)
		(if(== client NULL)
			(send gTimers:delete(self))
			(super:dispose())
		)
	)
)
/******************************************************************************/
(class TO of Obj
	(properties
		timeLeft 0
	)
	(method (doit)
		(if(timeLeft)
			--timeLeft
		)
	)
	(method (set newTime)
		= timeLeft newTime
	)
)
/******************************************************************************/
(procedure (CueClient) of Timer
	(var hClient)
	= hClient client
	= client 0
	(if(IsObject(hClient) and (send hClient:respondsTo(#timer)) )
		(send hClient:timer(0))
	)
	(if( (send hClient:respondsTo(#cue)) )
		(send hClient:cue())
	)
)
/******************************************************************************/
