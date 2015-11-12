.. RespondsTo

.. default - domain::js

.. include:: /includes/standard.rst

===================
RespondsTo (Kernel)
===================

.. function:: RespondsTo(obj aSelector)

	
	Checks to see if the object will respond to the specified selector (property or method).
	Call this before calling an object's method or accessing an object's property if you are unsure if the object actually contains it.
	
	:param heapPtr obj: The object.
	:param selector aSelector: The method or property selector.
	:returns: TRUE if the object does respond to the selector, otherwise, FALSE.
	
	Example::
	
		(if(RespondsTo(someObject #doit))
			(send someObject:doit())
		)
		(if(RespondsTo(someObject #text))
			(send someObject:text("Hello world!"))
		)


