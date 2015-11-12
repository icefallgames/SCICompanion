.. Clone

.. default - domain::js

.. include:: /includes/standard.rst

==============
Clone (Kernel)
==============

.. function:: Clone(object)

	
	Creates a new instance of a class or object and returns a pointer to it.
	
	:param heapPtr object: An object to clone.
	:returns: Pointer to a clone of the object.
	
	Example::
	
		Event newEvent;
		newEvent = Clone(Event); 	// create an instance of Event and stores the pointer into newEvent
		GetEvent($7FFF newEvent); 	// use the instance
		DisposeClone(Event) 		// dispose of the instance


