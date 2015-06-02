(version 2)
(include "sci.sh")
(use "Print")
(use "Obj")
(script 935)



(class Scaler of Code
    (properties
        client 0
        frontY 190
        backY 0
        frontSize 100
        backSize 0
        slopeNum 0
        slopeDen 0
        const 0
    )

    (method (init theClient theFrontSize theBackSize theFrontY theBackY)
        (if (paramTotal)
            = client theClient
            = frontSize theFrontSize
            = backSize theBackSize
            = frontY theFrontY
            = backY theBackY
        )
        = slopeNum (- frontSize backSize)
        (if (not = slopeDen (- frontY backY))
            TextPrint("<Scaler> frontY cannot be equal to backY")
            return 0
        )
        = const (- backSize (/ (* slopeNum backY) slopeDen))
        return (self:doit())
    )


    (method (doit)
        (var clientY, theBackSize)
        = clientY (send client:y)
        (if (< clientY backY)
            = theBackSize backSize
        )(else
            (if (> clientY frontY)
                = theBackSize frontSize
            )(else
                = theBackSize (+ (/ (* slopeNum clientY) slopeDen) const)
            )
        )
        = theBackSize (/ (* theBackSize 128) 100)
        (send client:
            scaleX(theBackSize)
            scaleY(theBackSize)
        )
    )

)
