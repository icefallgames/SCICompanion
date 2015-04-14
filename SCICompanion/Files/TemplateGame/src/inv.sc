/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 inv.sc
 Contains the main classes for your game's inventory, one of the most essential 
 parts of an adventure game.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script INVENTORY_SCRIPT)
/******************************************************************************/
(use "obj")
(use "main")
(use "controls")
(use "syswindow")
/******************************************************************************/
(local
  btnHandle = 0
)
/******************************************************************************/
(class InvI of Obj
	(properties
		said 0
		description 0
		owner 0
		view 0
		loop 0
		cel 0
		script 0
	)
	(method (showSelf)
		(if(description)
			IconPrint(description view loop cel)
		)(else
			IconPrint(objectName view loop cel)
		)
	)
	(method (saidMe)
		return(Said(said))
	)
	(method (ownedBy anObject)
		return(== owner anObject)
	)
	(method (moveTo newOwner)
		= owner newOwner
		return(self)
	)
	(method (changeState newState)
		(if(script)
			(send script:changeState(newState))
		)
	)
)
/******************************************************************************/
(class Inv of Set
	(properties
		elements 0
		size 0
		carrying "You are carrying:"
		empty "You are carrying nothing!"
	)
	(method (init)
		= gInv self
	)
	(method (showSelf theOwner)
		(invD:
			text(carrying)
			doit(theOwner)
		)
	)
	(method (saidMe)
		return((self:firstTrue(#saidMe)))
	)
	(method (ownedBy anObject)
		return((self:firstTrue(#ownedBy anObject)))
	)
)
/******************************************************************************/
(instance invD of Dialog
	(properties)
	(method (init theOwner)
		(var temp0, temp1, temp2, temp3, temp4, temp5, temp6)
		= temp1 4
		= temp0 4
		= temp2 4
		= temp3 0
		= temp5 (send gInv:first())
		(while(temp5)
		    = temp6 NodeValue(temp5)
		    (if(send temp6:ownedBy(theOwner))
			    ++temp3 
			    = temp4 (DText:new())
			    (self:add(
			    	(send temp4:
						value(temp6)
						text((send temp6:name))
						nsLeft(temp0)
						nsTop(temp1)
						state(3)
						font(gDefaultFont)
						setSize()
						yourself()
					)
				))	

			    (if(< temp2 (- (send temp4:nsRight) (send temp4:nsLeft)))
			    	= temp2 (- (send temp4:nsRight) (send temp4:nsLeft))
				)
			    = temp1 (+ temp1 (+ (- (send temp4:nsBottom) (send temp4:nsTop)) 1))
			    (if(> temp1 140)
			    	= temp1 4
			    	= temp0 (+ temp0 (+ temp2 10))
			    	= temp2 0
				)
			)
  			= temp5 (send gInv:next(temp5))
		)
		(if(not temp3 )
		    (self:dispose())
			return(0)
		)
		= window SysWindow
	    (self:setSize())
		= btnHandle (DButton:new())
		(send btnHandle:
			text("OK")
			setSize()
			moveTo((- nsRight (+ 4 (send btnHandle:nsRight))) nsBottom)
		)
	    (send btnHandle:
	    	move(
	    		(- (send btnHandle:nsLeft) (send btnHandle:nsRight))
	    		0
	    	)
	    )
		(self:
			add(btnHandle)
			setSize()
			center()
		)
		return(temp3)
	)
	(method (doit theOwner)
		(var temp0,temp1,ts[40])
		(if(not (self:init(theOwner)))
		    Print((send gInv:empty))
		    return
		)
		(self:open(nwTITLE 15))
		= temp0 btnHandle

  		(while(1) 
  			(if(not (= temp0 (super:doit(temp0))) or == temp0 -1 or == temp0 btnHandle)
  				break
  			)
  			(send (send temp0:value):showSelf())
		)
  		(self:dispose())
	)
	(method (handleEvent pEvent)
		(var temp0, temp1)
		= temp0 (send pEvent:message)
		= temp1 (send pEvent:type)
		(switch(temp1)
			(case 4
				(switch(temp0)
					(case $4800
		    			= temp0 $F00
					)
					(case $5000
		    			= temp0 9
					)
				)
			)
			(case $40
				(switch(temp0)
					(case 1
						= temp0 $F00
						= temp1 4
					)
					(case 5
						= temp0 9
						= temp1 4
		    		)
		    	)
			)
		)
		(send pEvent:
			type(temp1)
			message(temp0)
		)
		(super:handleEvent(pEvent))
	)
)
/******************************************************************************/
