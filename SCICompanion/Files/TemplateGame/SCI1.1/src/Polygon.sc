(version 2)
(include "sci.sh")
(use "Obj")
(script 946)


/*
	Polygon is a fundamental SCI1.1 class. One of its main uses is to create room obstacles. As a workflow aid, these can be created
	in SCI Companion's polygon editor, which will generate a list of points that can be included in a header file.
	
	There are four types of polgyons:
	
	PBarredAccess
		These bar access to the interior of the polygon.
	
	PContainedAccess
		These constrain an Actor within the polygon boundary (if the Actor is already inside).

	PTotalAccess
		These bar access to the interior of the polygon, *unless* the destination lies within the polygon.
		
	PNearestAccess
		These are just like PTotalAccess polygons, except when entering or leaving the polygon the Actor enters or leaves from the nearest edge.
	
	
*/
(class Polygon of Obj
    (properties
        size 0
        points 0
        type $0001
        dynamic FALSE
    )

	/*
	.. function:: init([thePoints ...])
	
		Initializes the Polygon with a list of points.
		
		:param number thePoints: An even number of parameters consisting of x and y coordinates.
		
		Example usage::
		
			(send global2:addObstacle((send ((Polygon:new())):
				type(PBarredAccess)
				init(185 137 181 149 135 148 128 137)	// Four points, defined by x and y coordinates.
				yourself()
			)

	*/
    (method (init thePoints)
        (var temp0[2])
        = size (/ paramTotal 2)
        = points Memory(memALLOC_CRIT (* 2 paramTotal))
        StrCpy(points @thePoints neg * paramTotal 2)
        = dynamic TRUE
    )

    (method (dispose)
        (if (dynamic and points)
            Memory(memFREE points)
        )
        (super:dispose())
    )

)
