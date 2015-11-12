.. GlobalToLocal

.. default - domain::js

.. include:: /includes/standard.rst

======================
GlobalToLocal (Kernel)
======================

.. function:: GlobalToLocal(event)

	
	Converts the event object from a screen relative one to a port relative one. It will make the event only be true if it is occuring in the currently active port (window).
	
	:param heapPtr event: An :class:`Event` object.


