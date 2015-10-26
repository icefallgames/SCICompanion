/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 feature.sc
 Contains the base class, Feature, as well as it's descendants, View, Prop, 
 Act and more. This is the main script for sprites (VIEWs).
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script FEATURE_SCRIPT)
/******************************************************************************/
(use "main")
(use "controls")
(use "cycle")
(use "obj")
/******************************************************************************/
(class Feature of Obj
	(properties
		y 0
		x 0
		z 0
		heading 0
	)
	(method (handleEvent pEvent)
		return( (send pEvent:claimed) )
	)
)
/******************************************************************************/
(class PV of Feature
	(properties
		y 0
		x 0
		z 0
		heading 0
		view 0
		loop 0
		cel 0
		priority -1
		signal 0
	)
	(method (init)
		(send gAddToPics:add(self))
	)
	(method (showSelf)
		Print(objectName #icon view loop cel)
	)
)
/******************************************************************************/
(class View of Feature
	(properties
		y 0
		x 0
		z 0
		heading 0
		yStep 2
		view 0
		loop 0
		cel 0
		priority 0
		underBits 0
		signal $101
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		lsTop 0
		lsLeft 0
		lsBottom 0
		lsRight 0
		brTop 0
		brLeft 0
		brBottom 0
		brRight 0
	)
	(method (init)
		= signal (& signal $7FFF)
		(if (not (send gCast:contains(self)) )
			= lsTop    0
			= lsLeft   0
			= lsBottom 0
			= lsRight  0
			= signal (& signal $FF77)
		)
		BaseSetter(self)
		(send gCast:add(self))
	)
	(method (dispose)
		(self:
			startUpd()
			hide()
		)
		= signal (| signal $8000)
	)
	(method (showSelf)
		Print(objectName #icon view loop cel)
	)
	(method (posn newX newY newZ) 
		(if(>= paramTotal 1)
			= x newX
			(if(>= paramTotal 2)
				= y newY
				(if(>= paramTotal 3)
					= z newZ
				)
			)
		)
		BaseSetter(self)
		(self:forceUpd())
	)
	(method (stopUpd)
		= signal (| signal 1)
		= signal (& signal $FFFD)
	)
	(method (forceUpd)
		= signal (| signal $40)
	)
	(method (startUpd)
		= signal (| signal 2)
		= signal (& signal $FFFE)
	)
	(method (setPri newPriority)
		(if(== paramTotal 0)
			= signal (| signal $10)
		)(else
			(if(== newPriority -1)
		    	= signal (& signal $FFEF)
			)(else
				= priority newPriority
				= signal (| signal $10)
		    )
		)
		(self:forceUpd())
	)
	(method (setLoop newLoop)
		(if(== paramTotal 0)
			= signal (| signal $800)
		)(else
			(if(== newLoop -1)
				= signal (& signal $F7FF)
			)(else
				= loop newLoop
				= signal (| signal $800)
			)
  		)
  		(self:forceUpd())
	)
	(method (setCel newCel)
		(if(== paramTotal 0)
			= signal (| signal $1000)
		)(else
			(if(== newCel -1)
				= signal (& signal $EFFF)
			)(else
				= signal (| signal $1000)
		    	(if(>= newCel (self:lastCel))
		    		= cel (self:lastCel)
				)(else
		    		= cel newCel 
				)
		    )
		)
		(self:forceUpd())
	)
	(method (ignoreActors fIGNORE)
		(if( (== 0 paramTotal) or fIGNORE)
			= signal (| signal $4000)
		)(else
			= signal (& signal $BFFF)
		)
	)
	(method (hide)
		= signal (| signal 8)
	)
	(method (show)
		= signal (& signal $FFF7)
	)
	(method (delete) 
		(if(& signal $8000)
			(if(& signal $20)
				(send gAddToPics:
					add(	
						(send (PV:new()):
							view(view)
							loop(loop)
							cel(cel)
							x(x)
							y(y)
							z(z)
							priority(priority)
							signal(signal)
							yourself()
						)
					)
				)
			)
  			= signal (& signal $7FFF)
  			(send gCast:delete(self))
			(if(underBits)
				UnLoad(rsMEMORY underBits)
				= underBits NULL
			)
  			(super:dispose())
  		)
	)
	(method (addToPic)
		(if( not((send gCast:contains(self))) )
			(self:init())
		)
		(self:signal(| signal $8021))
	)
	(method (lastCel)
		return(- NumCels(self) 1)
	)
	(method (isExtra fMAKE_EXTRA)
		(var fIS_EXTRA)
		= fIS_EXTRA (& signal $200)
		(if(paramTotal)
			(if(fMAKE_EXTRA)
				= signal (| signal $200)
			)(else
				= signal (& signal $FDFF)
		    )
		)
		return(fIS_EXTRA)
	)
	(method (motionCue))
)
/******************************************************************************/
(class Prop of View
	(properties
		y 0
		x 0
		z 0
		heading 0
		yStep 2
		view 0
		loop 0
		cel 0
		priority 0
		underBits 0
		signal 0
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		lsTop 0
		lsLeft 0
		lsBottom 0
		lsRight 0
		brTop 0
		brLeft 0
		brBottom 0
		brRight 0
		cycleSpeed 0
		script 0
		cycler 0
		timer 0
	)	
	(method (doit)		
		(if( & signal $8000 )
			return
		)
		(if(script)
			(send script:doit())
		)
		(if((& signal 4) and (not(& signal 2)))
			return
		)
		(if(cycler)
			(send cycler:doit())
		)
	)
	(method (handleEvent pEvent)
		(if(script)
			(send script:handleEvent(pEvent))
		)
		return((send pEvent:claimed))
	)
	(method (delete) 
		(if( (& signal $8000) )
			(self:
				setScript(0)
				setCycle(0)
			)
			(if(timer)
				(send timer:dispose())
			)
			(super:delete())
		)
	)
	(method (motionCue) 
		(if( cycler and (send cycler:completed) )
			(send cycler:motionCue())
		)
	)
	(method (setCycle theCycler sendParams)
		(if(cycler)
			(send cycler:dispose())
		)
		(if(theCycler)
			(self:setCel(-1))
			(self:startUpd())
			(if(& (send theCycler:{-info-}) $8000)
				= cycler (send theCycler:new())
			)(else
		    	= cycler theCycler 
			)
			(send cycler:init(self rest sendParams))
		)(else
			= cycler NULL
		)
	)
	(method (setScript theScript sendParams)
		(if(IsObject(script))
		    (send script:dispose())
		)
		(if(theScript)
			(send theScript:init(self rest sendParams))
		)
	)
	(method (cue)
		(if(script)
			(send script:cue())
		)
	)
)

/**
    The Act class is for actors in the game. These are the characters which bring your game to life.
 */
(class Act of Prop
	(properties
		y 0
		x 0
		z 0
		heading 0
		yStep 2
		view 0
		loop 0
		cel 0
		priority 0
		underBits 0
		signal 0
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		lsTop 0
		lsLeft 0
		lsBottom 0
		lsRight 0
		brTop 0
		brLeft 0
		brBottom 0
		brRight 0
		cycleSpeed 0
		script 0
		cycler 0
		timer 0
		illegalBits $8000
		xLast 0
		yLast 0
		xStep 3
		moveSpeed 0
		blocks 0
		baseSetter 0
		mover 0
		looper 0
		viewer 0
		avoider 0
	)
	(method (init) 
		(super:init())
		= xLast x
		= yLast y
	)
	(method (doit) 
		(var oldLeft, oldRight)
		(if(& signal $8000)
		    return
		)
		= signal (& signal $FBFF)
		(if(script)
			(send script:doit())
		)
		(if((& signal 4) and (not & signal 2))
		    return
		)
		(if(viewer)
			(send viewer:doit(self))
		)
		(if(avoider)
			(send avoider:doit())
		)(else
			(if(mover)
				(send mover:doit())
			)
		)
		(if(cycler)
			= oldLeft brLeft
			= oldRight brRight
			(send cycler:doit())
			(if(baseSetter)
				(send baseSetter:doit(self))
			)(else
				BaseSetter(self)
			)
			(if((<> oldLeft brLeft) or (<> oldRight brRight))
				(if(not (self:canBeHere()))
		    		(self:findPosn)
		    	)
		    )
		)
  		= xLast x
  		= yLast y
	)
	(method (posn newX newY sendParams)
		(super:
			posn(newX newY rest sendParams)
		)
		= xLast newX 
		= yLast newY
		(if( not (self:canBeHere()) )
			(self:findPosn())
		)
	)
	(method (setLoop theLooper sendParams)
		(var pLooper)
		(if(== paramTotal 0)
			(super:setLoop())
		    = pLooper NULL
		)(else
			(if(not (IsObject(theLooper)) )
				(super:
					setLoop(theLooper rest sendParams)
				)
		    	= pLooper NULL
			)(else
				(if( (& (send theLooper:{-info-}) $8000) )
					= pLooper (send theLooper:new())
				)(else
		    		= pLooper theLooper 
		    	)
  			)
  			(if(pLooper)
 				(if(looper)
 					(send looper:dispose())
 				)
				(send (= looper pLooper):
					init(self rest sendParams)
				)
			)
		)
	)
	(method (delete) 
		(if( & signal $8000)
			(if(<> mover -1)
				(self:setMotion(0))
			)
			(self:setAvoider(0))
			(if(baseSetter)
				(send baseSetter:dispose())
				= baseSetter NULL
			)
			(if(looper)
				(send looper:dispose())
				= looper NULL
			)
			(if(viewer)
				(send viewer:dispose())
				= viewer NULL
			)
			(if(blocks)
				(send blocks:dispose())
				= blocks NULL
			)
			(super:delete())
		)
	)
	(method (motionCue) 
		(if(mover and (send mover:completed))
			(send mover:motionCue())
		)
		(super:motionCue())
	)
	(method (setMotion theMover sendParams) 
		(if(mover and <> mover -1)
			(send mover:dispose())
		)
		(if(theMover)
			(self:startUpd())
			(if(& (send theMover:{-info-}) $8000)
				= mover (send theMover:new())
			)(else
		    	= mover theMover 
		    )
		    (send mover:
		    	init(self rest sendParams)
		    )
  		)(else
		    = mover NULL
		)
	)
	(method (setAvoider theAvoider sendParams) 
		(if(avoider)
			(send avoider:dispose())
		)
		(if(IsObject(theAvoider))
			(if(& (send theAvoider:{-info-}) $8000)
				= avoider (send theAvoider:new())
			)(else
				= avoider theAvoider
			)
		)(else
            = avoider NULL
        )
		(if(avoider)
			(send avoider:
				init(self rest sendParams)
			)
		)
	)
	(method (ignoreHorizon fIGNORE_HORIZON) 
		(if(not paramTotal or fIGNORE_HORIZON)
		    = signal(| signal $2000)
		)(else
		    = signal(& signal $DFFF)
		)
	)
	(method (observeControl bits) 
		(var i)
		(for (= i 0) (< i paramTotal) (++i)
			= illegalBits (| illegalBits bits[i])
		)
	)
	(method (ignoreControl bits)
		(var i)
		= i 0
		(while(< i paramTotal)
			= illegalBits (& illegalBits (bnot bits[i]))
		    ++i 
		)
	)
	(method (observeBlocks nodes) 
		(if( not blocks )
		    = blocks (Set:new())
		)
		(send blocks:
			add(rest nodes)
		)
	)
	(method (ignoreBlocks nodes) 
		(send blocks:
			delete(rest nodes)
		)
		(if(send blocks:isEmpty())
			(send blocks:dispose())
			= blocks NULL
		)
	)
	(method (isStopped) 
		(if(not (IsObject(mover)) or 
		  ((== x (send mover:xLast)) and (== y (send mover:yLast)) ))
			return(TRUE)
		)
		return(FALSE)
	)
	(method (isBlocked) 
		return(& signal $400)
	)
	(method (findPosn)
		(var i, j, oldX, oldY, fCAN_BE_HERE)
		= oldX x
		= oldY y
		= fCAN_BE_HERE FALSE
  		(for (= i 1) (not fCAN_BE_HERE) (++i)
  			(for (= j 0) ( (not fCAN_BE_HERE) and (< j 8) ) (++j)
		    	= x (+ oldX (* i IsPosOrNeg(CosMult( (* j 45) 100))))
		    	= y (- oldY (* i IsPosOrNeg(SinMult((* j 45) 100))))
		    	(if(= fCAN_BE_HERE (self:canBeHere()))
		    		= fCAN_BE_HERE (self:onControl())
		    	)
		    )
		)
		(self:posn(x y))
	)
	(method (inRect x1 y1 x2 y2)
		(if((<= x1 x) and (< x x2) and (<= y1 y) and (< y y2))
			return(TRUE)
		)
		return(FALSE)
	)
	(method (onControl fUSE_POINT) 
		(if(paramTotal and fUSE_POINT)
			return(OnControl(ocSPECIAL x y))
		)(else 
			return(OnControl(ocSPECIAL brLeft brTop brRight brBottom))
		)
	)
	(method (distanceTo pObj)
		return( GetDistance(x y (send pObj:x) (send pObj:y) gPicAngle) )
	)
/********---------********/
	(method (canBeHere)
		(if(baseSetter)
			(send baseSetter:doit(self))
		)(else
			BaseSetter(self)
		)
		(if((== illegalBits 0) and (& signal $2000)
		    or CanBeHere(self (send gCast:elements)) )
		    	(if( (& signal $2000) or (not IsObject(gRoom))
		    	   or (>= y (send gRoom:horizon)) )
		    		(if((== blocks 0) or (send blocks:allTrue(#doit self)))
                        (if(>= x 0 and <= x 320 and >= y 0 and <= y 190)
		    			   return(TRUE)
                        )
		    		)
		    	)
		)
		return(FALSE)
	)
/* LSL2's
		(if(baseSetter)
			(send baseSetter:doit(self))
		)(else
			BaseSetter(self)
		)
		(if(CanBeHere(self (send gCast:elements)) )
			(if((& signal $2000) or (not IsObject(gRoom)) or 
			    (>= y (send gRoom:horizon)) ) 
				(if((== blocks 0) or (send blocks:allTrue(#doit self)) )
					return(TRUE)
				)
			)
		)
		return(FALSE)*/
	(method (setStep newX newY) 
		(if( (>= paramTotal 1) and (<> newX -1) )
			= xStep newX
		)
		(if( (>= paramTotal 2) and (<> newY -1) )
			= yStep newY
		)
		(if((mover) and (<> mover -1) and ((send mover:isMemberOf(MoveTo))) )
		    (send (self:mover):init())
		)
	)
	(method (setDirection direction)
		(var vanX, vanY, newX, newY, angle, temp5)
		= vanX (send gRoom:vanishingX)
		= vanY (send gRoom:vanishingY)
		(if( (== xStep 0) and (== yStep 0) )
		    return
		)
		
		= temp5 (/ 32000 proc999_3(xStep yStep))
		(switch(direction)
			(case CENTER
				(self:setMotion(0))
				return
			)
			(case UP
				= newX (- vanX x)
				= newY (- vanY y)
			)
			(case DOWN
				= newX (- x vanX)
				= newY (- y vanY)
			)
			(case RIGHT
				= newX temp5
				= newY 0
			)
			(case LEFT
				= newX neg temp5
				= newY 0
		    )
		    (default
		    	(if(< 180 (= angle GetAngle(x y vanX vanY)))
		    		= angle (- angle 360)
				)
				= angle (+ (/ (+ angle 90) 2) (* 45 (- direction 2)))
				= newX SinMult(angle 100)
				= newY neg CosMult(angle 100)
		    )
		)
		= temp5 (/ temp5 5)
		(while((< Abs(newY) temp5) and (< Abs(newX) temp5))
			= newX (* newX 5)
			= newY (* newY 5)
		)
		(self:setMotion(MoveTo (+ x newX) (+ y newY)))
	)
)
/******************************************************************************/
(class Blk of Obj
	(properties
		top 0
		left 0
		bottom 0
		right 0
	)
	(method (doit pObj)
		(if(   (<= (send pObj:brBottom) top)
		  	or (>  (send pObj:brTop)    bottom)
		 	or (<  (send pObj:brRight)  left)
		 	or (>= (send pObj:brLeft)   right))
		 		return(TRUE)
		 )
		 return(FALSE)
	)
)
/******************************************************************************/
(class Cage of Blk
	(properties
		top 0
		left 0
		bottom 0
		right 0
	)
	(method (doit pObj)
		(if((>= (send pObj:brTop) top)
		    and (>= (send pObj:brLeft) left)
			and (<= (send pObj:brBottom) bottom)
		    and (<= (send pObj:brRight) right)
		   )
			return(TRUE)
		)
		return(FALSE)
	)
)
/******************************************************************************/
