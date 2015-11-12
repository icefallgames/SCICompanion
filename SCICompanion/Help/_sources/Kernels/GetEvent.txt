.. GetEvent

.. default - domain::js

.. include:: /includes/standard.rst

=================
GetEvent (Kernel)
=================

.. function:: GetEvent(types event)

	
	GetEvent retrieves the event type(s) and places them in the specified event object's properties.
	See :class:`Event` for further description of the event types.
	
	:param number types: A bitmask of event types.
	:param heapPtr event: An :class:`Event` object.
	
	:returns: TRUE if the event was created, otherwise FALSE.


