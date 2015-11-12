.. SetNowSeen

.. default - domain::js

.. include:: /includes/standard.rst

===================
SetNowSeen (Kernel)
===================

.. function:: SetNowSeen(view)

	
	Makes the view visible on the screen.
	
	:param heapPtr view: An object derived from :class:`View`.
	
	It looks up the view object's x and y properties,
	as well as the view/cel/loop to determine it's width and height,
	then sets it's nsLeft, nsTop, nsRight and nsBottom properties accordingly. 


