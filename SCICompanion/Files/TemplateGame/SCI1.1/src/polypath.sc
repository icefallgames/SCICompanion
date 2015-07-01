(version 2)
(include "sci.sh")
(use "Main")
(use "Cycle")
(use "Obj")
(script 945)

// A MotionClass that follows a path around polygon obstacles.

(class PolyPath of Motion
    (properties
        client 0
        caller 0
        x 0
        y 0
        dx 0
        dy 0
        {b-moveCnt} 0
        {b-i1} 0
        {b-i2} 0
        {b-di} 0
        {b-xAxis} 0
        {b-incr} 0
        completed 0
        xLast 0
        yLast 0
        value 2
        points 0
        finalX 0
        finalY 0
        obstacles 0
    )

    (method (init theClient theFinalX theFinalY theCaller optimizationLevel theObstacles)
        (var temp0[30])
        (if (paramTotal)
            = client theClient
            (if (> paramTotal 1)
                (if (>= paramTotal 6)
                    = obstacles theObstacles
                )(else
                    (if (not IsObject(obstacles))
                        = obstacles (send gRoom:obstacles)
                    )
                )
                (if (points)
                    Memory(memFREE points)
                )
                = points AvoidPath((send theClient:x) (send theClient:y) (= finalX theFinalX) (= finalY theFinalY) 
                        (if (obstacles)
                            (send obstacles:elements)
                        )(else
                            0
                        )
 
                        (if (obstacles)
                            (send obstacles:size)
                        )(else
                            0
                        )
 
                        (if (>= paramTotal 5)
                            optimizationLevel
                        )(else
                            1
                        )
								  )
                (if (> paramTotal 3)
                    = caller theCaller
                )
            )
            (self:setTarget())
        )
        (super:init())
    )


    (method (dispose)
        (if (points)
            Memory(memFREE points)
        )
        = points 0
        (super:dispose())
    )


    (method (moveDone)
        (if (== GetValueAt(points value) $7777)
            (super:moveDone())
        )(else
            (self:
                setTarget()
                init()
            )
        )
    )


    (method (setTarget)
        (var pathResult, theX, theY, polyCount, temp4[30])
        (if (<> GetValueAt(points value) $7777)
            = x GetValueAt(points value)
            = y GetValueAt(points ++value)
            ++value
            (if (IsObject(gAltPolyList) and (= polyCount (send gAltPolyList:size)))
                = pathResult AvoidPath((send client:x) (send client:y) x y (send gAltPolyList:elements) polyCount 0)
                = theX GetValueAt(pathResult 2)
                = theY GetValueAt(pathResult 3)
                (if ((<> x theX) or (<> y theY))
                    = x theX
                    = y theY
                    Memory(memPOKE (+ (+ points value) 2) $7777)
                )
                Memory(memFREE pathResult)
            )
        )
    )

)
