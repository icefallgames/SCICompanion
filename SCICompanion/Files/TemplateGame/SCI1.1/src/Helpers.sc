(version 2)
(include "sci.sh")
(exports
    0 IsOnScreen
    1 proc982_1
    2 AngleDiff
)
(use "Main")
(script 982)



(procedure public (IsOnScreen param1)
    return not 
        (if (((<= 0 (send param1:x)) and (<= (send param1:x) 319)) and (<= 0 (- (send param1:y) (send param1:z))))
            <= (- (send param1:y) (send param1:z)) 189
        )(else
            0
        )
)


(procedure public (proc982_1 param1 theTheGEgo param3 param4)
    (var theGEgo, temp1, temp2, temp3, temp4, theGEgoX, theGEgoY)
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
    = temp3 (send param1:x)
    = temp4 (send param1:y)
    = theGEgoX (send theGEgo:x)
    = theGEgoY (send theGEgo:y)
    return 
        (if (not (<> param1 theGEgo) or (< (/ temp1 2) Abs(AngleDiff(GetAngle(theGEgoX theGEgoY temp3 temp4) (send theGEgo:heading)))))
        )(else
            < temp2 GetDistance(theGEgoX theGEgoY temp3 temp4 gPicAngle)
        )
)


(procedure public (AngleDiff param1 param2)
    (if (>= paramTotal 2)
        = param1 (- param1 param2)
    )
    return 
        (if (<= param1 -180)
            + param1 360
        )(else
            (if (> param1 180)
                - param1 360
            )(else
                param1
            )
        )
)


