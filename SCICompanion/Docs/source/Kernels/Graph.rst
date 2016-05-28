.. Graph

.. default - domain::js

.. include:: /includes/standard.rst

==============
Graph (Kernel)
==============


.. function:: (Graph grGET_COLORS)
.. function:: (Graph grDRAW_LINE y1 x1 y2 x2 [visualColor priColor ctlColor])
	:noindex:
.. function:: (Graph grSAVE_BOX y1 x1 y2 x2 [screen])
	:noindex:
.. function:: (Graph grRESTORE_BOX handle)
	:noindex:
.. function:: (Graph grFILL_BOX y1 x1 y2 x2 screens [visualColor priColor ctlColor])
	:noindex:
.. function:: (Graph grFILL_BOX_BACKGROUND y1 x1 y2 x2)
	:noindex:
.. function:: (Graph grFILL_BOX_FOREGROUND y1 x1 y2 x2)
	:noindex:
.. function:: (Graph grUPDATE_BOX screen)
	:noindex:
.. function:: (Graph grREDRAW_BOX y1 x1 y2 x2)
	:noindex:
.. function:: (Graph grADJUST_PRIORITY min max)
	:noindex:

	Graph is a kernel with many sub functions. It should be noted that the order of the coordinates
	(y, followed by x) is the opposite of most kernels.

	Also note that most Graph functions require a follow-up call to grUPDATE_BOX to update the screen after drawing.

	:param number y1: The top of the box.
	:param number x1: The left side of the box.
	:param number y2: The bottom of the box.
	:param number x2: The right side of the box.
	:param number screen: A screen (VISUAL, PRIORITY or CONTROL).
	:param number screens: A bitmask of VISUAL, PRIORITY or CONTROL.
	:param number handle: A handle returned by grSAVE_BOX.
	:param number visualColor: The color value for the visual screen.
	:param number priColor: A priority value, or -1 to indicate none specified.
	:param number ctlColor: A control color, or -1 to indicate none specified.
	:param number min: The minimum priority band.
	:param number max: The maximum priority band.
	:returns: Various values. See below.

	The following functions are available:

	grGET_COLORS
		Obtains the color resolution which the interpreter is running in. **Returns** the number of colors.

	grDRAW_LINE
		Draws a line on the specified screen(s) with the specified color.

	grSAVE_BOX
		Saves the screen pixels in the specified area and **returns** a handle to them. If screen is specified, it saves the pixels of that screen (VISUAL,PRIORITY,CONTROL). Otherwise, it uses the VISUAL screen.

	grRESTORE_BOX
		Restores the pixels saved with grSAVE_BOX.

	grFILL_BOX
		Fills the specified rectangular region with the specified color(s). The screens parameter specifies which screens to drawn to.

	grFILL_BOX_BACKGROUND
		Operates just like grFILL_BOX, but draws a box on the visual screen with the current port's background color.

	grFILL_BOX_FOREGROUND
		Operates just like grFILL_BOX, but draws a box on the visual screen with the current port's foreground color.

	grUPDATE_BOX
		Draws the specified area of the specified screen to the screen. You can use this to redraw the background picture, and also to view the priority/control screens. 

	grREDRAW_BOX
		Redraws the specified area of visual screen to the screen.

	grADJUST_PRIORITY
		Sets the minimum and maximum bounds for the priority. By default, these values are 42 and 190 in SCI0.

	Example for grGET_COLORS::

		(= colors (Graph grGET_COLORS))
		(if (== colors 16)
			(Print "You are running in EGA mode")
		else
			(if (== colors -1)
				(Print "You are running in CGA mode")
			else
				; VGA returns 25 in the original interpreter, but 256 in ScummVM.
				(Print "You are probably in VGA mode")
			)
		)

	Example for grUPDATE_BOX::

		; Shows the priority screen for two seconds
		(Graph grUPDATE_BOX 0 0 199 319 PRIORITY)
		(Wait 120)

		; Shows the control screen for two seconds
		(Graph grUPDATE_BOX 0 0 199 319 CONTROL)
		(Wait 120)

		; Shows the visual screen
		(Graph grUPDATE_BOX 0 0 199 319 VISUAL)

	Example for grFILL_BOX::

		; Draws a box with the bounds of (30,20) and (200,100) on the visual screen in yellow, the priority screen in red, and the control screen in blue.
		(Graph grDRAW_BOX 20 30 100 200 ALL_SCREENS clYELLOW clRED clBLUE)

		; Doesn't draw anything though the colors are specified, no screens are
		(Graph grDRAW_BOX 20 30 100 200 0 clYELLOW clRED clBLUE)

		; Draws a box with the bounds of (60,40) and (160,130) on the visual screen in white, and the control screen in brown.
		(Graph grDRAW_BOX 40 60 130 160 VISUAL_CONTROL clWHITE -1 clBROWN)

	Example for grRESTORE_BOX and grSAVE_BOX::

		; this saves the center of the screen, draws a line, then restores the area leaving only the line's edges
		(= handle (Graph grSAVE_BOX 50 80 150 240))
		(Graph grDRAW_LINE 10 10 180 310 clYELLOW) 
		(Graph grRESTORE_BOX handle)

	Example for grDRAW_LINE::

		; Draw a line from (30,20) to (200,100) on the visual screen in yellow, the priority screen in red, and the control screen in blue.
		(Graph grDRAW_LINE 20 30 100 200 clYELLOW clRED clBLUE)

		; Draw a line from (60,40) to (160,130) on the visual screen in white, and the control screen in brown.
		(Graph grDRAW_LINE 40 60 130 160 clWHITE -1 clBROWN)

		; Draw a line from (10,20) to (40,50) on the visual screen in red.
		(Graph grDRAW_LINE 20 10 50 40 clRED)



