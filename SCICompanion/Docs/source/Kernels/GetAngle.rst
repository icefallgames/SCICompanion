.. GetAngle

.. default - domain::js

.. include:: /includes/standard.rst

=================
GetAngle (Kernel)
=================

.. function:: GetAngle(x1 y1 x2 y2)

	
	Calculates the angle of the two points and returns it. It always generates a positive angle.
	
	:param number x1: The x coordinate of the first point.
	:param number y1: The y coordinate of the first point.
	:param number x2: The x coordinate of the second point.
	:param number y2: The y coordinate of the second point.
	
	Example::
	
		; Get angle between (10, 20) and (100, 80)
		(= anAngle (GetAngle 10 20 100 80))


