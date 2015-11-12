.. Control

.. default - domain::js

.. include:: /includes/standard.rst

=========================
Control (of :class:`Obj`)
=========================

.. class:: Control

	Defined in Controls.sc.

	A common base class for GUI controls.


Subclasses: :class:`DText`, :class:`DSelector`, :class:`DButton`, :class:`DEdit`, :class:`DIcon`, :class:`_DItem`.

Properties
==========

Defined in Control:

======== ==============================================
Property Description                                   
======== ==============================================
type                                                   
state                                                  
nsTop                                                  
nsLeft                                                 
nsBottom                                               
nsRight                                                
key      The keyboard key associated with this control.
said                                                   
value    Arbitrary value associated with this control. 
======== ==============================================


Methods
==========

.. function:: doit()
	:noindex:



.. function:: enable(fEnable)
	:noindex:



.. function:: select(fSelect)
	:noindex:



.. function:: handleEvent(pEvent)
	:noindex:



.. function:: check(pEvent)
	:noindex:

	Returns TRUE if the x and y of the event lie inside the control.


.. function:: track(pEvent)
	:noindex:

	Highlights the control if the mouse is over it.


.. function:: setSize()
	:noindex:



.. function:: move(param1 param2)
	:noindex:



.. function:: moveTo(param1 param2)
	:noindex:



.. function:: draw()
	:noindex:

	Draws the control.


.. function:: isType(theType)
	:noindex:

	
	:param number theType: ctlBUTTON, ctlTEXT, ctlEDIT, ctlICON or ctlSELETOR.
	:returns: TRUE if this control is of the type specified.
	


.. function:: checkState(stateFlags)
	:noindex:

	
	:param number stateFlags: Any combination of csENABLED, csFOCUSED, csDISABLED or csSELECTED.
	:returns: A non-zero value if the control has this state.
	


.. function:: cycle()
	:noindex:



