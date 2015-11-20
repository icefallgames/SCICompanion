.. Obj

.. default - domain::js

.. include:: /includes/standard.rst

===
Obj
===

.. class:: Obj

	Defined in Obj.sc.

	
	The base class used for all SCI Objects. It provides some basic operations
	like asking if an object supports a particular selector, or if an object is
	subclass of a given class.


Subclasses: :class:`Sound`, :class:`Code`, :class:`Cycle`, :class:`IconItem`, :class:`Game`, :class:`Blk`, :class:`Messager`, :class:`Control`, :class:`Feature`, :class:`SysWindow`, :class:`Script`, :class:`Print`, :class:`Motion`, :class:`Sync`, :class:`MessageObj`, :class:`ClickMenu`, :class:`Polygon`, :class:`File`, :class:`Timer`, :class:`TO`, :class:`Flags`, :class:`Cue`, :class:`Rgn`, :class:`SL`, :class:`Event`, :class:`User`, :class:`Collect`, :class:`Cursor`.

Properties
==========


Methods
==========

.. function:: new()
	:noindex:

	Returns a clone of itself


.. function:: init()
	:noindex:



.. function:: doit()
	:noindex:



.. function:: dispose()
	:noindex:



.. function:: showStr(buffer)
	:noindex:

	Copies the object's name into the provided buffer.


.. function:: showSelf()
	:noindex:

	Prints the object's name on the screen.



.. function:: perform(object [...])
	:noindex:

	Calls the doit method on object with itself as the first parameter, and forwarding any additional supplied parameters.

	:param heapPtr object: Any object with a doit method.



.. function:: isKindOf(className)
	:noindex:

	
	Determines if this object is a particular class, or inherits from that class.
	
	:param class className: A class name, such as Actor or Room
	
	:returns: TRUE is this object is of type className, or inherits from className.
	


.. function:: isMemberOf(theClass)
	:noindex:

	
	:param class theClass: A class, like MoveTo or Feature.
	:returns: TRUE if this object is an instance of this exact class, otherwise FALSE.
	
	Example usage::
	
		(if (send gThing:isMemberOf(MoveTo))
			// Do something...
		)
	


.. function:: respondsTo(selectorName)
	:noindex:

	
	:param selector selectorName: A property or method selector.
	
	:returns: TRUE if this object has selectorName as a property or method.
	
	Example usage::
	
		(if (send theThing:respondTo(#doit))
			(send theThing:doit())
		)
	


.. function:: yourself()
	:noindex:

	
	Returns the object itself. This is a convenient shortcut when you want to call a series of selectors on
	a new object and then pass that object directly to a function::
	
		// Create a new polygon and add it to the room obstacles.		
		(send gRoom:addObstacle((send ((Polygon:new())):
				type(PBarredAccess)
				points(somePoints)
				size(4)
				dynamic(TRUE)
				yourself()
								)))
	


