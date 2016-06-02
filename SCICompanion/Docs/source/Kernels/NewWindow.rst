.. NewWindow

.. default - domain::js

.. include:: /includes/standard.rst

==================
NewWindow (Kernel)
==================

.. function:: NewWindow(y x bottom right title flags priority forecolor backcolor)

	
	Draws a new window on the screen with the specified properties and returns the structure's address. Note that the parameter order has x and y (and right and bottom) reversed from most API calls.
	
	:param number y: Top of the window.
	:param number x: Left side of the window.
	:param number bottom: Bottom coordinate of the window.
	:param number right: Right coordinate of the window.
	:param number flags: Flags that specify how the window is to be drawn. See below.
	:param number priority: Specifies if the window should be drawn above or below other windows. The higher the priority, the more visible it will be. With a priority of zero, it may be completely covered by other windows. To make sure your window is visible, you can use nwON_TOP as the priority.
	:param number foreColor: Specifies the foreground color for the window. It is used for text, buttons, etc...
	:param number backColor: Specifies the background color for the window.
	
	Window flags:
	
	========================= ==================================================================
	Flag                      Description
	========================= ==================================================================
	nwNORMAL                  The window will be drawn with a border.
	nwTRANSPARENT             The window will not have a border, background color, or titlebar.
	nwNOFRAME                 The window will be drawn without a border.
	nwTRANSPARENT_NOFRAME     The window will be drawn without a border and a transparent body.
	nwTITLE                   The window will be drawn with a titlebar and border.
	nwNODRAW                  The window will not be drawn.
	========================= ==================================================================
	
	Example::
	
		(= oldPort (GetPort))
		; Draw a white window at 50, 20 with a width of 200, a title, and a black foreground.
		(= hWnd NewWindow
			20 50 120 250
			"Test Window"
			nwTITLE
			nwON_TOP
			clBLACK
			clWHITE
		)
	
		(SetPort hWnd)
		(Display "Hello in the hWnd window!")
		(SetPort oldPort)
		(Display "Hello in the oldPort!")
		(DisposeWindow hWnd)


