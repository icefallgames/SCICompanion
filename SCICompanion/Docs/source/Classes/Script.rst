.. Script

.. default - domain::js

.. include:: /includes/standard.rst

===========================
Script (of :class:`Object`)
===========================

.. class:: Script

	Defined in System.sc.

	
	The Script class (different than a script file) is a very important part of the game. A Script instance can be attached
	to most game objects, including Props, Actors, and room objects. They allow you to attach pluggable behavior to an object.
	In fact, Script instances can themselves have Scripts attached to them.


Subclasses: :class:`CueObj`.

Properties
==========

Inherited from :class:`Object`:

======== ===========
Property Description
======== ===========
name                
======== ===========

Defined in Script:

=========== ===========
Property    Description
=========== ===========
client                 
state                  
start                  
timer                  
cycles                 
seconds                
lastSeconds            
ticks                  
lastTicks              
register               
script                 
caller                 
next                   
=========== ===========


Methods
==========

.. function:: init(theClient theCaller theRegister)
	:noindex:

	
	Sets the client property to theClient. If the other parameters are specified, it sets the caller property to theCaller,
	and the register property to theRegister. Finally, it calls the changeState method with the start property as its parameter.
	


.. function:: doit()
	:noindex:

	
	If the script property is not NULL, it calls the doit method of it.
	Next, if cycles property is not zero, it decrements it. If after decrementing, it is equal to zero, it calls the cue() method.
	
	If the cycles property was zero to begin with, and the seconds property is not equal to zero, it compares the actual seconds to the lastSeconds property.
	If they are not equal, it sets lastSeconds to the current seconds (from GetTime(gtTIME_OF_DAY)) and decrements the seconds property. If the seconds property is equal to zero, it calss the cue() method.	
	


.. function:: dispose()
	:noindex:



.. function:: changeState(newState)
	:noindex:

	
	Sets the state property to the newState. This method is commonly overridden by
	Script instances to implement custom behavior.
	


.. function:: cue(params)
	:noindex:

	Switches to the next state, forwarding any parameters to changeState.


.. function:: handleEvent(pEvent)
	:noindex:

	
	If the script property is not null, it calls the script's handleEvent() method with pEvent has its parameter.
	It then returns whether of not the pEvent object has been claimed.
	


.. function:: setScript(theScript sendParams)
	:noindex:

	Attaches a script to this Script object itself and initializes it with any sendParams provided.


