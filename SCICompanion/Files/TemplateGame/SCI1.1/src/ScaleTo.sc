(version 2)
(include "sci.sh")
(include "game.sh")
(use "Main")
(use "Scaler")
(script SCALETO_SRIPT)

/*
	ScaleTo allows you to grow or shrink an object over time.
	
	Example usage::
	
		// Scale the portal down to 50% of its normal size.
		(portal:setScale(ScaleTo 50))
*/
(class ScaleTo of Scaler
    (properties
        client 0
        frontY 190
        backY 0
        frontSize 100
        backSize 0
        slopeNum 0
        slopeDen 0
        const 0
        caller 0
        endScale 0
        step 6
        waitCount 1
        scaleDir 0
        saveWaitCount 0
    )

	/*
	.. function:: init(theClient theEndScale [theCaller])
	
	.. function:: init(theClient theEndScale theStep [theCaller])
	
	.. function:: init(theClient theEndScale theStep theWaitCount [theCaller])
	
		Initializes the ScaleTo.
		
		:param heapPtr theClient: The :class:`Prop` to which this is attached.
		:param number theEndScale: The final scale of the Prop in percentage of full size.
		:param number theStep: The amount the scale increases or decreases each cycle. Smaller values are smoother and slower.
		:param number theWaitCount: The number of ticks to wait before the scaling begins.
		:param heapPtr theCaller: The object that will be cue()'d when the final scale size is reached.
	*/
    (method (init theClient theEndScale theCaller theCaller_2 theCaller_3)
        (if (paramTotal)
            = client theClient
            (if (>= paramTotal 2)
                = endScale theEndScale
                (if (>= paramTotal 3)
                    (if (IsObject(theCaller))
                        = caller theCaller
                    )(else
                        = step theCaller
                        (if (>= paramTotal 4)
                            (if (IsObject(theCaller_2))
                                = caller theCaller_2
                            )(else
                                = waitCount theCaller_2
                                (if (>= paramTotal 5)
                                    = caller theCaller_3
                                )
                            )
                        )
                    )
                )
            )
        )
        = waitCount (+ (= saveWaitCount waitCount) gLastTicks)
        = scaleDir 
            (if (<= (send client:maxScale) endScale)
                1
            )(else
                0
            )
    )


    (method (doit)
        (var temp0)
        (if (> (- gLastTicks waitCount) 0)
            = temp0 
                (if (scaleDir)
                    (+ (send client:maxScale) step)
                )(else
                    (- (send client:maxScale) step)
                )
            (send client:
                maxScale(temp0)
                scaleX(temp0)
                scaleY(temp0)
            )
            (if (scaleDir)
                (if (>= (send client:maxScale) endScale)
                    (self:dispose())
                )
            )(else
                (if (<= (send client:maxScale) endScale)
                    (self:dispose())
                )
            )
            = waitCount (+ saveWaitCount gLastTicks)
        )
    )


    (method (dispose)
        (var theCaller)
        = endScale 0
        = step 6
        = waitCount 1
        (send client:scaler(0))
        (if (caller)
            = theCaller caller
            = caller 0
            (send theCaller:cue())
        )
        (super:dispose())
    )

)
