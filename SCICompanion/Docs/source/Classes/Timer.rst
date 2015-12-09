.. Timer

.. default - domain::js

.. include:: /includes/standard.rst

==========================
Timer (of :class:`Object`)
==========================

.. class:: Timer

	Defined in Timer.sc.

	
	A timer class that can count down in ticks, milliseconds, seconds, minutes or hours.
	
	Example usage::
	
		; Make a timer to cue() the current object in 3 seconds
		((Timer new:) set: self 3)


Properties
==========

Inherited from :class:`Object`:

======== ===========
Property Description
======== ===========
name                
======== ===========

Defined in Timer:

======== ===========
Property Description
======== ===========
cycleCnt            
seconds             
ticks               
lastTime            
client              
======== ===========


Methods
==========

.. function:: new()
	:noindex:



.. function:: init(theClient)
	:noindex:



.. function:: doit()
	:noindex:



.. function:: dispose()
	:noindex:




.. function:: set(theClient theSeconds [theMinutes theHours])
	:noindex:

	Sets the timer.

	:param heapPtr theClient: This object will have its cue() method called when the timer expires.
	:param number theSeconds: The number of seconds for the timer.
	:param number theMinutes: The number of minutes for the timer (optional).
	:param number theHours: The number of hours for the timer (optional).



.. function:: setCycle(theCycler sendParams)
	:noindex:




.. function:: setReal(theClient theMilliseconds [theSeconds theMinutes])
	:noindex:

	Sets the timer. This is more precise than set(), because you can specify milliseconds.

	:param heapPtr theClient: This object will have its cue() method called when the timer expires.
	:param number theMilliseconds: The number of seconds for the timer.
	:param number theSeconds: The number of seconds for the timer.
	:param number theMinutes: The number of minutes for the timer (optional).



.. function:: delete()
	:noindex:



.. function:: setTicks(param1 param2)
	:noindex:



