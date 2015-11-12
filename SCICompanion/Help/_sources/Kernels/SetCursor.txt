.. SetCursor

.. default - domain::js

.. include:: /includes/standard.rst

==================
SetCursor (Kernel)
==================


.. function:: SetCursor(resourceNumber [visible x y])

	.. IMPORTANT:: SCI0 only (See below for the SCI1.1 functions)

	Changes the mouse cursor, or shows and hides it.
	
	:param number resourceNumber: Number of the cursor resource to use.
	:param boolean visible: If TRUE or unspecifed, the cursor is shown.  Otherwise, it is hidden.
	:param number x: The x position at which to place the cursor.
	:param number y: The y position at which to place the cursor.

.. function:: SetCursor(command)
	:noindex:

	.. IMPORTANT:: SCI1.1 only

	:param number command: A command indicating what do do.

	Cursor commands:

	========= ===============================
	Command   Description
	========= ===============================
	0         Hide cursor.
	-1        Clear zoom zone.
	-2        Reset zoom zone.
	any other Show cursor.
	========= ===============================

.. function:: SetCursor(x y)
	:noindex:

	.. IMPORTANT:: SCI1.1 only

	Sets the position of the cursor.

	:param number x: The x position at which to place the cursor.
	:param number y: The y position at which to place the cursor.


.. function:: SetCursor(left top right bottom)
	:noindex:

	Sets the move zone of the cursor.

	.. IMPORTANT::
		SCI1.1

	:param number left: The left side of the move zone.
	:param number top: The top of the  move zone.
	:param number right: The right side of the  move zone.
	:param number bottom: The bottom of the  move zone.

	Changes the mouse cursor, or shows and hides it.

.. function:: SetCursor(view loop cel [hotX hotY])
	:noindex:

	Sets the view, loop and cel of the cursor, and optionally the hot spot.

	.. IMPORTANT::
		SCI1.1

	:param number view: View number.
	:param number loop: Loop number.
	:param number cel: Cel number.
	:param number hotX: The hotspot x coordinate.
	:param number hotY: The hotspot y coordinate.


