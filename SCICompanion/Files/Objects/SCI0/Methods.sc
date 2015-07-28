(version 2)


(class AllTheMethods of Obj
	(properties
	)

	(method (init params)
		(super:init(rest params))
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

    (method (doit)
        (super:doit())
    )
)

