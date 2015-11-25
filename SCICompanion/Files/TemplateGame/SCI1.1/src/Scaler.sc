(version 2)
(include "sci.sh")
(use "Print")
(use "Object")
(script 935)


/*
	Scaler lets you scale down the size of a view as it recedes into the distance.
		
	It is generally used as part of a setScale call on a :class:`Prop` or one of its subclasses::
		
		(send gEgo:setScale(Scaler frontSize backSize frontY backY))
		
*/
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

	/*
		:param heapPtr theClient: The object on which to apply the Scaler.
		:param number theFrontSize: The scale of the object (in percentage) when it's at theFrontY.
		:param number theBackSize: The scale of the object (in percentage) when it's at theBackY.
		:param number theFrontY: The largest (closest) y coordinate for the Scaler.
		:param number theBackY: The smallest (furthest) y coordinate for the Scaler.
	*/
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
            Prints("<Scaler> frontY cannot be equal to backY")
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
