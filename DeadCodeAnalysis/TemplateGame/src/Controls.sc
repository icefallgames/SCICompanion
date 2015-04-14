/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 controls.sc
 Contains the base menubar class, dialog window class, control classes 
 (button,edit,icon,etc.), as well as the Print methods. These allow you to 
 have a GUI in your game, as well as print messages. 
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script CONTROLS_SCRIPT)
/******************************************************************************/
(use "main")
(use "obj")
/******************************************************************************/
(class MenuBar of Obj
	(properties
		state DISABLED
	)
	(method (draw)
		= state ENABLED
		DrawMenuBar(TRUE)
	)
	(method (hide)
		DrawMenuBar(FALSE)
	)
	(method (handleEvent pEvent selectParams)
		(var menuItem, joyInfo)
		= menuItem NULL
		(if(state)
		    = joyInfo Joystick(12 30)
			= menuItem MenuSelect(pEvent rest selectParams)
 			Joystick(12 joyInfo)
	    )
		return(menuItem)
	)
	(method (add addParams)
		AddMenu(rest addParams)
	)
)
/******************************************************************************/
(class Control of Obj
	(properties
		type 0
		state 0
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		key 0
		said 0
		value 0
	)
	(method (doit)
		return(value)
	)
	(method (enable fENABLE)
		(if(fENABLE)
			= state (| state 1)
		)(else
			= state (& state $FFFE)
		)
	)
	(method (select fSELECT)
		(if(fSELECT)
			= state (| state 8)
		)(else
			= state (& state $FFF7)
		)
		(self:draw())
	)
	(method (handleEvent pEvent)
		(var tracked, evType)
		(if(send pEvent:claimed)
		    return(FALSE)
		)
  		= tracked FALSE
  		(if(& state 1)
		    = evType (send pEvent:type)
		    (if( (== evType evSAID) and Said(said)  or 
				 (== evType evKEYBOARD) and (== (send pEvent:message) key)  or
				 (== evType evMOUSEBUTTON) and (self:check(pEvent)) )
		    		(send pEvent:claimed(TRUE))
		    		= tracked (self:track(pEvent))
		    )
		)
		return(tracked)
	)
	(method (check pEvent)
		(if( (>= (send pEvent:x) nsLeft) and (>= (send pEvent:y) nsTop)
		   and (< (send pEvent:x) nsRight) and (< (send pEvent:y) nsBottom) )
             return(TRUE)
        )
        return(FALSE)
	)
	(method (track pEvent)
		(var mouseInArea, prevMouseInArea)
		(if (== evMOUSEBUTTON (send pEvent:type))
			= prevMouseInArea FALSE
			(do
				= pEvent (Event:new(evNOJOYSTICK))
	
				GlobalToLocal(pEvent)
				= mouseInArea (self:check(pEvent))
				(if(<> mouseInArea prevMouseInArea)
					HiliteControl(self)
					= prevMouseInArea mouseInArea
		    	)
		    	(send pEvent:dispose())
		    ) while( GetMouseRelease() )
			(if(mouseInArea)
				HiliteControl(self)
			)
			return(mouseInArea)
		)(else
			return(self)
		)
	)
	(method (setSize))
	(method (move theX theY)
		= nsRight  (+ nsRight  theX)
		= nsLeft   (+ nsLeft   theX)
		= nsTop    (+ nsTop    theY)
		= nsBottom (+ nsBottom theY)
	)
	(method (moveTo newX newY)
		(self:
			move( (- newX nsLeft) (- newY nsTop) )
        )
	)
	(method (draw)
		DrawControl(self)
	)
	(method (isType aType)
		return(== type aType)
	)
	(method (checkState aState)
		return(& state aState)
	)
	(method (cycle))
)
/******************************************************************************/
(class DText of Control
	(properties
		type ctlTEXT
		state 0
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		key 0
		said 0
		value 0
		text 0
		font 1
		mode 0
	)
	(method (new)
		(self:font(gDefaultFont))
		(super:
			yourself()
			new()
		)
	)
	(method (setSize maxWidth)
		(var rect[4])

		(if(paramTotal)
			TextSize(@rect text font maxWidth)
		)(else
			TextSize(@rect text font 0)
		)
		= nsBottom (+ nsTop  rect[rtBOTTOM])
		= nsRight  (+ nsLeft rect[rtRIGHT]) 
	)
)
/******************************************************************************/
(class DIcon of Control
	(properties
		type ctlICON
		state 0
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		key 0
		said 0
		value 0
		view 0
		loop 0
		cel 0
	)
	(method (setSize)
        = nsRight  (+ nsLeft CelWide(view loop cel))
		= nsBottom (+ nsTop  CelHigh(view loop cel))
	)
)
/******************************************************************************/
(class DButton of Control
	(properties
		type ctlBUTTON
		state 3
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		key 0
		said 0
		value 0
		text 0
		font 0
	)
	(method (setSize)
		(var rect[4])
		TextSize(@rect text font)
		= rect[rtBOTTOM] (+ rect[rtBOTTOM] 2)
		= rect[rtRIGHT] (+ rect[rtRIGHT] 2)
		= nsBottom (+ nsTop rect[rtBOTTOM])
		= rect[rtRIGHT] (* (/ (+ rect[rtRIGHT] 15) 16) 16)
		= nsRight (+ rect[rtRIGHT] nsLeft)
	)
)
/******************************************************************************/
(class DEdit of Control
	(properties
		type ctlEDIT
		state 1
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		key 0
		said 0
		value 0
		text 0
		font 0
		max 0
		cursor 0
	)
	(method (track pEvent)
		EditControl(self pEvent)
		return(self)
	)
	(method (setSize)
		(var rect[4])
		
		TextSize(@rect "M" font)
		= nsBottom (+ nsTop  rect[rtBOTTOM])
		= nsRight (+ nsLeft (/ (* (* rect[rtRIGHT] max) 3) 4)) 
		= cursor StrLen(text)
	)
)
/******************************************************************************/
(class DSelector of Control
	(properties
		type ctlSELECTOR
		state 0
		nsTop 0
		nsLeft 0
		nsBottom 0
		nsRight 0
		key 0
		said 0
		value 0
		font 0
		x 20
		y 6
		text 0
		cursor 0
		lsTop 0
		mark 0
	)
	(method (handleEvent pEvent)
		(var theMark, rect[4])
		(if(send pEvent:claimed)
			return(FALSE)
		)
		// if the joystick was used, set the 
		//  events to the keyboard equivilants
		(if(== evJOYSTICK (send pEvent:type))
			(send pEvent:type(evKEYBOARD))
			(switch( (send pEvent:message) )
				(case DOWN
					(send pEvent:message(KEY_DOWN))
				)
				(case UP
					(send pEvent:message(KEY_UP))
				)
				(default
					(send pEvent:type(evJOYSTICK))
		    	)
		    )
		)
		(switch( (send pEvent:type) )
			(case evKEYBOARD
				(send pEvent:claimed(TRUE))
				(switch( (send pEvent:message) )
					(case KEY_HOME
						(self:retreat(50))
					)
					(case KEY_END
						(self:advance(50))
					)
					(case KEY_PAGEUP
						(self:advance(- y 1))
					)
					(case KEY_PAGEDOWN
						(self:retreat(- y 1))
					)
					(case KEY_DOWN
						(self:advance(1))
					)
					(case KEY_UP
						(self:retreat(1))
					)
					(default
						(send pEvent:claimed(FALSE))
		    		)
		    	)
			)
			(case evMOUSEBUTTON
				(if(self:check(pEvent))
		    		(send pEvent:claimed(TRUE))
		    		(if(< (send pEvent:y) (+ nsTop 10))
		    			(while(1)
  							(self:retreat(1))
  							(if (not GetMouseRelease())
								(if( (send pEvent:claimed) and (& state 2) )
									return(self)
								)
								return(0)
  							)
  						)
					)
  					(if(> (send pEvent:y) (- nsBottom 10))
  						(while(1)
  							(self:advance(1))
  							(if(not GetMouseRelease()) 
								(if( (send pEvent:claimed) and (& state 2) )
									return(self)
								)
								return(0)
							)
						)
  					)
  					TextSize(@rect "M" font)
  					= theMark (/ (- (send pEvent:y) (+ nsTop 10)) rect[rtBOTTOM])
  					(if(> theMark mark)
		    			(self:advance(- theMark mark))
		    		)(else
  						(self:retreat(- mark theMark))
  					)
  				)
			)
		)
		(if( (send pEvent:claimed) and (& state 2) )
			return(self)
		)
		return(0)
	)
	(method (setSize)
		(var rect[4])
		TextSize(@rect "M" font)
		
		= nsBottom 	(+ (+ nsTop 20) (* rect[rtBOTTOM] y))
		= nsRight	(+ nsLeft (/ (* (* rect[rtRIGHT] x) 3) 4))
		= cursor	text
		= lsTop		text
		= mark 		0
	)
	(method (indexOf aString)
		(var pText, i)
		= pText text
  		(for (= i 0) (< i 300) (++i)
  			(if(== 0 StrLen(pText))
  				return(-1)
  			)
  			(if(not StrCmp(aString pText))
  				return(i)
			)
			= pText (+ pText x)
		)
	)
	(method (at position) 
		return (+ text (* x position))
	)
	(method (advance amount)
		(var DO_DRAW)
		= DO_DRAW FALSE
  		(while(amount and StrAt(cursor x))
		    = DO_DRAW TRUE
		    = cursor (+ cursor x)
		    (if(< (+ mark 1) y)
		    	++mark
			)(else
				= lsTop (+ lsTop x)
 			)
		    --amount 
		)
  		(if(DO_DRAW)
  			(self:draw())
  		)
	)
	(method (retreat amount)
		(var DO_DRAW)
		= DO_DRAW FALSE
  		(while(amount and (<> cursor text))
  			= DO_DRAW TRUE
  			= cursor (- cursor x)
  			(if(mark)
  				--mark
  			)(else
  				= lsTop (- lsTop x)
  			)
		    --amount 
		)
		(if(DO_DRAW)
			(self:draw())
		)
	)
)
/******************************************************************************/
(class Dialog of List
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
	)
	(method (doit pItem)
		(var hEvent, isClaimed)
		= busy TRUE
		(self:eachElementDo(#init))
		(if(theItem)
			(send theItem:select(FALSE))
		)
		(if(paramTotal and pItem)
		    = theItem pItem
		)(else
			= theItem (self:firstTrue(#checkState TRUE))
		)
		(if(theItem)
		    (send theItem:select(TRUE))
		)
		= isClaimed FALSE
		(while(not isClaimed)
		    (self:eachElementDo(#cycle))
			= hEvent (Event:new())
		    GlobalToLocal(hEvent)
  			= isClaimed (self:handleEvent(hEvent))
			(send hEvent:dispose())
			(self:check)
			(if( (== isClaimed -1) or (not busy) )
		    	= isClaimed FALSE
		    	EditControl(theItem 0)
		    	break
		    )
            Wait(1)
		)
		= busy FALSE
		return(isClaimed)
	)
	(method (dispose)
		(if(== self gPrintDlg)
			SetPort(gOldPort)
			= gPrintDlg NULL
			= gOldPort NULL
		)
		(if(window)
			(send window:dispose())
		)
		= window  NULL
		= theItem NULL
		(super:dispose())
	)
	(method (open theType thePriority)
		(if(PicNotValid() and gCast)
			Animate( (send gCast:elements()) 0 )
		)
		= window (send window:new())
		(send window:
			top(nsTop)
			left(nsLeft)
			bottom(nsBottom)
			right(nsRight)
			title(text)
			type(theType)
			priority(thePriority)
			color(gWndColor)
			back(gWndBack)
			open()
		)
		= seconds time
		(self:draw())
	)
	(method (draw)
		(self:eachElementDo(#draw))
	)
	(method (cue)
		(if(not busy)
		    (self:dispose())
		)(else
			= busy FALSE
		)
	)
    (method (advance)
		(var hItem)
		(if(theItem)
			(send theItem:select(0))
			= hItem (self:contains(theItem))
  			(do
  				= hItem (self:next(hItem))
		    	(if(not hItem)
		    		= hItem (self:first())
				)
				= theItem NodeValue(hItem)
			) while(not (& (send theItem:state) 1) )
			(send theItem:select(1))
		)
	)
	(method (retreat)
		(var hItem)
		(if(theItem)
			(send theItem:select(0))
			= hItem (self:contains(theItem))
			(do
  				= hItem (self:prev(hItem))
  				(if(not hItem)
  					= hItem (self:last)
  				)
  				= theItem NodeValue(hItem)
			) while(not (& (send theItem:state) 1) )
			(send theItem:select(1))
		)
	)
	(method (move theX theY)
		= nsRight   (+ nsRight   theX)
		= nsLeft    (+ nsLeft    theX)
		= nsTop     (+ nsTop     theY)
		= nsBottom  (+ nsBottom  theY)
	)
	(method (moveTo newX newY)
		(self:move(
			(- newX nsLeft)
			(- newY nsTop)
		))
	)
	(method (center)
		(self:moveTo(
		    (+ (send window:brLeft) (/ (- (- (send window:brRight ) (send window:brLeft)) (- nsRight nsLeft)) 2))
		    (+ (send window:brTop ) (/ (- (- (send window:brBottom) (send window:brTop )) (- nsBottom nsTop)) 2))
		))
	)
	(method (setSize)
		(var i, hControl, rect[4])
		(if(text)
			TextSize(@rect text 0 -1)
			= nsTop 	rect[rtTOP]
			= nsLeft 	rect[rtLEFT]
			= nsBottom	rect[rtBOTTOM]
			= nsRight	rect[rtRIGHT]
  		)(else
		    = nsTop		0
		    = nsLeft	0
		    = nsBottom	0
		    = nsRight	0
		)
		= i (self:first)
		(while(i)
		    = hControl NodeValue(i)
			(if(< (send hControl:nsLeft) nsLeft)
		    	= nsLeft (send hControl:nsLeft)
		    )
			(if(< (send hControl:nsTop) nsTop)
		    	= nsTop (send hControl:nsTop)
			)
  			(if(> (send hControl:nsRight) nsRight)
		    	= nsRight (send hControl:nsRight)
			)
  			(if(> (send hControl:nsBottom) nsBottom)
		    	= nsBottom (send hControl:nsBottom)
			)
  			= i (self:next(i))
		)
		= nsRight (+ nsRight 4)
		= nsBottom (+ nsBottom 4)
		(self:moveTo(0 0))
	)
	(method (handleEvent pEvent)
		(var hItem)
		(if( (send pEvent:claimed) or (== (send pEvent:type) 0)
		     or (<> evMOUSEBUTTON (send pEvent:type))
		         and (<> evKEYBOARD (send pEvent:type))
		         and (<> evJOYSTICK (send pEvent:type))
			     and (<> $100 (send pEvent:type))
		   )	
		   		EditControl(theItem pEvent)
		  		return(0)
		)
  		(if(= hItem (self:firstTrue(#handleEvent pEvent)))
		    EditControl(theItem 0)
		    (if ( not (send hItem:checkState(2)) )
			  	(if(theItem)
		    		(send theItem:select(0))
		    	)
  				= theItem hItem
  				(send theItem:select(1))
				(send hItem:doit())
				return(NULL)
			)
  		)
  			(if((== (send pEvent:type) $100) or
  			    (== evKEYBOARD (send pEvent:type)) and (== $D (send pEvent:message)) )
  			    (if(theItem and (send theItem:(checkState(1))) )
		    		= hItem theItem
		    		EditControl(theItem 0)
		    		(send pEvent:claimed(TRUE))
		    		return(theItem)
		    	)
		    )
 	 			(if((== evKEYBOARD (send pEvent:type)) and (== KEY_ESC (send pEvent:message)))
			    		(send pEvent:claimed(TRUE))
			    		return(-1)
			    )
			    (if(not (self:firstTrue(#checkState 1)))
			    	(if((== evKEYBOARD (send pEvent:type)) and (== KEY_RETURN (send pEvent:message))
				    	or  (== evMOUSEBUTTON (send pEvent:type)) or (== $100 (send pEvent:type)))
			    		(send pEvent:claimed(TRUE))
			    		return(-1)
			    	)
			    )
 	 				(if((== evKEYBOARD (send pEvent:type)) and (== KEY_TAB (send pEvent:message)))
 	 					(send pEvent:claimed(TRUE))
			    		(self:advance())
			    	)(else
		    			(if((== evKEYBOARD (send pEvent:type)) and (== KEY_SHIFTTAB (send pEvent:message)))
		    				(send pEvent:claimed(TRUE))
		    				(self:retreat())
			    		)(else
  							EditControl(theItem pEvent)
  						)
  					)
  				
  		return(hItem)
	)
	(method (check)
		(var theTime)
		(if(seconds)
			= theTime GetTime(gtTIME_OF_DAY)
			(if(<> lastSeconds theTime)
				= lastSeconds theTime
				(if(not --seconds)
		    		(self:cue())
		    	)
		    )
		)
	)
)
/******************************************************************************/
(class Controls of List
	(properties
		elements 0
		size 0
	)
	(method (draw)
		(self:eachElementDo(#setSize))
		(self:eachElementDo(#draw))
	)
	(method (handleEvent pEvent)
		(var hCtl)
		(if( (send pEvent:claimed) )
			return(NULL)
		)
		= hCtl (self:firstTrue(#handleEvent pEvent))
		(if(hCtl and (not (send hCtl:checkState(2)) ) )
			(send hCtl:doit())
			= hCtl NULL
		)
		return(hCtl)
	)
)
/******************************************************************************/
(procedure (GetMouseRelease)
	(var hEvent, retVal)
	= hEvent (Event:new())
	= retVal (<> (send hEvent:type) evMOUSERELEASE)
	(send hEvent:dispose())
	return(retVal)
)
/******************************************************************************/
(procedure public (Print params)
	(var hDialog, hDText, hIcon, hEdit, btnPressed, paramCnt, diagX, diagY,
		 maxWidth, newPrintDlg, hFirstEnabled, oldPort, hButtons[6], btnsWidth, 
		 buttonCnt, btnX, msgBuf[1013], moveToX, moveToY)
	= diagY  -1
	= diagX  -1
	= buttonCnt 0
	= hEdit  NULL
	= hIcon  NULL
	= btnsWidth 0
	= maxWidth  0
	= newPrintDlg  NULL
	= hDialog (Dialog:new())
	(send hDialog:
		window(gTheWindow)
		name("PrintD")
	)
	= hDText (DText:new())
	(if(<u params[0] 1000)
		GetFarText(params[0] params[1] @msgBuf)
		= paramCnt 2
	)(else
  		(if(params[0])
  			StrCpy(@msgBuf params[0])
  			= paramCnt 1
		)(else
			= msgBuf 0
			= paramCnt  0
		)
	)	
	(send hDText:
		text(@msgBuf)
		moveTo(4 4)
		font(gDefaultFont)
		setSize()
	)
	(send hDialog:add(hDText))
  	(while(< paramCnt paramTotal)
  		(switch(params[paramCnt])
  			(case #mode
  				++paramCnt
  				(send hDText:mode(params[paramCnt]))
			)
			(case #font
  				++paramCnt
  				(send hDText:
  					font(params[paramCnt])
  					setSize(maxWidth)
  				)
			)
			(case #width
				= maxWidth params[++paramCnt]
				(send hDText:setSize(maxWidth))
			)
			(case #time
				++paramCnt
				(send hDialog:time(params[paramCnt]))
			)
			(case #title
				++paramCnt
				(send hDialog:text(params[paramCnt]))
			)
			(case #at
				= diagX params[++paramCnt]
				= diagY params[++paramCnt]
			)
			(case #draw
				Animate(
					(send gCast:elements)
					0
				)
			)
			(case #edit
				++paramCnt
				= hEdit (DEdit:new())
				(send hEdit:
					text(params[paramCnt])
				)
				++paramCnt
				(send hEdit:
					max(params[paramCnt])
					setSize()
				)
			)
			(case #button
		    	= hButtons[buttonCnt] (DButton:new())
				(send hButtons[buttonCnt]:
					text(params[++paramCnt])
					value(params[++paramCnt])
					setSize()
		    	)
		    	= btnsWidth (+ btnsWidth (+ (send hButtons[buttonCnt]:nsRight) 4))
		    	++buttonCnt
			)
			(case #icon
				(if(IsObject(params[+ paramCnt 1]))
					= hIcon (send params[+ paramCnt 1]:new())
					(send hIcon:setSize())
					= paramCnt (+ paramCnt 1)
		    	)(else
		    		= hIcon (DIcon:new())
		    		(send hIcon:
		    			view(params[+ paramCnt 1])
		    			loop(params[+ paramCnt 2])
		    			cel(params[+ paramCnt 3])
		    			setSize()
		    		)
		    		= paramCnt (+ paramCnt 3)
		    	)
			)
			(case #dispose
				(if(gPrintDlg)
					(send gPrintDlg:dispose())
				)
				= newPrintDlg hDialog
			)
			(case #window
				++paramCnt
				(send hDialog:window(params[paramCnt]))
			)
		)
		++paramCnt
	)
	(if(hIcon)
		(send hIcon:moveTo(4 4))
		(send hDText:moveTo( (+ 4 (send hIcon:nsRight)) (send hIcon:nsTop) ))
		(send hDialog:add(hIcon))
	)
  	(send hDialog:setSize())
  	(if(hEdit)
		    (send hEdit:moveTo(
		    	(send hDText:nsLeft) 
		    	(+ 4 (send hDText:nsBottom))
		    ))
		    (send hDialog:
		    	add(hEdit)
		    	setSize()
		    )
	)
  	(if(> btnsWidth (send hDialog:nsRight))
		= btnX 4
	)(else
  		= btnX (- (send hDialog:nsRight) btnsWidth)
        //If you wanted the button centered...
        //   = btnX / ( - (send hDialog:nsRight) btnsWidth ) 2
	)
	= paramCnt 0
	(while(< paramCnt buttonCnt)
		(send hButtons[paramCnt]:moveTo( btnX (send hDialog:nsBottom) ))
		(send hDialog:add(hButtons[paramCnt]))
		= btnX (+ 4 (send hButtons[paramCnt]:nsRight))
		++paramCnt
	)
  	(send hDialog:
  		setSize()
  		center()
  	)
  	(if(hIcon and not StrLen(@msgBuf))
		(send hIcon:
		 	moveTo(
		    	(
		    		/ (- (- (send hDialog:nsRight) (send hDialog:nsLeft))
		    		(- (send hIcon:nsRight) (send hIcon:nsLeft))) 2
		    	)
		    	4
		    )
		)
	)
	
	(if(== -1 diagX)
		= moveToX (send hDialog:nsLeft)
	)(else
		= moveToX diagX
	)
	(if(== -1 diagY)
		= moveToY (send hDialog:nsTop)
	)(else
		= moveToY diagY
	)
 	(send hDialog:moveTo( moveToX moveToY ))

	= oldPort GetPort()
			
	(if(send hDialog:text)
		= moveToX nwTITLE
	)(else
		= moveToX nwNORMAL
	)
	(if(newPrintDlg)
		= moveToY 15
	)(else
		= moveToY -1
	)
	(send hDialog:open(moveToX moveToY))

	(if(newPrintDlg)
		= gOldPort GetPort()
		SetPort(oldPort)
		return(= gPrintDlg newPrintDlg)
	)
  	(if( (= hFirstEnabled (send hDialog:firstTrue(#checkState 1)))
  	     and (not (send hDialog:firstTrue(#checkState 2)) ) )
		    (send hFirstEnabled:state(| (send hFirstEnabled:state) 2))
	)
  	= btnPressed (send hDialog:doit(hFirstEnabled))
  	(if(== btnPressed -1)
		= btnPressed 0
	)

 	(for (= paramCnt 0) (< paramCnt buttonCnt) (++paramCnt)
 		(if(== btnPressed hButtons[paramCnt])
		    = btnPressed (send btnPressed:value)
		    break
		)
	)
  	(if(not (send hDialog:theItem) )
		= btnPressed 1
	)
  	(send hDialog:dispose())
	return(btnPressed)
)
/******************************************************************************/
(procedure public (IconPrint message view loop cel params)
	Print(message #icon view loop cel rest params)
)
/******************************************************************************/
(procedure public (EditPrint theText theMax aMessage printParams)
	(if (>= paramTotal 3)
		(if(Print(aMessage #edit theText theMax rest printParams))
			return(StrLen(theText))
		)
	)(else
		(if(Print("" #edit theText theMax rest printParams))
			return(StrLen(theText))
		)
	)
)
/******************************************************************************/
(procedure public (GetNumber aMessage defaultNumber)
	(var temps[40])
	= temps 0
	(if( > paramTotal 1)
		Format(@temps "%d" defaultNumber)
	)
	(if(EditPrint(@temps 5 aMessage))
		return(ReadNumber(@temps))
	)(else
	    return(-1)
	)
)
/******************************************************************************/
(procedure public (FormatPrint textorstring textid params)
	(var temps[500])
	(if(<u textorstring 1000)
		Format(@temps textorstring textid rest params)
	)(else
	   Format(@temps rest textorstring)
    )
	Print(@temps)
)
/******************************************************************************/
