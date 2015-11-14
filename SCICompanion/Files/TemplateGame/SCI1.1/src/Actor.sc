/*
	The Actor script contains classes to handle views, actors, props, and any other sprite in your game.
*/
(version 2)
(include "sci.sh")
(use "Main")
(use "Print")
(use "PolyPath")
(use "Feature")
(use "Cycle")
(use "Obj")
(script 998)


/*
	The View class is an essential part of SCI games. It is the base class for :class:`Prop`, class:`Actor` and the like.
	It extends :class:`Feature` by providing the ability to be dynamically positioned at different places, and by automatically setting
	its bounds based on its view, loop and cel. It is often (though not exclusively) used to add static views to the background, via
	its addToPic method.
	
	Example definition::
	
		(instance ship of View
			(properties
				x 6
				y 92
				noun N_SHIP
				view 113
				loop 2
				cel 1
				signal ignAct	// Don't interact with Actors
			)
		)
	
	Example initialization::
	
		(ship:init())

*/
(class View of Feature
    (properties
        x 0					// x position. See posn().
        y 0					// y position. See posn().
        z 0					// z position. See posn().
        heading 0			// The angle direction the View faces.
        noun 0				// The noun for the View (for messages).
        modNum -1			// Module number (for messages)
        nsTop 0				// "Now seen" rect. The visual bounds of the View.
        nsLeft 0
        nsBottom 0
        nsRight 0
        sightAngle $6789
        actions 0
        onMeCheck omcDISABLE	// The type of onMe checks that are done.
        state $0000
        approachX 0			// The approach spot x.
        approachY 0			// The approach spot y.
        approachDist 0		// The approach distance.
        _approachVerbs 0	// Bitmask indicating which verbs cause the ego to approach.
        yStep 2				
        view -1				// The view number for View.
        loop 0				// The loop of the View.
        cel 0				// The cel of the View.
        priority 0			// The priority of the View.
        underBits 0
        signal $0101
        lsTop 0				// The "last seen" rect.
        lsLeft 0
        lsBottom 0
        lsRight 0
        brTop 0				// The "base rect".
        brLeft 0
        brBottom 0
        brRight 0
        scaleSignal $0000
        scaleX 128			// Current x scale.
        scaleY 128			// Current y scale.
        maxScale 128		// Max scale.
    )

    (method (init)
        (var temp0)
        = temp0 
            (if (& signal $0020)
                gOldATPs
            )(else
                gOldCast
            )
        = signal (& signal $7fff)
        (if (not (send temp0:contains(self)))
            = lsRight (= lsBottom (= lsLeft (= lsTop 0)))
            = signal (& signal $ff77)
        )
        BaseSetter(self)
        (send temp0:add(self))
        (if (== temp0 gOldATPs)
            (if (not & signal fixPriOn)
                = priority CoordPri(y)
            )
            SetNowSeen(self)
            (send temp0:doit())
        )
        (self:
            initialize()
            checkDetail()
        )
    )


    (method (dispose)
        (self:
            startUpd()
            hide()
        )
        = signal (| signal $8000)
    )

	// Prints a description of the View on the screen.
    (method (showSelf)
        (Print:
            addText(name)
            addIcon(view loop cel)
            init()
        )
    )


    (method (isNotHidden)
        return not & signal $0088
    )

	/*
	.. function:: onMe(x y)
	
	.. function:: onMe(obj)
	
		Determines if an object or an (x, y) coordinate lies within the View.
	
		:param heapPtr obj: An object with x and y properties.
		:returns: TRUE if it's on the View, otherwise FALSE.
	*/
    (method (onMe param1 param2)
        (var temp0, temp1)
        (if (IsObject(param1))
            = temp0 (send param1:x)
            = temp1 (send param1:y)
        )(else
            = temp0 param1
            = temp1 param2
        )
        (if (& signal $0080)
            0
        )(else
            (if (not IsObject(onMeCheck) and (& signal noCycler))
                (if (not 
                (if ((nsLeft or nsRight) or nsTop)
                )(else
                    nsBottom
                )
 or ((((<= nsLeft temp0) and (<= temp0 nsRight)) and (<= nsTop temp1)) and (<= temp1 nsBottom)))
                    not IsItSkip(view loop cel - temp1 nsTop - temp0 nsLeft)
                )
            )(else
                (super:onMe(temp0 temp1))
            )
        )
    )

	/*
	.. function:: posn(theX theY [theZ])
	
		Sets the position of the View and updates its base rectangle.
	*/
    (method (posn theX theY theZ)
        (if (>= paramTotal 1)
            = x theX
            (if (>= paramTotal 2)
                = y theY
                (if (>= paramTotal 3)
                    = z theZ
                )
            )
        )
        BaseSetter(self)
        (self:forceUpd())
    )


    (method (stopUpd)
        = signal (| signal notUpd)
        = signal (& signal $fffd)
    )


    (method (forceUpd)
        = signal (| signal $0040)
    )


    (method (startUpd)
        = signal (| signal $0002)
        = signal (& signal $fffe)
    )

	/*
		:param number thePriority: The new priority of the View. If -1 is specified, the View updates its priority based on its y coordinate.
	*/
    (method (setPri thePriority)
        (if (== paramTotal 0)
            = signal (| signal fixPriOn)
        )(else
            (if (== thePriority -1)
                = signal (& signal $ffef)
            )(else
                = priority thePriority
                = signal (| signal fixPriOn)
            )
        )
        (self:forceUpd())
    )

	// Sets the loop of the View.
    (method (setLoop theLoop)
        (if (== paramTotal 0)
            = signal (| signal noTurn)
        )(else
            (if (== theLoop -1)
                = signal (& signal $f7ff)
            )(else
                = loop theLoop
                = signal (| signal noTurn)
            )
        )
        (self:forceUpd())
    )

	// Sets the cel of the View.
    (method (setCel theCel)
        (if (== paramTotal 0)
            0
        )(else
            (if (== theCel -1)
                0
            )(else
                = cel 
                    (if (>= theCel (self:lastCel()))
                        (self:lastCel())
                    )(else
                        theCel
                    )
            )
        )
        (self:forceUpd())
    )

	/*
		Tells the View if it should or should not interact with (bump into) other objects.
		
		:param boolean shouldIgnore: If TRUE (or not specified), the View should ignore other objects.
	*/
    (method (ignoreActors shouldIgnore)
        (if ((== 0 paramTotal) or shouldIgnore)
            = signal (| signal ignAct)
        )(else
            = signal (& signal $bfff)
        )
    )

	// Hides the View.
    (method (hide)
        = signal (| signal $0008)
    )

	// Shows the View.
    (method (show)
        = signal (& signal $fff7)
    )


    (method (delete)
        (if (& signal $8000)
            = signal (& signal $7fff)
            (if ((send gOldATPs:contains(self)))
                (send gOldATPs:delete(self))
                = signal (& signal $ffdf)
            )(else
                (if (& signal $0020)
                    (send gOldCast:delete(self))
                    (send gOldATPs:add(self))
                    return 
                )(else
                    (send gOldCast:delete(self))
                )
            )
            (if (underBits)
                UnLoad(133 underBits)
                = underBits 0
            )
            (super:dispose())
            (if (IsObject(actions))
                (send actions:dispose())
            )
            = actions 0
        )
    )

	// Draws the View permanently on the background.
    (method (addToPic)
        (if ((send gOldCast:contains(self)))
            = signal (| signal $8021)
        )(else
            = signal (| signal $0020)
            (self:init())
        )
    )

	// Returns the last valid cel of the View.
    (method (lastCel)
        return - NumCels(self) 1
    )


    (method (motionCue)
    )


    (method (checkDetail)
    )

	// Sets the scale of the View.
    (method (setScale theScale)
        (var temp0, temp1, temp2, temp3[40])
        (if (not paramTotal)
            = scaleSignal (| scaleSignal $0001)
            = scaleSignal (& scaleSignal $fffd)
        )(else
            (if (not theScale)
                = scaleSignal (& scaleSignal $fffc)
            )(else
                (if (< theScale (send gRoom:vanishingY))
                    FormatPrint("<%s setScale:> y value less than vanishingY" name)
                )(else
                    = temp0 (- theScale (send gRoom:vanishingY))
                    = temp1 (- 190 theScale)
                    = temp2 (+ (/ (* temp1 100) temp0) 100)
                    = scaleSignal (| scaleSignal $0003)
                    = maxScale (/ (* temp2 128) 100)
                )
            )
        )
    )
)

