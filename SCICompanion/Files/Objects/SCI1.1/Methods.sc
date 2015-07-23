(version 2)


(class AllTheMethods of Obj
	(properties
	)

	(method (init params)
		(super:init(rest params))
	)

	(method (doVerb theVerb params)
		(switch (theVerb)
			(default
				(super:doVerb(theVerb rest params))
			)
		)
	)

	(method (handleEvent pEvent)
		(super:handleEvent(pEvent))
	)

	(method (changeState newState)
		(= state newState)
		(switch (state)
			(case 0
			)
		)
	)
)

