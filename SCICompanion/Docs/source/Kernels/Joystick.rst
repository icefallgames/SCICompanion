.. Joystick

.. default - domain::js

.. include:: /includes/standard.rst

=================
Joystick (Kernel)
=================

.. function:: Joystick(subFunction state)

	
	Calls the joystick driver with subfunction and state as it's parameters. If subfunction is not jsCALL_DRIVER, it doesn't do anything. It returns the previous state of the joystick.
	
	Example::
	
		(= joyState (Joystick jsCALL_DRIVER 30))
		(= menuItem (MenuSelect pEvent))
		(Joystick jsCALL_DRIVER joyState)