/*
	Prop extends :class:`View` by providing the following additional abilities:
		
		- cycling through animation frames
		- attach Scripts
		- attach a Scaler

	Example definition::
	
		(instance monitor of Prop
			(properties
				x 13
				y 161
				noun N_MONITOR
				view 1142
				loop 1
				priority 15
				signal fixPriOn
				cycleSpeed 14
			)
		)
		
	Example initialization::
	
		(monitor:
			init()
			setCycle(Fwd)
			setScript(sPlayMC)
		)
*/
(class Prop of View
    (properties
        x 0								// x position. See posn().
        y 0								// y position. See posn().
        z 0								// z position. See posn().
        heading 0						// The angle direction the Prop faces.
        noun 0							// The noun for the Prop (for messages).
        modNum -1						// Module number (for messages)
        nsTop 0							// "Now seen" rect. The visual bounds of the Prop.
        nsLeft 0
        nsBottom 0
        nsRight 0
        sightAngle $6789
        actions 0
        onMeCheck omcDISABLE			// The type of onMe checks that are done.
        state $0000
        approachX 0						// The approach spot x.
        approachY 0						// The approach spot y.
        approachDist 0					// The approach distance.
        _approachVerbs 0				// Bitmask indicating which verbs cause the ego to approach.
        yStep 2
        view -1							// The view for Prop.
        loop 0							// Loop for the Prop.
        cel 0							// Current cel of the Prop.
        priority 0						// Priority screen value of the Prop.
        underBits 0
        signal $0000
        lsTop 0							// The "last seen" rect...
        lsLeft 0
        lsBottom 0
        lsRight 0
        brTop 0							// The "base rect", used for collison detection.
        brLeft 0
        brBottom 0
        brRight 0
        scaleSignal $0000
        scaleX 128						// Current x scale.
        scaleY 128						// Current y scale.
        maxScale 128					// Max scale.
        cycleSpeed 6					// How quickly the Prop animation cycles.
        script 0						// Arbitrary :class:`Script` object.
        cycler 0						// :class:`Cycle` attached to the Prop.
        timer 0
        detailLevel 0
        scaler 0						// :class:`Scaler` object attached to the Prop.
    )

    (method (doit)
        (var temp0)
        (if (& signal $8000)
            return 
        )
        (if (script)
            (send script:doit())
        )
        (if ((& signal $0004) and not (& signal $0002))
            return 
        )
        (if (cycler)
            (send cycler:doit())
        )
        (if (scaler)
            (send scaler:doit())
        )
    )

	// Lets the Prop's script have a chance at handling the event.
    (method (handleEvent pEvent)
        (if (script)
            (send script:handleEvent(pEvent))
        )
        (super:handleEvent(pEvent))
    )


    (method (delete)
        (if (& signal $8000)
            (self:
                setScript(0)
                setCycle(0)
            )
            (if (timer)
                (send timer:dispose())
            )
            (if (IsObject(scaler))
                (send scaler:dispose())
                = scaler 0
            )
            (super:delete())
        )
    )

    (method (motionCue)
        (if (cycler and (send cycler:completed))
            (send cycler:motionCue())
        )
    )


    (method (checkDetail param1)
        (if (not detailLevel)
        )(else
            (if (< 
            (if (paramTotal)
                param1
            )(else
                (send gGame:detailLevel())
            )
 detailLevel)
                (self:stopUpd())
            )(else
                (if (cycler)
                    (self:startUpd())
                )
            )
        )
    )

	/*
	.. function:: setScale(class params)
	
	.. function:: setScale(obj params)
	
	.. function:: setScale(-1 otherObj)
	
	.. function:: setScale(scale)
	
		Provides various ways to control the scaling of a Prop. See :class:`Scaler` and :class:`ScaleTo`.
	
		Example usage for attaching a dynamic scaler::
		
			(send gEgo:setScale(Scaler frontSize backSize frontY backY))

		Example usage for setting an explicit scale::
		
			(send gEgo:setScale(50)) // 50 percent size.
	
		:param class class: A scaler class, such as Scaler.
		:param heapPtr obj: An instance of a scaler class.
		:param params: Initialization parameters for the scaler.
		:param heapPtr otherObj: Another object from which to copy scaling information.
		:param number scale: A percentage scale.
	*/
    (method (setScale param1 param2 param3)
        (if (scaler)
            (send scaler:dispose())
            = scaler 0
        )
        (if (not paramTotal)
            (super:setScale())
        )(else
            (if (IsObject(param1))
                = scaleSignal (| scaleSignal $0001)
                = scaleSignal (& scaleSignal $fffd)
                = scaler 
                    (if (& (send param1:{-info-}) $8000)
                        (send param1:new())
                    )(else
                        param1
                    )
                (send scaler:init(self param2 rest param3))
            )(else
                (if (== param1 -1)
                    (if ((send param2:scaleSignal))
                        = scaleSignal (send param2:scaleSignal)
                        = maxScale (send param2:maxScale)
                        (if (IsObject((send param2:scaler)))
                            (send (= scaler (send ((send param2:scaler)):new())):client(self))
                        )
                    )
                )(else
                    (super:setScale(param1))
                )
            )
        )
    )

	/*
		Sets a cycler object on the Prop, optionally supplying initialization parameters for the cycler.
		
		Example usage for telling a Prop to cycle forward::
		
			(theFrog:setCycle(Fwd))
			
		Example usage for telling a Prop to cycle to the end, then cue its caller::
		
			(theFrog:setCycle(End self))
		
		:param class theCycler: A class derived from :class:`Cycle`, or NULL to remove the current cycler.
		
	*/
    (method (setCycle theCycler sendParams)
        (if (cycler)
            (send cycler:dispose())
        )
        (if (theCycler)
            (self:startUpd())
            = cycler 
                (if (& (send theCycler:{-info-}) $8000)
                    (send theCycler:new())
                )(else
                    theCycler
                )
            (send cycler:init(self rest sendParams))
        )(else
            = cycler 0
        )
    )

	/*
		Attaches an arbitrary script to the Prop, optionally providing initialization parameters.
		
		Example usage::

			(self:setScript(theRoomScript))

	*/
    (method (setScript theScript sendParams)
        (if (IsObject(script))
            (send script:dispose())
        )
        (if (theScript)
            (send theScript:init(self rest sendParams))
        )
    )


    (method (cue)
        (if (script)
            (send script:cue())
        )
    )

)

