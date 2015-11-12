.. NumCels

.. default - domain::js

.. include:: /includes/standard.rst

================
NumCels (Kernel)
================

.. function:: NumCels(view)

.. function:: NumCels(icon)
	:noindex:

	
	By specifying a instance or class as the parameter, it looks up the view and loop properties and
	returns the total number of cels in that loop. If the view is not loaded, it will automatically load it.
	
	:param heapPtr view: An instance or class of :class:`View`.
	:param heapPtr icon: An instance or class of :class:`DIcon`.
	:returns: The total number of cels in the current loop of the View or icon.


