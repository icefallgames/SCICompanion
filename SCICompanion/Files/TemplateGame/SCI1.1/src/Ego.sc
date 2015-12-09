(version 2)
(include "sci.sh")
(use "Main")
(use "PolyPath")
(use "Cycle")
(use "Actor")
(script 988)

/*
	The Ego represents the player's main character. It extends :class:`Actor` by handling the following:
	
	- Sends the player to a new room when they walk off screen.
	- Moves the player to a new destination when the user clicks the walk icon somewhere.
	- Handles getting and losing inventory items.
*/
(class Ego of Actor
    (properties
        x 0
        y 0
        z 0
        heading 0
        noun 0
        modNum -1
        nsTop 0
        nsLeft 0
        nsBottom 0
        nsRight 0
        sightAngle $6789
        actions 0
        onMeCheck omcDISABLE
        state $0000
        approachX 0
        approachY 0
        approachDist 0
        _approachVerbs 0
        yStep 2
        view -1
        loop 0
        cel 0
        priority 0
        underBits 0
        signal $2000
        lsTop 0
        lsLeft 0
        lsBottom 0
        lsRight 0
        brTop 0
        brLeft 0
        brBottom 0
        brRight 0
        scaleSignal $0000
        scaleX 128
        scaleY 128
        maxScale 128
        cycleSpeed 6
        script 0
        cycler 0
        timer 0
        detailLevel 0
        scaler 0
        illegalBits $8000
        xLast 0
        yLast 0
        xStep 3
        origStep 770
        moveSpeed 6
        blocks 0
        baseSetter 0
        mover 0
        looper 0
        viewer 0
        avoider 0
        code 0
        edgeHit 0
    )

    (method (init)
        (super:init())
        (if (not cycler)
            (self:setCycle(Walk))
        )
    )


    (method (doit)
        (super:doit())
        = edgeHit 
            (if (<= x gEdgeDistance)
                4
            )(else
                (if (>= x (- 319 gEdgeDistance))
                    2
                )(else
                    (if (>= y (- 189 (/ gEdgeDistance 2)))
                        3
                    )(else
                        (if (<= y (send gRoom:horizon))
                            1
                        )(else
                            0
                        )
                    )
                )
            )
    )

	// Handles moving the ego around.
    (method (handleEvent pEvent)
        (var temp0, temp1, temp2)
        = temp1 (send pEvent:type)
        = temp2 (send pEvent:message)
        (if (script and (send script:handleEvent(pEvent)))
            1
        )(else
            (if (& temp1 $0040)
                = temp0 temp2
                (if ((== temp0 0) and (& temp1 $0004))
                    (send pEvent:claimed)
                    return 
                )
                (if (((& temp1 $0004) and (== temp0 (send gUser:prevDir))) and IsObject(mover))
                    = temp0 0
                )
                (send gUser:prevDir(temp0))
                (self:setDirection(temp0))
                (send pEvent:claimed(TRUE))
            )(else
                (if (& temp1 evVERB)
                    (if (& temp1 evMOVE)
                        (switch (gEgoUseObstacles)
                            (case 0
                                (self:setMotion(MoveTo (send pEvent:x) (+ (send pEvent:y) z)))
                            )
                            (case 1
                                (self:setMotion(PolyPath (send pEvent:x) (+ (send pEvent:y) z)))
                            )
                            (case 2
                                (self:setMotion(PolyPath (send pEvent:x) (+ (send pEvent:y) z) 0 0))
                            )
                        )
                        (send gUser:prevDir(0))
                        (send pEvent:claimed(TRUE))
                    )(else
                        (super:handleEvent(pEvent))
                    )
                )(else
                    (super:handleEvent(pEvent))
                )
            )
        )
        (send pEvent:claimed)
    )

	// Always returns TRUE. You **are** me!
    (method (facingMe)
        return TRUE
    )

	/*
	.. function:: get(invNumber [...])
	
		Gives the ego an inventory item (or items).
		
		:param number invNumber: The numerical identifier of the inventory item.
		
		Example usage::
		
			; Give the ego the crystal and the lizard tail.
			( gEgo get: INV_CRYSTAL INV_LIZARDTAIL)
	*/
    (method (get invNumbers)
        (var temp0)
        = temp0 0
        (while (< temp0 paramTotal)
            (send ((send gInv:at(invNumbers[temp0]))):moveTo(self))
            ++temp0
        )
    )

	/*
	.. function:: put(invNumber [newOwner])
	
		Removes an inventory item from the ego, and optionally gives it to a new owner.
		
		:param number invNumber: The numerical identifier of the inventory item.
		:param number newOwner: The optional new owner (typically a room number).
		
		Example usage::
		
			; The ego used lizard tail, so remove it from the inventory:
			(gEgo put: INV_LIZARDTAIL)
	*/
    (method (put invNumber newOwner)
        (var temp0)
        (if ((self:has(invNumber)))
            = temp0 (send gInv:at(invNumber))
            (send temp0:moveTo(
                (if (== paramTotal 1)
                    -1
                )(else
                    newOwner
                )
))
            (if (gIconBar and (== (send gIconBar:curInvIcon) temp0))
                (send gIconBar:
                    curInvIcon(0)
                    disable((send ((send gIconBar:useIconItem)):
                            cursor(999)
                            yourself()
                        )
)
                )
            )
        )
    )

	/*
		Determines if the ego has an inventory item.
		
		:param number invNumber: The numerical identifier of the inventory item.
		
		Example usage::
		
			(if (gEgo has: INV_LIZARDTAIL)
				; Do something...
			)
	*/
    (method (has invNumber)
        (var temp0)
        = temp0 (send gInv:at(invNumber))
        (if (temp0)
            (send (= temp0 (send gInv:at(invNumber))):ownedBy(self))
        )
    )

)
