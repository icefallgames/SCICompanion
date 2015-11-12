.. ShakeScreen

.. default - domain::js

.. include:: /includes/standard.rst

====================
ShakeScreen (Kernel)
====================

.. function:: ShakeScreen(shakeCount [direction])

	
	Shakes the screen a number of times.
	
	:param number shakeCount: The number of times to shake the screen.
	:param number direction: The direction in which to shake. If not specified, it shakes from left to right.
	
	================ =======================
	Direction        Description
	================ =======================
	ssLEFTRIGHT      Shakes from left to right.
	ssUPDOWN         Shakes up and down.
	ssFULL_SHAKE     Shakes the screen diagonally from the top left corner to the bottom right.
	================ =======================
	
	Example::
	
		ShakeScreen(8);				// Does a left to right shake screen eight times
		ShakeScreen(20 ssFULL_SHAKE); // Does a complete shake screen twenty times
	


