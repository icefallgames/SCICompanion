.. EventHandler

.. default - domain::js

.. include:: /includes/standard.rst

==============================
EventHandler (of :class:`Set`)
==============================

.. class:: EventHandler

	Defined in System.sc.

	The EventHandler class is an extension of the Set class. Is used as a multi event handler.


Subclasses: :class:`Sounds`.

Properties
==========

Inherited from :class:`Set`:

======== ===========
Property Description
======== ===========
elements            
size                
name                
======== ===========


Methods
==========

.. function:: handleEvent(pEvent)
	:noindex:

	For each element in the list, it calls handleEvent with pEvent as the event object. It then returns whether or not the pEvent has been claimed.


