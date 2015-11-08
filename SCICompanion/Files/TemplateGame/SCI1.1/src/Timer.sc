// The Timer script contains the classes for timers, allowing the user to set up timed events.
(version 2)
(include "sci.sh")
(use "Main")
(use "Obj")
(script 973)



(procedure (localproc_0068) of Timer
    (var theClient)
    = theClient client
    = client 0
    (if (IsObject(theClient))
        (if ((send theClient:respondsTo(#timer)))
            (send theClient:timer(0))
        )
        (if ((send theClient:respondsTo(#cue)))
            (send theClient:cue())
        )
    )
)

/*
	A timer class that can count down in ticks, milliseconds, seconds, minutes or hours.
	
	Example usage::
	
		// Make a timer to cue() the current object in 3 seconds
		(send ((Timer:new())):set(self 3))
*/
(class Timer of Obj
    (properties
        cycleCnt -1
        seconds -1
        ticks -1
        lastTime -1
        client 0
    )

    (method (new)
        return 
            (if (== self Timer)
                (super:new())
            )(else
                self
            )
    )


    (method (init theClient)
        = client theClient
        (send gTimers:add(self))
        (if ((send theClient:respondsTo(#timer)))
            (if (IsObject((send theClient:timer)))
                (send ((send theClient:timer)):dispose())
            )
            (send theClient:timer(self))
        )
    )


    (method (doit)
        (var theLastTime)
        (if (<> cycleCnt -1)
            (if (not --cycleCnt)
                localproc_0068()
            )
        )(else
            (if (<> seconds -1)
                = theLastTime GetTime(gtTIME_OF_DAY)
                (if (<> lastTime theLastTime)
                    = lastTime theLastTime
                    (if (not --seconds)
                        localproc_0068()
                    )
                )
            )(else
                (if (> (- gLastTicks ticks) 0)
                    localproc_0068()
                )
            )
        )
    )


    (method (dispose)
        (if (IsObject(client) and (send client:respondsTo(#timer)))
            (send client:timer(0))
        )
        = client 0
    )

	/*
	.. function:: set(theClient theSeconds [theMinutes theHours])
	
		Sets the timer.
	
		:param heapPtr theClient: This object will have its cue() method called when the timer expires.
		:param number theSeconds: The number of seconds for the timer.
		:param number theMinutes: The number of minutes for the timer (optional).
		:param number theHours: The number of hours for the timer (optional).
	*/
    (method (set theClient theSeconds theMinutes theHours)
        (var temp0, temp1, temp2)
        = temp2 6
        (if (== temp2 0)
            = temp2 1
        )
        = temp1 (/ (* theSeconds 60) temp2)
        (if (> paramTotal 2)
            = temp1 (+ temp1 (/ (* theMinutes 3600) temp2))
        )
        (if (> paramTotal 3)
            = temp1 (+ temp1 (* (/ (* theHours 3600) temp2) 60))
        )
        = temp0 
            (if (& {-info-} $8000)
                (self:new())
            )(else
                self
            )
        (send temp0:
            init(theClient)
            cycleCnt(temp1)
        )
        return temp0
    )


    (method (setCycle theCycler sendParams)
        (var temp0)
        = temp0 
            (if (& {-info-} $8000)
                (self:new())
            )(else
                self
            )
        (send temp0:
            init(theCycler)
            cycleCnt(sendParams)
        )
        return temp0
    )

	/*
	.. function:: setReal(theClient theMilliseconds [theSeconds theMinutes])
	
		Sets the timer. This is more precise than set(), because you can specify milliseconds.
	
		:param heapPtr theClient: This object will have its cue() method called when the timer expires.
		:param number theMilliseconds: The number of seconds for the timer.
		:param number theSeconds: The number of seconds for the timer.
		:param number theMinutes: The number of minutes for the timer (optional).
	*/
    (method (setReal theClient theMilliseconds theSeconds theMinutes)
        (var temp0, ms)
        = ms theMilliseconds
        (if (> paramTotal 2)
            = ms (+ ms (* theSeconds 60))
        )
        (if (> paramTotal 3)
            = ms (+ ms (* theMinutes 3600))
        )
        = temp0 
            (if (& {-info-} $8000)
                (self:new())
            )(else
                self
            )
        (send temp0:
            init(theClient)
            seconds(ms)
        )
        return temp0
    )


    (method (delete)
        (if (== client 0)
            (send gTimers:delete(self))
            (super:dispose())
        )
    )


    (method (setTicks param1 param2)
        (var temp0)
        = temp0 
            (if (& {-info-} $8000)
                (self:new())
            )(else
                self
            )
        (send temp0:
            ticks(+ gLastTicks param1)
            init(param2)
        )
        return temp0
    )

)
(class TO of Obj
    (properties
        timeLeft 0
    )

    (method (doit)
        (if (timeLeft)
            --timeLeft
        )
    )


    (method (set theTimeLeft)
        = timeLeft theTimeLeft
    )

)
