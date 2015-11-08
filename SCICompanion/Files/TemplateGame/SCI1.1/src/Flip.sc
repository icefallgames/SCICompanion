(version 2)
(include "sci.sh")
(include "game.sh")
(exports
    0 FlipPolygons
    1 FlipFeature
)
(use "Main")
(use "Obj")
(script FLIPPOLY_SCRIPT)

/*
.. function:: FlipPolygons()

	Flips the current room's polygons.

.. function:: FlipPolygons(polygonCollection)

.. function:: FlipPolygons(polygon)

	Flips a single polygon, or a collection of polygons.
*/
(procedure public (FlipPolygons param1)
    (var temp0)
    (if (not paramTotal)
        = temp0 (send gRoom:obstacles)
    )(else
        (if ((send param1:isKindOf(Collect)))
            = temp0 param1
        )(else
            (send param1:perform(flipPoly))
            return 
        )
    )
    (send temp0:eachElementDo(#perform flipPoly))
    DisposeScript(FLIPPOLY_SCRIPT)
)

/*
.. function:: FlipFeature()

	Flips the current room's features.

.. function:: FlipFeature(featureCollection)

.. function:: FlipFeature(feature)

	Flips a single feature, or a collection of features.
*/
(procedure public (FlipFeature param1)
    (var temp0)
    (if (not paramTotal)
        (send gOldFeatures:eachElementDo(#perform flipFeature))
    )(else
        = temp0 0
        (while (< temp0 paramTotal)
            (if ((send param1[temp0]:isKindOf(Collect)))
                (send param1[temp0]:eachElementDo(#perform flipFeature))
            )(else
                (send param1[temp0]:perform(flipFeature))
            )
            ++temp0
        )
    )
    DisposeScript(FLIPPOLY_SCRIPT)
)


(instance flipPoly of Code
    (properties)

    (method (doit param1)
        (var temp0, temp1, temp2)
        = temp2 (send param1:size)
        = temp1 Memory(memALLOC_CRIT (* 4 temp2))
        = temp0 0
        (while (< temp0 temp2)
            Memory(memPOKE + temp1 (* 4 temp0) - 320 Memory(memPEEK (- (+ (send param1:points) (* 4 temp2)) (+ 4 (* 4 temp0)))))
            Memory(memPOKE + (+ temp1 (* 4 temp0)) 2 Memory(memPEEK - (+ (send param1:points) (* 4 temp2)) (+ 2 (* 4 temp0))))
            ++temp0
        )
        (if ((send param1:dynamic))
            Memory(memFREE (send param1:points))
        )
        (send param1:
            points(temp1)
            dynamic(1)
        )
    )

)
(instance flipFeature of Code
    (properties)

    (method (doit param1)
        (var temp0)
        (if (IsObject((send param1:onMeCheck)))
            FlipPolygons((send param1:onMeCheck))
        )(else
            = temp0 (send param1:nsLeft)
            (send param1:
                nsLeft(- 320 (send param1:nsRight))
                nsRight(- 320 temp0)
            )
        )
    )

)
