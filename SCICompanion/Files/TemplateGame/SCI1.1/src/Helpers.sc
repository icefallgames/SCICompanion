// This script contains various useful procedures.
(version 2)
(include "sci.sh")
(exports
    0 IsOnScreen
    1 IsFacing
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


(procedure public (IsFacing theObj theTheGEgo param3 param4)
    (var theGEgo, temp1, temp2, objX, objY, theGEgoX, theGEgoY)
    = theGEgo theTheGEgo
    = temp1 param3
    = temp2 param4
    (if (< paramTotal 4)
        = temp2 32767
        (if (< paramTotal 3)
            (if (< paramTotal 2)
                = theGEgo gEgo
            )
            = temp1 (- 360 
                (if (== theGEgo gEgo)
                    (* 2 (send theGEgo:sightAngle))
                )(else
                    0
                )
					)
        )
    )
    = objX (send theObj:x)
    = objY (send theObj:y)
    = theGEgoX (send theGEgo:x)
    = theGEgoY (send theGEgo:y)
    return 
        (if (not (<> theObj theGEgo) or (< (/ temp1 2) Abs(AngleDiff(GetAngle(theGEgoX theGEgoY objX objY) (send theGEgo:heading)))))
        )(else
            < temp2 GetDistance(theGEgoX theGEgoY objX objY gPicAngle)
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


