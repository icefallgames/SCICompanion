/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 user.sc
 Contains the classes to handle user input and the main character (ego).
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script USER_SCRIPT)
/******************************************************************************/
(use "main")
(use "controls")
(use "sound")
(use "cycle")
(use "menubar")
(use "feature")
(use "obj")
/******************************************************************************/
(local
  inputStr[51]
  maxInputLen
)
/******************************************************************************/
(instance uEvt of Event
	(properties)
)
/******************************************************************************/
(class User of Obj
	(properties
		alterEgo 0
		canInput 0
		controls 0
		echo $20
		prevDir 0
		prompt "Enter Input:"
		inputLineAddr 0
		x -1
		y -1
		blocks 1
		mapKeyToDir 1
		curEvent 0
	)
	(method (init pInputStr maxLen)
		(if(paramTotal)
		    = inputLineAddr pInputStr 
		)(else
		    = inputLineAddr @inputStr
		)
		(if( == paramTotal 2)
		    = maxInputLen maxLen
		)(else
		    = maxInputLen 45
		)
		= curEvent uEvt
	)
	(method (doit)
		(if(== 0 gSetRegions)
			(send curEvent:
				type(0)
				message(0)
				modifiers(0)
				y(0)
				x(0)
				claimed(0)
			)
			GetEvent(evALL_EVENTS curEvent)
			(self:handleEvent(curEvent))
		)
	)
	(method (canControl fCONTROLS)
		(if(paramTotal)
			= controls fCONTROLS
			= prevDir CENTER
		)
		return(controls)
	)
	(method (getInput pEvent)
		(var prevSound, strLen)
		(if(<> (send pEvent:type) evKEYBOARD)
		    = inputStr 0
		)
		(if(<> (send pEvent:message) echo)
			Format(@inputStr "%c" (send pEvent:message))
		)
		= prevSound (Sound:pause(blocks))
		= strLen EditPrint(@inputStr maxInputLen prompt #at x y)
		(Sound:pause(prevSound))
		return(strLen)
	)
	(method (said pEvent)
		(send gSFeatures:add(gCast gFeatures))

		(if(TheMenuBar)
			(send gSFeatures:addToFront(TheMenuBar))
		)
		(send gSFeatures:
			addToEnd(gGame)
			handleEvent(pEvent)
			release()
		)
		(if((== (send pEvent:type) evSAID) and (not (send pEvent:claimed)))
			(send gGame:pragmaFail(@inputStr))
		)
	)
	(method (handleEvent pEvent)
		(var evType)
		(if(send pEvent:type)
			= gUserEvent pEvent
			= evType (send pEvent:type)
			(if(mapKeyToDir)
				MapKeyToDir(pEvent)
			)
			(if(TheMenuBar)
				(TheMenuBar:handleEvent(pEvent evType))
			)
		    GlobalToLocal(pEvent)
			(if(not (send pEvent:claimed))
				(send gGame:handleEvent(pEvent evType))
			)

		    (if(controls and (not (send pEvent:claimed))
		      and (send gCast:contains(alterEgo)))
		    	(send alterEgo:handleEvent(pEvent))
		    )
			
	       (if(canInput and not(send pEvent:claimed))
				(if( (== (send pEvent:message) echo) or (<= $20 (send pEvent:message)) and (<= (send pEvent:message) 255))
						(if( (self:getInput(pEvent)) and Parse(@inputStr pEvent))
							(send pEvent:type(evSAID))
		    				(self:said(pEvent))
						)
                )
            )
		)
  		= gUserEvent NULL
	)
)
/******************************************************************************/
(class Ego of Act
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
		signal $2000
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
		edgeHit 0
	)
	(method (init)
		(super:init())
		(if( not cycler )
			(self:setCycle(Walk))
		)
	)
	(method (doit)
		(super:doit())
		(if(<= x 3)
		    = edgeHit EDGE_LEFT
		)(else
  			(if(<= y (send gRoom:horizon))
		    	= edgeHit EDGE_TOP
			)(else
				(if(>= x 316)
		    		= edgeHit EDGE_RIGHT 
		    	)(else
			    	(if(>= y 186)
		    			= edgeHit EDGE_BOTTOM
		    		)(else
		    			= edgeHit EDGE_NONE
		    		)
		    	)
		    )
		)
	)
	(method (handleEvent pEvent)
		(var direction)
		(if( not (super:handleEvent(pEvent)) )
		    (switch( (send pEvent:type) )
				(case evMOUSEBUTTON
					// Make sure it's the left button
					(if(not (& (send pEvent:modifiers) emRIGHT_BUTTON)
					    and (User:controls) )  
					 		(self:
					 			setMotion(
					 				MoveTo
					 				(send pEvent:x)	 
					 				(send pEvent:y)	 
					 			)
					 		)	
					 		(User:prevDir(CENTER))
					 		(send pEvent:claimed(TRUE))
					 )
				)
  				(case evJOYSTICK
  					= direction (send pEvent:message)
  					(if( (== direction (User:prevDir)) and (IsObject(mover)))
		    			= direction CENTER
		    		)
		    		(User:prevDir(direction))
		    		(self:setDirection(direction))
		    		(send pEvent:claimed(TRUE))
  				)
  			)
  		)
  		return(send pEvent:claimed)
	)
	(method (get items)
		(var i, invItem)
		(for (= i 0) (< i paramTotal) (++i)
			= invItem (send gInv:at(items[i]))
			(send invItem:moveTo(self))
		 )
	)
	(method (put item newOwner)
		(var invItem)
		(if( (self:has(item)) )
			= invItem (send gInv:at(item))
		    (if(== paramTotal 1)
				(send invItem:moveTo(-1))
			)(else
				(send invItem:moveTo(newOwner))
			)
		)
	)
	(method (has item)
		(var invItem)
		= invItem (send gInv:at(item))
		(if(invItem)
			(send invItem:ownedBy(self))
		)
	)
)
/******************************************************************************/
