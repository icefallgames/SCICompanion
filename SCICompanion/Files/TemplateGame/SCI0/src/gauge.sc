/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 gauge.sc
 Contains a dialog window which contains a custom gauge control. This is used 
 for things such as volume control and the game speed adjustment.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script GAUGE_SCRIPT)
/******************************************************************************/
(use "main")
(use "controls")
(use "syswindow")
/******************************************************************************/
(local
  /*controls*/
  txtDescription = NULL
  btnHigher = NULL
  btnLower = NULL
  txtGuage = NULL
  btnOK = NULL
  btnNormal = NULL
  btnCancel = NULL
  /*text buffer*/
  strGauge[41] 
)
/******************************************************************************/
(class Gauge of Dialog
	(properties
		elements 0
		size 0
		text 0
		window 0
		theItem 0
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		time 0
		busy 0
		seconds 0
		lastSeconds 0
		description 0
		higher "up"
		lower "down"
		normal 7
		minimum 0
		maximum 15
	)
	(method (init gaugePos)
		(var ctlX, ctlY)
		= window SysWindow
		(self:update(gaugePos))
		
		= btnLower (DButton:new())
		(send btnLower:
			text(lower)
			moveTo(4 4)
			setSize()
		)
		(self:
			add(btnLower)
			setSize()
		)
		= txtGuage (DText:new())
		(send txtGuage:
			text(@strGauge)
			moveTo( (+ (send btnLower:nsRight) 4) 4 )
			font(0)
			setSize()
		)
		(self:
			add(txtGuage)
			setSize()
		)
		= btnHigher (DButton:new())
		(send btnHigher:
			text(higher)
			moveTo( (+ (send txtGuage:nsRight) 4) 4 )
			setSize()
		)
		(self:
			add(btnHigher)
			setSize()
		)
		= btnOK (DButton:new())
		= nsBottom (+ nsBottom 8)
		(send btnOK:
			text("OK")
			setSize()
			moveTo(4 nsBottom)
		)
		= btnNormal (DButton:new())
		(send btnNormal:
			text("Normal")
			setSize()
			moveTo((+ (send btnOK:nsRight) 4) nsBottom)
		)
		= btnCancel (DButton:new())
		(send btnCancel:
			text("Cancel")
			setSize()
			moveTo((+ (send btnNormal:nsRight) 4) nsBottom)
		)
		(self:
			add(btnOK btnNormal btnCancel)
			setSize()
		)	
		= ctlX (- (- nsRight (send btnCancel:nsRight)) 4)
		= txtDescription (DText:new())
		(send txtDescription:
			text(description)
			font(gSaveRestoreFont)
			setSize(- nsRight 8)
			moveTo(4 4)
		)
		= ctlY (+ (send txtDescription:nsBottom) 4)
		(self:add(txtDescription))
		
		(send btnHigher:move(0    ctlY))
		(send btnLower: move(0    ctlY))
		(send txtGuage: move(0    ctlY))
		(send btnOK:    move(ctlX ctlY))
		(send btnNormal:move(ctlX ctlY))
		(send btnCancel:move(ctlX ctlY))
		
		(self:
			setSize()
			center()
			open(nwTITLE 15)
		)
	)
	(method (doit gaugePos)
		(var btnPressed, newGaugePos)
		(self:init(gaugePos))
		= newGaugePos gaugePos
		(while(1)
  			(self:update(newGaugePos))
  			(send txtGuage:draw())
  			= btnPressed (super:doit(btnOK))
			(if(== btnPressed btnHigher)
				(if(< newGaugePos maximum)
					++newGaugePos
				)
		    )(else
		    	(if(== btnPressed btnLower)
		    		(if(> newGaugePos minimum)
		    			--newGaugePos
		    		)
				)
				(if(== btnPressed btnOK)
					(self:dispose())
					return(newGaugePos)
		    	)(else
		    		(if(== btnPressed btnNormal)
		    			= newGaugePos normal
		    		)(else
		    			(if(== btnPressed 0 or == btnPressed btnCancel)
							(self:dispose())
							return(gaugePos)
		    			)
		    		)
		    	)
			)
		)
	)
	(method (handleEvent pEvent)
		(switch( (send pEvent:type) )
			(case evKEYBOARD
				(switch( (send pEvent:message) )
					(case KEY_LEFT
						(send pEvent:claimed(TRUE))
						return(btnLower)
					)
					(case KEY_RIGHT
						(send pEvent:claimed(TRUE))
						return(btnHigher)
		    		)
				)
  			)
  			(case evJOYSTICK
  				(switch( (send pEvent:message) ) 
  					(case JOY_LEFT
						(send pEvent:claimed(TRUE))
						return(btnLower)
					)
					(case JOY_RIGHT
						(send pEvent:claimed(TRUE))
						return(btnHigher)
					)
		    	)
			)
		)
		(super:handleEvent(pEvent))
	)
	(method (update gaugePos) // Draws the guage
		(var i, strLen)
		= strLen (- maximum minimum)
		= i 0
  		(while(< i strLen)
			(if(< i gaugePos)
				StrAt(@strGauge i 6) // black block character
			)(else
				StrAt(@strGauge i 7) // white block character
			)
		    ++i 
		)
	)
)
/******************************************************************************/
