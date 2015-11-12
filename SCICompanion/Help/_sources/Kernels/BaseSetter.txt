.. BaseSetter

.. default - domain::js

.. include:: /includes/standard.rst

===================
BaseSetter (Kernel)
===================

.. function:: BaseSetter(view)

	
	Reads the views properties and generates it's bounding rectangle, storing
	it in the brLeft, brRight, brTop and brBottom properties.
	This rectangle is used for things such as collision detection.
	
	:param heapPtr view: An object that inherits from :class:`View`.
	
	See Also:
	:func:`View`


