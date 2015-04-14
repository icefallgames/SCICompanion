/******************************************************************************
 SCI Template Game
 By Brian Provinciano
 ******************************************************************************
 window.sc
 Contains the extension of the SysWindow class.
 ******************************************************************************/
(include "sci.sh")
(include "game.sh")
/******************************************************************************/
(script WINDOW_SCRIPT)
/******************************************************************************/
(use "syswindow")
/******************************************************************************/
(class Window of SysWindow
	(properties
		top 0
		left 0
		bottom 0
		right 0
		color clBLACK
		back clWHITE
		priority -1
		window 0
		type 0
		title 0
		brTop 0
		brLeft 0
		brBottom 190
		brRight 320
		underBits 0
	)
	(method (doit))
	(method (dispose)
		(self:restore())
		(if(window)
			DisposeWindow(window)
			= window 0
		)
		(super:dispose())
	)
	(method (open)
		= window NewWindow(
			top left bottom right
		    title
		    type
		    priority
		    color
		    back
		)
	)
	(method (handleEvent)
		return(FALSE)
	)
	(method (setMapSet)
		(var mapSet)
		= mapSet 0
		(if(<> -1 color)
		    = mapSet (| mapSet 1)
		)
		(if(<> -1 priority)
		    = mapSet (| mapSet 2)
		)
		return(mapSet)
	)
	(method (move theX theY)
		= left   (+ left   theX)
		= right  (+ right  theX)
		= right  (+ right  theY)
		= bottom (+ bottom theY)
	)
	(method (moveTo newX newY)
		(self:move( (- newX left) (- newY top) ))
	)
	(method (draw newColor newPriority)
		(if(>= paramTotal 1)
			= color newColor
		)
		(if(>= paramTotal 2)
			= priority newPriority
		)
		Graph(
			11
			top left right bottom
			(self:setMapSet())
			color
			priority
		)
	)
	(method (save)
		= underBits Graph(
			grSAVE_BOX
			top left bottom right
			(self:setMapSet())
		)
	)
	(method (restore)
		(if(underBits)
			Graph(grRESTORE_BOX underBits)
		)
	)
	(method (inset theX theY)
		= top (+ top theY)
		= left (+ left theX)
		= bottom (- bottom theY)
		= right (- right theX)
	)
	(method (show)
		Graph(
			grUPDATE_BOX
			top left bottom right
			(self:setMapSet())
		)
	)
	(method (erase)
		(self:draw(back -1))
	)
	(method (center)
		(self:moveTo(
			(/ (- (- brRight left) (- right left)) 2)
		    (/ (- (- brBottom top) (- bottom top)) 2)
		))
	)
)
/******************************************************************************/
