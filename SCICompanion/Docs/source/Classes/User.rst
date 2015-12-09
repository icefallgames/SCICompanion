.. User

.. default - domain::js

.. include:: /includes/standard.rst

=========================
User (of :class:`Object`)
=========================

.. class:: User

	Defined in User.sc.

	
	This class manages user input. The main method of interest here is canControl().
	
	Example usage::
	
		(if (gUser canControl:)
			; The user is in control...
		)


Properties
==========

Inherited from :class:`Object`:

======== ===========
Property Description
======== ===========
name                
======== ===========

Defined in User:

=========== ===========
Property    Description
=========== ===========
alterEgo               
input                  
controls               
prevDir                
x                      
y                      
mapKeyToDir            
curEvent               
=========== ===========


Methods
==========

.. function:: init()
	:noindex:



.. function:: doit()
	:noindex:




.. function:: canControl()
	:noindex:

	:returns: TRUE if the user is currently controlling the game, otherwise FALSE.

.. function:: canControl(shouldControl)
	:noindex:

	:param boolean shouldControl: TRUE if the user should be in control, otherwise FALSE.

	.. IMPORTANT::
		Generally, (gGame handsOff:) and (gGame handsOn:) should be used to give and take away user control of the game.



.. function:: handleEvent(pEvent)
	:noindex:




.. function:: canInput()
	:noindex:

	:returns: TRUE if input is enabled, FALSE otherwise.

.. function:: canInput(shouldInput)
	:noindex:

	:param boolean shouldInput: TRUE if input should be enabled, otherwise FALSE.



