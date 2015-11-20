// This script contains some useful procedures for determining if something is in sight.
(version 2)
(include "sci.sh")
(exports
    0 IsOnScreen
    1 CantBeSeen
    2 AngleDiff
)
(use "Main")
(script 982)


/*
	Determines if an object is onscreen.
	
	:param heapPtr theObj: An object with x and y properties.
	:returns: TRUE if the object is onscreen, FALSE otherwise.
*/
(procedure public (IsOnScreen theObj)
    return not 
        (if (((<= 0 (send theObj:x)) and (<= (send theObj:x) 319)) and (<= 0 (- (send theObj:y) (send theObj:z))))
            <= (- (send theObj:y) (send theObj:z)) 189
        )(else
            0
        )
)

/*
.. function:: CantBeSeen(theSight [optSeer fieldAngle fieldDistance])

	Determines if an object can't be seen by another.
	
	:param heapPtr theSight: The object in question.
	:param heapPtr optSeer: The object we are asking, or the ego if not specified.
	:param number fieldAngle: The angle the looker object is looking, or lookerObj:sightAngle if not supplied.
	:param number fieldDistance: An optional distance limit.
	:returns: TRUE if the seer cannot see the sight, FALSE otherwise. FALSE if the sight is the seer.
*/
(procedure public (CantBeSeen theSight optSeer fieldAngle fieldDistance)
    (var theSeer, angleTemp, distanceTemp, sightX, sightY, seerX, seerY)
    = theSeer optSeer
    = angleTemp fieldAngle
    = distanceTemp fieldDistance
    (if (< paramTotal 4)
        = distanceTemp 32767
        (if (< paramTotal 3)
            (if (< paramTotal 2)
                = theSeer gEgo
            )
            = angleTemp (- 360 
                (if (== theSeer gEgo)
                    (* 2 (send theSeer:sightAngle))
                )(else
                    0
                )
					)
        )
    )
    = sightX (send theSight:x)
    = sightY (send theSight:y)
    = seerX (send theSeer:x)
    = seerY (send theSeer:y)
    return 
    	(if (== theSight theSeer)
    		FALSE
		)
		(else
			(if ((< (/ angleTemp 2) Abs(AngleDiff(GetAngle(seerX seerY sightX sightY) (send theSeer:heading)))) or
				(< distanceTemp GetDistance(seerX seerY sightX sightY gPicAngle))	)
				return TRUE
			)
			(else
				return FALSE
			)
		)
)

/*
	Calculates the difference between two angles given in degrees.
	
	:returns: The difference between the two angles, guarateed to be in the interval [-180,180).
*/
(procedure public (AngleDiff angle1 angle2)
    (if (>= paramTotal 2)
        = angle1 (- angle1 angle2)
    )
    return 
        (if (<= angle1 -180)
            + angle1 360
        )(else
            (if (> angle1 180)
                - angle1 360
            )(else
                angle1
            )
        )
)


