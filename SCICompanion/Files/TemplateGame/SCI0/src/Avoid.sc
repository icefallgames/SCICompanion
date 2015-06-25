/******************************************************************************
 decompiled from SQ3 by Phil Fortier
 ******************************************************************************
 avoid.sc
 Contains an avoider class to allow actors to do simple pathfinding.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script AVOID_SCRIPT)
/******************************************************************************/
(use "cycle")
(use "obj")
(use "controls")

/******************************************************************************/


(class Avoid of Obj
    (properties
        client 0
        heading 64536
        bumpTurn 0
        lastBumped 0
        thisTurn 1
        escaping 0
        escapes 0
        escapeTurn 1
        nearestDist 32000
        counter 0
        nonBumps 10
        targetX 0
        targetY 0
        motionInited 0
        outOfTouch 0
        offScreenOK 0
    )

    (method (init theClient theOffScreenOk)
        (if (>= paramTotal 1)
            (= client theClient)
            (if (>= paramTotal 2)
                (= offScreenOK theOffScreenOk)
            )
            (= heading (send client:heading))
        )
        (= counter 0)
        (= nonBumps 10)
        (= escaping 0)
    )

    (method (doit)
        (var newHeading, distanceToTarget, clientX, clientY, moverX, moverY, theMover, angle,
             temp8, newMoveCount, temp10, temp11, temp12, dummyVar, degreeQuantum, numDivisions)
         //   proc999_3 is a Max function
        (= numDivisions proc999_3(4 (* (/ NumLoops(client) 4) 4 )))

        (= theMover (send client:mover))

        (if (theMover)
            // This tells the mover to update its target, if appropriate
            // (i.e. MoveTo doesn't do anything, but Follow does).
            (send theMover:setTarget())
        )(else
            // There's no mover, so nothing to do.
            (= heading 64536)
            return
        )

        (= moverX (send theMover:x))
        (= moverY (send theMover:y))
        // Can we be where the mover wants us to be, or at least within its "distance"?
        (if ((not (self:canBeHere(moverX moverY)) )   and (not (send theMover:respondsTo(#distance)) ))
            // If we can't, then just let the mover do its thing and return.
            // If we can't be at our final destirnation, I guess there isn't really
            // any point in avoiding things. This means that if you want something to
            // move to the ego's position, you need to use a mover that supports #distance, like
            // Follow.
            (send theMover:doit())
            return
        )

        (if (send theMover:onTarget())
            // If the mover is on target, there is nothing for us to do.
            (if (not motionInited)
                InitBresen(theMover)
            )
            (send theMover:doit())
            return
        )
        (if (== heading 64536)
            // Initialize ourselves if we aren't already
            (self:init())
        )

        (= dummyVar 0)
        (= clientX (send client:x))
        (= clientY (send client:y))

        // Calculate the angle between our client and our destination.
        (= angle (ClampAngle(GetAngle(clientX clientY moverX moverY) 360)))

        (= distanceToTarget GetDistance(clientX clientY moverX moverY))

        // How many degress do we turn when we turn?
        (= degreeQuantum (/ 180 numDivisions))

        // Set our heading to our old heading, but quantized to degreeQuantum
        // This doesn't seem to accomplish much in terms of getting us to where we want to go!
        (= heading (ClampAngle( (* degreeQuantum (/ heading degreeQuantum))  360)))
        (= newHeading heading)

        (if ((not escaping) and (not (= temp12 (<= nonBumps 2))))
            // If we aren't , and as long as nonBumps is greater than 2,
            // then do this:

            (if (not motionInited)
                (= motionInited TRUE)
                InitBresen(theMover)
            )

            (send theMover:doit())

            (if (== (send client:x) clientX)
                (if (== (send client:y) clientY)
                    // goto code_0254
                )(else
                    (self:pickLoop(angle))
                    return (++nonBumps)
                )
            )(else
                (self:pickLoop(angle))
                return (++nonBumps)
            )

            (= nonBumps 0)
            (= dummyVar 1)
            (= motionInited 0)
        )(else
            (if (< (= newMoveCount (send theMover:{b-moveCnt})) (send client:moveSpeed))
                (send theMover:{b-moveCnt}(++newMoveCount))
            )(else
                // Reset moveCnt to 0
                (send theMover:{b-moveCnt}(0))
            )
        )


        // Note: the byte code generated looks in correct here, but the avoider behaviour
        // is better as a result?
        (if  (  ((== targetX (= targetX moverX)) and (== targetY (= targetY moverY))) and
                (  (not escaping) or
                   (  (not (<= (--counter) 0)) and
                      ( (>= distanceToTarget nearestDist) or (>= counter 40))
                   )
                )
             )

            // The mover's target did not change (and our counter didn't reach 0)
            (if (not escaping)
                (if (< distanceToTarget nearestDist)
                    // This is our closest distance so far.
                    (= nearestDist distanceToTarget)
                    (= counter 0)
                )(else
                    // We're not getting any closer.  Time to escape
                    // if the counter hit 50.
                    (= escaping (>= (++counter) 50))
                    (if (escaping)
                        (= counter Random(40 80))
                        // Negate the escape turn?
                        (= escapeTurn (- 0 escapeTurn))
                    )
                )
            )

        )(else
             // We are resetting ourselves
             // The mover's target changed, or our counter reset to 0
            (= escaping 0)
            (= motionInited 0)
            (= nearestDist 32000)
            (= counter 0)
        )


        // Calculate the angle between our ideal angle and our new heading.
        // i.e. if we want to go 270 degrees, but our new heading is 180, then
        // the angle is 90
        (= angle (CalcAngleDiff(angle newHeading)))
        (= temp8 Abs(angle))
        // This is useless because we never check temp11 again:
        (= temp11 (<= temp8 90))

        // thisTurn is set to -1 or +1 depending on the positiveness of the angle.
        // If we are heading right on (angle 0) then it is set to escapeTurn.
        // In other words, this seems to be the direction we need to turn
        // (clockwise or counterclockwise)
        (if (== 0 (= thisTurn IsPosOrNeg(angle)))
            (= thisTurn escapeTurn)
        )

        // If the amount that we need to turn is less than half a quantum,
        // then if we're not escaping, we'll increment our heading by a quantum.
        // just so we end up doing something I guess.
        (if (> temp8 (/ degreeQuantum 2))
            (if (not escaping)
                (= heading (+ heading (* thisTurn degreeQuantum)))
            )
        )

        (if (escaping)
            (= thisTurn escapeTurn)
        )

        // Increment our position! Finally!
        (self:incClientPos())

        (if (self:canBeHere)
            // If I can be here, then increment the number of nonbumps.
            // and we're done (unless we are escaping)
            (++nonBumps)
            (= bumpTurn 0)
            (if (escaping)
                // goto code_037F;
            )(else
                return
            )
        )(else
            // Uhoh... we can't be here.
            (= lastBumped bumpTurn)
            (= bumpTurn thisTurn)
            (= nonBumps 0)
        )

        // numDivisions (tends to be 4 * 2 = 8)
        (= numDivisions (* numDivisions 2))

        // If we are escaping...
        (if (escaping)
            // Quantize the "newHeading".  This is our cache of the "current heading"
            (= newHeading (* degreeQuantum (/ heading degreeQuantum)))
            (= temp10 0)

            // For each degree quantum...
            (while (< temp10 numDivisions)

                (if (not (self:canBeHere))
                    // We can't be here... get out of the loop
                    break
                )

                // Try a new offset from the current heading (do a loop around through all degrees)
                (= heading (ClampAngle( (+ newHeading (* (* degreeQuantum temp10) escapeTurn) )  360)))
                (send client:
                    x(clientX)
                    y(clientY)
                )
                // Try to inc our position.
                (self:incClientPos())

                (++temp10)

            )

            (if (== temp10 numDivisions)
                // Strangely, this case is only hit if we executed the loop above
                // almost all the way to the end, but not quite.
                // Here, we are adjust our heading by one quantum.
                (= heading (+ heading (* escapeTurn degreeQuantum)))
            )

            (= newHeading heading)

        )

        (= newMoveCount 1)
        (= temp10 1)

        (while (not (self:canBeHere))
            // While we can't be here...

            (if (>= newMoveCount numDivisions)
                break
            )

            (send client:
                x(clientX)
                y(clientY)
            )

            // Adjust our heading by one quantum in a direction opposite to thisTurn
            (= heading (- newHeading (* (* degreeQuantum temp10) thisTurn)))

            (self:incClientPos)

            (if (escaping)
                (= temp10 (++ newMoveCount))
            )(else
                // If we're not escaping
                (if (> temp10 0)
                    (= temp10 (- 0 temp10)) // I think
                )(else
                    (= temp10 (++newMoveCount))
                )
            )
        )

        (self:pickLoop(heading))
    )


    (method (incClientPos)
        (send client:
            x(+ (send client:x)  ( * (send client:xStep)  (IsPosOrNeg(SinMult(heading 100)))  )  )
            y(- (send client:y)  ( * (send client:yStep)  (IsPosOrNeg(CosMult(heading 100)))  )  )
            heading(heading)
        )
    )

    (method (pickLoop theHeading)
        (var theLooper)
        (send client:heading(theHeading))
        (if (send client:looper)
            (= theLooper (send client:looper))
            (send theLooper:
                doit(client theHeading 0 1)
            )
        )(else
            DirLoop(client theHeading)
        )
    )

    (method (canBeHere newX newY)
        (var x, y, tmpCanBeHere)
        (= x (send client:x))
        (= y (send client:y))
        (if (paramTotal)
            (send client:
                x(newX)
                y(newY))
        )


        (if ( (= tmpCanBeHere (send client:canBeHere()))  and (not offScreenOK))
            (= tmpCanBeHere (not (OffScreen(client))))
        )


        // Restore the client's position.
        (send client:
            x(x)
            y(y)
        )

        return (tmpCanBeHere)
    )

)




// Calculates the difference between two angles, and returns a number between
// -180 and 180.
(procedure (CalcAngleDiff param1 param2)
    (if (>= paramTotal 2)
        // Calc the difference between the two
        (= param1 (- param1 param2))
    )

    // If it's less than (65536-180), then return param1+360
    (if (<= param1 (-180))
        return (+ param1 360)
    )

    (if (> param1 180)
        return (- param1 360)
    )
    return (param1)
)


// Too lazy to decompile this (unsure about pprev instruction),
// but it appears to calculate if something is offscreen.
(procedure (OffScreen param1)
	(var x, y)
	(= x (send param1:x))
	(if (< 0 x)
		(if (< x 320)
			(= y (- (send param1:y) (send param1:z)))
			(if (< 0 y)
				(if (< y 200)
					return TRUE
				)
			)
		)
	)
	return FALSE
	/*
    (asm
            push0
            pushi 4 // $4 x
            push0
            lap param1
            send 4

            lt?
            bnt code_003E
            pprev
            ldi 320 // $140 vWalking
            lt?
            bnt code_003E
            push0
            pushi 3 // $3 y
            push0
            lap param1
            send 4

            push
            pushi 85 // $55 z
            push0
            lap param1
            send 4

            sub
            lt?
            bnt code_003E
            pprev
            ldi 200 // $C8 stopUpd
            lt?
  code_003E:
            not
            ret
    )*/
    return FALSE
)


// This clamps an angle to between 0 and param2
// Examples:
//  (135 360)  ->  135
//  (435 360)  ->  75
//  (-50 360)  ->  310
(procedure (ClampAngle param1 param2)
    (= param1 (- param1 (* param2 (/ param1 param2))))
    (if (< param1 0)
        return (+ param1 param2)
    )
    return (param1)
)

