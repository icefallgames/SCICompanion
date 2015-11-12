.. CantBeHere

.. default - domain::js

.. include:: /includes/standard.rst

===================
CantBeHere (Kernel)
===================

.. function:: CantBeHere(view)

.. function:: CantBeHere(view castList)
	:noindex:

	
	Checks the view to see if it is not allowed to be at its current coordinates on the screen.
	
	.. IMPORTANT:: SCI1.1 only. For SCI0, see :func:`CanBeHere`.
	
	:param heapPtr view: An object that inherits from :class:`View`.
	:param k_list cast: Optional cast list.
	:returns: TRUE if it **cannot** be where it is, FALSE otherwise.
	
	.. NOTE::
		This is used in the class system when the object moves.
		If it can't be where it is, it sets it to it's previous coordinates.
		If castList is specified, it checks the viewObj against the objects in the list
		to see if it can be where it is. 