/*
	Actor is the base class for moving objects in your game. It extends :class:`Prop` by providing the following additional capabilities:
	
		- A mover property that is responsible for controlling how the Actor moves. This is assigned with setMotion().
		- An optional Avoider that makes the Actor avoid objects.
		- Optional "blocks" that indicate areas the Actor can or can't be.
		
	Example definition::
	
		(instance wd40 of Actor
			(properties
				x 20
				y 20
				noun N_ROBOT
				view 400
				loop 8
				signal ignAct
			)
		)

	Example initialization::
	
		(wd40:
			init()
			setMotion(PolyPath 127 128)
		)

*/
(class Actor of Prop
    (properties
        x 0						// x position. See posn().
        y 0						// y position. See posn().
        z 0						// z position. See posn().
        heading 0				// The angle direction the Actor faces.
        noun 0					// The noun for the Actor (for messages).
        modNum -1				// Module number (for messages)
        nsTop 0					// "Now seen" rect. The visual bounds of the Actor.
        nsLeft 0
        nsBottom 0
        nsRight 0
        sightAngle $6789
        actions 0
        onMeCheck omcDISABLE	// The type of onMe checks that are done.
        state $0000
        approachX 0				// The approach spot x.
        approachY 0				// The approach spot y.
        approachDist 0			// The approach distance.
        _approachVerbs 0		// Bitmask indicating which verbs cause the ego to approach.
        yStep 2					// The number of pixels moved in the y direction each cycle.
        view -1					// The view for Actor.
        loop 0
        cel 0
        priority 0
        underBits 0
        signal $0000
        lsTop 0					// The "last seen" rect...
        lsLeft 0
        lsBottom 0
        lsRight 0
        brTop 0					// The "bounds rect" (near the feet of the Actor).
        brLeft 0
        brBottom 0
        brRight 0
        scaleSignal $0000
        scaleX 128				// Current x scale.
        scaleY 128				// Current y scale.
        maxScale 128			// Max scale.
        cycleSpeed 6			// How quickly the Actor animation cycles.
        script 0				// Arbitrary :class:`Script` object.
        cycler 0				// :class:`Cycle` attached to the Actor.
        timer 0
        detailLevel 0
        scaler 0				// :class:`Scaler` object attached to the Actor.
        illegalBits $8000
        xLast 0
        yLast 0
        xStep 3
        origStep 770
        moveSpeed 6				// How quickly the Actor moves.
        blocks 0
        baseSetter 0
        mover 0					// The :class:`Motion` object attached to the Actor.
        looper 0				// Optional looper code.
        viewer 0
        avoider 0
        code 0
    )

    (method (init params)
        (super:init(rest params))
        = xLast x
        = yLast y
    )


    (method (doit)
        (var temp0, theBrLeft, theBrRight, temp3, temp4, temp5, temp6, temp7)
        (if (& signal $8000)
            return 
        )
        (if (script)
            (send script:doit())
        )
        (if (code)
            (send code:doit(self))
        )
        (if ((& signal $0004) and not (& signal $0002))
            return 
        )
        (if (viewer)
            (send viewer:doit(self))
        )
        (if (avoider)
            (send avoider:doit())
        )
        (if (mover)
            (if ((& scaleSignal $0001) and not (& scaleSignal $0004))
                = temp5 (>> origStep $0008)
                = temp6 (& origStep $00ff)
                = temp7 (/ (* temp5 scaleX) 128)
                = temp3 
                    (if (temp7)
                    )(else
                        1
                    )
                = temp7 (/ (* temp6 scaleY) 128)
                = temp4 
                    (if (temp7)
                    )(else
                        1
                    )
                (if ((<> temp3 xStep) or (<> temp4 yStep))
                    (self:setStep(temp3 temp4 1))
                )
            )
            (if (mover)
                (send mover:doit())
            )
        )
        (if (scaler)
            (send scaler:doit())
        )
        (if (cycler)
            = theBrLeft brLeft
            = theBrRight brRight
            (send cycler:doit())
            (if (baseSetter)
                (send baseSetter:doit(self))
            )(else
                BaseSetter(self)
            )
        )
        = xLast x
        = yLast y
    )

	/*
	.. function:: posn(theX theY [theZ])
		
		Sets the position of the Actor.
	*/
    (method (posn theX theY theZ)
        (super:posn(theX theY rest theZ))
        = xLast theX
        = yLast theY
    )

	/*
	.. function:: setLoop(loop)
	
	.. function:: setLoop(loopClass [params ...])
	
	.. function:: setLoop(looper [params ...])
	
		Sets a loop on the Actor, or sets an object that controls which loop is used.
		
		:param number loop: A loop number.
		:param class loopClass: A class that has a doit method that controls its client loop (e.g. see :class:`Grooper`).
		:param heapPtr looper: An instance of a looper class.
	*/
    (method (setLoop param1 param2)
        (var theLooper)
        = theLooper 
            (if (== paramTotal 0)
                (super:setLoop())
                0
            )(else
                (if (not IsObject(param1))
                    (super:setLoop(param1 rest param2))
                    0
                )(else
                    (if (& (send param1:{-info-}) $8000)
                        (send param1:new())
                    )(else
                        param1
                    )
                )
            )
        (if (theLooper)
            (if (looper)
                (send looper:dispose())
            )
            (send (= looper theLooper):init(self rest param2))
        )
    )


    (method (delete)
        (if (& signal $8000)
            (if (<> mover -1)
                (self:setMotion(0))
            )
            (self:setAvoider(0))
            (if (baseSetter)
                (send baseSetter:dispose())
                = baseSetter 0
            )
            (if (looper)
                (send looper:dispose())
                = looper 0
            )
            (if (viewer)
                (send viewer:dispose())
                = viewer 0
            )
            (if (blocks)
                (send blocks:dispose())
                = blocks 0
            )
            (if (code)
                (send code:dispose())
                = code 0
            )
            (if (IsObject(actions))
                (send actions:dispose())
                = actions 0
            )
            (super:delete())
        )
    )


    (method (motionCue)
        (if (mover and (send mover:completed))
            (send mover:motionCue())
        )
        (super:motionCue())
    )


    (method (checkDetail param1)
        (if (not detailLevel)
        )(else
            (if (< 
            (if (paramTotal)
                param1
            )(else
                (send gGame:detailLevel())
            )
 detailLevel)
                (self:stopUpd())
            )(else
                (if (cycler or mover)
                    (self:startUpd())
                )
            )
        )
    )

	/*
		Assigns a mover object to the Actor. The mover is initialized with the Actor and any
		sendParams that have been provided.
		
		:param theMover: A class name, or an instance that inherits from :class:`Motion`.
		:param sendParams: Any params that should be forwarded to the mover's init() method.

		Movers control the Actor's motion, whether it be via
		mouse or keyboard input, or some in-game logic.
		
		Example usage for moving a ball to a particular position, and cueing the caller when it's done::
		
			(myBall:setMotion(MoveTo 123 100 self))
	*/
    (method (setMotion theMover sendParams)
        (if (mover and (<> mover -1))
            (send mover:dispose())
        )
        (if (theMover)
            (self:startUpd())
            = mover 
                (if (& (send theMover:{-info-}) $8000)
                    (send theMover:new())
                )(else
                    theMover
                )
            (send mover:init(self rest sendParams))
        )(else
            = mover 0
        )
    )

    (method (setAvoider theAvoider sendParams)
        (if (avoider)
            (send avoider:dispose())
        )
        = avoider 
            (if (IsObject(theAvoider) and (& (send theAvoider:{-info-}) $8000))
                (send theAvoider:new())
            )(else
                theAvoider
            )
        (if (avoider)
            (send avoider:init(self rest sendParams))
        )
    )


    (method (ignoreHorizon param1)
        (if (not paramTotal or param1)
            = signal (| signal ignoreHorizon)
        )(else
            = signal (& signal $dfff)
        )
    )

	/*
		Specifies the control colors which the Actor's movement. This is not used commonly in SCI1.1. Constraining
		an Actor's motion is generally done with Polygons instead.
	*/
    (method (observeControl bits)
        (var temp0)
        = temp0 0
        (while (< temp0 paramTotal)
            = illegalBits (| illegalBits bits[temp0])
            ++temp0
        )
    )

	/*
		Specifies which control colors should no longer block the Actor's movement.
	*/
    (method (ignoreControl bits)
        (var temp0)
        = temp0 0
        (while (< temp0 paramTotal)
            = illegalBits (& illegalBits bnot bits[temp0])
            ++temp0
        )
    )

	/*
		Adds a block (an instance which inherits from :class:`Blk`) to the Actor's list of blocks.
		These control where an Actor is allowed to go. In SCI1.1, these have generally been replaced by Polygons.
	*/
    (method (observeBlocks block)
        (if (not blocks)
            = blocks (Set:new())
        )
        (send blocks:add(rest block))
    )

	// Removes a block from the Actor's list of blocks.
    (method (ignoreBlocks block)
        (if (blocks)
            (send blocks:delete(rest block))
            (if ((send blocks:isEmpty()))
                (send blocks:dispose())
                = blocks 0
            )
        )
    )
    
    // Returns TRUE if the Actor is not moving, FALSE otherwise.
    (method (isStopped)
    	(if (IsObject(mover))
    		(if ((== x (send mover:xLast)) and (== y send mover:yLast))
    			return TRUE
			)
			return FALSE
		)
		return TRUE
    )

    (method (isBlocked)
        return & signal $0400
    )

	// Returns TRUE if the Actor is inside the specified rectangle, FALSE otherwise.
    (method (inRect left top right bottom)
        return 
            (if (((<= left x) and (<= x right)) and (<= top y))
                <= y bottom
            )(else
                0
            )
    )

	/*
	.. function:: onControl([fUsePoint])
	
		Provides a bitmask of the control colors on which an Actor is located.
		
		:param boolean fUsePoint: If TRUE, the Actor's location is used. If FALSE (or not specified), the Actor's base rectangle (near its feet) is used.
		:returns: A bitmask of ctl flags. These should usually be tested with the & operator.
		
		Example usage::
		
			(if (& ctlGREEN (send gEgo:onControl()))
				Print("The ego is on Green")
			)
	*/
    (method (onControl fUsePoint)
        (if (paramTotal and fUsePoint)
            OnControl(4 x y)
        )(else
            OnControl(4 brLeft brTop brRight brBottom)
        )
    )

	/*
		:param heapPtr obj: An object with x and y properties.
		:returns: the distance between this Actor and the object.
	*/
    (method (distanceTo obj)
        GetDistance(x y (send obj:x) (send obj:y) gPicAngle)
    )

	// Returns TRUE if the Actor can't be in its current location.
    (method (cantBeHere)
        (var temp0)
        (if (baseSetter)
            (send baseSetter:doit(self))
        )(else
            BaseSetter(self)
        )
        = temp0 
            (if (CantBeHere(self (send gOldCast:elements)))
            )(else
                (if ((not (& signal ignoreHorizon) and IsObject(gRoom)) and (< y (send gRoom:horizon)))
                    -1
                )(else
                    (if (blocks and not (send blocks:allTrue(#doit self)))
                        -2
                    )
                )
            )
    )

	/*
	.. function:: setStep(newX newY [fDontSetOrigStep])
	
		Sets the pixel increments in which the Actor moves. Bigger increments means the Actor will cover
		larger distances in each frame.
		
		:param number newX: The xStep, or -1 if not provided.
		:param number newY: The yStep, or -1 if not provided.
		:param boolean fDontSetOrigStep: Optional flag telling us not to set origStep.
	*/
    (method (setStep newX newY fDontSetOrigStep)
        (var theXStep, theYStep)
        = theXStep (>> origStep $0008)
        = theYStep (& origStep $00ff)
        (if ((>= paramTotal 1) and (<> newX -1))
            = theXStep newX
        )
        (if ((>= paramTotal 2) and (<> newY -1))
            = theYStep newY
        )
        (if ((< paramTotal 3) or not fDontSetOrigStep)
            = origStep (+ (<< theXStep $0008) theYStep)
        )
        = xStep theXStep
        = yStep theYStep
        (if (IsObject(mover) and ((send mover:isMemberOf(MoveTo)) or (send mover:isMemberOf(PolyPath))))
            (send mover:init())
        )
    )

	/*
		Sets the direction that the Actor faces.
		
		:param number newDirection: One of CENTER, UP, UPRIGHT, RIGHT, DOWNRIGHT, DOWN, DOWNLEFT, LEFT or UPLEFT.
	*/
    (method (setDirection newDirection)
        (var temp0, temp1, temp2, temp3, temp4, temp5, temp6, temp7)
        = temp1 (send gRoom:vanishingY)
        = temp0 
            (if (== temp1 -30000)
                x
            )(else
                (send gRoom:vanishingX)
            )
        (if ((== xStep 0) and (== yStep 0))
            return 
        )
        = temp5 (/ 32000 Max(xStep yStep))
        (switch (newDirection)
            (case 0
                (self:setMotion(0))
                return 
            )
            (case 1
                = temp2 (- temp0 x)
                = temp3 (- temp1 y)
            )
            (case 5
                = temp2 (- x temp0)
                = temp3 (- y temp1)
            )
            (case 3
                = temp2 temp5
                = temp3 0
            )
            (case 7
                = temp2 neg temp5
                = temp3 0
            )
            (default 
                = temp4 GetAngle(x y temp0 temp1)
                (if (< 180 temp4)
                    = temp4 (- temp4 360)
                )
                = temp4 (+ (/ (+ temp4 90) 2) (* 45 (- newDirection 2)))
                = temp2 SinMult(temp4 100)
                = temp3 neg CosMult(temp4 100)
            )
        )
        = temp5 (/ temp5 5)
        (while ((< Abs(temp3) temp5) and (< Abs(temp2) temp5))
            = temp2 (* temp2 5)
            = temp3 (* temp3 5)
        )
        = temp7 (send gRoom:obstacles)
        (if (temp7 and global67)
            = temp6 AvoidPath(x y (+ x temp2) (+ y temp3) (send temp7:elements) (send temp7:size) 0)
            = temp2 (- GetValueAt(temp6 2) x)
            = temp3 (- GetValueAt(temp6 3) y)
            Memory(memFREE temp6)
        )
        (if (temp2 or temp3)
            (self:setMotion(MoveTo (+ x temp2) (+ y temp3)))
        )(else
            (if (newDirection)
                (self:
                    setMotion(0)
                    setHeading(* (- newDirection 1) 45)
                )
            )(else
                (self:setMotion(0))
            )
        )
    )

	// Sets the angle heading of the Actor.
    (method (setHeading theHeading cueObj cueValues)
        (if (paramTotal)
            = heading theHeading
        )
        (if (looper)
            (send looper:doit(self heading 
                (if (>= paramTotal 2)
                    cueObj
                )(else
                    0
                )
							 )
			)
        )(else
            DirLoop(self heading)
            (if ((>= paramTotal 2) and IsObject(cueObj))
                (send cueObj:cue(rest cueValues))
            )
        )
        return heading
    )

	// Sets the speed of the Actor. This controls both the move and cycle speed.
    (method (setSpeed newSpeed)
        (if (paramTotal)
            = moveSpeed (= cycleSpeed newSpeed)
        )
        return moveSpeed
    )

)
