.. IsObject

.. default - domain::js

.. include:: /includes/standard.rst

=================
IsObject (Kernel)
=================

.. function:: IsObject(obj)

	
	Determines whether a value is an object.
	
	:param obj: Something that might be an object.
	:returns: TRUE if obj is an object.
	
	Example::
	
		(= objPtr (Event new:))
		(IsObject objPtr) ; returns TRUE
		(= objPtr 0)
		(IsObject objPtr) ; returns FALSE


