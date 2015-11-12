.. Show

.. default - domain::js

.. include:: /includes/standard.rst

=============
Show (Kernel)
=============

.. function:: Show(screen)

	
	Shows the visual, priority or control screen.
	
	:param number screen: VISUAL, PRIORITY or CONTROL.	
	
	This can be useful for debugging your game.
	
	Sets bit 1 of the picNotValid flag, making the picture "showable".
	The picNotValid flag is used by other functions to determine whether or not the background picture is considered valid.	


