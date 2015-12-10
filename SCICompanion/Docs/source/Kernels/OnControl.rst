.. OnControl

.. default - domain::js

.. include:: /includes/standard.rst

==================
OnControl (Kernel)
==================

.. function:: OnControl(map x y)

.. function:: OnControl(map left top width height)
	:noindex:

	
	Checks to see which colors are at the specified coords or in the rect. It checks the specified map and returns a 16 bit color map.
	
	:param number map: ocVISUAL, ocPRIORITY or ocSPECIAL (ocSPECIAL is the control information).
	:param number x: The x coordinate.
	:param number y: The y coordinate.
	:param number left: Left side of the rectangle.
	:param number top: Top of the rectangle.
	:param number width: Width of the rectangle.
	:param number height: Height of the rectangle.
	
	Example::
	
		; Using a point
		(if (== ctlRED (OnControl ocSPECIAL 120 90))
			(Print "It is on a red control line!")
		)
	
		; Using a rect, multiple colors may be returned, so you should use the & operator.
		(if (& ctlBLUE (OnControl ocSPECIAL 50 20 100 40)
			(Print "It is on a blue control line!")
		)
	


