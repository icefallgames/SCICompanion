(version 2)



(instance {name} of Feature
	(properties
		x 150
		y 100
		onMeCheck omcCOLORS
		noun N_NOUN
		nsTop_ 0
		nsLeft_ 0
		nsBottom_ 190
		nsRight_ 320
		sightAngle_ 40
		approachX_ 130
		approachY_ 120
		approachDist_ 10
	)

	(method (init_ params)
		(super:init(rest params))
	)

	(method (doVerb_ theVerb params)
		(switch (theVerb)
			(default
				(super:doVerb(theVerb rest params))
			)
		)
	)

	(method (handleEvent_ pEvent)
		(super:handleEvent(pEvent))
	)
)


(instance {name} of View
	(properties
		view 0
		x 150
		y 100
		signal ignAct
		z_ 0
		loop_ 0
		cel_ 0
		noun N_NOUN
		nsTop_ 0
		nsLeft_ 0
		nsBottom_ 190
		nsRight_ 320
		sightAngle_ 40
		approachX_ 130
		approachY_ 120
		approachDist_ 10
		priority_ 0
	)
	
	(method (init_ params)
		(super:init(rest params))
	)
	
	(method (doVerb_ theVerb params)
		(switch (theVerb)
			(default
				(super:doVerb(theVerb rest params))
			)
		)
	)

	(method (handleEvent_ pEvent)
		(super:handleEvent(pEvent))
	)
)

(instance {name} of Prop
	(properties
		view 0
		x 150
		y 100
		signal ignAct
		z_ 0
		loop_ 0
		cel_ 0
		noun N_NOUN
		nsTop_ 0
		nsLeft_ 0
		nsBottom_ 190
		nsRight_ 320
		sightAngle_ 40
		approachX_ 130
		approachY_ 120
		approachDist_ 10
		priority_ 0
		cycleSpeed_ 3
	)
	
	(method (init_ params)
		(super:init(rest params))
	)
	
	(method (doVerb_ theVerb params)
		(switch (theVerb)
			(default
				(super:doVerb(theVerb rest params))
			)
		)
	)

	(method (handleEvent_ pEvent)
		(super:handleEvent(pEvent))
	)
)



(instance {name} of Actor
	(properties
		view 0
		x 150
		y 100
		signal ignAct
		z_ 0
		loop_ 0
		cel_ 0
		noun N_NOUN
		nsTop_ 0
		nsLeft_ 0
		nsBottom_ 190
		nsRight_ 320
		sightAngle_ 40
		approachX_ 130
		approachY_ 120
		approachDist_ 10
		priority_ 0
		cycleSpeed_ 3
	)
	
	(method (init_ params)
		(super:init(rest params))
	)
	
	(method (doVerb_ theVerb params)
		(switch (theVerb)
			(default
				(super:doVerb(theVerb rest params))
			)
		)
	)

	(method (handleEvent_ pEvent)
		(super:handleEvent(pEvent))
	)
)

(instance {name} of Script
	(properties)

	(method (doit_)
		// This code gets called on every update.
		(super:doit())
	)

	(method (changeState newState)
		(= state newState)
		(switch (state)
			(case 0
			)
		)
	)

	(method (handleEvent_ pEvent)
		(super:handleEvent(pEvent))
	)
)

(instance public {name} of Talker
    (properties
        x 150
        y 100
        view 100
        loop 0
        talkWidth 150
        back 5
        textX 120
        textY 10
    )

    (method (init params)
        = font gFont
        (super:init(theBustProp theEyesProp theMouthProp rest params))
    )
)

(instance public {name} of Narrator
    (properties
        talkWidth 120
    )

    (method (init params)
        = font gFont
        = gWindow SpeakWindow
        (send gWindow:
            tailX(85)
            tailY(135)
            xOffset(15)
        )
        (super:init(rest params))
    )

    (method (dispose param1)
        = gWindow gWindow2
        (super:dispose(rest param1))
    )
)
