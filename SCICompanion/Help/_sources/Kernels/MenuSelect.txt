.. MenuSelect

.. default - domain::js

.. include:: /includes/standard.rst

===================
MenuSelect (Kernel)
===================


.. function:: MenuSelect(event [fPauseSound])

	Checks to see if the menu has been activated (either by clicking or pressing ESC).
	If it has, the user can select an item from the menu. If an item is selected, it returns the index of the item. If not, it returns -1.	

	:param heapPtr event: An :class:`Event` object.
	:param boolean fPauseSound: If TRUE or unspecified, the sound is paused.
	:returns: Index of the item selected, or -1 if none selected.


