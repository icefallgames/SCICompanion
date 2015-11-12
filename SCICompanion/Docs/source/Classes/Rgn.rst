.. Rgn

.. default - domain::js

.. include:: /includes/standard.rst

=====================
Rgn (of :class:`Obj`)
=====================

.. class:: Rgn

	Defined in Game.sc.

	
	Rgn ("region") serves as a base class for :class:`Rm`. It can also be used on its own, to contain
	common logic that applies to several rooms.
	
	To make a region part of a room, put the following code in the room's init() method::
	
		(self:setRegions(MY_REGION))
		// Where MY_REGION would be a script number for a script that contains a public Rgn instance.


Subclasses: :class:`Rm`.

Properties
==========

Defined in Rgn:

=========== ===========
Property    Description
=========== ===========
script                 
number                 
modNum                 
noun                   
timer                  
keep                   
initialized            
=========== ===========


Methods
==========

.. function:: init()
	:noindex:

	
	Override this method to provide any custom initialization needed for your Rgn. Make sure you call (super:init()), or
	your region won't be added to the current room's regions.
	


.. function:: doit()
	:noindex:

	
	This is commonly overridden in Rgn instances to provide custom logic that gets executed every game cycle.
	


.. function:: dispose()
	:noindex:



.. function:: handleEvent(pEvent)
	:noindex:

	
	This is commonly overridden in Rgn instances to handle keyboard or mouse events.
	


.. function:: doVerb(theVerb)
	:noindex:

	
	This is commonly overridden in Rgn instances to code custom logic for verbs.
	



.. function:: setScript(theScript [sendParams ...])
	:noindex:

	This attaches a :class:`Script` to the Rgn.

	:param heapPtr theScript: A Script instance.
	:param sendParams: Any additional parameters needed to initialize the script.



.. function:: cue()
	:noindex:



.. function:: newRoom()
	:noindex:



.. function:: notify()
	:noindex:



